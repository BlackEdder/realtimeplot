#include "cairo_plot.h"

namespace cairo_plot {

	Plot::Plot(PlotConfig *config) {
		pConf = config;

		//create the surface to draw on
		create_plot_surface();
		
		//draw initial axes etc
		draw_axes_surface();

		//starts the event_loop
		pEvent_thrd = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &cairo_plot::Plot::event_loop, this ) ) );
	}

	Plot::~Plot() { 
		pEvent_thrd->join(); 
	}

	void Plot::event_loop() {
		Window rootwin, win;
		Display *dpy;
		int scr, white, black;
		Cairo::RefPtr<Cairo::XlibSurface> xSurface;
		Cairo::RefPtr<Cairo::Context> xContext;
		XEvent report;

		if(!(dpy=XOpenDisplay(NULL))) {
			fprintf(stderr, "ERROR: Could not open display\n");
			throw;
		}

		scr = DefaultScreen(dpy);
		rootwin = RootWindow(dpy, scr);
		white = WhitePixel(dpy,scr);
		black = BlackPixel(dpy,scr);
		win = XCreateSimpleWindow(dpy,
				rootwin,
				0, 0,   // origin
				plot_area_width+50, plot_area_height+50, // size
				0, black, // border
				white );
		XStoreName(dpy, win, "hello");
		XMapWindow(dpy, win);
		XSelectInput( dpy, win, StructureNotifyMask | ExposureMask );
		xSurface = Cairo::XlibSurface::create( dpy, win , DefaultVisual(dpy, 0), 
				plot_area_width+50, plot_area_height+50);
		xContext = Cairo::Context::create( xSurface );

		xContext->set_source( plot_surface, 0, 0 );

		xContext->set_source( axes_surface, 0, 0 );
		xContext->paint();
		sleep(1);
		while(1) {
			if (plot_surface_update) {
				xContext->set_source( axes_surface, 0, 0 );
				xContext->paint(); 
				//calculate plot coordinates to use with xContext
				transform_to_plot_units();
				double x = pConf->min_x;
				double y = pConf->max_y;
				plot_context->user_to_device( x, y );
				transform_to_device_units(plot_context);
				xContext->rectangle(50,0,xSurface->get_width(), xSurface->get_height()-50);
				xContext->set_source( plot_surface, -x+50, -y );
				xContext->fill();
			usleep(100000);	
				plot_surface_update = false;
			}	else {
				usleep(100000);	
			}
		}
		XCloseDisplay(dpy);
	}

	void Plot::create_plot_surface() {
		//calculate minimum plot area width/height based on aspect ratio
		double x = 500/sqrt(pConf->aspect_ratio);
		plot_area_width = round( pConf->aspect_ratio*x );
		plot_area_height = round( x );
		//create the surfaces and contexts
		//
		//plot_surface, the shown part of this surface is 500 by 500
		//The rest is for when plotting outside of the area
		plot_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
				5*plot_area_width, 5*plot_area_height );
		plot_context = Cairo::Context::create(plot_surface);
		//give the plot its background color
		set_background_color( plot_context );
		plot_context->rectangle( 0, 0,
				plot_surface->get_width(), plot_surface->get_height() );
		plot_context->fill();

		//set helper variables
		plot_surface_min_x = pConf->min_x-2*(pConf->max_x-pConf->min_x);
		plot_surface_max_x = pConf->max_x+2*(pConf->max_x-pConf->min_x);
		plot_surface_min_y = pConf->min_y-2*(pConf->max_y-pConf->min_y);
		plot_surface_max_y = pConf->max_y+2*(pConf->max_y-pConf->min_y);
	}

	void Plot::transform_to_plot_units( ) {
		transform_to_device_units( plot_context );
		plot_context->translate( 0, plot_surface->get_height() );
		plot_context->scale( plot_surface->get_width()/(plot_surface_max_x-plot_surface_min_x),
				-plot_surface->get_height()/(plot_surface_max_y-plot_surface_min_y) );
		plot_context->translate( -plot_surface_min_x, -plot_surface_min_y );
	}
	
	void Plot::transform_to_plot_units_with_origin( 
			Cairo::RefPtr<Cairo::ImageSurface> pSurface, 
			Cairo::RefPtr<Cairo::Context> pContext, int origin_x, int origin_y ) {
		transform_to_device_units( pContext );
		pContext->translate( origin_x, pSurface->get_height()-origin_y );
		pContext->scale( plot_area_width/((pConf->max_x-pConf->min_x)),
				-plot_area_height/((pConf->max_y-pConf->min_y)) );
		pContext->translate( -pConf->min_x, -pConf->min_y );
	}

	void Plot::transform_to_device_units(Cairo::RefPtr<Cairo::Context> pContext) {
		pContext->set_identity_matrix();
	}

	void Plot::draw_axes_surface() {
		Cairo::RefPtr<Cairo::ToyFontFace> font =
			Cairo::ToyFontFace::create("Bitstream Charter",
					Cairo::FONT_SLANT_ITALIC,
					Cairo::FONT_WEIGHT_BOLD);
		//axes_surface, extra 50 pixels for axes and labels
		axes_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
				plot_area_width+50, plot_area_height+50 );
		axes_context = Cairo::Context::create(axes_surface);
		transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );
		//Plot background for axes (remember this will be used as a mask and plotted points
		//should not appear outside the axes
		set_background_color( axes_context );
		double dx=50;
		double dy=-50;
		axes_context->device_to_user_distance( dx, dy );
		axes_context->move_to( pConf->min_x, pConf->min_y );
		axes_context->line_to( pConf->min_x, pConf->max_y );
		axes_context->line_to( pConf->min_x-dx, pConf->max_y );
		axes_context->line_to( pConf->min_x-dx, pConf->min_y-dy );
		axes_context->line_to( pConf->max_x, pConf->min_y-dy );
		axes_context->line_to( pConf->max_x, pConf->min_y );
		axes_context->move_to( pConf->min_x, pConf->min_y );
		axes_context->fill();

		//Plot the main axes lines
		set_foreground_color( axes_context );
		axes_context->move_to( pConf->min_x, pConf->min_y );
		axes_context->line_to( pConf->min_x, pConf->max_y );
		axes_context->move_to( pConf->min_x, pConf->min_y );
		axes_context->line_to( pConf->max_x, pConf->min_y );

		//Plot the ticks + tick labels
		double dtick_x = (pConf->max_x-pConf->min_x)/pConf->nr_of_ticks;
		double dtick_y = (pConf->max_y-pConf->min_y)/pConf->nr_of_ticks;
		double length_tick_x = pConf->ticks_length;
		double length_tick_y = -pConf->ticks_length;
		axes_context->device_to_user_distance( length_tick_x, length_tick_y );
		for (int i = 0; i < pConf->nr_of_ticks; ++i) {
			axes_context->move_to( pConf->min_x+i*(dtick_x), pConf->min_y );
			axes_context->line_to( pConf->min_x+i*(dtick_x), pConf->min_y+length_tick_y );
			axes_context->move_to( pConf->min_x+i*(dtick_x), pConf->min_y-2*length_tick_y );
			transform_to_device_units( axes_context );
			axes_context->show_text( stringify( pConf->min_x+i*dtick_x ) );
			transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );

			axes_context->move_to( pConf->min_x, pConf->min_y+i*(dtick_y) );
			axes_context->line_to( pConf->min_x+length_tick_x, pConf->min_y+i*(dtick_y) );
			axes_context->move_to( pConf->min_x-3*length_tick_x, pConf->min_y+i*(dtick_y) );
			transform_to_device_units( axes_context );
			axes_context->show_text( stringify( pConf->min_y+i*dtick_y ) );
			transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );

		}
 
		transform_to_device_units( axes_context );
		
		axes_context->move_to( 20, round(0.5*plot_area_height+25) );
		axes_context->show_text( pConf->ylabel );
		axes_context->move_to( round(0.5*plot_area_width+25), plot_area_height+25 );
		axes_context->show_text( pConf->xlabel );
		axes_context->stroke();
	}

	void Plot::set_background_color( Cairo::RefPtr<Cairo::Context> pContext ) {
		pContext->set_source_rgb(1,1,1);
	}

	void Plot::set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext ) {
		pContext->set_source_rgb(0,0,0);
	}

	void Plot::point( float x, float y) {
		if (!within_plot_bounds(x,y)) {
			rolling_update(x, y);
		}
		double dx = 5;
		double dy = 5;
		set_foreground_color( plot_context );
		transform_to_plot_units(); 
		plot_context->device_to_user_distance(dx,dy);
		plot_context->rectangle( x-0.5*dx, y-0.5*dy, dx, dy );
		transform_to_device_units( plot_context );
		plot_context->stroke();
		plot_surface_update = true;
	}

	void Plot::number( float x, float y, float i) {
		if (!within_plot_bounds(x,y)) {
			rolling_update(x, y);
		}
		transform_to_plot_units(); 
		plot_context->move_to( x, y );
		transform_to_device_units( plot_context );
		set_foreground_color( plot_context );
		plot_context->show_text( stringify( i ) );
		plot_surface_update = true;
	}


	void Plot::rolling_update( float x, float y ) {
		std::vector<int> direction;
		direction.push_back( 0 );
		direction.push_back( 0 );
		if (x>pConf->max_x) {
			direction[0] = 1;
		} else if (x<pConf->min_x) {
			direction[0] = -1;
		} else if (y>pConf->max_y) {
			direction[1] = 1;
		} else if (y<pConf->min_y) {
			direction[1] = -1;
		}

		//update min_x etc
		double xrange = pConf->max_x-pConf->min_x;
		pConf->min_x += direction[0]*xrange*(1-pConf->overlap);
		pConf->max_x = pConf->min_x+xrange;
		double yrange = pConf->max_y-pConf->min_y;
		pConf->min_y += direction[1]*yrange*(1-pConf->overlap);
		pConf->max_y = pConf->min_y+yrange;
		
		if (!plot_bounds_within_surface_bounds()) {
			//copy old plot surface
			Cairo::RefPtr<Cairo::ImageSurface> old_plot_surface = plot_surface;
			double old_plot_max_y = plot_surface_max_y;
			double old_plot_min_x = plot_surface_min_x;
			//create new plot surface
			create_plot_surface();
			//copy old plot surface onto new plot surface
			transform_to_plot_units();
			plot_context->user_to_device( old_plot_min_x, old_plot_max_y );
			transform_to_device_units( plot_context );
			plot_context->set_source( old_plot_surface, old_plot_min_x, old_plot_max_y );
			plot_context->reset_clip();
			plot_context->paint();
		}
		//be recursive about it :)
		if (within_plot_bounds( x, y )) {
			draw_axes_surface();
		} else {
			rolling_update( x, y );
		}
	}

	bool Plot::within_plot_bounds( float x, float y ) {
		if ( x < pConf->min_x || x > pConf->max_x ||
				y < pConf->min_y || y > pConf->max_y )
			return false;
		else
			return true;
	}

	bool Plot::plot_bounds_within_surface_bounds( ) {
		if ( pConf->min_x <= plot_surface_min_x || pConf->max_x >= plot_surface_max_x ||
				pConf->min_y <= plot_surface_min_y || pConf->max_y >= plot_surface_max_y )
			return false;
		else
			return true;
	}
}
