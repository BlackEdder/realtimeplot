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

	EventHandler::EventHandler()
		: 
		processing_events( true ),
		window_closed( false ),
		queue_size( 0 ), 
		priority_queue_size( 0 )
	{
		//start processing thread
		pEventProcessingThrd = boost::shared_ptr<boost::thread>( 
				new boost::thread( boost::bind( 
						&realtimeplot::EventHandler::process_events, this ) ) );
	}

	EventHandler::~EventHandler() {
		pEventProcessingThrd->join();
	}

	void EventHandler::add_event( boost::shared_ptr<Event> pEvent, 
			bool high_priority ) {
		//block if many events are present
		if (queue_size>100000) {
			std::cout << "RealTimePlot: blocking because queue is full" << std::endl;
			while (queue_size>10) {
				usleep(10000);
			}
		}
		if (high_priority) {
			m_mutex.lock();
			priority_event_queue.push_back( pEvent );
			++priority_queue_size;
			m_mutex.unlock();
		} else {
			m_mutex.lock();
			event_queue.push_back( pEvent );
			++queue_size;
			m_mutex.unlock();
		}
	}

	int EventHandler::get_queue_size() {
		return queue_size+priority_queue_size;
	}

	void EventHandler::process_events() {
		//Ideally event queue would have a blocking get function
		while ( processing_events || !window_closed ) {
			if (priority_queue_size > 0) {
				boost::shared_ptr<Event> pEvent = priority_event_queue.front();
				m_mutex.lock();
				priority_event_queue.pop_front();
				--priority_queue_size;
				m_mutex.unlock();
				pEvent->execute( pBPlot );
			} else if ( queue_size>0 ) {
				boost::shared_ptr<Event> pEvent = event_queue.front();
				m_mutex.lock();
				event_queue.pop_front();
				--queue_size;
				m_mutex.unlock();
				pEvent->execute( pBPlot );
			}
			if (queue_size + priority_queue_size == 0) {
				if (pBPlot != NULL) {
					pBPlot->display();
				}
				usleep(100000);
			}

			// After the window has been closed we want to stop
			// as soon as processing_events = false (FinalEvent has
			// been sent)
			/*if (!window_closed && pBPlot != NULL) {
				window_closed = true;
			}*/
		}
		// Make sure we let pBPlot go/freed.
		if (pBPlot != NULL)
			pBPlot.reset();
	}
}

