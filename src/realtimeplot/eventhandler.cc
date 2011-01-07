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

#include "realtimeplot/eventhandler.h"
#include "realtimeplot/plot.h"
#include "realtimeplot/backend.h"

namespace realtimeplot {

	EventHandler::EventHandler( PlotConfig config )
		: queue_size( 0 ),
		xevent_queue_size( 0 )
	{
		//This should be done explicitely with an openplotevent
		//plot_is_closed should be true then, but no way for the event to set it to 
		//false
		//pBPlot.reset( new BackendPlot( config, this ) );

		//start processing thread
		pEventProcessingThrd = boost::shared_ptr<boost::thread>( 
				new boost::thread( boost::bind( 
						&realtimeplot::EventHandler::process_events, this ) ) );

		//std::cout << "EventHandler started" << std::endl;
		//std::cout << this << std::endl;
		//std::cout << pEventProcessingThrd << std::endl;
	}

	EventHandler::~EventHandler() {
		pEventProcessingThrd->join();
	}

	void EventHandler::add_event( boost::shared_ptr<Event> pEvent ) {
		//block if many events are present
		if (queue_size>100000) {
			std::cout << "RealTimePlot: blocking because queue is full" << std::endl;
			while (queue_size>1000) {
				usleep(10000);
			}
		}
		m_mutex.lock();
		event_queue.push_back( pEvent );
		++queue_size;
		m_mutex.unlock();
	}

	int EventHandler::get_queue_size() {
		return queue_size + xevent_queue_size;
	}

	void EventHandler::process_events() {
		//Ideally event queue would have a blocking get function
		while (true) { 
			if (pBPlot != NULL && xevent_queue_size == 0 && pBPlot->xSurface ) {
				m_mutex.lock();
				xevent_queue_size = XPending(pBPlot->dpy);
				m_mutex.unlock();
			}
			
			if (queue_size==0 && xevent_queue_size == 0) 
				usleep(100000);
			else if ( xevent_queue_size > 0 && pBPlot->xSurface ) {
				XEvent report;
				XNextEvent( pBPlot->dpy, &report );
				pBPlot->handle_xevent( report );
				m_mutex.lock();
				--xevent_queue_size;
				m_mutex.unlock();
				//This is to work around problems when the last event in the queue
				//doesn't call display. This way the plot will be refreshed anyway
				//Not an ideal solution, because when the last event called display
				//this will do refresh twice instead of the needed one.
				if (xevent_queue_size == 0)
					pBPlot->display();
			}
			else if ( queue_size>0 ) {
				boost::shared_ptr<Event> pEvent = event_queue.front();
				m_mutex.lock();
				event_queue.pop_front();
				--queue_size;
				m_mutex.unlock();
				pEvent->execute( pBPlot );
				//This is to work around problems when the last event in the queue
				//doesn't call display. This way the plot will be refreshed anyway
				//Not an ideal solution, because when the last event called display
				//this will do refresh twice instead of the needed one.
				if (queue_size == 0)
					pBPlot->display();
			} else {}
		}
	}
}

