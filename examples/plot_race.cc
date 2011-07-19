/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, Edwin van Leeuwen

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

/*
 * Testing for crashes (due to threading mostly)
 */
#include "X11/Xlib.h"
#include "realtimeplot/plot.h"

using namespace realtimeplot;

int main() {
	// This has no effect in my experience, but added it to make sure it is not the
	// cause of the problem
	XInitThreads();


	PlotConfig config = PlotConfig();
	config.min_x = -0.111;
	config.max_x = 1.04;
	config.min_y = -20;
	config.max_y = 30;
	//config.margin_x = 200;
	//config.font = "sans 12";
	config.aspect_ratio = 0.5;
	config.point_size = 2;
	config.xlabel = "Very Long Long label";
	Plot pl = Plot();
	pl.reset( config );
	sleep( 1 );
	config.xlabel = "Bla";
	Plot pl2 = Plot();
	// This plot does not get shown unless we sleep first
	//sleep( 1 );
	pl2.reset( config );
	return 0;
}
