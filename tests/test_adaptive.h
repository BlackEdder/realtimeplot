/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, 2011 Edwin van Leeuwen

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
#include <cxxtest/TestSuite.h>

#include "testhelpers.h"
#include "turtlemock.h"

#include "realtimeplot/events.h"
#include "realtimeplot/adaptive.h"

using namespace realtimeplot;

class TestAdaptive : public CxxTest::TestSuite 
{
	public:
		PlotConfig conf;

		void setUp() {
			conf = PlotConfig();
			conf.area = 50*50;
			conf.min_x = -5;
			conf.max_x = 5;
			conf.min_y = -5;
			conf.max_y = 5;
			conf.margin_x = 20;
			conf.margin_y = 20;
			conf.display = false;
		}
		/*
		 * Adaptive EventHandler
		 */
		void testPlotOpenClose() {
			boost::shared_ptr<MockEvent> e = boost::shared_ptr<MockEvent>(new MockEvent());
			MOCK_EXPECT( e->execute ).exactly(1);
			boost::shared_ptr<EventHandler> pEventHandler( 
					new AdaptiveEventHandler() );
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new OpenPlotEvent( conf, pEventHandler ) ) );
			pEventHandler->add_event( boost::shared_ptr<Event>( e ) );
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new FinalEvent(pEventHandler, false ) ) );
			pEventHandler->pEventProcessingThrd->join();
			TS_ASSERT_EQUALS( pEventHandler->get_queue_size(), 0 );
 		}


		void testProcessedEvents() {
			boost::shared_ptr<AdaptiveEventHandler> pEventHandler( 
					new AdaptiveEventHandler() );
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new OpenPlotEvent( conf, pEventHandler ) ) );
			pEventHandler->add_event( 
					boost::shared_ptr<Event>( new PointEvent(0, 0) ) ); 
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new FinalEvent(pEventHandler, false ) ) );
			pEventHandler->pEventProcessingThrd->join();
			TS_ASSERT_EQUALS( pEventHandler->get_queue_size(), 0 );
			pEventHandler->proc_mutex.lock();
			TS_ASSERT_EQUALS( pEventHandler->processed_events.size(), 3 );
			pEventHandler->proc_mutex.unlock();
		}

		void testAEHreprocess() {
			boost::shared_ptr<MockEvent> e1 = boost::shared_ptr<MockEvent>(new MockEvent());
			MOCK_EXPECT( e1->execute ).exactly(2);
			boost::shared_ptr<MockEvent> e2 = boost::shared_ptr<MockEvent>(new MockEvent());
			MOCK_EXPECT( e2->execute ).exactly(1);
			boost::shared_ptr<AdaptiveEventHandler> pEventHandler( 
					new AdaptiveEventHandler() );
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new OpenPlotEvent( conf, pEventHandler ) ) );

			pEventHandler->add_event( e1 );
			pEventHandler->event_queue.wait_till_empty();
			usleep( 100 );


			boost::shared_ptr<AdaptiveEventHandler> pAEH =
				boost::static_pointer_cast<AdaptiveEventHandler, 
					EventHandler>( pEventHandler );

			// Reprocess is normally only called from BackendPlot etc, where no locking is
			// needed. We need to lock manually here
			pAEH->reprocess();
			pEventHandler->event_queue.wait_till_empty();


			pEventHandler->add_event( e2 );

			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new FinalEvent(pEventHandler, false ) ) );
			pEventHandler->pEventProcessingThrd->join();
			TS_ASSERT_EQUALS( pEventHandler->get_queue_size(), 0 );
		}

		void testAEHLimit() {
			boost::shared_ptr<MockEvent> e1 = boost::shared_ptr<MockEvent>(new MockEvent());
			MOCK_EXPECT( e1->execute ).exactly(5);
			boost::shared_ptr<MockEvent> e2 = boost::shared_ptr<MockEvent>(new MockEvent());
			MOCK_EXPECT( e2->execute ).exactly(1);

			boost::shared_ptr<EventHandler> pEventHandler( 
				new AdaptiveEventHandler( 3 ) );
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new OpenPlotEvent( conf, pEventHandler ) ) );

			for (size_t i = 0; i < 2; ++i) {
				pEventHandler->add_event( e1 );
			}
			pEventHandler->event_queue.wait_till_empty();

			boost::shared_ptr<AdaptiveEventHandler> pAEH =
				boost::static_pointer_cast<AdaptiveEventHandler, 
					EventHandler>( pEventHandler );

			// Reprocess is normally only called from BackendPlot etc, where no locking is
			// needed. We need to lock manually here
			pAEH->reprocess();
			pEventHandler->event_queue.wait_till_empty();

			pEventHandler->add_event( e1 );

			pEventHandler->event_queue.wait_till_empty();

			pAEH->reprocess();
			pEventHandler->event_queue.wait_till_empty();

			pEventHandler->add_event( e2 );

			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new FinalEvent(pEventHandler, false ) ) );
			pEventHandler->pEventProcessingThrd->join();
		}

		/*
		 * Adaptive plot
		 */
		void testAPEmptyPlot() {
			BackendAdaptivePlot bpl = BackendAdaptivePlot( conf, 
					boost::shared_ptr<EventHandler>() );
			TS_ASSERT_EQUALS( bpl.pAxesArea->width, 80 );
			TS_ASSERT_EQUALS( bpl.pAxesArea->height, 80 );
			TS_ASSERT_EQUALS( bpl.pPlotArea->plot_area_width, 50 );
			TS_ASSERT_EQUALS( bpl.pPlotArea->plot_area_height, 50 );
			bpl.save( fn( "empty_plot" ) );
			TS_ASSERT( check_plot( "empty_plot" ) );
		}

		void testAdapt() {
			BackendAdaptivePlot bpl = BackendAdaptivePlot( conf, 
					boost::shared_ptr<EventHandler>() );
			bpl.max_data_x = bpl.min_data_x;
			bpl.max_data_y = bpl.min_data_y;
			bpl.adapt();
			TS_ASSERT_EQUALS( bpl.config.max_x, 0.5 );
			TS_ASSERT_EQUALS( bpl.config.min_x, -0.5 );
			TS_ASSERT_EQUALS( bpl.config.max_y, 0.5 );
			TS_ASSERT_EQUALS( bpl.config.min_y, -0.5 );
			bpl.max_data_x = 1;
			bpl.max_data_y = 5;
			bpl.adapt();
			TS_ASSERT_DELTA( bpl.config.max_x, 1.2, 0.0001 );
			TS_ASSERT_DELTA( bpl.config.min_x, -0.2, 0.0001 );
			TS_ASSERT_EQUALS( bpl.config.max_y, 6 );
			TS_ASSERT_EQUALS( bpl.config.min_y, -1 );
		}

		void testAdaptCloseValues() {
			BackendAdaptivePlot bpl = BackendAdaptivePlot( conf, 
					boost::shared_ptr<EventHandler>() );
			bpl.max_data_x = bpl.min_data_x;
			bpl.max_data_y = bpl.min_data_y;
			bpl.adapt();
			TS_ASSERT_EQUALS( bpl.config.max_x, 0.5 );
			TS_ASSERT_EQUALS( bpl.config.min_x, -0.5 );
			TS_ASSERT_EQUALS( bpl.config.max_y, 0.5 );
			TS_ASSERT_EQUALS( bpl.config.min_y, -0.5 );
			bpl.max_data_x = 0.1;
			bpl.max_data_y = 0.5;
			bpl.adapt();
			TS_ASSERT_DELTA( bpl.config.max_x, 0.12, 0.0001 );
			TS_ASSERT_DELTA( bpl.config.min_x, -0.02, 0.0001 );
			TS_ASSERT_DELTA( bpl.config.max_y, 0.6, 0.0001 );
			TS_ASSERT_DELTA( bpl.config.min_y, -0.1, 0.0001 );
		}


		void testWithinBounds() {
			BackendAdaptivePlot bpl = BackendAdaptivePlot( conf, 
					boost::shared_ptr<EventHandler>() );
			bpl.within_plot_bounds( 0, 0 );
			TS_ASSERT_EQUALS( bpl.config.max_x, 0.5 );
			TS_ASSERT_EQUALS( bpl.config.min_x, -0.5 );
			TS_ASSERT_EQUALS( bpl.config.max_y, 0.5 );
			TS_ASSERT_EQUALS( bpl.config.min_y, -0.5 );
			bpl.within_plot_bounds( 1, 5 );
			TS_ASSERT_DELTA( bpl.config.max_x, 1.2, 0.0001 );
			TS_ASSERT_DELTA( bpl.config.min_x, -0.2, 0.0001 );
			TS_ASSERT_EQUALS( bpl.config.max_y, 6 );
			TS_ASSERT_EQUALS( bpl.config.min_y, -1 );
		}

		void testWithinBoundsCloseValues() {
			BackendAdaptivePlot bpl = BackendAdaptivePlot( conf, 
					boost::shared_ptr<EventHandler>() );
			bpl.within_plot_bounds( 0, 0 );
			TS_ASSERT_EQUALS( bpl.config.max_x, 0.5 );
			TS_ASSERT_EQUALS( bpl.config.min_x, -0.5 );
			TS_ASSERT_EQUALS( bpl.config.max_y, 0.5 );
			TS_ASSERT_EQUALS( bpl.config.min_y, -0.5 );
			bpl.within_plot_bounds( 0.1, 0.5 );
			TS_ASSERT_DELTA( bpl.config.max_x, 0.12, 0.0001 );
			TS_ASSERT_DELTA( bpl.config.min_x, -0.02, 0.0001 );
			TS_ASSERT_DELTA( bpl.config.max_y, 0.6, 0.0001 );
			TS_ASSERT_DELTA( bpl.config.min_y, -0.1, 0.0001 );
		}


		void testWithinBoundsFixed() {
			conf.fixed_plot_area = true;
			BackendAdaptivePlot bpl = BackendAdaptivePlot( conf, 
					boost::shared_ptr<EventHandler>() );
			bpl.within_plot_bounds( 0, 0 );
			TS_ASSERT_EQUALS( bpl.config.max_x, 5 );
			TS_ASSERT_EQUALS( bpl.config.min_x, -5 );
			TS_ASSERT_EQUALS( bpl.config.max_y, 5 );
			TS_ASSERT_EQUALS( bpl.config.min_y, -5 );
			bpl.within_plot_bounds( 1, 5 );
			TS_ASSERT_EQUALS( bpl.config.max_x, 5 );
			TS_ASSERT_EQUALS( bpl.config.min_x, -5 );
			TS_ASSERT_EQUALS( bpl.config.max_y, 5 );
			TS_ASSERT_EQUALS( bpl.config.min_y, -5 );
		}


		void testOnePoint() {
			boost::shared_ptr<MockAdaptiveEventHandler2> pEH(
					new MockAdaptiveEventHandler2() ); 
			pEH->add_event( boost::shared_ptr<Event>( 
						new AdaptiveOpenPlotEvent( conf, pEH ) ) );
			pEH->add_event( boost::shared_ptr<Event>( 
						new PointEvent( 0, 1 ) ) );
			TS_ASSERT_EQUALS( pEH->pBPlot->config.min_x, -0.5 );
			pEH->add_event( boost::shared_ptr<Event>( 
						new PointEvent( -1, -5 ) ) );
			TS_ASSERT_DELTA( pEH->pBPlot->config.min_x, -1.2, 0.0001 );
			pEH->adaptive = false;
			pEH->pBPlot->config.fixed_plot_area = true; // Stop rolling updates from occuring
			pEH->add_event( boost::shared_ptr<Event>( 
						new PointEvent( -3, -5 ) ) );
			TS_ASSERT_DELTA( pEH->pBPlot->config.min_x, -1.2, 0.0001 );
			pEH->pBPlot->save( fn( "adaptive_plot" ) );
			TS_ASSERT( check_plot( "adaptive_plot" ) );
		}

};
