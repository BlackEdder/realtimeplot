/*
  -------------------------------------------------------------------
  
  Copyright (C) 2012, Edwin van Leeuwen
  
  This file is part of RealTimePlot.
  
  RealTimePlot is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  RealTimePlot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with RealTimePlot. If not, see <http://www.gnu.org/licenses/>.

  -------------------------------------------------------------------
*/

#include "realtimeplot/xcbhandler.h"

#ifndef NO_X
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_event.h>
#include <X11/keysym.h>

#ifndef CAIRO_HAS_XCB_SURFACE
#error Cairo was not compiled with XCB support
#endif

#include <cairomm/xcb_surface.h>
#endif

#include "realtimeplot/events.h"

namespace realtimeplot {
	boost::mutex DisplayHandler::i_mutex;
	DisplayHandler* DisplayHandler::pInstance = NULL;

#ifndef NO_X
	DisplayHandler* XcbHandler::Instance() {
		i_mutex.lock();
		if (pInstance == NULL) {
			pInstance = new XcbHandler();
		}
		i_mutex.unlock();
		return pInstance;
	}

	void XcbHandler::send_event( xcb_drawable_t window, 
			boost::shared_ptr<Event> pEvent ) {
		boost::mutex::scoped_lock( map_mutex );
		mapWindow[window]->add_event( pEvent, true );
	}

	size_t XcbHandler::open_window(size_t width, size_t height,
			boost::shared_ptr<EventHandler> pEventHandler ) {
		xcb_drawable_t win;

		win = xcb_generate_id(connection);
		xcb_create_window(connection,XCB_COPY_FROM_PARENT,win,
				screen->root,0,0,width,height,0,
				XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,mask,values);

		map_mutex.lock();
		mapWindow[win] = pEventHandler;
		size_t id = mapWindowId.size();
		mapWindowId[id] = win;
		map_mutex.unlock();

		xcb_change_property(connection, XCB_PROP_MODE_REPLACE, win, reply->atom, 4, 32, 1,
				&reply2->atom);

		xcb_map_window(connection,win);

		//xcb_flush(connection);

		return id;
	}

	XcbHandler::XcbHandler() : DisplayHandler() 
	{
		connection = xcb_connect(NULL,NULL);
		screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

		visual_type = get_root_visual_type( screen );

		mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		values[0] = screen->white_pixel;
		values[1] = //XCB_EVENT_MASK_NO_EVENT |
			XCB_EVENT_MASK_KEY_PRESS |
			//XCB_EVENT_MASK_KEY_RELEASE |
			XCB_EVENT_MASK_BUTTON_PRESS |
			XCB_EVENT_MASK_BUTTON_RELEASE |
			//XCB_EVENT_MASK_ENTER_WINDOW |
			//XCB_EVENT_MASK_LEAVE_WINDOW |
			XCB_EVENT_MASK_POINTER_MOTION |
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

		bool move_tracking = false;
		size_t last_tracked_x, last_tracked_y;
		while ((event = xcb_wait_for_event (connection))) {
			switch(XCB_EVENT_RESPONSE_TYPE(event)) {
				case XCB_CLIENT_MESSAGE:
					xcb_client_message_event_t* msg;
					msg = (xcb_client_message_event_t *)event;
					if(msg->data.data32[0] ==
							reply2->atom)
					{
						send_event( msg->window, boost::shared_ptr<Event>( 
									new CloseWindowEvent() ) ); 
					}
					break;
				case XCB_UNMAP_WINDOW:
					break;
				case XCB_CONFIGURE_NOTIFY:
					xcb_configure_notify_event_t *conf;
					conf = (xcb_configure_notify_event_t *)event;
					send_event( conf->window, boost::shared_ptr<Event>( 
								new ScaleXSurfaceEvent( conf->width, conf->height ) ) ); 
					break;
				case XCB_EXPOSE:
					//display();
					break;
				case XCB_KEY_PRESS:
					/* Handle the Key Press event type */
					xcb_key_press_event_t *ev;
					ev = (xcb_key_press_event_t *)event;
					xcb_key_symbols_t *p_symbols;
					p_symbols = xcb_key_symbols_alloc(connection);
					xcb_keysym_t key;
					key = xcb_key_symbols_get_keysym(p_symbols,ev->detail,0);
					if (key == XK_space)  {
						send_event( conf->window, boost::shared_ptr<Event>( 
								new PauseEvent() ) ); 
					}
					else if (key == XK_w)  {
						send_event( conf->window, boost::shared_ptr<Event>( 
									new SaveEvent( "realtimeplot.png" ) ) );
					}
					else if (key == XK_Left) {
						send_event( conf->window, boost::shared_ptr<Event>( 
									new MoveEvent( -1, 0 ) ) );
					} else if (key == XK_Right) {
						send_event( conf->window, boost::shared_ptr<Event>( 
									new MoveEvent( 1, 0 ) ) );
					} else if (key == XK_Up) {
						send_event( conf->window, boost::shared_ptr<Event>( 
									new MoveEvent( 0, 1 ) ) );
					} else if (key == XK_Down) {
						send_event( conf->window, boost::shared_ptr<Event>( 
									new MoveEvent( 0, -1 ) ) );
					} else if (key == XK_KP_Add) { 
						send_event( conf->window, boost::shared_ptr<Event>( 
									new ZoomEvent( 0.95 ) ) );
					} else if (key == XK_KP_Subtract) { 
						send_event( conf->window, boost::shared_ptr<Event>( 
									new ZoomEvent( 1/0.95 ) ) );
					}
					xcb_key_symbols_free( p_symbols );
					break;
				case XCB_BUTTON_PRESS:
					xcb_button_press_event_t *bp;
					bp = (xcb_button_press_event_t *)event;
					switch (bp->detail) {
						case 4:
							send_event( conf->window, boost::shared_ptr<Event>( 
										new ZoomEvent( 0.95 ) ) );
							break;
						case 5:
							send_event( conf->window, boost::shared_ptr<Event>( 
										new ZoomEvent( 1/0.95 ) ) );
							break;
						case 3:
							move_tracking = true;
							last_tracked_x = bp->event_x;
							last_tracked_y = bp->event_y;
							break;
						default:
							break;
					}
					break;
				case XCB_BUTTON_RELEASE: 
					xcb_button_release_event_t *br;
					br = (xcb_button_release_event_t *) event;
					switch (br->detail) {
						case 3:
							move_tracking = false;
							break;
						default:
							break;
					}
					break;
				case XCB_MOTION_NOTIFY: // Mouse motion tracking 
					if (move_tracking) {
						xcb_motion_notify_event_t *motion;
						motion = (xcb_motion_notify_event_t *) event;
						send_event( conf->window, boost::shared_ptr<Event>( 
									new MovePixelsEvent( last_tracked_x-motion->event_x, 
										last_tracked_y-motion->event_y ) ) );
						last_tracked_x = motion->event_x;
						last_tracked_y = motion->event_y;
					}
					break;
				default:
					break;
			}
		}
		free(event);
	}
	
	Cairo::RefPtr<Cairo::Surface> XcbHandler::get_cairo_surface( size_t window_id, 
			size_t width, size_t height ) {
		boost::mutex::scoped_lock( map_mutex );
		xcb_drawable_t draw = mapWindowId[window_id];
		return Cairo::XcbSurface::create( connection, draw, 
				visual_type, width, height );
	}


	void XcbHandler::set_title( size_t window_id, std::string title ) {
		xcb_change_property_checked (connection, XCB_PROP_MODE_REPLACE, 
				mapWindowId[window_id],
				XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
				title.length(), title.c_str());
	}

	void XcbHandler::close_window( size_t window_id ) {
		boost::mutex::scoped_lock( map_mutex );
		xcb_drawable_t win = mapWindowId[window_id];
			xcb_unmap_window( connection, win );
		xcb_destroy_window( connection, win );
		xcb_flush(connection);

		//Remove from maps!
		mapWindowId.erase( window_id );
		mapWindow.erase( win );
	}

	xcb_visualtype_t *XcbHandler::get_root_visual_type(xcb_screen_t *s)
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
#endif

	// DummyHandler
	DisplayHandler* DummyHandler::Instance() {
		i_mutex.lock();
		if (pInstance == NULL) {
			pInstance = new DummyHandler();
		}
		i_mutex.unlock();
		return pInstance;
	}

	DummyHandler::DummyHandler() : DisplayHandler(), latest_id( 0 ) {
	}

	size_t DummyHandler::open_window( size_t width, size_t height,
			boost::shared_ptr<EventHandler> pEventHandler ) {
		++latest_id;
		return latest_id-1;
	}

	/**
	 * \brief Return a cairo surface that draws onto a window
	 *
	 * Used by BackendPlot to get a surface to draw to.
	 */
	Cairo::RefPtr<Cairo::Surface> 
		DummyHandler::get_cairo_surface( size_t window_id, 
				size_t width, size_t height ) {
			return Cairo::ImageSurface::create( Cairo::FORMAT_ARGB32, width, height );
		}

	void DummyHandler::set_title( size_t window_id, std::string title ) {}
	void DummyHandler::close_window( size_t window_id ) {}

};
