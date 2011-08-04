#include "realtimeplot/xcbhandler.h"

#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

#include "realtimeplot/events.h"

namespace realtimeplot {
	XcbHandler* XcbHandler::pInstance = 0;
	XcbHandler* XcbHandler::Instance() {
		boost::mutex::scoped_lock(i_mutex);
		if (pInstance == 0) {
			pInstance = new XcbHandler();
		}
		return pInstance;
	}

	xcb_drawable_t XcbHandler::open_window(size_t width, size_t height,
			boost::shared_ptr<EventHandler> pEventHandler ) {
		xcb_drawable_t win;

		win = xcb_generate_id(connection);
		xcb_create_window(connection,XCB_COPY_FROM_PARENT,win,
				screen->root,0,0,width,height,0,
				XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);

		xcb_change_property(connection, XCB_PROP_MODE_REPLACE, win, reply->atom, 4, 32, 1,
				&reply2->atom);

		xcb_map_window(connection,win);

		xcb_flush(connection);

		mapWindow[win] = pEventHandler;
		return win;
	}

	XcbHandler::XcbHandler() {
		connection = xcb_connect(NULL,NULL);
		screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

		mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		values[0] = screen->white_pixel;
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
		

		xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12,
				"WM_PROTOCOLS");
		reply = xcb_intern_atom_reply(connection, cookie, 0);
		xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16,
				"WM_DELETE_WINDOW");
		reply2 = xcb_intern_atom_reply(connection, cookie2, 0);

		pXEventProcessingThrd = boost::shared_ptr<boost::thread>( 
				new boost::thread( boost::bind( 
						&realtimeplot::XcbHandler::process_xevents, this ) ) );

	}

	void XcbHandler::process_xevents() {
		xcb_generic_event_t *event;

		while (event = xcb_wait_for_event (connection)) {
			switch(event->response_type) {
				// Documentation seemed to indicate that this should be XCB_CLIENT_MESSAGE, but seems to be 161
				case 161: 
					xcb_client_message_event_t* msg;
					msg = (xcb_client_message_event_t *)event;
					if(msg->data.data32[0] ==
							reply2->atom)
					{
						mapWindow[msg->window]->add_event( boost::shared_ptr<Event>( 
									new CloseWindowEvent() ) ); 
					}
					break;
				case XCB_UNMAP_WINDOW:
					break;
				case XCB_CONFIGURE_NOTIFY:
					xcb_configure_notify_event_t *conf;
					conf = (xcb_configure_notify_event_t *)event;
					mapWindow[conf->window]->add_event( boost::shared_ptr<Event>( 
								new ScaleXSurfaceEvent( conf->width, conf->height ) ) ); 
					break;
				case XCB_EXPOSE:
					//display();
					break;
				case XCB_KEY_PRESS:
					/* Handle the Key Press event type */
					xcb_key_press_event_t *ev;
					ev = (xcb_key_press_event_t *)event;
					xcb_keysym_t key;
					key = xcb_key_symbols_get_keysym(xcb_key_symbols_alloc(connection),ev->detail,0);
					if (key == XK_space)  {
						mapWindow[conf->window]->add_event( boost::shared_ptr<Event>( 
								new PauseEvent() ) ); 
					}
					else if (key == XK_w)  {
						mapWindow[conf->window]->add_event( boost::shared_ptr<Event>( 
									new SaveEvent( "realtimeplot.png" ) ) );
					}
					else if (key == XK_Left) {
						mapWindow[conf->window]->add_event( boost::shared_ptr<Event>( 
									new MoveEvent( -1, 0 ) ) );
					} else if (key == XK_Right) {
						mapWindow[conf->window]->add_event( boost::shared_ptr<Event>( 
									new MoveEvent( 1, 0 ) ) );
					} else if (key == XK_Up) {
						mapWindow[conf->window]->add_event( boost::shared_ptr<Event>( 
									new MoveEvent( 0, 1 ) ) );
					} else if (key == XK_Down) {
						mapWindow[conf->window]->add_event( boost::shared_ptr<Event>( 
									new MoveEvent( 0, -1 ) ) );
					} else if (key == XK_KP_Add) { 
						mapWindow[conf->window]->add_event( boost::shared_ptr<Event>( 
									new ZoomEvent( 0.95 ) ) );
					} else if (key == XK_KP_Subtract) { 
						mapWindow[conf->window]->add_event( boost::shared_ptr<Event>( 
									new ZoomEvent( 1.05 ) ) );
					}
					break;	
				default:
					break;
			}
		}
		free(event);
	}
};
