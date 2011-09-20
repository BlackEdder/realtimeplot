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

#include "realtimeplot/plot.h"

using namespace realtimeplot;

int main() {
	PlotConfig config = PlotConfig();
	config.min_x = -0.111;
	config.max_x = 1.04;
	config.min_y = -20;
	config.max_y = 30;
	config.aspect_ratio = 0.5;
	config.point_size = 2;
	config.xlabel = "Very Long Long label";
	config.scaling = true;

	Plot pl = Plot( config );

	Color color = Color::black();
	for (int i=0; i<10;++i) {
		pl.point( (i-1)*0.1, (i-4)*5, color );
		sleep(1);
		if (i==2) {
			pl.clear();
			color = Color::red();
			//sleep(1);
		}
		if (i==3)
			color = Color::blue();
		if (i==4)
			color = Color::green();

	}
	pl.config.xlabel = "bla";
	pl.config.max_x = 0.9;
	pl.update_config();
	pl.point( 0.2, -15 );
	pl.save( "plot_points.png" );
	pl.text( 0.5, 0.5, "middle" );
	pl.title( "abc" );

	sleep(5);
	pl.close();
	return 0;
}
