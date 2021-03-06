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

#include "realtimeplot/actor.hh"
#include "realtimeplot/backend.h"

using namespace realtimeplot;

size_t msg_count;

class DummyActor : public event_based_actor {
	public:
	void init() {
		become(
			on(atom("close")) >> []() {
				unbecome(); 
			},
			others() >> [&] {
				++msg_count;
			}
		);
	}
};

class TestActor : public CxxTest::TestSuite 
{
	public:
		void testCache() {
			actor_ptr dummy_actor = spawn<DummyActor>();
			actor_ptr actor = spawn<CacheActor>( dummy_actor );
			int i = 0;
			for ( ; i<10; ++i ) {
				actor << make_any_tuple( i );
			}
			actor << make_any_tuple( atom("resend") );
			wait_for_exit( actor );
			TS_ASSERT_EQUALS( 20, msg_count );
		}
		

		void wait_for_exit( actor_ptr actor ) {
			actor << make_any_tuple( atom("close") );
			await_all_others_done();
		}

		actor_ptr open_empty_plot() {
			actor_ptr actor = spawn<Actor>();
			actor << make_any_tuple( atom("open"), "plot", false );
			actor << make_any_tuple( atom("area_size"), 50*50 );
			actor << make_any_tuple( atom("xrange"), -5.0 , 5.0 );
			actor << make_any_tuple( atom("yrange"), -5.0 , 5.0 );
			actor << make_any_tuple( atom("margin_x"), 20 );
			actor << make_any_tuple( atom("margin_y"), 20 );
			return actor;
		}

		void testClose() {
			actor_ptr actor = spawn<Actor>();
			wait_for_exit( actor );
			TS_ASSERT( true );
		}

		void testOpen() {
			actor_ptr actor = open_empty_plot();
			actor << make_any_tuple( atom("save"), fn("empty_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "empty_plot" ) );
		}

		void testPoint() {
			actor_ptr actor = open_empty_plot();
			actor << make_any_tuple( atom("point"), 1, 1 );
			actor << make_any_tuple( atom("save"), fn("point_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "point_plot" ) );
		}

		void testColor() {
			actor_ptr actor = open_empty_plot();
			actor << make_any_tuple( atom("color"), Color::red() );
			actor << make_any_tuple( atom("point"), 1, 1 );
			actor << make_any_tuple( atom("save"), fn("red_point_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "red_point_plot" ) );
		}

		void testText() {
			actor_ptr actor = open_empty_plot();
			actor << make_any_tuple( atom("text"), 1.0, 1.0, "test" );
			actor << make_any_tuple( atom("color"), Color::red() );
			actor << make_any_tuple( atom("point"), 1.0, 1.0 );
			actor << make_any_tuple( atom("text"), -3.0, -2.0, "test" );
			actor << make_any_tuple( atom("save"), fn("text_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "text_plot" ) );
		}

		void testPlotLines() { // Need to change id and then add point
			actor_ptr actor = open_empty_plot();
			actor << make_any_tuple( atom("color"), Color::green() );
			actor << make_any_tuple( atom("line_id"), 1 );
			actor << make_any_tuple( atom("line_add"), 1, 1 );
			actor << make_any_tuple( atom("color"), Color::red() );
			actor << make_any_tuple( atom("line_id"), 2 );
			actor << make_any_tuple( atom("line_add"), 1, 1 );
			actor << make_any_tuple( atom("line_id"), 1 );
			actor << make_any_tuple( atom("line_add"), 1, 2 );
			actor << make_any_tuple( atom("line_add"), 2, 2 );
			actor << make_any_tuple( atom("line_id"), 2 );
			actor << make_any_tuple( atom("line_add"), -1, 2 );
			actor << make_any_tuple( atom("line_add"), -2, 2 );
			actor << make_any_tuple( atom("save"), fn("line_plot1") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "line_plot1" ) );
		}

		void testPlotNewLines() { // Need to change id and then add point
			actor_ptr actor = open_empty_plot();
			actor << make_any_tuple( atom("color"), Color::green() );
			actor << make_any_tuple( atom("line_id"), 1 );
			actor << make_any_tuple( atom("line_add"), 1, 1 );
			actor << make_any_tuple( atom("line_add"), 1, 2 );
			actor << make_any_tuple( atom("line_add"), 2, 2 );
			actor << make_any_tuple( atom("color"), Color::red() );
			actor << make_any_tuple( atom("new_line") );
			actor << make_any_tuple( atom("line_add"), 1, 1 );
			actor << make_any_tuple( atom("line_add"), -1, 2 );
			actor << make_any_tuple( atom("line_add"), -2, 2 );
			actor << make_any_tuple( atom("save"), fn("line_plot1") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "line_plot1" ) );
		}

		void testAxes() {
			actor_ptr actor = open_empty_plot();
			actor << make_any_tuple( atom("xrange"), 0.0, 1.0 );
			actor << make_any_tuple( atom("yrange"), -1.0, 0.0 );
			actor << make_any_tuple( atom("point"), 0.5, -0.5 );
			actor << make_any_tuple( atom("xlabel"), "xlabel" );
			actor << make_any_tuple( atom("ylabel"), "ylabel" );
			actor << make_any_tuple( atom("save"), fn("empty2_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "empty2_plot" ) );
		}

		void testRectangle() {
			actor_ptr actor = open_empty_plot();
			actor << make_any_tuple( atom("color"), Color::red() );
			actor << make_any_tuple( atom("rectangle"), -1.0, -1.0, 
					3.0, 2.0, true );
			actor << make_any_tuple( atom("save"), fn("rectangle_fill_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "rectangle_fill_plot" ) );

			actor_ptr actor2 = open_empty_plot();
			actor2 << make_any_tuple( atom("color"), Color::red() );
			actor2 << make_any_tuple( atom("rectangle"), -1.0, -1.0, 
					3.0, 2.0, false );
			actor2 << make_any_tuple( atom("save"), fn("rectangle_unfill_plot") );
			wait_for_exit( actor2 );
			TS_ASSERT( check_plot( "rectangle_unfill_plot" ) );
		}
};
