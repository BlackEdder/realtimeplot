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
					int get_queue_size();

				private:
					BackendPlot *pBPlot;
					boost::shared_ptr<boost::thread> pEventProcessingThrd;
					boost::mutex m_mutex;
					std::list<Event*> event_queue;
					int queue_size;

					void process_events();
		};
}
#endif
