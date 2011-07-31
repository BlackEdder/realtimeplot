/* Thomas Coppi (c) 2005, 2006, 2007
 * tcoppi@gmail.com
 *
 * Demo on how to use
 * cairo, xcb, and xrender together in
 * harmony.  Actually, if you want to
 * use it for just that, you can pretty
 * much ignore anything not in main()
 *
 * It also shows how you can throw
 * event processing into a seperate thread
 * for easier animation.
 *
 * NOTE: I never said this is a perfect
 * example, or that this code is optimal.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

#include <xcb/xcb.h>
#include <cairo.h>

#ifndef CAIRO_HAS_XCB_SURFACE
#error Cairo was not compiled with XCB support, no dice
#endif

#include <cairomm/context.h>
#include "cairomm/xcb_surface.h"
//#include <cairo-xcb-xrender.h>

/* I can do ~1000 squares @ 100 FPS on an athlon64 x2 4200+
 * with no slowdown, so adjust up or down from there.
 * Although, with more than about 500 squares, it gets kinda
 * cluttered.
 */

/* 25 FPS is the lowest I recommend */
#define FPS 500
#define NUM_SQUARES 1000

/* Lower, more "slippery" */
#define FRICTION .20
/* Higher, bounce farther when hit */
#define COLLISION .25
/* Higher, more the big block "pulls" */
#define GRAVITY .70
/* Higher, more the big block "repels" */
#define REPULSION .50

//Cairo::RefPtr<Cairo::XlibSurface> xSurface;


/* Function protos */
xcb_visualtype_t *get_root_visual_type(xcb_screen_t *s);

/* Global vars */
xcb_connection_t *c;
xcb_drawable_t win;
cairo_surface_t *win_surf = NULL;
cairo_surface_t *im_surf = NULL;

Cairo::RefPtr<Cairo::XcbSurface> xSurface;
Cairo::RefPtr<Cairo::Context> xContext;

int win_width = 640;
int win_height = 480;
int main(void)
{
	xcb_screen_t *screen;
 	int mask = 0;
	uint32_t values[2];
	pthread_t thr;
	int i;

	c = xcb_connect(NULL,NULL);
	screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

	//populate_square_thingys();

	win = xcb_generate_id(c);

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE           | XCB_EVENT_MASK_BUTTON_PRESS
             	  | XCB_EVENT_MASK_BUTTON_RELEASE      | XCB_EVENT_MASK_POINTER_MOTION
                  | XCB_EVENT_MASK_ENTER_WINDOW        | XCB_EVENT_MASK_LEAVE_WINDOW
                  | XCB_EVENT_MASK_KEY_PRESS           | XCB_EVENT_MASK_KEY_RELEASE
	          | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
	          | XCB_EVENT_MASK_ENTER_WINDOW	   | XCB_EVENT_MASK_LEAVE_WINDOW
	          | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

	xcb_create_window(c,XCB_COPY_FROM_PARENT,win,screen->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);

	//win_surf = cairo_xcb_surface_create(c,win,get_root_visual_type(screen),win_width,win_height);


	xSurface = Cairo::XcbSurface::create( c, win, get_root_visual_type(screen), win_width, win_height);
	//im_surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24,win_width,win_height);

	if(!xSurface)
		fprintf(stderr,"Error creating surface\n");

	xcb_map_window(c,win);

	xcb_flush(c);

	//while(1) {
	for (size_t i=0; i<10; ++i) {
	xContext = Cairo::Context::create( xSurface );
	xContext->rectangle(i*50,i*50,50,50);
	xContext->fill();
	}
	//}

	//cairo_t *cr_win = cairo_create(win_surf);
	//cairo_set_source_rgb(cr_win,0,1,0);
	//cairo_rectangle(cr_win,0,0,win_width,win_height);
	//cairo_fill(cr_win);

	sleep(1);

	return 0;
}

xcb_visualtype_t *get_root_visual_type(xcb_screen_t *s)
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

