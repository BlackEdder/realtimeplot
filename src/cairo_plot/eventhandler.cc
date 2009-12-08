#include "cairo_plot/eventhandler.h"
#include "cairo_plot/plot.h"

namespace cairo_plot {

    EventHandler::EventHandler( PlotConfig config ) {
        //create a backend plot
        pBPlot = new BackendPlot( config, this );

        queue_size = 0;

        //start processing thread
        pEventProcessingThrd = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &cairo_plot::EventHandler::process_events, this ) ) );
    }

    EventHandler::~EventHandler() {
        pEventProcessingThrd->join();
        delete pBPlot;
    }

    void EventHandler::add_event( Event *pEvent ) {
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

    int EventHandler::get_queue_size() {
        return queue_size;
    }

    void EventHandler::process_events() {
        //Ideally event queue would have a blocking get function
        while (1) {
            if (queue_size==0 && XPending(pBPlot->dpy) == 0) 
                usleep(100000);
            else if ( XPending(pBPlot->dpy) > 0 ) {
                XEvent report;
                XNextEvent( pBPlot->dpy, &report );
                pBPlot->handle_xevent( report ); 
            }
            else if ( queue_size>0 ) {
                Event *pEvent = event_queue.front();
                pEvent->execute( pBPlot );
                m_mutex.lock();
                event_queue.pop_front();
                --queue_size;
                m_mutex.unlock();
                delete pEvent;
            } else {}
        }
    }
}

