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

namespace realtimeplot {

    EventHandler::EventHandler( PlotConfig config ) {
        //create a backend plot
        pBPlot = new BackendPlot( config, this );

        queue_size = 0;
        xevent_queue_size = 0;

        //start processing thread
        pEventProcessingThrd = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &realtimeplot::EventHandler::process_events, this ) ) );
    }

    EventHandler::~EventHandler() {
        pEventProcessingThrd->join();
        if (pBPlot)
					delete pBPlot;
    }

		void EventHandler::plot_closed() {
			delete pBPlot;
			pBPlot = NULL;
		}

		void EventHandler::add_event( Event *pEvent ) {
			//ignore if no plot present (for example because plot window was closed)
			//->EventHandler shouldn't crash because it isn't plotting anywhere
			if (pBPlot!=NULL) {
				//block if many events are present
				if (queue_size>1000) {
					while (queue_size>100) {
						usleep(10000);
					}
				}
				m_mutex.lock();
				event_queue.push_back( pEvent );
				++queue_size;
				m_mutex.unlock();
			}
		}

        void EventHandler::add_events( std::vector<Event*> events ) {
            for (std::vector<Event*>::iterator it = events.begin(); 
                    it!=events.end(); ++it) {
                add_event(*it);
            }
        }

    int EventHandler::get_queue_size() {
        return queue_size + xevent_queue_size;
    }

    void EventHandler::process_events() {
        //Ideally event queue would have a blocking get function
        while (pBPlot!=NULL) {
            if (queue_size==0 && XPending(pBPlot->dpy) == 0) 
                usleep(100000);
            else if ( XPending(pBPlot->dpy) > 0 ) {
                xevent_queue_size = XPending(pBPlot->dpy) - 1; 
                XEvent report;
                XNextEvent( pBPlot->dpy, &report );
                pBPlot->handle_xevent( report ); 
            }
            else if ( queue_size>0 ) {
                Event *pEvent = event_queue.front();
                m_mutex.lock();
                event_queue.pop_front();
                --queue_size;
                m_mutex.unlock();
                pEvent->execute( pBPlot );
                delete pEvent;
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

