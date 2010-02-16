/*
  -------------------------------------------------------------------
  
  Copyright (C) 2010, Edwin van Leeuwen
  
  This file is part of CairoPlot.
  
  CairoPlot is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  CairoPlot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with CairoPlot. If not, see <http://www.gnu.org/licenses/>.

  -------------------------------------------------------------------
*/

#include "cairo_plot/plot.h"

using namespace cairo_plot;

int main() {
    PlotConfig config = PlotConfig();
    config.min_x = 0;
    config.max_x = 1000;
    config.min_y = 0;
    config.max_y = 1000;
    config.fixed_plot_area = false;
    config.overlap = 0.99;

    Plot pl = Plot( config );
    int y = 0;
    while (1) {
        if (float(std::rand())/RAND_MAX < 0.01) {
            //usleep(10000);
        }
        pl.point( -y, 50+900*float(std::rand())/RAND_MAX );
        ++y;
    }
    return 0;
}