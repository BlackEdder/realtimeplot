/*
	 -------------------------------------------------------------------

	 Copyright (C) 2011, Edwin van Leeuwen

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

#include "realtimeplot/plot.h"
#include "realtimeplot/adaptive.h"

using namespace realtimeplot;

class TestPlot : public CxxTest::TestSuite {
	public:
		/*
		 * ColorMap
		 */
		void testColorMap() {
			ColorMap cm = ColorMap();
			TS_ASSERT_EQUALS( cm.alpha, -1 );
			TS_ASSERT_EQUALS( cm.beta, -1 );
			TS_ASSERT_EQUALS( cm.scaling, false );
			Color c1 = cm( 1 );
			Color c2 = cm( 0.5 );
			Color c3 = cm( 0 );
			TS_ASSERT_EQUALS( c1, Color( 0, 0, 0, 1 ) );
			TS_ASSERT_DIFFERS( c1, Color( 1, 0, 0, 1 ) );
			TS_ASSERT_EQUALS( c2, Color( 1, 0, 0, 1 ) );
			TS_ASSERT_EQUALS( c3, Color( 1, 1, 0, 1 ) );
		}

		void testColorMapScaling() {
			ColorMap cm = ColorMap();
			TS_ASSERT_EQUALS( cm.scale( 0.5 ), 0.5 );
			ColorMap cms = ColorMap();
			TS_ASSERT_EQUALS( cm( 0.1 ), cms(0.1) );
			cms.calculate_height_scaling( 0.5, 0.01 );
			TS_ASSERT( cms.scaling );
			TS_ASSERT_LESS_THAN( cms.scale( 0.1 ), cm.scale( 0.1 ) );
			TS_ASSERT_LESS_THAN( cm.scale( 0.9 ), cms.scale( 0.9 ) );
			TS_ASSERT_EQUALS( cms.scale( 0.5 ), cm.scale( 0.5 ) );

			TS_ASSERT_DIFFERS( cm( 0.1 ), cms(0.1) );
		}

		void testResetAdaptive() {
			PlotConfig conf = PlotConfig();
			conf.display = false;
			Plot pl = Plot( conf );
			boost::shared_ptr<AdaptiveEventHandler> pAEH =
				boost::static_pointer_cast<AdaptiveEventHandler, 
					EventHandler>( pl.pEventHandler );

			TS_ASSERT_EQUALS( pAEH->adaptive, true );
			conf.fixed_plot_area = true;
			pl.reset( conf );
			TS_ASSERT_EQUALS( pAEH->adaptive, false );
		}
};

