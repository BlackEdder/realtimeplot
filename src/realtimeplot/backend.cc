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
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

#include <boost/shared_ptr.hpp>
#include "realtimeplot/backend.h"

#include <boost/math/special_functions/beta.hpp>

namespace realtimeplot {
	/*
	 * BackendPlot
	 */
	boost::mutex BackendPlot::global_mutex;

	BackendPlot::BackendPlot(PlotConfig conf, boost::shared_ptr<EventHandler> pEventHandler) :
		pEventHandler( pEventHandler )
	{
		config = conf;

		//calculate minimum plot area width/height based on aspect ratio
		double x = 500/sqrt(config.aspect_ratio);
		plot_area_width = round( config.aspect_ratio*x );
		plot_area_height = round( x );
		//create the surfaces and contexts
		//
		//plot_surface, the shown part of this surface is 250000 pixels (default 500x500)
		//The rest is for when plotting outside of the area
		plot_surface_width = 5*plot_area_width;
		plot_surface_height = 5*plot_area_height;

		//create the surface to draw on
		plot_surface = create_plot_surface();
		plot_context = Cairo::Context::create(plot_surface);
		set_foreground_color();

		//create_xlib_window
		create_xlib_window();

		//draw initial axes etc
		draw_axes_surface();
		
		time_of_last_update = boost::posix_time::microsec_clock::local_time() - 
			boost::posix_time::microseconds(500000);

		pause_display = false;

		//pEventHandler->processing_events = true;

		display();
	}

	BackendPlot::~BackendPlot() {
	}

	void BackendPlot::display() {
		//Has the display been paused?
		if ( !pause_display && xSurface ) {
			boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
			//Only do this if event queue is empty 
			//or last update was more than a 0.5 seconds ago
			if  (pEventHandler->get_queue_size() < 1 
					|| (( now-time_of_last_update )>( boost::posix_time::microseconds(500000))))  {
				temporary_display_surface = create_temporary_surface();
				//copy the temporary surface onto the xlib surface
				xContext->set_source( temporary_display_surface, 0, 0 );
				xContext->paint();

				time_of_last_update = boost::posix_time::microsec_clock::local_time();
				//only sleep if no more events are coming
				//if (pEventHandler->get_queue_size() < 1) {
				//	usleep(100000);
				//}
			}
		}
	}

	void BackendPlot::clear() {
		//give the plot its background color
		transform_to_device_units( plot_context );
		set_background_color( plot_context );
		plot_context->rectangle( 0, 0,
				plot_surface->get_width(), plot_surface->get_height() );
		plot_context->fill();
		set_foreground_color( plot_context );
		display();
	}

	void BackendPlot::handle_xevent( xcb_generic_event_t *e ) {
		switch(e->response_type) {
			case XCB_UNMAP_SUBWINDOWS:
				std::cout << "Destroying" << std::endl;
				break;
			case XCB_UNMAP_WINDOW:
				std::cout << "Destroying2" << std::endl;
				break;
			case XCB_UNMAP_NOTIFY:
				std::cout << "Destroying3" << std::endl;
				break;
			case XCB_DESTROY_WINDOW:
				std::cout << "Destroying4" << std::endl;
				break;
			case XCB_DESTROY_NOTIFY:
				std::cout << "Destroying5" << std::endl;
				break;
			case XCB_KILL_CLIENT:
				std::cout << "Destroying6" << std::endl;
				break;
			case XCB_DESTROY_SUBWINDOWS:
				std::cout << "Destroying7" << std::endl;
				break;
			case XCB_CONFIGURE_NOTIFY:
				xcb_configure_notify_event_t *conf;
				conf = (xcb_configure_notify_event_t *)e;
				scale_xsurface( conf->width, conf->height );
				break;
			case XCB_EXPOSE:
				display();
				break;
	    case XCB_KEY_PRESS:
      	/* Handle the Key Press event type */
	      xcb_key_press_event_t *ev;
				ev = (xcb_key_press_event_t *)e;
				xcb_keysym_t key;
				key = xcb_key_symbols_get_keysym(xcb_key_symbols_alloc(dpy),ev->detail,0);
				if (key == XK_space)  {
					if (pause_display) {
						pause_display = false;
						display();
					}
					else
						pause_display = true;
				} else if (key == XK_w)  {
					save( "realtimeplot.png", temporary_display_surface );
				} else if (key == XK_Left) {
					move( -1, 0 );
				} else if (key == XK_Right) {
					move( 1, 0 );
				} else if (key == XK_Up) {
					move( 0, 1 );
				} else if (key == XK_Down) {
					move( 0, -1 );
				} else if (key == XK_KP_Add) { 
					double xrange = config.max_x-config.min_x;
					config.min_x+=0.05*xrange;
					config.max_x-=0.05*xrange;
					double yrange = config.max_y-config.min_y;
					config.min_y+=0.05*yrange;
					config.max_y-=0.05*yrange;
					update_config();
				} else if (key == XK_KP_Subtract) { 
					double xrange = config.max_x-config.min_x;
					config.min_x-=0.05*xrange;
					config.max_x+=0.05*xrange;
					double yrange = config.max_y-config.min_y;
					config.min_y-=0.05*yrange;
					config.max_y+=0.05*yrange;
					update_config();
				}
				break;	
		default:
				break;
		}
		/*switch( report.type ) {
			case ConfigureNotify:
				scale_xsurface( report.xconfigure.width, report.xconfigure.height );
				display();
				break;
			case Expose:
				display();
				break;
			case KeyPress:
				if (XLookupKeysym(&report.xkey, 0) == XK_space)  {
					if (pause_display) {
						pause_display = false;
						display();
					}
					else
						pause_display = true;
				} else if (XLookupKeysym(&report.xkey, 0) == XK_w)  {
					save( "realtimeplot.png", temporary_display_surface );
				} else if (XLookupKeysym(&report.xkey, 0) == XK_Left) {
					move( -1, 0 );
				} else if (XLookupKeysym(&report.xkey, 0) == XK_Right) {
					move( 1, 0 );
				} else if (XLookupKeysym(&report.xkey, 0) == XK_Up) {
					move( 0, 1 );
				} else if (XLookupKeysym(&report.xkey, 0) == XK_Down) {
					move( 0, -1 );
				} else if (XLookupKeysym(&report.xkey, 0) == XK_KP_Add) { 
					double xrange = config.max_x-config.min_x;
					config.min_x+=0.05*xrange;
					config.max_x-=0.05*xrange;
					double yrange = config.max_y-config.min_y;
					config.min_y+=0.05*yrange;
					config.max_y-=0.05*yrange;
					update_config();
				} else if (XLookupKeysym(&report.xkey, 0) == XK_KP_Subtract) { 
					double xrange = config.max_x-config.min_x;
					config.min_x-=0.05*xrange;
					config.max_x+=0.05*xrange;
					double yrange = config.max_y-config.min_y;
					config.min_y-=0.05*yrange;
					config.max_y+=0.05*yrange;
					update_config();
				}
				break;
				//close window
			case ClientMessage:
				close_window();
				//XCloseDisplay(dpy);
				break;
		}*/
	}

	void BackendPlot::close_window() {
		if (xSurface) {
			xContext.clear();
			xSurface.clear();
			xcb_disconnect( dpy );
		}
	}


	Cairo::RefPtr<Cairo::ImageSurface> BackendPlot::create_plot_surface() {

		Cairo::RefPtr<Cairo::ImageSurface> surface = 
			Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					plot_surface_width, plot_surface_height );

		//Create context to draw background color
		Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);

		//give the plot its background color
		set_background_color( context );
		context->rectangle( 0, 0,
				surface->get_width(), surface->get_height() );
		context->fill();

		//set helper variables
		double xratio = ((double(plot_surface_width)/plot_area_width)-1)/2.0;
		double yratio = ((double(plot_surface_height)/plot_area_height)-1)/2.0;
		plot_surface_min_x = config.min_x-xratio*(config.max_x-config.min_x);
		plot_surface_max_x = config.max_x+xratio*(config.max_x-config.min_x);
		plot_surface_min_y = config.min_y-yratio*(config.max_y-config.min_y);
		plot_surface_max_y = config.max_y+yratio*(config.max_y-config.min_y);
		return surface;
	}

	void BackendPlot::create_xlib_window() {
		int mask = 0;
		uint32_t values[2];
		pthread_t thr;
		int i;

		dpy = xcb_connect(NULL,NULL);
		screen = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;

		//populate_square_thingys();

		win = xcb_generate_id(dpy);

		mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		values[0] = screen->white_pixel;
		values[1] = XCB_EVENT_MASK_EXPOSURE           | XCB_EVENT_MASK_BUTTON_PRESS
			| XCB_EVENT_MASK_BUTTON_RELEASE      | XCB_EVENT_MASK_POINTER_MOTION
			| XCB_EVENT_MASK_ENTER_WINDOW        | XCB_EVENT_MASK_LEAVE_WINDOW
			| XCB_EVENT_MASK_KEY_PRESS           | XCB_EVENT_MASK_KEY_RELEASE
			| XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
			| XCB_EVENT_MASK_ENTER_WINDOW	   | XCB_EVENT_MASK_LEAVE_WINDOW
			| XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_DESTROY_NOTIFY;

		x_surface_width = plot_area_width+config.margin_y;
		x_surface_height = plot_area_height+config.margin_x;
		xcb_create_window(dpy,XCB_COPY_FROM_PARENT,win,screen->root,0,0,x_surface_width,x_surface_width,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);

		//win_surf = cairo_xcb_surface_create(c,win,get_root_visual_type(screen),win_width,win_height);


		xSurface = Cairo::XcbSurface::create( dpy, win, get_root_visual_type(screen), plot_area_width+config.margin_y, plot_area_height+config.margin_x);
		//im_surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24,win_width,win_height);

		if(!xSurface)
			fprintf(stderr,"Error creating surface\n");

		xcb_map_window(dpy,win);

		xcb_flush(dpy);

		xContext = Cairo::Context::create( xSurface );
	}

	xcb_visualtype_t *BackendPlot::get_root_visual_type(xcb_screen_t *s)
	{
		xcb_visualid_t root_visual;
		xcb_visualtype_t  *visual_type = NULL;
		xcb_depth_iterator_t depth_iter;

		depth_iter = xcb_screen_allowed_depths_iterator(s);

		for(;depth_iter.rem;xcb_depth_next(&depth_iter)) {
			xcb_visualtype_iterator_t visual_iter;

			visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
			for(;visual_iter.rem;xcb_visualtype_next(&visual_iter)) {
				if(s->root_visual == visual_iter.data->visual_id) {
					visual_type = visual_iter.data;
					break;
				}
			}
		}

		return visual_type;
	}

	void BackendPlot::transform_to_plot_units( ) {
		transform_to_plot_units( plot_context );
	}

	void BackendPlot::transform_to_plot_units( Cairo::RefPtr<Cairo::Context> pContext ) {
		transform_to_device_units( pContext );
		pContext->translate( 0, plot_surface_height );
		pContext->scale( plot_surface_width/(plot_surface_max_x-plot_surface_min_x),
				-plot_surface_height/(plot_surface_max_y-plot_surface_min_y) );
		pContext->translate( -plot_surface_min_x, -plot_surface_min_y );
	}

	void BackendPlot::transform_to_plot_units_with_origin( 
			Cairo::RefPtr<Cairo::ImageSurface> pSurface, 
			Cairo::RefPtr<Cairo::Context> pContext, int margin_x, int margin_y ) {
		transform_to_device_units( pContext );
		//pContext->translate( margin_y, pSurface->get_height()-margin_x );
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
					plot_area_width+config.margin_y, plot_area_height+config.margin_x);
			axes_context = Cairo::Context::create(axes_surface);
		}

		int text_width, text_height;
		Glib::RefPtr<Pango::Layout> pango_layout = Pango::Layout::create(axes_context);
		Pango::FontDescription pango_font = Pango::FontDescription(config.font);
		//pango_font.set_weight( Pango::WEIGHT_HEAVY );
		pango_layout->set_font_description( pango_font );

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
				pango_layout->set_text( stringify( xaxis_ticks[i] ) );
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
				pango_layout->set_text( stringify( yaxis_ticks[i] ) );
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
		set_foreground_color( plot_context );
	}
	void BackendPlot::set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext ) {
		pContext->set_source_rgba(0, 0, 0, 1);
	}

	void BackendPlot::set_color( Color color ) {
		plot_context->save();
		plot_context->set_source_rgba( color.r, color.g, color.b, color.a );
	}

	void BackendPlot::restore() {
		plot_context->restore();
	}



	void BackendPlot::point( float x, float y ) {
		if (!within_plot_bounds(x,y)) {
			if (!config.fixed_plot_area)
				rolling_update(x, y);
		}
		double dx = config.point_size;
		double dy = config.point_size;
		transform_to_plot_units(); 
		plot_context->device_to_user_distance(dx,dy);
		plot_context->rectangle( x-0.5*dx, y-0.5*dy, dx, dy );
		transform_to_device_units( plot_context );
		plot_context->fill();
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
		transform_to_plot_units(); 
		plot_context->rectangle( min_x, min_y, width_x, width_y );
		transform_to_device_units( plot_context );
		if (fill) 
			plot_context->fill_preserve();
		plot_context->stroke();
		display();
	}


	void BackendPlot::line_add( float x, float y, int id, Color color ) {
		if (!within_plot_bounds(x,y)) {
			if (!config.fixed_plot_area)
				rolling_update(x, y);
		}

		boost::shared_ptr<LineAttributes> line( new LineAttributes( x, y, id ) );

		//check if line already exists
		bool exists = false;
		std::list<boost::shared_ptr<LineAttributes> >::iterator i;
		for (i=lines.begin(); i != lines.end(); ++i) {
			if ((*i)->id == id) {
				line = (*i);
				exists = true;
				break;
			}
		}

		if (!exists) {
			//Push to the front assuming that new lines are more likely to added to
			//and the check if line already exists will be quicker
			lines.push_front( line );
		} else {
			//plot_surface might have been updated, for example due to rolling_update
			line->context = Cairo::Context::create( plot_surface );

			transform_to_device_units( line->context );
			set_foreground_color( line->context );
			line->context->set_source_rgba( color.r, color.g, color.b, color.a );    
			transform_to_plot_units( line->context );
			line->context->move_to( line->current_x, line->current_y );
			line->context->line_to( x, y );
			transform_to_device_units( line->context );
			line->context->stroke();
			line->current_x = x;
			line->current_y = y;
			display();
		}
	}

	void BackendPlot::save( std::string fn ) {
		Cairo::RefPtr<Cairo::ImageSurface> surface = create_temporary_surface();
		save( fn, surface );
	}

	void BackendPlot::save( std::string fn, 
			Cairo::RefPtr<Cairo::ImageSurface> pSurface ) {
		pSurface->write_to_png( fn );
	}



	void BackendPlot::number( float x, float y, float i) {
		if (!within_plot_bounds(x,y)) {
			if (!config.fixed_plot_area)
				rolling_update(x, y);
		}
		transform_to_plot_units(); 
		plot_context->move_to( x, y );
		transform_to_device_units( plot_context );
		set_foreground_color( plot_context );
		plot_context->show_text( stringify( i ) );

		display();
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
			//copy old plot surface
			double old_plot_max_y = plot_surface_max_y;
			double old_plot_min_x = plot_surface_min_x;

			//create new blank surface
			Cairo::RefPtr<Cairo::ImageSurface> surface = create_plot_surface();
			Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);

			//copy old plot surface onto the blank surface
			transform_to_plot_units( context );
			context->user_to_device( old_plot_min_x, old_plot_max_y );
			transform_to_device_units( context );
			context->set_source( plot_surface, old_plot_min_x, old_plot_max_y );
			context->paint();

			//copy the newly created surface over the old plot
			plot_context->set_source( surface, 0, 0 );
			plot_context->paint();

			set_foreground_color();
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
		if ( config.min_x <= plot_surface_min_x || config.max_x >= plot_surface_max_x ||
				config.min_y <= plot_surface_min_y || config.max_y >= plot_surface_max_y )
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
			while (step/pow(10,power)<=1) {
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
			surface_width = plot_area_width+config.margin_y;
			surface_height = plot_area_height+config.margin_x;
		}
		Cairo::RefPtr<Cairo::ImageSurface> surface = 
			Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					surface_width, surface_height );
		Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( surface );
		
		double x = plot_surface_min_x;
		double y = plot_surface_max_y;
		transform_to_plot_units_with_origin( surface, context,
				config.margin_x, config.margin_y );
		context->user_to_device( x, y );

		transform_to_device_units( context );
		context->translate( x, y );
		context->scale( double(surface_width-config.margin_y)/plot_area_width,
				double(surface_height-config.margin_x)/plot_area_height );

		//copy the plot onto our temporary image surface
		context->set_source( plot_surface, 0, 0 );
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
		if (config.max_x>plot_surface_max_x) {
			config.max_x = plot_surface_max_x;
			config.min_x = config.max_x-xrange;
		} else if (config.min_x<plot_surface_min_x) {
			config.min_x = plot_surface_min_x;
			config.max_x = config.min_x+xrange;
		}
		if (config.max_y>plot_surface_max_y) {
			config.max_y = plot_surface_max_y;
			config.min_y = config.max_y-yrange;
		} else if (config.min_y<plot_surface_min_y) {
			config.min_y = plot_surface_min_y;
			config.max_y = config.min_y+yrange;
		}

		draw_axes_surface();
		display();
	}

	void BackendPlot::update_config() { 
		//Check that it can be done:
		if (config.max_x > plot_surface_max_x)
			config.max_x = plot_surface_max_x;
		if (config.min_x < plot_surface_min_x)
			config.min_x = plot_surface_min_x;
		if (config.max_y > plot_surface_max_y)
			config.max_y = plot_surface_max_y;
		if (config.min_y < plot_surface_min_y)
			config.min_y = plot_surface_min_y;
		//Temporary put here, should only be done when min_x/max_x change
		//recalculate plot_area_width
		transform_to_plot_units( plot_context );
		double width, height;
		width = config.max_x-config.min_x;
		height = config.max_y-config.min_y;
		plot_context->user_to_device_distance( width, height );
		transform_to_device_units( plot_context );
		plot_area_width = round(width);
		plot_area_height = round(-height);
		if (xSurface) {
			width = x_surface_width;
			height = x_surface_height;
			/*width = xSurface->get_width();
			height = xSurface->get_height();*/
			xSurface = Cairo::XcbSurface::create( dpy, win, get_root_visual_type(screen), plot_area_width+config.margin_y, plot_area_height+config.margin_x);
			scale_xsurface( width, height );
		}
		draw_axes_surface();
		display();
	}

	void BackendPlot::scale_xsurface( double width, double height ) {
		if (config.scaling) {
			x_surface_width = width;
			x_surface_height = height;
			xSurface->set_size( width, height );
		}
		xContext = Cairo::Context::create( xSurface );
		draw_axes_surface();
		//xContext->scale( float(xSurface->get_width())/(plot_area_width+config.margin_y),
		//        float(xSurface->get_height())/(plot_area_height+config.margin_x) );
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
				plot_context->move_to( tr.vertices[2]->x, tr.vertices[2]->y );

				for (size_t j=0; j<3; ++j) {
					plot_context->line_to( tr.vertices[j]->x, tr.vertices[j]->y );
				}
				plot_context->set_source( pGradient );
				transform_to_device_units(plot_context);
				plot_context->fill_preserve();
				plot_context->stroke();
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

