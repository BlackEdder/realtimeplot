#include "cairo_plot.h"

namespace cairo_plot {

    PlotSurface::PlotSurface( Cairo::RefPtr<Cairo::ImageSurface> surface ) {
        pSurface = surface;
        origin_x = 10;
        origin_y = 10;
        min_x = 0;
        max_x = 100;
        min_y = 0;
        max_y = 50;
    }

    /*
     * Should draw axes, now drawing just a box
     */

    void PlotSurface::paint( Cairo::RefPtr<Cairo::Context> pContext ) {
   		pContext->set_source_rgb(0, 0, 0);
		pContext->rectangle( origin_x, 0,
                this->get_pixel_width(), 
                this->get_pixel_height() );
		pContext->stroke();
		pContext->set_source_rgb(1, 1, 1);
    }

    int PlotSurface::get_pixel_width() {
        return pSurface->get_width() - origin_x;
    }
    
    int PlotSurface::get_pixel_height() {
        return pSurface->get_height() - origin_y;
    }

    Coord PlotSurface::to_pixel_coord( Coord plot_coords ) {
        Coord pixel_coord = Coord( 
                this->get_pixel_width()*plot_coords.x/(max_x-min_x)+origin_x,
                pSurface->get_height()-(origin_y+this->get_pixel_height()*plot_coords.y/(max_y-min_y) ));
        return pixel_coord;
    }

    Plot::Plot( int x_size, int y_size ) {
        width = x_size;
        height = y_size;

        
        surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
        context = Cairo::Context::create(surface);

        plot_surface = PlotSurface( surface );
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
        Display *dpy;
        Window win;

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
                width, height, // size
                0, black, // border
                white );
        XStoreName(dpy, win, "hello");
        XMapWindow(dpy, win);
        //nu snap ik het weer. Maar bovenstaande kan mooier

        xSurface = Cairo::XlibSurface::create( dpy, win , DefaultVisual(dpy, 0), width, height);
        xContext = Cairo::Context::create( xSurface );

        xContext->set_source( surface, 0, 0 );
        
        while(1) {
            xContext->paint();
            sleep(1);
        }

    }

	void Plot::plot_point( float x, float y ) {
        Coord pixel_coord = plot_surface.to_pixel_coord( Coord(x, y) );
		context->set_source_rgb(0, 0, 0);
		context->rectangle( pixel_coord.x, pixel_coord.y, 1, 1 );
		context->stroke();
		context->set_source_rgb(1, 1, 1);
	}
}
