/*
  -------------------------------------------------------------------
  
  Copyright (C) 2010, Edwin van Leeuwen
  
  This file is part of CairoPlot.
  
  CairoPlot is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  CairoPlot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with CairoPlot. If not, see <http://www.gnu.org/licenses/>.

  -------------------------------------------------------------------
*/

#ifndef CAIRO_PLOT_EVENTHANDLER_H
#define CAIRO_PLOT_EVENTHANDLER_H

#include <list>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

namespace cairo_plot {
    class PlotConfig;
    class BackendPlot;
    class EventHandler;

    //General event class that all events should inherit
    class Event {
        public:
            Event() {}
            virtual void execute( BackendPlot *bPl ) {}
    };

    /**
    \brief Accepts events and starts a thread which handles those events

		\future Implement a lock, so that one can send a number of events and be certain
		they will be processed directly after each other. Important for example if we set
		transparency, plot point, unset transparency. Or want to plot a line in a specific
		order.
		*/
    class EventHandler {
        public:
            //Only three public methods
            //Constructor
					EventHandler( PlotConfig config );
					~EventHandler();
					//If the plot is closed (normally only called from backendplot
					void plot_closed();
					//Add an event to the event queue
					void add_event( Event *pEvent );
					int get_queue_size();

				private:
					BackendPlot *pBPlot;
					boost::shared_ptr<boost::thread> pEventProcessingThrd;
					boost::mutex m_mutex;
					std::list<Event*> event_queue;
					int queue_size;
					int xevent_queue_size;

					void process_events();
		};
}
#endif
