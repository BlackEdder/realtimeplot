#include "cairo_plot/eventhandler.h"
#include "cairo_plot/plot.h"

namespace cairo_plot {
    PointEvent::PointEvent( float x, float y ) {
        x_crd = x;
        y_crd = y;
    }

    void PointEvent::execute( BackendPlot *pBPlot ) {
        pBPlot->point( x_crd, y_crd );
    }

    NumberEvent::NumberEvent( float x, float y, float i ) {
        x_crd = x;
        y_crd = y;
        nr = i;
    }

    void NumberEvent::execute( BackendPlot *pBPlot ) {
        pBPlot->number( x_crd, y_crd, nr );
    }


    EventHandler::EventHandler( PlotConfig config ) {
        //create a backend plot
        pBPlot = new BackendPlot( config, this );

        //start processing thread
		pEventProcessingThrd = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &cairo_plot::EventHandler::process_events, this ) ) );
    }

    EventHandler::~EventHandler() {
        pEventProcessingThrd->join();
        delete pBPlot;
    }

    void EventHandler::add_event( Event *pEvent ) {
        //block if many events are present
        if (event_queue.size()>1000) {
            while (event_queue.size()>100) {
                usleep(10000);
            }
        }
        m_mutex.lock();
        event_queue.push_back( pEvent );
        m_mutex.unlock();
    }

    void EventHandler::process_events() {
        //Ideally event queue would have a blocking get function
        while (1) {
            if (event_queue.size()==0 && XPending(pBPlot->dpy) == 0) 
                usleep(100000);
            else if ( XPending(pBPlot->dpy) > 0 ) {
                XEvent report;
                XNextEvent( pBPlot->dpy, &report );
                pBPlot->handle_xevent( report ); 
            }
            else if ( event_queue.size()>0 ) {
                Event *pEvent = event_queue.front();
                pEvent->execute( pBPlot );
                m_mutex.lock();
                event_queue.pop_front();
                m_mutex.unlock();
                delete pEvent;
            } else {}
        }
    }
}

