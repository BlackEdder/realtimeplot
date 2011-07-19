#include <cxxtest/TestSuite.h>

#include <boost/shared_ptr.hpp>

#include "realtimeplot/backend.h"
#include "realtimeplot/eventhandler.h"
#include "realtimeplot/events.h"

using namespace realtimeplot;
namespace realtimeplot {
	class TestBackendWithCairo : public CxxTest::TestSuite 
	{
		public:
			void testOpenAndClosePlot() {
				PlotConfig config = PlotConfig();
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );
				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}

			void testDrawPoint() {
				PlotConfig config = PlotConfig();
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );
				pEventHandler->add_event( boost::shared_ptr<Event>( new PointEvent(5, 20) ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}

			void testResetPlot() {
				/**
				 * Notes on hunting down this problem:
				 *
				 * 1) Tested if it has something to do with the MultiEvent. It hasn't
				 */

				PlotConfig config = PlotConfig();
				PlotConfig conf = PlotConfig();
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );

				std::vector<boost::shared_ptr<Event> > events(2);
				events[0] = boost::shared_ptr<Event>( new CloseWindowEvent() );
				config = conf;
				events[1] = boost::shared_ptr<Event>( new OpenPlotEvent( config, 
							pEventHandler ) );
				pEventHandler->add_event( 
						boost::shared_ptr<Event>( new MultipleEvents( events ) ));

				TS_ASSERT( pEventHandler->pBPlot.use_count() > 0 );

				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}


	};
};
	
