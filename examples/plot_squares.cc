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
	config.min_x = -5;
	config.max_x = 5;
	config.min_y = -5;
	config.max_y = 5;
	//config.margin_x = 200;
	//config.font = "sans 12";
	config.aspect_ratio = 0.5;
	config.point_size = 2;
    config.fixed_plot_area = true;
	config.xlabel = "Very Long Long label";

	Plot pl = Plot( config );
	for (int i=0; i<10;++i) {
        pl.line_add( -2.5*(i+1), -2.5*(i+1), i );
        pl.line_add( -2.5*(i+1), 2.5*(i+1), i );
        pl.line_add( 2.5*(i+1), 2.5*(i+1), i );
        pl.line_add( 2.5*(i+1), -2.5*(i+1), i );
        pl.line_add( -2.5*(i+1), -2.5*(i+1), i );
        if (i==9) 
            sleep(1);
	}
    pl.save( "plot_squares.png" );
	return 0;
}
