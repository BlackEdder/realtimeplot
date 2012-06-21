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
			boost::shared_ptr<EventHandler> pEventHandler( 
					new AdaptiveEventHandler() );
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new OpenPlotEvent( conf, pEventHandler ) ) );
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new FinalEvent(pEventHandler, false ) ) );
			pEventHandler->pEventProcessingThrd->join();
			TS_ASSERT_EQUALS( pEventHandler->get_queue_size(), 0 );
			pEventHandler->add_event( 
					boost::shared_ptr<Event>( new PointEvent(0, 0) ) ); 
			TS_ASSERT_EQUALS( pEventHandler->get_queue_size(), 1 );
		}

		void testMockPlotOpenClose() {
			boost::shared_ptr<EventHandler> pEventHandler( 
					new AdaptiveEventHandler() );

			boost::shared_ptr<MockBackendPlot> pl( new MockBackendPlot( conf, 
						pEventHandler ) );

			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new MockOpenPlotEvent(  pl ) ) );
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new MockEvent( 1 ) ) );
			pEventHandler->add_event( boost::shared_ptr<Event>( 
						new FinalEvent(pEventHandler, false ) ) );
			pEventHandler->pEventProcessingThrd->join();
			TS_ASSERT_EQUALS( pEventHandler->get_queue_size(), 0 );

			TS_ASSERT_EQUALS( pl->state, "1" );
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
			TS_ASSERT_EQUALS( pEventHandler->processed_events.size(), 3 );
		}


		/*
		 * Adaptive plot
		 */
		void testAPEmptyPlot() {
			BackendAdaptivePlot bpl = BackendAdaptivePlot( conf, 
					boost::shared_ptr<EventHandler>()  );
			TS_ASSERT_EQUALS( bpl.pAxesArea->width, 80 );
			TS_ASSERT_EQUALS( bpl.pAxesArea->height, 80 );
			TS_ASSERT_EQUALS( bpl.pPlotArea->plot_area_width, 50 );
			TS_ASSERT_EQUALS( bpl.pPlotArea->plot_area_height, 50 );
			bpl.save( fn( "empty_plot" ) );
			TS_ASSERT( check_plot( "empty_plot" ) );
		}

		// Make sure to double check that move events etc update the max_x etc properly
		// Probably ok if we use max x given by plotarea
		//
		// What about scaling?
		//
		// Check for very close together values -> i.e. plot should calculate ranges after
		// second point, even if second point falls within
};
