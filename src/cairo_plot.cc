#include "cairo_plot.h"

#include<X11/Xlib.h>
#include <cairomm/surface.h>
#include <cairomm/xlib_surface.h>
#include <cairomm/context.h>

namespace cairo_plot {
	Plot::Plot( float max_x, float max_y ) {
		
		//Snap dit niet helemaal
		Display *dpy;
		Window rootwin;
		int scr;
		//cairo_surface_t *cs;

		if(!(dpy=XOpenDisplay(NULL))) {
			fprintf(stderr, "ERROR: Could not open display\n");
			throw;
		}


		scr=DefaultScreen(dpy);
		rootwin = RootWindow(dpy, scr);
    int white = WhitePixel(dpy,scr);
    int black = BlackPixel(dpy,scr);
	  Window win = XCreateSimpleWindow(dpy,
                    rootwin,
                    0, 0,   // origin
                    600, 400, // size
                    0, black, // border
                    black );
		XMapWindow(dpy, win);
		//nu snap ik het weer. Maar bovenstaande kan mooier


		Cairo::RefPtr<Cairo::XlibSurface> surface =
			Cairo::XlibSurface::create( dpy, rootwin , DefaultVisual(dpy, 0), 600, 400);
		Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

		cr->save(); // save the state of the context
    cr->set_source_rgb(0.86, 0.85, 0.47);
    cr->paint();    // fill image with the color
    cr->restore();  // color is back to black now
		sleep( 5 );
	}
	void Plot::plot_point( float x, float y ) {}
}
