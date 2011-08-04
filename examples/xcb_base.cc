#include <iostream>
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
	// These need to be in the proper order
	// All unused values are commented out, but kept in the list, to make changes easier
	values[1] = //XCB_EVENT_MASK_NO_EVENT |
		XCB_EVENT_MASK_KEY_PRESS |
		//XCB_EVENT_MASK_KEY_RELEASE |
		//XCB_EVENT_MASK_BUTTON_PRESS |
		//XCB_EVENT_MASK_BUTTON_RELEASE |
		//XCB_EVENT_MASK_ENTER_WINDOW |
		//XCB_EVENT_MASK_LEAVE_WINDOW |
		//XCB_EVENT_MASK_POINTER_MOTION |
		//XCB_EVENT_MASK_POINTER_MOTION_HINT |
		//XCB_EVENT_MASK_BUTTON_1_MOTION |
		//XCB_EVENT_MASK_BUTTON_2_MOTION |
		//XCB_EVENT_MASK_BUTTON_3_MOTION |
		//XCB_EVENT_MASK_BUTTON_4_MOTION |
		//XCB_EVENT_MASK_BUTTON_5_MOTION |
		//XCB_EVENT_MASK_BUTTON_MOTION |
		//XCB_EVENT_MASK_KEYMAP_STATE |
		XCB_EVENT_MASK_EXPOSURE |
		//XCB_EVENT_MASK_VISIBILITY_CHANGE |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY |
		//XCB_EVENT_MASK_RESIZE_REDIRECT |
		//XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
		//XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
		XCB_EVENT_MASK_FOCUS_CHANGE;
	//| XCB_EVENT_MASK_PROPERTY_CHANGE |
	//XCB_EVENT_MASK_COLOR_MAP_CHANGE |
	//XCB_EVENT_MASK_OWNER_GRAB_BUTTON;


	// This one will be closed
	xcb_connection_t *c;
	xcb_drawable_t win;
	xcb_drawable_t win1;

	xcb_screen_t *screen;
	c = xcb_connect(NULL,NULL);
	screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
	values[0] = screen->white_pixel;

	win = xcb_generate_id(c);
	win1 = xcb_generate_id(c);

	xcb_create_window(c,XCB_COPY_FROM_PARENT,win,screen->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);
	xcb_create_window(c,XCB_COPY_FROM_PARENT,win1,screen->root,0,0,win_width,win_height,0,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);

	xcb_map_window(c,win);
	xcb_map_window(c,win1);

	xcb_flush(c);
	sleep(5);
	std::cout << "Unmapping" << std::endl;
	xcb_unmap_window( c, win1 );
	xcb_destroy_window( c, win1 );
	xcb_flush(c);
	sleep(5);
	xcb_disconnect(c);

	return 0;
}

void *event_thread(void *p)
{
	// Does nothing for now
}
