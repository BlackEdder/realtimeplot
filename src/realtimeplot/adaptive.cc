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

#include "realtimeplot/adaptive.h"

namespace realtimeplot {
	/**
	 * Adaptive Plot
	 */
	BackendAdaptivePlot::BackendAdaptivePlot( PlotConfig conf,
			boost::shared_ptr<EventHandler> pEventHandler )
		: BackendPlot( conf, pEventHandler ), max_data_x( -1 ), max_data_y( -1 ),
		min_data_x( 0 ), min_data_y( 0 )
	{}

	void BackendAdaptivePlot::adapt() {
		// Only one data point
		if (min_data_x == max_data_x) {
			config.max_x = max_data_x + 0.5;
			config.min_x = min_data_x - 0.5;
			config.max_y = max_data_y + 0.5;
			config.min_y = min_data_y - 0.5;
		} else {
			double xrange = max_data_x-min_data_x;
			config.max_x = max_data_x + 0.2*xrange;
			config.min_x = min_data_x - 0.2*xrange;
			double yrange = max_data_y-min_data_y;
			config.max_y = max_data_y + 0.2*yrange;
			config.min_y = min_data_y - 0.2*yrange;
		}
	}

	/**
	 * Adaptive EventHandler
	 */
	AdaptiveEventHandler::AdaptiveEventHandler( size_t no_events ) : 
		EventHandler(), max_no_events( no_events ), adaptive( true ) {}

	void AdaptiveEventHandler::reprocess() {
		std::list<boost::shared_ptr<Event> >::iterator it = processed_events.begin();
		++it; // Skip openplotevent
		if ( processing_events || !window_closed ) {
			for (; it != processed_events.end(); ++it) {
				(*it)->execute( pBPlot );
			}
		}
	}

	void AdaptiveEventHandler::process_events() {
		//Ideally event queue would have a blocking get function
		while ( processing_events || !window_closed ) {
			if (priority_queue_size > 0) {
				boost::shared_ptr<Event> pEvent = priority_event_queue.front();
				m_mutex.lock();
				priority_event_queue.pop_front();
				--priority_queue_size;
				m_mutex.unlock();

				if (adaptive && processed_events.size() < max_no_events)
					processed_events.push_back( pEvent );
				else {
					adaptive = false;
					processed_events.clear();
				}

				pEvent->execute( pBPlot );
			} else if ( queue_size>0 ) {
				boost::shared_ptr<Event> pEvent = event_queue.front();
				m_mutex.lock();
				event_queue.pop_front();
				--queue_size;
				m_mutex.unlock();
				if (adaptive && processed_events.size() < max_no_events)
					processed_events.push_back( pEvent );
				else {
					adaptive = false;
					processed_events.clear();
				}

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

};
