/*
 * Simple version of my plotting setup
 * Used to test new ideas, without having to refactor whole thing
 */
#include <string>
#include <iostream>

#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <cairomm/xlib_surface.h>

#include <boost/thread.hpp>


Cairo::RefPtr<Cairo::ImageSurface> surface;
Cairo::RefPtr<Cairo::Context> context;
Cairo::RefPtr<Cairo::ImageSurface> xaxis_surface;
Cairo::RefPtr<Cairo::Context> xaxis_context;


float min_x = -10;
float max_x = 40;
float min_y = -20;
float max_y = 80;

void expose_pixmap() {
    Window rootwin;
    int scr, white, black;
    Cairo::RefPtr<Cairo::XlibSurface> pSurface;
    Cairo::RefPtr<Cairo::Context> pContext;
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
            200, 200, // size
            0, black, // border
            white );
    XStoreName(dpy, win, "hello");
    XMapWindow(dpy, win);
    //nu snap ik het weer. Maar bovenstaande kan mooier


    pSurface = Cairo::XlibSurface::create( dpy, win , DefaultVisual(dpy, 0), 200, 200);
    pContext = Cairo::Context::create( pSurface );

		while(1) {
			//this seems wrong order to me, but it works this way and not in reverse
			pContext->rectangle(20,0, pSurface->get_width(), pSurface->get_height());
			pContext->clip();
			pContext->set_source( xaxis_surface, 0, pSurface->get_height()-28 );
			pContext->paint();
			pContext->reset_clip();
			pContext->rectangle(20,0, pSurface->get_width(), pSurface->get_height()-20);
			pContext->clip(  );
			pContext->set_source( surface, 0, 0 );
			pContext->paint();
			pContext->reset_clip();
			sleep(1);
		}
}

void draw_point(float x, float y) {
    double dx = 1;
    double dy = 1;
    context->device_to_user_distance(dx,dy);
    //not really working due to line_width issues
    context->rectangle(x,y,dx,dy);
}

int main() {
    surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 200, 200);
    context = Cairo::Context::create(surface);
    xaxis_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 200, 28);
    xaxis_context = Cairo::Context::create(xaxis_surface);
		xaxis_context->scale( xaxis_surface->get_width()/(max_x-min_x), 1 );
    xaxis_context->translate( -min_x, 0 );
		xaxis_context->move_to(0,0);
		xaxis_context->line_to(0,8);
		xaxis_context->move_to(min_x,8);
		xaxis_context->line_to(max_x,8);
		xaxis_context->stroke();
 
     
    //set white background
		context->set_source_rgb(1,1,1);
    context->rectangle(0,0,surface->get_width(),surface->get_height());
    context->fill();

    context->set_source_rgb(0.5,0.5,0);
    context->rectangle(0,0,surface->get_width(),surface->get_height());
    context->stroke();

    //transform data range
    context->scale( surface->get_width()/(max_x-min_x),
            -surface->get_height()/(max_y-min_y) );
    context->translate( -min_x, -max_y );
    
    context->set_source_rgb(0,0,0);
    draw_point(0,0);
    draw_point(2,2);
    draw_point(max_x-1, max_y-1);
    draw_point(min_x+1, min_y+1);
    context->stroke();
		context->clip();
    draw_point(-5,-5);
    draw_point(5,5);
    context->stroke();

		//now work on the clipping

    boost::thread thrd( &expose_pixmap );
    thrd.join();
    return 0;
}    

/*
 * Follows are copies of commands, so that I won't have to look them up everytime
 */

  /*Cairo::RefPtr<Cairo::Context> cr1;
    Cairo::RefPtr<Cairo::Context> cr2;
    //Initialize pixmap
    surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 200, 200);
    cr1 = Cairo::Context::create(surface); 
    cr2 = Cairo::Context::create(surface); 

    //thread that keeps showing pixmap
    boost::thread thrd( &expose_pixmap );
    
    

    //draw some stuff
    cr1->save(); // save the state of the context
    cr1->set_source_rgb(0.86, 0.85, 0.47);
    cr1->paint();    // fill image with the color
    cr1->set_source_rgba(0.0, 0.0, 0.0, 0.7);
    // draw a circle in the center of the image
    cr1->arc(surface->get_width() / 2.0, surface->get_height() / 2.0, 
            surface->get_height() / 4.0, 0.0, 2.0 * M_PI);
    cr1->stroke();
    cr1->save();
    // draw a border around the image
    cr1->set_source_rgb(0.5, 0.5, 0.5);
    cr1->set_line_width(20.0);    // make the line wider
    cr1->rectangle(0.0, 0.0, surface->get_width(), surface->get_height());
    cr1->stroke();

    cr2->set_source_rgb(0.1,0.1,0.1);
    cr2->set_line_width(20.0);    // make the line wider
    cr2->move_to(0,0);    // make the line wider
    cr2->line_to(200,200);    // make the line wider
    cr2->stroke();

    
    sleep(1);
    cr1->restore();
    cr1->paint();
    //cr2->paint();*/

