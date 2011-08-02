#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

#include <xcb/xcb.h>

void *event_thread(void *p);

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


	// This one will be closed
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
	xcb_disconnect(c);
	

	//First window
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
	
	// This one will be closed
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
	xcb_disconnect(c2);

	//Second window
	xcb_connection_t *c3;
	xcb_drawable_t win3;

	xcb_screen_t *screen3;
	c3 = xcb_connect(NULL,NULL);
	screen3 = xcb_setup_roots_iterator(xcb_get_setup(c3)).data;
	values[0] = screen3->white_pixel;

	win3 = xcb_generate_id(c3);

	xcb_create_window(c3,XCB_COPY_FROM_PARENT,win3,screen3->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen3->root_visual,mask,values);

	xcb_map_window(c3,win3);

	xcb_flush(c3);
		
	// This one will be closed
	xcb_connection_t *c4;
	xcb_drawable_t win4;

	xcb_screen_t *screen4;
	c4 = xcb_connect(NULL,NULL);
	screen4 = xcb_setup_roots_iterator(xcb_get_setup(c4)).data;
	values[0] = screen4->white_pixel;

	win4 = xcb_generate_id(c4);

	xcb_create_window(c4,XCB_COPY_FROM_PARENT,win4,screen4->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen4->root_visual,mask,values);

	xcb_map_window(c4,win4);

	xcb_flush(c4);
	xcb_disconnect(c4);
	
	//Third window
	xcb_connection_t *c5;
	xcb_drawable_t win5;

	xcb_screen_t *screen5;
	c5 = xcb_connect(NULL,NULL);
	screen5 = xcb_setup_roots_iterator(xcb_get_setup(c5)).data;
	values[0] = screen5->white_pixel;

	win5 = xcb_generate_id(c5);

	xcb_create_window(c5,XCB_COPY_FROM_PARENT,win5,screen5->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen5->root_visual,mask,values);

	xcb_map_window(c5,win5);

	xcb_flush(c5);

	// This one will be closed
	xcb_connection_t *c6;
	xcb_drawable_t win6;

	xcb_screen_t *screen6;
	c6 = xcb_connect(NULL,NULL);
	screen6 = xcb_setup_roots_iterator(xcb_get_setup(c6)).data;
	values[0] = screen6->white_pixel;

	win6 = xcb_generate_id(c6);

	xcb_create_window(c6,XCB_COPY_FROM_PARENT,win6,screen6->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen6->root_visual,mask,values);

	xcb_map_window(c6,win6);

	xcb_flush(c6);
	xcb_disconnect(c6);
	//Fourth window
	xcb_connection_t *c7;
	xcb_drawable_t win7;

	xcb_screen_t *screen7;
	c7 = xcb_connect(NULL,NULL);
	screen7 = xcb_setup_roots_iterator(xcb_get_setup(c7)).data;
	values[0] = screen7->white_pixel;

	win7 = xcb_generate_id(c7);

	xcb_create_window(c7,XCB_COPY_FROM_PARENT,win7,screen7->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen7->root_visual,mask,values);

	xcb_map_window(c7,win7);

	xcb_flush(c7);
				
	sleep(5);

	return 0;
}

void *event_thread(void *p)
{
	// Does nothing for now
}
