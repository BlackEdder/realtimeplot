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

    EventHandler::EventHandler( PlotConfig config ) {
        //create a backend plot
        pBPlot = new BackendPlot( config, this );

        //start processing thread
		pEventProcessingThrd = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &cairo_plot::EventHandler::process_events, this ) ) );
    }

    EventHandler::~EventHandler() {
        pEventProcessingThrd.join();
        delete pBPL;
    }

    void EventHandler::add_event( Event *pEvent ) {
        //block if many events are present
        if (event_queue.size()>50) {
            while (event_queue.size()>0) {
                usleep(10000);
            }
        }
        event_queue.push_back( pEvent );
    }

    void EventHandler::process_events() {
        //Ideally event queue would have a blocking get function
        while (1) {
            if (event_queue.size()==0) 
                usleep(100000);
            else {
                Event *pEvent = event_queue.front();
                pEvent->execute( pBPlot );
                event_queue.pop_front();
                delete pEvent;
            }
        }
    }
}

