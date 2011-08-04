#include "realtimeplot/xcbhandler.h"
namespace realtimeplot {
	XcbHandler* XcbHandler::pInstance = 0;
	XcbHandler* XcbHandler::Instance() {
		boost::mutex::scoped_lock(i_mutex);
		if (pInstance == 0)
			pInstance = new XcbHandler();
		return pInstance;
	}

	xcb_drawable_t XcbHandler::open_window(size_t width, size_t height) {
 		int mask = 0;
		uint32_t values[2];
		mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		values[1] = XCB_EVENT_MASK_EXPOSURE           | XCB_EVENT_MASK_BUTTON_PRESS
             	  | XCB_EVENT_MASK_BUTTON_RELEASE      | XCB_EVENT_MASK_POINTER_MOTION
                  | XCB_EVENT_MASK_ENTER_WINDOW        | XCB_EVENT_MASK_LEAVE_WINDOW
                  | XCB_EVENT_MASK_KEY_PRESS           | XCB_EVENT_MASK_KEY_RELEASE
	          | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
	          | XCB_EVENT_MASK_ENTER_WINDOW	   | XCB_EVENT_MASK_LEAVE_WINDOW
	          | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

		
		xcb_drawable_t win;
		connection = xcb_connect(NULL,NULL);
		screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
		values[0] = screen->white_pixel;

		win = xcb_generate_id(connection);

		xcb_create_window(connection,XCB_COPY_FROM_PARENT,win,
				screen->root,0,0,width,height,0,
				XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);

		xcb_map_window(connection,win);

		xcb_flush(connection);

		return win;
	}
};
