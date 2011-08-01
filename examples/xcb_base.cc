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
void *event_thread(void *p);

/* Global vars */
xcb_connection_t *c;
xcb_drawable_t win;

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


	//xSurface = Cairo::XcbSurface::create( c, win, get_root_visual_type(screen), win_width, win_height);
	//im_surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24,win_width,win_height);

	xcb_map_window(c,win);

	xcb_flush(c);
	
	pthread_create(&thr,0,event_thread,0);

	//xContext = Cairo::Context::create( xSurface );
	//xContext->rectangle(0,0,50,50);
	//xContext->fill();

	//cairo_t *cr_win = cairo_create(win_surf);
	//cairo_set_source_rgb(cr_win,0,1,0);
	//cairo_rectangle(cr_win,0,0,win_width,win_height);
	//cairo_fill(cr_win);

	sleep(1);

	return 0;
}

void *event_thread(void *p)
{
	xcb_generic_event_t *e;
	xcb_button_press_event_t *bpress;
	xcb_motion_notify_event_t *motion;
	xcb_configure_notify_event_t *conf;

	while((e = xcb_wait_for_event(c))) {
		if(e) {
			switch(e->response_type){
/*				case XCB_BUTTON_PRESS:
					cr = cairo_create(win_surf);

					bpress = (xcb_button_press_event_t *)e;
					cairo_rectangle(cr,squares[0].x,squares[0].y,squares[0].width,squares[0].height);

					if(cairo_in_fill(cr,bpress->event_x,bpress->event_y))
						moving = 1;

					cairo_destroy(cr);
					break;
				case XCB_MOTION_NOTIFY:
					motion = (xcb_motion_notify_event_t *)e;

					if(moving) {
						squares[0].x = motion->event_x;
						squares[0].y = motion->event_y;
					}

					break;
				case XCB_BUTTON_RELEASE:
					if(moving)
						moving = !moving;

					break;
				case XCB_EXPOSE:
					break;
				case XCB_CONFIGURE_NOTIFY:
					conf = (xcb_configure_notify_event_t *)e;

					win_width = conf->width;
					win_height = conf->height;

					tmp = cairo_image_surface_create_for_data(cairo_image_surface_get_data(im_surf),
									CAIRO_FORMAT_RGB24,win_width,win_height,0);
					cairo_surface_destroy(im_surf);
					im_surf = tmp;
					tmp = NULL;

					break;*/
				default:
					break;
			}
		}

		free(e);
	}
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

