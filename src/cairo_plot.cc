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
        std::cout << "debug " << std::endl;

        xContext->set_source( surface, 0, 0 );
        std::cout << "debug 1" << std::endl;
        
        while(1) {
            xContext->paint();
            sleep(1);
        }

    }

	void Plot::plot_point( float x, float y ) {
		context->set_line_width(20.0);
		context->set_source_rgb(0.86, 0.85, 0.47);
		context->rectangle( 0.0, 0.0, surface->get_width(), surface->get_height());
		context->stroke();
		context->set_source_rgb(0, 0, 0);
		context->fill();
	}
}
