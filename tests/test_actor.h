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

using namespace realtimeplot;
class TestActor : public CxxTest::TestSuite 
{
	public:
		void wait_for_exit( actor_ptr actor ) {
			actor << make_any_tuple( atom("close") );
			bool open = true;
			do_receive (
				on( atom("DONE") ) >> [&open]() { open = false; }
			).until( gref(open) == false );
		}

		void testClose() {
			actor_ptr actor = spawn<Actor>();
			wait_for_exit( actor );
			TS_ASSERT( true );
		}

		void testOpen() {
			actor_ptr actor = spawn<Actor>();
			actor << make_any_tuple( atom("open"), "plot" );
			actor << make_any_tuple( atom("save"), fn("empty_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "empty_plot" ) );
		}

		void testPoint() {
			TS_ASSERT( false );
		}

		void testColor() {
			TS_ASSERT( false );
		}

		void testPlotLines() { // Need to change id and then add point
			TS_ASSERT( false );
		}
};
