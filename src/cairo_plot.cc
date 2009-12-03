#include "cairo_plot.h"

namespace cairo_plot {

    Plot::Plot( int x_size, int y_size ) {
        width = x_size;
        height = y_size;
        
        surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
        context = Cairo::Context::create(surface); 

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
		context->set_source_rgb(0, 0, 0);
		context->rectangle( x*width, y*height, 1, 1 );
		context->stroke();
		context->set_source_rgb(1, 1, 1);
	}
}
