#ifndef CAIRO_PLOT_EVENTHANDLER_H
#define CAIRO_PLOT_EVENTHANDLER_H

#include <list>

#include <boost/thread.hpp>
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

    //Event that draws a point at x,y
    class PointEvent : public Event {
        public:
            PointEvent( float x, float y );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd;
    };

    class NumberEvent : public Event {
        public:
            NumberEvent( float x, float y, float i );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd, nr;
    };

    //EventHandler
    //Accepts events and starts a thread which handles those events
    class EventHandler {
        public:
            //Only three public methods
            //Constructor
            EventHandler( PlotConfig config );
            ~EventHandler();
            //Add an event to the event queue
            void add_event( Event *pEvent );
        private:
            friend class BackendPlot;

            BackendPlot *pBPlot;
			boost::shared_ptr<boost::thread> pEventProcessingThrd;
            std::list<Event*> event_queue;

            void process_events();
    };
}
#endif
