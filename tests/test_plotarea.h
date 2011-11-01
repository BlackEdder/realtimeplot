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

#include "realtimeplot/plotarea.h"

using namespace realtimeplot;

class TestPlotArea : public CxxTest::TestSuite 
{
	public:
		void testPlotArea() {
			PlotConfig conf = PlotConfig();
			conf.area = 100*100;
			conf.min_x = -5;
			conf.max_x = 5;
			conf.min_y = -5;
			conf.max_y = 5;
			PlotArea pl_area = PlotArea( conf );
			TS_ASSERT_EQUALS( pl_area.min_x, -25 );
			TS_ASSERT_EQUALS( pl_area.max_x, 25 );
			TS_ASSERT_EQUALS( pl_area.min_y, -25 );
			TS_ASSERT_EQUALS( pl_area.max_y, 25 );


			TS_ASSERT_EQUALS( pl_area.plot_surface_width, 500 );
			TS_ASSERT_EQUALS( pl_area.plot_surface_height, 500 );
		}

		void testAspectRatio() {
			PlotConfig conf = PlotConfig();
			conf.area = 100*100;
			conf.min_x = -5;
			conf.max_x = 5;
			conf.min_y = -5;
			conf.max_y = 5;
			conf.aspect_ratio = 1.5;

			PlotArea pl_area = PlotArea( conf );
			TS_ASSERT_DELTA( pl_area.plot_surface_width/pl_area.plot_surface_height, 1.5, 0.2 );
		}

};
	
