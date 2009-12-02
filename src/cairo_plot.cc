#include "cairo_plot.h"

#include<X11/Xlib.h>
#include<iostream>

namespace cairo_plot {
	Plot::Plot( float max_x, float max_y ) {
		
		//Snap dit niet helemaal
		Display *dpy;
		Window rootwin, win;
		int scr, white, black;

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
                    100, 100, // size
                    0, black, // border
                    white );
		XStoreName(dpy, win, "hello");
		XMapWindow(dpy, win);
		//nu snap ik het weer. Maar bovenstaande kan mooier


		pSurface = Cairo::XlibSurface::create( dpy, win , DefaultVisual(dpy, 0), 100, 100);
		pContext = Cairo::Context::create( pSurface );

		pContext->save(); // save the state of the context
    pContext->set_source_rgb(0.86, 0.85, 0.47);
    pContext->paint();    // fill image with the color
		sleep( 5 );
    pContext->restore();  // color is back to black now
	}

	void Plot::plot_point( float x, float y ) {
		pContext->set_line_width(20.0);
		pContext->set_source_rgb(0.86, 0.85, 0.47);
		pContext->rectangle( 0.0, 0.0, pSurface->get_width(), pSurface->get_height());
		pContext->stroke();
		pContext->set_source_rgb(0, 0, 0);
		pContext->fill();
		sleep( 5 );
	}
}
