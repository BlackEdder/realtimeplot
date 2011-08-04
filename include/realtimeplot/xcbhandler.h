/*
  -------------------------------------------------------------------
  
  Copyright (C) 2010, Edwin van Leeuwen
  
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

#ifndef REALTIMEPLOT_XCBHANDLER_H
#define REALTIMEPLOT_XCBHANDLER_H
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <xcb/xcb.h>

#include "realtimeplot/eventhandler.h"


namespace realtimeplot {
	/**
	 *	\brief Singleton class that maintains an x_connection and handles xevents
	 *
	 * This class is used in the following way. Each plot will request a window from
	 * this class. With that request it will also give provide a ptr to itself.
	 *
	 * The handler starts a thread that handles all x events and sends those events to
	 * the plot that belongs with the window. 
	 *
	 * When a plot is closed it will need to tell the xcb handler that, which will need to
	 * remove the window/plot from its list of windows/plots. Or (probably better) xcbhandler
	 * can keep an eye out for unmap/destroy events and remove it itself.
	 */
	class XcbHandler {
		public:
			xcb_connection_t *connection;

			// Should probably be enough to give access to root visual type
			// see get_root_visual_type(pXcbHandler->screen) in backend.cc
			xcb_screen_t *screen;
			static XcbHandler* Instance();

		xcb_drawable_t open_window(size_t width, size_t height,
					boost::shared_ptr<EventHandler> pEventHandler = 
					boost::shared_ptr<EventHandler>() );
		private:
			boost::shared_ptr<boost::thread> pXEventProcessingThrd;

			static boost::mutex i_mutex; 
			XcbHandler();
			~XcbHandler() { pXEventProcessingThrd->join(); }
			static XcbHandler *pInstance;

			void process_xevents();
			std::map<xcb_drawable_t, boost::shared_ptr<EventHandler> > mapWindow;
	};
}
#endif
