#include "eventhandler.h"

namespace cairo_plot {
    PointEvent::Pointevent( float x, float y ) {
        x_crd = x;
        y_crd = y;
    }

    void PointEvent::execute( BackendPlot *pBPl ) {
        pBPl->point( x, y );
    }

    void EventHandler::EventHandler( PlotConfig config ) {
        //create a backend plot
        pBPl = &BackendPlot( config );

        //start processing thread
		pEvent_Processing_thrd = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &cairo_plot::EventHandler::process_events, this ) ) );
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
                event_queue.front.execute( pBPl );
                event_queue.pop_front();
            }
        }
    }
}

