#include "cairo_plot.h"

namespace cairo_plot {

	Plot::Plot(PlotConfig *config) {
		pConf = config;
		//create the surfaces and contexts
		//
		//plot_surface, the shown part of this surface is 500 by 500
		//The rest is for when plotting outside of the area
		plot_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
				2500, 2500 );
		plot_context = Cairo::Context::create(plot_surface);
		//give the plot its background color
		set_background_color( plot_context );
		plot_context->rectangle( 0, 0, plot_surface->get_width(), plot_surface->get_height() );
		plot_context->fill();

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
				550, 550, // size
				0, black, // border
				white );
		XStoreName(dpy, win, "hello");
		XMapWindow(dpy, win);
		XSelectInput( dpy, win, StructureNotifyMask | ExposureMask );
		xSurface = Cairo::XlibSurface::create( dpy, win , DefaultVisual(dpy, 0), 
				550, 550);
		xContext = Cairo::Context::create( xSurface );

		xContext->set_source( plot_surface, 0, 0 );

		while(1) {
			xContext->rectangle(50,0, xSurface->get_width(), xSurface->get_height()-50);
			xContext->clip();
			//This needs to be adaptive using pConf->min_x etc
			//maybe transform plot_surface to plot units and then plot_surface.user_to_device( pConf->min_x, pConf->max_x )
			xContext->set_source( plot_surface, -950, -1000 );
			//xContext->set_source( axes_surface, 0, 0 );
			xContext->paint();
			xContext->reset_clip();
			sleep(1);	
		}
		XCloseDisplay(dpy);
	}

	void Plot::transform_to_plot_units( ) {
		transform_to_plot_units_with_origin( plot_surface, plot_context, 1000, 1000 );
	}
	
	void Plot::transform_to_plot_units_with_origin( 
			Cairo::RefPtr<Cairo::ImageSurface> pSurface, 
			Cairo::RefPtr<Cairo::Context> pContext, int origin_x, int origin_y ) {
		transform_to_device_units( pContext );
		pContext->translate( origin_x, pSurface->get_height()-origin_y );
		pContext->scale( 500/((pConf->max_x-pConf->min_x)),
				-500/((pConf->max_y-pConf->min_y)) );
		pContext->translate( -pConf->min_x, -pConf->min_y );
	}

	void Plot::transform_to_device_units(Cairo::RefPtr<Cairo::Context> pContext) {
		pContext->set_identity_matrix();
	}

	void Plot::draw_axes_surface() {
		//axes_surface, extra 50 pixels for axes and labels
		axes_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
				550, 550 );
		axes_context = Cairo::Context::create(axes_surface);
		set_foreground_color( axes_context );
		transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );
		axes_context->move_to( pConf->min_x, pConf->min_y );
		axes_context->line_to( pConf->min_x, pConf->max_y );
		axes_context->move_to( pConf->min_x, pConf->min_y );
		axes_context->line_to( pConf->max_x, pConf->min_y );
		transform_to_device_units( axes_context );
		axes_context->stroke();
	}

	void Plot::set_background_color( Cairo::RefPtr<Cairo::Context> pContext ) {
		pContext->set_source_rgb(1,1,1);
	}

	void Plot::set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext ) {
		pContext->set_source_rgb(0,0,0);
	}

	void Plot::point( float x, float y) {
		double dx = 1;
		double dy = 1;
		set_foreground_color( plot_context );
		transform_to_plot_units(); 
		plot_context->device_to_user_distance(dx,dy);
		plot_context->rectangle( x-0.5*dx, y-0.5*dy, dx, dy );
		transform_to_device_units( plot_context );
		plot_context->stroke();
	}

}
