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
			if (pEventHandler != NULL)
				pEventHandler->force_close = true;
		}
#endif
#ifdef NO_X
		pDisplayHandler = DummyHandler::Instance();
		config.display = false;
		if (pEventHandler != NULL)
			pEventHandler->force_close = true;
#endif

		pPlotArea = boost::shared_ptr<PlotArea> (new PlotArea( config ));

		//create_xlib_window
		x_surface_width = pPlotArea->plot_area_width+config.left_margin+config.right_margin;
		x_surface_height = pPlotArea->plot_area_height+config.bottom_margin+config.bottom_margin;
		win = pDisplayHandler->open_window(x_surface_width, x_surface_height,
				pEventHandler);
		// Set the title
		pDisplayHandler->set_title( win, config.title );

		xSurface = pDisplayHandler->get_cairo_surface( win,
				x_surface_width, x_surface_height );
		if(!xSurface)
			fprintf(stderr,"Error creating surface\n");

		//draw initial axes etc
		global_mutex.lock();
		pAxesArea = boost::shared_ptr<AxesArea>( new AxesArea() );
		global_mutex.unlock();
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
		if (config.margin_x > 0)
			config.bottom_margin = config.margin_x;
		if (config.margin_y > 0)
			config.left_margin = config.margin_y;

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
		global_mutex.lock();
		pPlotArea->setup( conf );
		global_mutex.unlock();
		set_foreground_color();

		x_surface_width = pPlotArea->plot_area_width+config.left_margin+config.right_margin;
		x_surface_height = pPlotArea->plot_area_height+config.bottom_margin+config.top_margin;
		xSurface = pDisplayHandler->get_cairo_surface( win, 
				x_surface_width, x_surface_height );
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
		if (pEventHandler != NULL)
			pEventHandler->force_close = true;
	}

	void BackendPlot::draw_axes_surface() {
		boost::mutex::scoped_lock lock(global_mutex);
		pAxesArea->setup( config, x_surface_width, x_surface_height );
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

	// FIXME: Move to pPlotArea
	void BackendPlot::text( float x, float y, std::string &text ) {
		if (!within_plot_bounds(x,y)) {
			if (!config.fixed_plot_area)
				rolling_update(x, y);
		}
		global_mutex.lock();
		pPlotArea->transform_to_plot_units(); 
		Glib::RefPtr<Pango::Layout> pango_layout = Pango::Layout::create(
				pPlotArea->context);
		Pango::FontDescription pango_font = Pango::FontDescription(config.font);
		//pango_font.set_weight( Pango::WEIGHT_HEAVY );
		pango_font.set_size( config.numerical_labels_font_size*Pango::SCALE );
		pango_layout->set_font_description( pango_font );

		pPlotArea->context->move_to( x, y );
		pPlotArea->transform_to_device_units();
		set_foreground_color( pPlotArea->context );
		//plot_context->show_text( text );
		pango_layout->set_text( text );
		//pango_layout->add_to_cairo_context(plot_context); //adds text to cairos stack of stuff to be drawn
		pango_layout->show_in_cairo_context( pPlotArea->context );

		global_mutex.unlock();
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

	/** \brief Create an temporary imagesurface 
	 *
	 * (using a temp surface gets rid of flickering we get if we 
	 * plot plot_surface and then axes_surface
	 * directly onto xlibsurface
	 */
	Cairo::RefPtr<Cairo::ImageSurface> BackendPlot::create_temporary_surface() {
		global_mutex.lock();
		Cairo::RefPtr<Cairo::ImageSurface> surface = 
			Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					pAxesArea->width, pAxesArea->height );
		Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( surface );

		double x = pPlotArea->min_x;
		double y = pPlotArea->max_y;
		pAxesArea->transform_to_plot_units();
		pAxesArea->context->user_to_device( x, y );

		context->save();

		context->translate( x, y );
		context->scale( 
				double(
					pAxesArea->width-config.left_margin-config.right_margin)/pPlotArea->plot_area_width,
				double(
					pAxesArea->height-config.bottom_margin-config.top_margin)/pPlotArea->plot_area_height );

		//copy the plot onto our temporary image surface
		context->set_source( pPlotArea->surface, 0, 0 );
		context->paint();
		//copy the axes onto our temporary image surface
		context->restore();
		context->set_source( pAxesArea->surface, 0, 0 );
		context->paint();
		global_mutex.unlock();
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
		pPlotArea->transform_to_plot_units();
		double width, height;
		width = config.max_x-config.min_x;
		height = config.max_y-config.min_y;
		pPlotArea->context->user_to_device_distance( width, height );
		pPlotArea->transform_to_device_units();
		pPlotArea->plot_area_width = round(width);
		pPlotArea->plot_area_height = round(-height);
		if (xSurface) {
			xSurface = pDisplayHandler->get_cairo_surface( win,
					x_surface_width, x_surface_height );
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
	}

	/*
	 * Histogram
	 */

	BackendHistogram::BackendHistogram( PlotConfig conf, bool frequency, 
			size_t no_bins, boost::shared_ptr<EventHandler> pEventHandler ) 
		: BackendPlot( conf, pEventHandler ), frequency( frequency ),
		no_bins( no_bins ), rebin( false )
	{
		config.min_y = 0;
		config.max_y = 1.2;
		if (!config.fixed_plot_area) {
			config.min_x = 0;
			config.max_x = 1;
			rebin = true;
		}
		reset( config );
		bins_y = utils::calculate_bins( config.min_x, config.max_x, no_bins, data );
	}

	double BackendHistogram::bin_width() {
		return (max()-min())/no_bins;
	}

	double BackendHistogram::min() {
		if (config.fixed_plot_area)
			return config.min_x;
		else if (data.size() == 0) {
			return 0;
		}	else if (data_min<data_max) {
			double x = 0.1; // max-x*bin_width = data_max
			return (data_min*x+data_max*x-data_min*no_bins)/(2*x-no_bins);
		} else
			return data_min - 0.5;
	}

	double BackendHistogram::max() {
		if (config.fixed_plot_area)
			return config.max_x;
		else if (data.size() == 0) {
			return 1;
		} else if (data_min<data_max) {
			double x = 0.1; // max-x*bin_width = data_max
			return (data_min*x+data_max*x-data_max*no_bins)/(2*x-no_bins);
		} else
			return data_min + 0.5;
	}

	void BackendHistogram::add_data( double new_data ) {
		data.push_back( new_data );
		if (data.size() == 1) {
			data_min = data[0];
			data_max = data_min;
			rebin = true;
		} else if (new_data<data_min) {
			data_min = new_data;
			rebin = true;
		} else if (new_data>data_max) {
			data_max = new_data;
			rebin = true;
		} 
		
		if (config.fixed_plot_area)
			rebin = false;
		if (!rebin && new_data>=min() && new_data<max()) {
			size_t id = utils::bin_id(min(), bin_width(), new_data);
			++bins_y[id];
			if (!frequency && bins_y[id]>config.max_y)
				config.max_y = bins_y[id]*1.2;
		}
	}

	void BackendHistogram::rebin_data() {
		bins_y = utils::calculate_bins( min(), max(), no_bins, data );

		config.max_y = 1.2;
		if (!frequency) {
			for (size_t i=0; i<no_bins; ++i) {
				if (bins_y[i] > config.max_y)
					config.max_y = 1.2*bins_y[i];
			}
		}
		rebin = false;
	}

	void BackendHistogram::optimize_bounds( double proportion ) {
		//Start fresh
		config.fixed_plot_area = false;
		rebin_data();
		config.min_x = min();
		config.max_x = max();
		config.fixed_plot_area = true;

		double width, tmp_min;
		std::vector<size_t> range;
		do {
			range = utils::range_of_bins_covering( proportion, bins_y );
			width = bin_width();
			tmp_min = min();
			config.min_x = tmp_min+range.front()*width;
			config.max_x = tmp_min+(range.back()+1)*width;
			bins_y = utils::calculate_bins( min(), max(), no_bins, data );
		} while (range.size()<2);
		
		if (config.min_x < data_min) {
			double x = 0.1; // max-x*bin_width = data_max
			config.min_x = (data_min*x+config.max_x*x-data_min*no_bins)/(2*x-no_bins);
			bins_y = utils::calculate_bins( min(), max(), no_bins, data );
		}
		if (config.max_x > data_max ) {
			double x = 0.1; // max-x*bin_width = data_max
			config.max_x = (config.min_x*x+data_max*x-data_max*no_bins)/(2*x-no_bins);
			bins_y = utils::calculate_bins( min(), max(), no_bins, data );
		}
		config.max_y = 1.2;
		if (!frequency) {
			for (size_t i=0; i<no_bins; ++i) {
				if (bins_y[i] > config.max_y)
					config.max_y = 1.2*bins_y[i];
			}
		}
	}

	void BackendHistogram::plot() {
		double width = bin_width();
		if (rebin) {
			rebin_data();
		}
		if (!config.fixed_plot_area) {
			config.min_x = min() - 0.5*width;
			config.max_x = max() + 0.5*width;
		}
		bool before = pause_display;
		pause_display = true; // Don't draw while updating the screen
		reset( config );
		for (size_t i=0; i<no_bins; ++i) {
			double height = bins_y[i];
			if (frequency && data.size()>0)
				height/=data.size();
			line_add( min()+i*width, 0, -1, Color::black() );
			line_add( min()+i*width, height, -1, Color::black() );
			line_add( min()+(i+1)*width, height, -1, Color::black() );
			line_add( min()+(i+1)*width, 0, -1, Color::black() );
		}
		pause_display = before;
		display();
	}

	/*
	 * Histogram3D
	 */
	BackendHistogram3D::BackendHistogram3D( PlotConfig cfg, 
			boost::shared_ptr<EventHandler> pEventHandler, 
			size_t no_bins_x, size_t no_bins_y ) : 
		BackendPlot( cfg, pEventHandler ),
		data( std::vector<delaunay::Vertex>() ), 
		no_bins_x( no_bins_x ), no_bins_y( no_bins_y ),
		bins_xy( std::vector<size_t>( no_bins_x*no_bins_y ) )
 		{
		}
	
	size_t BackendHistogram3D::xytoindex( size_t x, size_t y ) {
		return x*no_bins_y + y;
	}

	std::vector<size_t> BackendHistogram3D::indextoxy( size_t index ) {
		std::vector<size_t> xy(2);
		xy[1] = index%no_bins_y;
		xy[0] = (index-xy[1])/no_bins_y;
		return xy;
	}

	double BackendHistogram3D::bin_width_x() {
		return (max_x()-min_x())/no_bins_x;
	}

	double BackendHistogram3D::bin_width_y() {
		return (max_y()-min_y())/no_bins_y;
	}

	double BackendHistogram3D::min_x() {
		if (config.fixed_plot_area)
			return config.min_x;
		else if (data.size() == 0) {
			return 0;
		}	else if (data_min_x<data_max_x) {
			double x = 0.1; // max-x*bin_width = data_max
			return (data_min_x*x+data_max_x*x-data_min_x*no_bins_x)/(2*x-no_bins_x);
		} else
			return data_min_x - 0.5;
	}

	double BackendHistogram3D::min_y() {
		if (config.fixed_plot_area)
			return config.min_y;
		else if (data.size() == 0) {
			return 0;
		}	else if (data_min_y<data_max_y) {
			double x = 0.1; // max-x*bin_width = data_max
			return (data_min_y*x+data_max_y*x-data_min_y*no_bins_y)/(2*x-no_bins_y);
		} else
			return data_min_y - 0.5;
	}

	double BackendHistogram3D::max_x() {
		if (config.fixed_plot_area)
			return config.max_x;
		else if (data.size() == 0) {
			return 1;
		} else if (data_min_x<data_max_x) {
			double x = 0.1; // max-x*bin_width = data_max
			return (data_min_x*x+data_max_x*x-data_max_x*no_bins_x)/(2*x-no_bins_x);
		} else
			return data_min_x + 0.5;
	}

	double BackendHistogram3D::max_y() {
		if (config.fixed_plot_area)
			return config.max_y;
		else if (data.size() == 0) {
			return 1;
		} else if (data_min_y<data_max_y) {
			double x = 0.1; // max-x*bin_width = data_max
			return (data_min_y*x+data_max_y*x-data_max_y*no_bins_y)/(2*x-no_bins_y);
		} else
			return data_min_y + 0.5;
	}

	void BackendHistogram3D::rebin_data() {
		max_z = 1;
		//bins_xy.clear();
		bins_xy = std::vector<size_t>( no_bins_x*no_bins_y );
		for (size_t i = 0; i<data.size(); ++i) {
			size_t x_index = utils::bin_id( min_x(), bin_width_x(), data[i].x ); 
			size_t y_index = utils::bin_id( min_y(), bin_width_y(), data[i].y ); 
			size_t index = xytoindex( x_index, y_index );
			++bins_xy[ index ];
			if (max_z < bins_xy[index])
				max_z = bins_xy[index];
		}

		rebin = false;
	}

	void BackendHistogram3D::add_data( double x, double y ) {
		delaunay::Vertex v = delaunay::Vertex( x, y );
		data.push_back( v );
		if (data.size() == 1) {
			data_min_x = v.x;
			data_max_x = data_min_x;
			data_min_y = v.y;
			data_max_y = data_min_y;
			rebin = true;
		} else {
			if (v.x<data_min_x) {
				data_min_x = v.x;
				rebin = true;
			} else if (v.x>data_max_x) {
				data_max_x = v.x;
				rebin = true;
			}
			if (v.y<data_min_y) {
				data_min_y = v.y;
				rebin = true;
			} else if (v.y>data_max_y) {
				data_max_y = v.y;
				rebin = true;
			}
		}

		if (config.fixed_plot_area)
			rebin = false;
		if (!rebin && 
				v.x>=min_x() && v.x<max_x() &&
				v.y>=min_y() && v.y<max_y()
			 ) {
			size_t x_index = utils::bin_id( min_x(), bin_width_x(), v.x ); 
			size_t y_index = utils::bin_id( min_y(), bin_width_y(), v.y ); 
			size_t index = xytoindex( x_index, y_index );
			++bins_xy[ index ];
			if (max_z < bins_xy[index])
				max_z = bins_xy[index];
		}
	}

	void BackendHistogram3D::plot() {
		double width_x = bin_width_x();
		double width_y = bin_width_y();
		if (rebin) {
			rebin_data();
		}
		if (!config.fixed_plot_area) {
			config.min_x = min_x() - 0.5*width_x;
			config.min_y = min_y() - 0.5*width_y;
			config.max_x = max_x() + 0.5*width_x;
			config.max_y = max_y() + 0.5*width_y;
		}
		bool before = pause_display;
		pause_display = true; // Don't draw while updating the screen
		reset( config );
		for (size_t x = 0; x<no_bins_x; ++x) {
			for (size_t y = 0; y<no_bins_y; ++y) {
				Color color = color_map( 
							((double) bins_xy[xytoindex(x,y)])/max_z );
				rectangle( min_x()+width_x*x, min_y()+width_y*y, 
						width_x, width_y, true, color );
			}
		}
		pause_display = before;
		display();
	}

	void BackendHistogram3D::calculate_height_scaling() {
		if (rebin)
			rebin_data();
		double mean = 0;
		double v = 0;
		size_t dim = 0;

		//We'll ignore values of zero, since they skew the result
		for (size_t x = 0; x<no_bins_x; ++x) {
			for (size_t y = 0; y<no_bins_y; ++y) {
				double fraction = ((double) bins_xy[xytoindex(x,y)])/max_z;
				if (fraction > 0 && fraction <= 1) {
					mean += fraction; 
					v += pow(fraction, 2);
					++dim;
				}
			}
		}
	
		mean /= dim;
		v = v/dim-pow(mean,2);

		color_map.calculate_height_scaling( mean, v );

		plot();
	}

	/*
	 * HeightMap
	 */
	BackendHeightMap::BackendHeightMap( PlotConfig cfg, 
			boost::shared_ptr<EventHandler> pEventHandler ) : 
		BackendPlot( cfg, pEventHandler ),
		zmin( 0 ), zmax( 0 ),
		delaunay( delaunay::Delaunay( config.min_x, 
					config.max_x, config.min_y, config.max_y ) )
	{}


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
		bool before = pause_display;
		pause_display = true; // Don't draw while updating the screen
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
				pPlotArea->transform_to_plot_units();
				pPlotArea->context->move_to( tr.vertices[2]->x, tr.vertices[2]->y );

				for (size_t j=0; j<3; ++j) {
					pPlotArea->context->line_to( tr.vertices[j]->x, tr.vertices[j]->y );
				}
				pPlotArea->context->set_source( pGradient );
				pPlotArea->transform_to_device_units();
				pPlotArea->context->fill_preserve();
				pPlotArea->context->stroke();
				//line_add( delaunay.triangles[i]->corners[0]->vertex->x,
				//delaunay.triangles[i]->corners[0]->vertex->y, i, Color::red() );
			}
		}
		pause_display = before;
		display();
	}

	Color BackendHeightMap::colorMap( float z ) {
		float fraction = (z-zmin)/(zmax-zmin);
		return color_map( fraction );
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
			if (fraction >= 0 && fraction <= 1) {
				mean += fraction; 
				v += pow(fraction, 2);
			}
			else
				--dim;
		}
		mean /= dim;
		v = v/dim-pow(mean,2);

		color_map.calculate_height_scaling( mean, v );

		if (delaunay.vertices.size()>=3)
			plot();
	}
}

