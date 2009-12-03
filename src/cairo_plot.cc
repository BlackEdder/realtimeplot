#include "cairo_plot.h"

namespace cairo_plot {

    /*
     * Implementation of PlotSurface class
     */

    PlotSurface::PlotSurface( PlotConfig conf,
            Cairo::RefPtr<Cairo::ImageSurface> surface ) {
        pSurface = surface;
        config = conf;
   }

    /*
     * Should draw axes, now drawing just a box
     */

		void PlotSurface::paint( Cairo::RefPtr<Cairo::Context> pContext ) {
			pContext->set_source_rgb(0, 0, 0);
			pContext->rectangle( config.origin_x, 0,
					this->get_pixel_width(), 
					this->get_pixel_height() );

			for (int i=0; i<config.nr_of_ticks; ++i) {
				Coord coord_x = to_pixel_coord( Coord( i*(config.max_x-config.min_x)/(config.nr_of_ticks-1), 0 ) );
				pContext->move_to( coord_x.x, coord_x.y );
				pContext->line_to( coord_x.x, coord_x.y-config.ticks_length );
				Coord coord_y = to_pixel_coord( Coord( 0, i*(config.max_y-config.min_y)/(config.nr_of_ticks-1) ) );
				pContext->move_to( coord_y.x, coord_y.y );
				pContext->line_to( coord_y.x+config.ticks_length, coord_y.y );
			}
			pContext->stroke();
			pContext->set_source_rgb(1, 1, 1);
		}

		int PlotSurface::get_pixel_width() {
			return pSurface->get_width() - config.origin_x;
		}

		int PlotSurface::get_pixel_height() {
			return pSurface->get_height() - config.origin_y;
		}

    Coord PlotSurface::to_pixel_coord( Coord plot_coords ) {
        Coord pixel_coord = Coord( 
                this->get_pixel_width()*plot_coords.x/(config.max_x-config.min_x)+config.origin_x,
                pSurface->get_height()-(config.origin_y+this->get_pixel_height()*plot_coords.y/(config.max_y-config.min_y) ));
        return pixel_coord;
    }

    /*
     * Implementation of Plot class
     */

    Plot::Plot( PlotConfig conf ) {
        config = conf;

        surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                config.pixel_width, config.pixel_height );
        context = Cairo::Context::create(surface);

        plot_surface = PlotSurface( config, surface );
        plot_surface.paint( context );

        pEvent_thrd = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &cairo_plot::Plot::event_loop, this ) ) );
    }

    Plot::~Plot() { 
        pEvent_thrd->join(); 
    }

    void Plot::event_loop() {
        Window rootwin;
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
                config.pixel_width, config.pixel_height, // size
                0, black, // border
                white );
        XStoreName(dpy, win, "hello");
        XMapWindow(dpy, win);
        XSelectInput( dpy, win, StructureNotifyMask | ExposureMask );
        //nu snap ik het weer. Maar bovenstaande kan mooier

        xSurface = Cairo::XlibSurface::create( dpy, win , DefaultVisual(dpy, 0), 
                config.pixel_width, config.pixel_height);
        xContext = Cairo::Context::create( xSurface );

        xContext->set_source( surface, 0, 0 );

        /*xSurface->set_size( 100, 100 );
          xContext->scale(0.5,0.5);
          xContext->set_source( surface, 0, 0 );*/
				while(1) {
					//At least draw three times a second
					if (XPending(dpy)>0) {
						XNextEvent( dpy, &report ); 
						switch( report.type ) {
							case ConfigureNotify:
								xSurface->set_size( report.xconfigure.width,
										report.xconfigure.height );
								std::cout << report.xconfigure.width << std::endl;
								std::cout << report.xconfigure.height << std::endl;
								xContext = Cairo::Context::create( xSurface );
								xContext->scale( float(xSurface->get_width())/surface->get_width(),
										float(xSurface->get_height())/surface->get_height() );
								xContext->set_source( surface, 0, 0 );
								break;
							case Expose:
								if (report.xexpose.count<1) {
									xContext->paint();
								}
								break;
						}
					} else {
						usleep(300000);
						XClearWindow( dpy, win );
						xContext->paint();
					}
				}
        XCloseDisplay(dpy);
    }


	void Plot::plot_point( float x, float y ) {
        Coord pixel_coord = plot_surface.to_pixel_coord( Coord(x, y) );
		context->set_source_rgb(0, 0, 0);
		context->rectangle( pixel_coord.x, pixel_coord.y, 1, 1 );
		context->stroke();
		context->set_source_rgb(1, 1, 1);
	}
}
