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
			TS_ASSERT_DELTA( ((double) pl_area.width)/pl_area.height, 1.5, 0.2 );
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
			pl_area.set_color( Color::red() );
			pl_area.rectangle( -4, -4, 8, 8, true );
			pl_area.surface->write_to_png( fn( "draw_rectangle_fill" ) );
			TS_ASSERT( check_plot( "draw_rectangle_fill" ) );

			pl_area = PlotArea( conf );
			pl_area.set_color( Color::red() );
			pl_area.rectangle( -4, -4, 8, 8, false );
			pl_area.surface->write_to_png( fn( "draw_rectangle_unfill" ) );
			TS_ASSERT( check_plot( "draw_rectangle_unfill" ) );
		}

		void testClear() {
			PlotArea pl_area = PlotArea( conf );
			pl_area.surface->write_to_png( fn( "empty" ) );
			TS_ASSERT( check_plot( "empty" ) );

			pl_area.set_color( Color::red() );
			pl_area.rectangle( -4, -4, 8, 8, true );
			pl_area.surface->write_to_png( fn( "draw_rectangle_fill" ) );
			TS_ASSERT( check_plot( "draw_rectangle_fill" ) );

			pl_area.clear();
			pl_area.surface->write_to_png( fn( "empty" ) );
			TS_ASSERT( check_plot( "empty" ) );
		}

		void testPoint() {
			PlotArea pl_area = PlotArea( conf );
			pl_area.point( 1, 1 );
			pl_area.point( 23, 23 );
			pl_area.point( 30, 30 );
			pl_area.surface->write_to_png( fn( "pa_point" ) );
			TS_ASSERT( check_plot( "pa_point" ) );
		}

		void testLineAdd() {
			PlotArea pl_area = PlotArea( conf  );
			TS_ASSERT_EQUALS( pl_area.lines.size(), 0 );
			pl_area.line_add( 1, 1, 1 );
			TS_ASSERT_EQUALS( pl_area.lines.size(), 1 );
			pl_area.line_add( 1, 1, 2 );
			TS_ASSERT_EQUALS( pl_area.lines.size(), 2 );
			pl_area.line_add( 1, 6, 1 );
			TS_ASSERT_EQUALS( pl_area.lines.size(), 2 );
			pl_area.line_add( -5, 6, 2 );
			TS_ASSERT_EQUALS( pl_area.lines.size(), 2 );
			pl_area.line_add( 6, 6, 1 );
			TS_ASSERT_EQUALS( pl_area.lines.size(), 2 );
			pl_area.line_add( -6, 6, 2 );
			pl_area.surface->write_to_png( fn( "pa_line" ) );
			TS_ASSERT( check_plot( "pa_line" ) );
			// Make sure lines get cleared, i.e. line 1 is not at 2,2 anymore
			pl_area.clear();
			TS_ASSERT_EQUALS( pl_area.lines.size(), 0 );
			pl_area.line_add( 1, 1, 1 );
			TS_ASSERT_EQUALS( pl_area.lines.size(), 1 );
			pl_area.line_add( -1, 6, 1 );
			TS_ASSERT_EQUALS( pl_area.lines.size(), 1 );
			//pl_area.surface->write_to_png( fn( "pa_line2" ) );
			//TS_ASSERT( check_plot( "pa_line2" ) );
		}

		void testReposition() {
			PlotArea pl_area = PlotArea( conf  );
			pl_area.reposition( 5, 5 );
			TS_ASSERT_EQUALS( pl_area.min_x, -20 );
			TS_ASSERT_EQUALS( pl_area.max_x, 30 );
			TS_ASSERT_EQUALS( pl_area.min_y, -20 );
			TS_ASSERT_EQUALS( pl_area.max_y, 30 );
			pl_area.point( 15, -5 );
			pl_area.reposition( 15, -5 );
			TS_ASSERT_EQUALS( pl_area.min_x, -10 );
			TS_ASSERT_EQUALS( pl_area.max_x, 40 );
			TS_ASSERT_EQUALS( pl_area.min_y, -30 );
			TS_ASSERT_EQUALS( pl_area.max_y, 20 );
			pl_area.set_color( Color::red() );
			pl_area.point( 15, -10 );
			pl_area.set_color( Color::green() );
			pl_area.point( 10, -10 );
			pl_area.surface->write_to_png( fn( "pa_reposition" ) );
			TS_ASSERT( check_plot( "pa_reposition" ) );
		}

		void testAxesArea() {
			AxesArea ax_area = AxesArea( conf, 
					50 + conf.margin_y, 50 + conf.margin_x );
			TS_ASSERT_EQUALS( ax_area.width, 50 + conf.margin_y );
			TS_ASSERT_EQUALS( ax_area.height, 50 + conf.margin_x );
		}
};
