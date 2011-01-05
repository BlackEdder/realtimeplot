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

#ifndef CAIRO_PLOT_EVENTHANDLER_H
#define CAIRO_PLOT_EVENTHANDLER_H

#include <list>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

namespace realtimeplot {
    class PlotConfig;
    class BackendPlot;
    class EventHandler;

    /**
		 \brief General event class that all events should inherit

		 The BackendPlot object calls the method execute, when it wants to handle
		 the event and it passes a pointer of itself, such that the event can call
		 the BackendPlot methods to edit/add to the plot (like adding a point)

		 \future Why a pointer and not just pass by reference?
		 */
    class Event {
        public:
            Event() {}
            virtual void execute( boost::shared_ptr<BackendPlot> &bPl ) {}
    };

    /**
    \brief Accepts events and starts a thread which handles those events

		In theory the event class should almost never slow down the main program, but
		currently if the queueu gets to big (>1000) add_event will block, so that 
		backendplot has time to clear some events.
		*/
    class EventHandler {
        public:
            //Constructor
					EventHandler( PlotConfig config );
					~EventHandler();
					//If the plot is closed (normally only called from backendplot
					void plot_closed();
					//Add an event to the event queue
					void add_event( boost::shared_ptr<Event> pEvent );
					int get_queue_size();

					bool plot_is_closed;
				private:
					boost::shared_ptr<BackendPlot> pBPlot;
					boost::shared_ptr<boost::thread> pEventProcessingThrd;
					boost::mutex m_mutex;
					std::list<boost::shared_ptr<Event> > event_queue;
					int queue_size;
					int xevent_queue_size;

					void process_events();

		};
}
#endif
