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
void *event_thread(void *p);

/* Global vars */
int win_width = 640;
int win_height = 480;
int main(void)
{
 	int mask = 0;
	uint32_t values[2];
	pthread_t thr;
	int i;
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[1] = XCB_EVENT_MASK_EXPOSURE           | XCB_EVENT_MASK_BUTTON_PRESS
             	  | XCB_EVENT_MASK_BUTTON_RELEASE      | XCB_EVENT_MASK_POINTER_MOTION
                  | XCB_EVENT_MASK_ENTER_WINDOW        | XCB_EVENT_MASK_LEAVE_WINDOW
                  | XCB_EVENT_MASK_KEY_PRESS           | XCB_EVENT_MASK_KEY_RELEASE
	          | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
	          | XCB_EVENT_MASK_ENTER_WINDOW	   | XCB_EVENT_MASK_LEAVE_WINDOW
	          | XCB_EVENT_MASK_STRUCTURE_NOTIFY;


	xcb_connection_t *c;
	xcb_drawable_t win;

	xcb_screen_t *screen;
	c = xcb_connect(NULL,NULL);
	screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
	values[0] = screen->white_pixel;

	win = xcb_generate_id(c);

	xcb_create_window(c,XCB_COPY_FROM_PARENT,win,screen->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);

	xcb_map_window(c,win);

	xcb_flush(c);
	
	pthread_create(&thr,0,event_thread,0);

	//Second window
	xcb_connection_t *c1;
	xcb_drawable_t win1;

	xcb_screen_t *screen1;
	c1 = xcb_connect(NULL,NULL);
	screen1 = xcb_setup_roots_iterator(xcb_get_setup(c1)).data;
	values[0] = screen1->white_pixel;

	win1 = xcb_generate_id(c1);

	xcb_create_window(c1,XCB_COPY_FROM_PARENT,win1,screen1->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen1->root_visual,mask,values);

	xcb_map_window(c1,win1);

	xcb_flush(c1);
	
	//Third window
	xcb_connection_t *c2;
	xcb_drawable_t win2;

	xcb_screen_t *screen2;
	c2 = xcb_connect(NULL,NULL);
	screen2 = xcb_setup_roots_iterator(xcb_get_setup(c2)).data;
	values[0] = screen2->white_pixel;

	win2 = xcb_generate_id(c2);

	xcb_create_window(c2,XCB_COPY_FROM_PARENT,win2,screen2->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen2->root_visual,mask,values);

	xcb_map_window(c2,win2);

	xcb_flush(c2);
	
		
	sleep(1);

	return 0;
}

void *event_thread(void *p)
{
	/*xcb_generic_event_t *e;
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
				/*default:
					break;
			}
		}

		free(e);
	}*/
}
