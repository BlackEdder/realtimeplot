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
#include <iostream>
#include <fstream>
#include <string>

#include <cxxtest/TestSuite.h>

#include "testhelpers.h"

#include "realtimeplot/plotarea.h"

using namespace realtimeplot;

class TestPlotArea : public CxxTest::TestSuite 
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
		}

		void testPlotArea() {
			PlotArea pl_area = PlotArea( conf );
			TS_ASSERT_EQUALS( pl_area.min_x, -25 );
			TS_ASSERT_EQUALS( pl_area.max_x, 25 );
			TS_ASSERT_EQUALS( pl_area.min_y, -25 );
			TS_ASSERT_EQUALS( pl_area.max_y, 25 );


			TS_ASSERT_EQUALS( pl_area.width, 250 );
			TS_ASSERT_EQUALS( pl_area.height, 250 );

			TS_ASSERT( pl_area.surface );
			TS_ASSERT( pl_area.context );
		}

		void testAspectRatio() {
			conf.aspect_ratio = 1.5;

			PlotArea pl_area = PlotArea( conf );
			TS_ASSERT_DELTA( pl_area.width/pl_area.height, 1.5, 0.2 );
			conf.aspect_ratio = 1.0;
		}

		void testTransformToPlotUnits() {
			PlotArea pl_area = PlotArea( conf );
			pl_area.transform_to_plot_units();
			double x = 0;
			double y = 0;
			pl_area.context->user_to_device( x, y );
			TS_ASSERT_EQUALS( x, 125 );
			TS_ASSERT_EQUALS( y, 125 );
			pl_area.transform_to_device_units();
			x=0; y=0;
			pl_area.context->user_to_device( x, y );
			TS_ASSERT_EQUALS( x, 0 );
			TS_ASSERT_EQUALS( y, 0 );
		}

		void testDrawRectangle() {
			PlotArea pl_area = PlotArea( conf );
			pl_area.rectangle( -4, -4, 8, 8, true, Color::red() );
			pl_area.surface->write_to_png( "tests/tmp_plots/test_draw_rectangle_fill.png" );
			TS_ASSERT( compare_files( "tests/tmp_plots/test_draw_rectangle_fill.png", 
				"tests/correct_plots/test_draw_rectangle_fill.png" ) );

			pl_area = PlotArea( conf );
			pl_area.rectangle( -4, -4, 8, 8, false, Color::red() );
			pl_area.surface->write_to_png( "tests/tmp_plots/test_draw_rectangle_unfill.png" );
			TS_ASSERT( compare_files( "tests/tmp_plots/test_draw_rectangle_unfill.png", 
				"tests/correct_plots/test_draw_rectangle_unfill.png" ) );
		}
		void testClear() {
			PlotArea pl_area = PlotArea( conf );
			pl_area.surface->write_to_png( "tests/tmp_plots/test_empty.png" );
			TS_ASSERT( compare_files( "tests/tmp_plots/test_empty.png", 
				"tests/correct_plots/test_empty.png" ) );

			pl_area.rectangle( -4, -4, 8, 8, true, Color::red() );
			pl_area.surface->write_to_png( "tests/tmp_plots/test_draw_rectangle_fill.png" );
			TS_ASSERT( compare_files( "tests/tmp_plots/test_draw_rectangle_fill.png", 
				"tests/correct_plots/test_draw_rectangle_fill.png" ) );

			pl_area.clear();
			pl_area.surface->write_to_png( "tests/tmp_plots/test_empty.png" );
			TS_ASSERT( compare_files( "tests/tmp_plots/test_empty.png", 
				"tests/correct_plots/test_empty.png" ) );
		}
};
	
