/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, Edwin van Leeuwen

	 This file is part of RealTimePlot.

	 RealTimePlot is free software; you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation; either version 3 of the License, or
	 (at your option) any later version.

	 RealTimePlot is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License
	 along with RealTimePlot. If not, see <http://www.gnu.org/licenses/>.

	 -------------------------------------------------------------------
	 */

#include <boost/shared_ptr.hpp>
#include "realtimeplot/backend.h"
#include "realtimeplot/utils.h"

#include <boost/math/special_functions/beta.hpp>

#include <limits>

namespace realtimeplot {
	/*
	 * BackendPlot
	 */
	boost::mutex BackendPlot::global_mutex;

	BackendPlot::BackendPlot(PlotConfig conf, boost::shared_ptr<EventHandler> pEventHandler) : pEventHandler( pEventHandler )
	{
		config = conf;
		checkConfig();

#ifndef NO_X
		if (config.display)
			pDisplayHandler = XcbHandler::Instance();
		else {
			pDisplayHandler = DummyHandler::Instance();
		}
#endif
#ifdef NO_X
		pDisplayHandler = DummyHandler::Instance();
		config.display = false;
#endif
	
		pPlotArea = boost::shared_ptr<PlotArea> (new PlotArea( config ));

		//create_xlib_window
		x_surface_width = pPlotArea->plot_area_width+config.margin_y;
		x_surface_height = pPlotArea->plot_area_height+config.margin_x;
		win = pDisplayHandler->open_window(x_surface_width, x_surface_height,
			 pEventHandler);
		// Set the title
		pDisplayHandler->set_title( win, config.title );

		xSurface = pDisplayHandler->get_cairo_surface( win,
				x_surface_width, x_surface_height );
		if(!xSurface)
			fprintf(stderr,"Error creating surface\n");

		//draw initial axes etc
		draw_axes_surface();
		
		time_of_last_update = boost::posix_time::microsec_clock::local_time() - 
			boost::posix_time::microseconds(500000);

		pause_display = false;

		//pEventHandler->processing_events = true;

		xContext = Cairo::Context::create( xSurface );
		display();
	}

	BackendPlot::~BackendPlot() {
	}

	void BackendPlot::checkConfig() {
		if ( config.overlap >= 1 )
			config.fixed_plot_area = true;
		else if (config.overlap < 0)
			config.overlap = 0;
	}

	void BackendPlot::display() {
		//Has the display been paused?
		if ( !pause_display && config.display && xSurface ) {
			boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
			//Only do this if event queue is empty 
			//or last update was more than a 0.5 seconds ago
			if  (pEventHandler && pEventHandler->get_queue_size() < 1 
					|| (( now-time_of_last_update )>( boost::posix_time::microseconds(500000))))  {
				temporary_display_surface = create_temporary_surface();
				//copy the temporary surface onto the xcb surface
				//Appears that this is not completely thread safe (probably problem in xcb)
				xContext->set_source( temporary_display_surface, 0, 0 );
				global_mutex.lock();
				xContext->paint();
				global_mutex.unlock();

				time_of_last_update = boost::posix_time::microsec_clock::local_time();
			}
		}
	}

	void BackendPlot::clear() {
		//give the plot its background color
		pPlotArea->clear();
		display();
	}

	void BackendPlot::reset( PlotConfig conf ) {
		config = conf;
		pPlotArea->setup( conf );
		set_foreground_color();

		x_surface_width = pPlotArea->plot_area_width+config.margin_y;
		x_surface_height = pPlotArea->plot_area_height+config.margin_x;
		xSurface = pDisplayHandler->get_cairo_surface( win, 
				pPlotArea->plot_area_width+config.margin_y, 
				pPlotArea->plot_area_height+config.margin_x);
		xContext = Cairo::Context::create( xSurface );

		//draw initial axes etc
		draw_axes_surface();
		
		time_of_last_update = boost::posix_time::microsec_clock::local_time() - 
			boost::posix_time::microseconds(500000);

		//pEventHandler->processing_events = true;
		//update_config();

		display();
	}

	void BackendPlot::close_window() {
		if (xSurface) {
			xContext.clear();
			xSurface.clear();
			pDisplayHandler->close_window( win );
		}
	}


	void BackendPlot::transform_to_plot_units( ) {
		transform_to_plot_units( pPlotArea->context );
	}

	void BackendPlot::transform_to_plot_units( Cairo::RefPtr<Cairo::Context> pContext ) {
		transform_to_device_units( pContext );
		pContext->translate( 0, pPlotArea->height );
		pContext->scale( pPlotArea->width/(pPlotArea->max_x-pPlotArea->min_x),
				-pPlotArea->height/(pPlotArea->max_y-pPlotArea->min_y) );
		pContext->translate( -pPlotArea->min_x, -pPlotArea->min_y );
	}

	void BackendPlot::transform_to_plot_units_with_origin( 
			Cairo::RefPtr<Cairo::ImageSurface> pSurface, 
			Cairo::RefPtr<Cairo::Context> pContext, int margin_x, int margin_y ) {
		transform_to_device_units( pContext );
		pContext->translate( margin_y, pSurface->get_height()-margin_x );
		pContext->scale( (pSurface->get_width()-margin_y)/((config.max_x-config.min_x)),
				-(pSurface->get_height()-margin_x)/((config.max_y-config.min_y)) );
		pContext->translate( -config.min_x, -config.min_y );
	}

	void BackendPlot::transform_to_device_units(
			Cairo::RefPtr<Cairo::Context> pContext) {
		pContext->set_identity_matrix();
	}

	void BackendPlot::draw_axes_surface() {
		boost::mutex::scoped_lock lock(global_mutex);
		//draw them non transparent (else we get weird interactions that when 
		//drawing a transparent point and a rolling update happens the axes 
		//become transparent as well)
		std::vector<float> xaxis_ticks;
		std::vector<float> yaxis_ticks;

		Pango::init();

		//axes_surface, extra margin_x/margin_y pixels for axes and labels
		//if xSurface is not closed, width depends on xSurface width.
		if (xSurface) {
			axes_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					x_surface_width, x_surface_height);
				//xSurface->get_width(), 
				//xSurface->get_height() );
			axes_context = Cairo::Context::create(axes_surface);
		} else {
			axes_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					pPlotArea->plot_area_width+config.margin_y, pPlotArea->plot_area_height+config.margin_x);
			axes_context = Cairo::Context::create(axes_surface);
		}

		int text_width, text_height;
		Glib::RefPtr<Pango::Layout> pango_layout = Pango::Layout::create(axes_context);
		Pango::FontDescription pango_font = Pango::FontDescription(config.font);
		//pango_font.set_weight( Pango::WEIGHT_HEAVY );
		//pango_layout->set_font_description( pango_font );

		/*Cairo::FontOptions font_options = Cairo::FontOptions();
			font_options.set_hint_metrics( Cairo::HINT_METRICS_OFF );
			font_options.set_hint_style( Cairo::HINT_STYLE_NONE );
			font_options.set_antialias( Cairo::ANTIALIAS_NONE );
			pango_layout->get_context()->set_cairo_font_options( font_options );*/


		transform_to_plot_units_with_origin( axes_surface, axes_context, 
				config.margin_x, config.margin_y );
		//plot background color outside the axes (to cover points plotted outside)
		set_background_color( axes_context );
		double dx=config.margin_x;
		double dy=-config.margin_y;
		axes_context->device_to_user_distance( dx, dy );
		axes_context->move_to( config.min_x, config.min_y );
		axes_context->line_to( config.min_x, config.max_y );
		axes_context->line_to( config.min_x-dx, config.max_y );
		axes_context->line_to( config.min_x-dx, config.min_y-dy );
		axes_context->line_to( config.max_x, config.min_y-dy );
		axes_context->line_to( config.max_x, config.min_y );
		axes_context->move_to( config.min_x, config.min_y );
		axes_context->fill();

		//Plot the main axes lines
		set_foreground_color( axes_context );
		axes_context->move_to( config.min_x, config.min_y );
		axes_context->line_to( config.min_x, config.max_y );
		axes_context->move_to( config.min_x, config.min_y );
		axes_context->line_to( config.max_x, config.min_y );

		//Plot the ticks + tick labels
		xaxis_ticks = axes_ticks( config.min_x, config.max_x, config.nr_of_ticks );
		yaxis_ticks = axes_ticks( config.min_y, config.max_y, config.nr_of_ticks );

		double length_tick_x = config.ticks_length;
		double length_tick_y = -config.ticks_length;
		axes_context->device_to_user_distance( length_tick_x, length_tick_y );

		//set font size. 
		//Thought pango_font.set_absolute_size would work, like this 
		//(i.e. wouldn't need Pango::SCALE, but apparently not)
		pango_font.set_size( config.numerical_labels_font_size*Pango::SCALE );
		pango_layout->set_font_description( pango_font );

		for (unsigned int i = 0; i < xaxis_ticks.size(); ++i) {
			axes_context->move_to( xaxis_ticks[i], config.min_y );
			axes_context->rel_line_to( 0, length_tick_y );
			//Do not add text to last tick (this will be cut off otherwise
			if (i != xaxis_ticks.size()-1) {
				transform_to_device_units( axes_context );
				pango_layout->set_text( utils::stringify( xaxis_ticks[i] ) );
				pango_layout->get_pixel_size( text_width, text_height );
				axes_context->rel_move_to( -0.5*text_width, 1*text_height );
				//pango_layout->add_to_cairo_context(axes_context); //adds text to cairos stack of stuff to be drawn
				pango_layout->show_in_cairo_context( axes_context );
				transform_to_plot_units_with_origin( axes_surface, axes_context, 
						config.margin_x, config.margin_y );
			}
		}

		for (unsigned int i = 0; i < yaxis_ticks.size(); ++i) {
			axes_context->move_to( config.min_x, yaxis_ticks[i] );
			axes_context->rel_line_to( length_tick_x, 0 );

			//Do not add text to last tick (this will be cut off otherwise
			if (i != yaxis_ticks.size()-1) {
				transform_to_device_units( axes_context );
				axes_context->rotate_degrees( -90 );
				pango_layout->set_text( utils::stringify( yaxis_ticks[i] ) );
				pango_layout->get_pixel_size( text_width, text_height );
				axes_context->rel_move_to( -0.5*text_width, -2*text_height );
				pango_layout->show_in_cairo_context( axes_context );
				axes_context->rotate_degrees( 90 ); //think the tranform_to_plot_units also unrotates
				transform_to_plot_units_with_origin( axes_surface, axes_context, 
						config.margin_x, config.margin_y );
			}
		}

		transform_to_device_units( axes_context );

		pango_font.set_size( config.label_font_size*Pango::SCALE );
		pango_layout->set_font_description( pango_font );

		pango_layout->set_text( config.ylabel );
		pango_layout->get_pixel_size( text_width, text_height );

		axes_context->move_to( config.margin_y-3*text_height, 
				0.5*axes_surface->get_height()+0.5*text_width );
		axes_context->save();
		axes_context->rotate_degrees( -90 );
		pango_layout->show_in_cairo_context( axes_context );
		axes_context->restore();

		pango_layout->set_text( config.xlabel );
		pango_layout->get_pixel_size( text_width, text_height );
		axes_context->move_to( 
				config.margin_y+0.5*axes_surface->get_width()-0.5*text_width, 
				axes_surface->get_height()-config.margin_x+1.5*text_height );
		pango_layout->show_in_cairo_context( axes_context );

		axes_context->stroke();
	}

	void BackendPlot::set_background_color( Cairo::RefPtr<Cairo::Context> pContext ) {
		pContext->set_source_rgba(1, 1, 1, 1);
	}

	void BackendPlot::set_foreground_color() {
		pPlotArea->set_color(Color::black());
		//set_foreground_color( pPlotArea->context );
	}
	void BackendPlot::set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext ) {
		pContext->set_source_rgba(0, 0, 0, 1);
	}

	void BackendPlot::set_color( Color color ) {
		pPlotArea->context->save();
		pPlotArea->set_color( color );
	}

	void BackendPlot::restore() {
		pPlotArea->context->restore();
	}

	void BackendPlot::point( float x, float y ) {
		if (!within_plot_bounds(x,y)) {
			if (!config.fixed_plot_area)
				rolling_update(x, y);
		}
		global_mutex.lock();
		pPlotArea->point( x, y );
		global_mutex.unlock();

		display();
	}

	void BackendPlot::rectangle( float min_x, float min_y, float width_x, float width_y, 
			bool fill, Color color ) {
		if (!within_plot_bounds(min_x,min_y)) {
			if (!config.fixed_plot_area)
				rolling_update(min_x, min_y);
		}
		/*if (!within_plot_bounds(max_x,max_y)) {
			if (!config.fixed_plot_area)
			rolling_update(max_x, max_y);
			}*/
		global_mutex.lock();
		pPlotArea->set_color( color );
		pPlotArea->rectangle( min_x, min_y, width_x, width_y, fill );
		global_mutex.unlock();
		display();
	}


	void BackendPlot::line_add( float x, float y, int id, Color color ) {
		if (!within_plot_bounds(x,y)) {
			if (!config.fixed_plot_area)
				rolling_update(x, y);
		}

		global_mutex.lock();
		pPlotArea->set_color( color );
		pPlotArea->line_add( x, y, id );
		global_mutex.unlock();

		display();
	}

	void BackendPlot::title( std::string &title ) {
		pDisplayHandler->set_title( win, title );
		config.title = title;
	}

	void BackendPlot::text( float x, float y, std::string &text ) {
		if (!within_plot_bounds(x,y)) {
			if (!config.fixed_plot_area)
				rolling_update(x, y);
		}
		transform_to_plot_units(); 
		Glib::RefPtr<Pango::Layout> pango_layout = Pango::Layout::create(
				pPlotArea->context);
		Pango::FontDescription pango_font = Pango::FontDescription(config.font);
		//pango_font.set_weight( Pango::WEIGHT_HEAVY );
		pango_font.set_size( config.numerical_labels_font_size*Pango::SCALE );
		pango_layout->set_font_description( pango_font );

		pPlotArea->context->move_to( x, y );
		transform_to_device_units( pPlotArea->context );
		set_foreground_color( pPlotArea->context );
		//plot_context->show_text( text );
		pango_layout->set_text( text );
		//pango_layout->add_to_cairo_context(plot_context); //adds text to cairos stack of stuff to be drawn
		pango_layout->show_in_cairo_context( pPlotArea->context );

		display();
	}

	void BackendPlot::save( std::string fn ) {
		Cairo::RefPtr<Cairo::ImageSurface> surface = create_temporary_surface();
		save( fn, surface );
	}

	void BackendPlot::save( std::string fn, 
			Cairo::RefPtr<Cairo::ImageSurface> pSurface ) {
		pSurface->write_to_png( fn );
	}

	void BackendPlot::rolling_update( float x, float y ) {
		std::vector<int> direction;
		direction.push_back( 0 );
		direction.push_back( 0 );
		if (x>config.max_x) {
			direction[0] = 1;
		} else if (x<config.min_x) {
			direction[0] = -1;
		} else if (y>config.max_y) {
			direction[1] = 1;
		} else if (y<config.min_y) {
			direction[1] = -1;
		}

		//update min_x etc
		double xrange = config.max_x-config.min_x;
		config.min_x += direction[0]*xrange*(1-config.overlap);
		config.max_x = config.min_x+xrange;
		double yrange = config.max_y-config.min_y;
		config.min_y += direction[1]*yrange*(1-config.overlap);
		config.max_y = config.min_y+yrange;

		if (!plot_bounds_within_surface_bounds()) {
			pPlotArea->reposition( config.min_x + (config.max_x-config.min_x)/2.0,
					config.min_y + (config.max_y-config.min_y)/2.0 );
		}
		//be recursive about it :)
		if (within_plot_bounds( x, y )) {
			draw_axes_surface();
		} else {
			rolling_update( x, y );
		}
	}

	bool BackendPlot::within_plot_bounds( float x, float y ) {
		if ( x < config.min_x || x > config.max_x ||
				y < config.min_y || y > config.max_y )
			return false;
		else
			return true;
	}

	bool BackendPlot::plot_bounds_within_surface_bounds( ) {
		if ( config.min_x <= pPlotArea->min_x || config.max_x >= pPlotArea->max_x ||
				config.min_y <= pPlotArea->min_y || config.max_y >= pPlotArea->max_y )
			return false;
		else
			return true;
	}

	std::vector<float> BackendPlot::axes_ticks( float min, float max, int nr ) {
		std::vector<float> ticks;
		int power = 0;
		float tick;
		float step = (max-min)/nr;

		//Calculate power of step (i.e. 0.01 -> power is -2)
		//Using straightforward method. Is probably much easier way
		if (step <= 1) {
			while (step/pow(10,power)<=1) {
				--power;
			}
		} else if (step >=10) {
			while (step/pow(10,power)>=10) {
				++power;
			}
		}

		//round our step
		step = round(step/pow(10,power))*pow(10,power);

		//first tick is rounded version of min
		tick = round(min/pow(10,power))*pow(10,power);

		while (tick <= max ) {
			if (tick>=min) {
				ticks.push_back( tick );
			}
			tick += step;
		}
		return ticks;
	}

	/** \brief Create an temporary imagesurface 
	 *
	 * (using a temp surface gets rid of flickering we get if we 
	 * plot plot_surface and then axes_surface
	 * directly onto xlibsurface
	 */
	Cairo::RefPtr<Cairo::ImageSurface> BackendPlot::create_temporary_surface() {

		size_t surface_width, surface_height; 
		if (xSurface) {
			surface_width = x_surface_width;
			surface_height = x_surface_height;
			/*surface_width = xSurface->get_width();
			surface_height = xSurface->get_height();*/
		} else {
			surface_width = pPlotArea->plot_area_width+config.margin_y;
			surface_height = pPlotArea->plot_area_height+config.margin_x;
		}
		Cairo::RefPtr<Cairo::ImageSurface> surface = 
			Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					surface_width, surface_height );
		Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( surface );
		
		double x = pPlotArea->min_x;
		double y = pPlotArea->max_y;
		transform_to_plot_units_with_origin( surface, context,
				config.margin_x, config.margin_y );
		context->user_to_device( x, y );

		transform_to_device_units( context );
		context->translate( x, y );
		context->scale( double(surface_width-config.margin_y)/pPlotArea->plot_area_width,
				double(surface_height-config.margin_x)/pPlotArea->plot_area_height );

		//copy the plot onto our temporary image surface
		context->set_source( pPlotArea->surface, 0, 0 );
		context->paint();
		//copy the axes onto our temporary image surface
		transform_to_device_units( context );
		context->set_source( axes_surface, 0, 0 );
		context->paint();
		return surface;
	}

	void BackendPlot::move( int direction_x, int direction_y ) {
		double xrange = config.max_x-config.min_x;
		config.min_x += 0.05*direction_x*xrange;
		config.max_x = config.min_x+xrange;
		double yrange = config.max_y-config.min_y;
		config.min_y += 0.05*direction_y*yrange;
		config.max_y = config.min_y+yrange;

		//don't move outside of the plot_surface, since we don't have that data anymore
		if (config.max_x>pPlotArea->max_x) {
			config.max_x = pPlotArea->max_x;
			config.min_x = config.max_x-xrange;
		} else if (config.min_x<pPlotArea->min_x) {
			config.min_x = pPlotArea->min_x;
			config.max_x = config.min_x+xrange;
		}
		if (config.max_y>pPlotArea->max_y) {
			config.max_y = pPlotArea->max_y;
			config.min_y = config.max_y-yrange;
		} else if (config.min_y<pPlotArea->min_y) {
			config.min_y = pPlotArea->min_y;
			config.max_y = config.min_y+yrange;
		}

		draw_axes_surface();
		display();
	}
	
	void BackendPlot::zoom( double scale ) {
		double xrange = config.max_x-config.min_x;
		double xshift = (scale-1)*xrange/2.0;;
		config.max_x += xshift;
		config.min_x -= xshift;
		double yrange = config.max_y-config.min_y;
		double yshift = (scale-1)*yrange/2.0;;
		config.max_y += yshift;
		config.min_y -= yshift;
		update_config();
		display();
	}

	void BackendPlot::update_config() { 
		//Check that it can be done:
		if (config.max_x > pPlotArea->max_x)
			config.max_x = pPlotArea->max_x;
		if (config.min_x < pPlotArea->min_x)
			config.min_x = pPlotArea->min_x;
		if (config.max_y > pPlotArea->max_y)
			config.max_y = pPlotArea->max_y;
		if (config.min_y < pPlotArea->min_y)
			config.min_y = pPlotArea->min_y;
		//Temporary put here, should only be done when min_x/max_x change
		//recalculate plot_area_width
		transform_to_plot_units( pPlotArea->context );
		double width, height;
		width = config.max_x-config.min_x;
		height = config.max_y-config.min_y;
		pPlotArea->context->user_to_device_distance( width, height );
		transform_to_device_units( pPlotArea->context );
		pPlotArea->plot_area_width = round(width);
		pPlotArea->plot_area_height = round(-height);
		if (xSurface) {
			width = x_surface_width;
			height = x_surface_height;
			/*width = xSurface->get_width();
			height = xSurface->get_height();*/
		xSurface = pDisplayHandler->get_cairo_surface( win,
				pPlotArea->plot_area_width+config.margin_y, pPlotArea->plot_area_height+config.margin_x );
			scale_xsurface( width, height );
		}
		draw_axes_surface();
		display();
	}

	void BackendPlot::scale_xsurface( double width, double height ) {
		if (config.scaling) {
			x_surface_width = width;
			x_surface_height = height;
			//xSurface->set_size( width, height );
			xSurface = pDisplayHandler->get_cairo_surface( win, width, height );
		}
		xContext = Cairo::Context::create( xSurface );
		draw_axes_surface();
		//xContext->scale( float(xSurface->get_width())/(plot_area_width+config.margin_y),
		//        float(xSurface->get_height())/(plot_area_height+config.margin_x) );
	}

	BackendHistogram::BackendHistogram( PlotConfig config, 
			boost::shared_ptr<EventHandler> pEventHandler,
					double min_x, double max_x, size_t no_bins ) :
				BackendPlot( config, pEventHandler ),
				min_x( min_x ), max_x( max_x ), no_bins( no_bins ), rebin( false ), 
				bins_y(no_bins), min_bin_size( 1e-06)
	{
		if (max_x<min_x)
			max_x = min_x+no_bins*min_bin_size;
		bin_width = ( max_x-min_x )/no_bins;
	}

	void BackendHistogram::add_data( double new_data, bool show, 
			bool freq, size_t n_no_bins, bool n_frozen_bins_x ) {
		frequency = freq;
		no_bins = n_no_bins;
		frozen_bins_x = n_frozen_bins_x;
		data.push_back( new_data );
		if (frozen_bins_x) {
			// No need to reset bounds, just add it to the correct bin
			if (new_data>=min_x && new_data<max_x)
				++bins_y[utils::bin_id(min_x, bin_width, new_data)];
		} else {
			// Special cases, using the first data point to initialize binsize etc
			if (data.size() == 1) {
				min_x = new_data;
				max_x = min_x + no_bins*min_bin_size;
				bin_width = ( max_x-min_x )/no_bins;
				rebin = true;
			} else {
				//First check that data is not smaller or larger than the current range
				if (new_data < min_x) {
					min_x = new_data;
					bin_width = ( max_x-min_x )/no_bins;
					rebin = true;
				} else if (new_data >= max_x) {
					max_x = new_data + 0.5*bin_width;
					bin_width = ( max_x-min_x )/no_bins;
					rebin = true;
				} else if (!rebin) {
					++bins_y[utils::bin_id(min_x, bin_width, new_data)];
				}
			}
		}

		if (show)
			plot();
	}

	void BackendHistogram::plot() {
		if (rebin) {
			bins_y = utils::calculate_bins( min_x, max_x, no_bins, data );
			rebin = false;
		}

		double max_y = 1.1;
		if (!frequency) {
			for (size_t i=0; i<no_bins; ++i) {
				if (bins_y[i] > max_y)
					max_y = 1.1*bins_y[i];
			}
		}

		if ( (min_x == max_x) || 
				!(frequency && config.max_y > max_y && config.max_y <= 2*max_y) ||
				!(config.max_x >= max_x && config.max_x <= max_x + 4*bin_width	) ||
				!(config.min_x <= min_x && config.min_x >= min_x - 4*bin_width	) ) {
			PlotConfig new_config = PlotConfig(config);
			new_config.min_x = min_x-bin_width;
			new_config.max_x = max_x+bin_width;
			if (!frequency)
				new_config.max_y = 1.1*max_y;
			else
				new_config.max_y = 1.1;

			reset( new_config );
		} else {
			clear();
		}

		for (size_t i=0; i<no_bins; ++i) {
			double height = bins_y[i];
			if (frequency && data.size()>0)
				height/=data.size();
			line_add( min_x+i*bin_width, 0, -1, Color::black() );
			line_add( min_x+i*bin_width, height, -1, Color::black() );
			line_add( min_x+(i+1)*bin_width, height, -1, Color::black() );
			line_add( min_x+(i+1)*bin_width, 0, -1, Color::black() );
		}
		display();
	}

	BackendHeightMap::BackendHeightMap( PlotConfig cfg, 
			boost::shared_ptr<EventHandler> pEventHandler ) : 
				BackendPlot( cfg, pEventHandler ),
				zmin( 0 ), zmax( 0 ),
				delaunay( delaunay::Delaunay( config.min_x, 
					config.max_x, config.min_y, config.max_y ) ),
				scale(false)
		{
		}


	void BackendHeightMap::add_data( float x, float y, float z, bool show) {
		if (delaunay.vertices.size() == 0) {
			zmin = z;
			zmax = z;
		}
		if (z<zmin)
			zmin = z;
		else if (z>zmax)
			zmax = z;
		boost::shared_ptr<delaunay::Vertex> vertex( new Vertex3D( x, y, z ) );
		delaunay.add_data( vertex );
		if (show && delaunay.vertices.size()>=3)
			plot();
	}


	void BackendHeightMap::plot() {
		// Only display it after it has been drawn completely
		pause_display = true;
		clear();
		for (size_t i=0; i<delaunay.triangles.size(); ++i) {
			bool part_of_super = false;
			for (size_t j=0; j<3; ++j) {
				for (size_t k=0; k<3; ++k) {
					if (delaunay.triangles[i]->corners[j]->vertex == delaunay.vertices[k])
						part_of_super = true;
				}
			}

			if (!part_of_super) {
				Triangle3D tr = Triangle3D( delaunay.triangles[i] );
				std::vector<boost::shared_ptr<Vertex3D> > v = tr.gradientVector();
				
				double x0 = v[0]->x;
				double y0 = v[0]->y;
				double x1 = v[1]->x;
				double y1 = v[1]->y;
				//plot_context->user_to_device( x0, y0 );
				//plot_context->user_to_device( x1, y1 );

		    Cairo::RefPtr< Cairo::LinearGradient > pGradient = Cairo::LinearGradient::create(
						x0, y0, x1, y1 );

				Color shade = colorMap( v[0]->z );
				pGradient->add_color_stop_rgba( 0, shade.r, shade.g, shade.b, shade.a ); 
				shade = colorMap( v[1]->z );
				pGradient->add_color_stop_rgba( 1, shade.r, shade.g, shade.b, shade.a ); 
				transform_to_plot_units();
				pPlotArea->context->move_to( tr.vertices[2]->x, tr.vertices[2]->y );

				for (size_t j=0; j<3; ++j) {
					pPlotArea->context->line_to( tr.vertices[j]->x, tr.vertices[j]->y );
				}
				pPlotArea->context->set_source( pGradient );
				transform_to_device_units(pPlotArea->context);
				pPlotArea->context->fill_preserve();
				pPlotArea->context->stroke();
				//line_add( delaunay.triangles[i]->corners[0]->vertex->x,
						//delaunay.triangles[i]->corners[0]->vertex->y, i, Color::red() );
			}
		}
		pause_display = false;
		display();
	}

	Color BackendHeightMap::colorMap( float z ) {
		float fraction = (z-zmin)/(zmax-zmin);
		float r, g, b;
		if (scale && fraction >= 0 && fraction <= 1)
			fraction = boost::math::ibeta(alpha, beta, fraction);
		/* // Red green blue
		if (fraction < 0.5) {
			r = 1-2*fraction;
			g = 2*fraction;
			b = 0;
		} else {
			r = 0;
			g = 1-2*(fraction-0.5);
			b = 0+2*(fraction-0.5);
		}*/

		// Yellow - red - black
		if (fraction < 0.5) {
			r = 1;
			g = 1-2*fraction;
			b = 0;
		} else {
			r = 1-2*(fraction-0.5);
			g = 0;
			b = 0;
		}
		return Color( r, g, b, 1 );
	}

	void BackendHeightMap::calculate_height_scaling() {
		double mean = 0;
		double v = 0;
		double dz = zmax - zmin;
		// calculate mean and sd
		size_t dim = delaunay.vertices.size();
		for (size_t i=0; i<delaunay.vertices.size(); ++i) {
			double fraction = (boost::static_pointer_cast<Vertex3D, delaunay::Vertex>( 
						delaunay.vertices[i] )->z-zmin)/dz;
			if (fraction >= 0 && fraction <= 1)
				mean += (boost::static_pointer_cast<Vertex3D, delaunay::Vertex>( 
						delaunay.vertices[i] )->z-zmin)/dz; 
			else
				--dim;
		}
		mean /= dim;
		// calculate alpha beta
		for (size_t i=0; i<delaunay.vertices.size(); ++i) {
			double fraction = (boost::static_pointer_cast<Vertex3D, delaunay::Vertex>( 
						delaunay.vertices[i] )->z-zmin)/dz;
			if (fraction >= 0 && fraction <= 1)
				v += pow((boost::static_pointer_cast<Vertex3D, delaunay::Vertex>( 
						delaunay.vertices[i] )->z-zmin)/dz-mean,2); 
		}
		v /= dim;
		alpha = mean*((mean*(1-mean))/v-1);
		beta = (1-mean)*((mean*(1-mean))/v-1);
		// Sometimes this doesn't work properly -> no scaling
		if (alpha <=0 || beta <=0)
			scale = false;
		else 
			scale = true;
		
		if (delaunay.vertices.size()>=3)
			plot();
	}
}

