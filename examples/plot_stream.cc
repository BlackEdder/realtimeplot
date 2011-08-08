/*
  -------------------------------------------------------------------
  
  Copyright (C) 2011, Juan Carlos Vallejo
  
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

#include <cstdio>

#include "realtimeplot/plot.h"

using namespace realtimeplot;

int
main ()
{
  PlotConfig config = PlotConfig();
  config.min_x =  0;
  config.max_x = 10;
  config.min_y = -1.1;
  config.max_y =  1.10;
  
  Plot pl = Plot (config);
  float x = 0.0;
  double v;
  while (! feof (stdin)) {
    fread (&v, sizeof (double), 1, stdin);
    pl.line_add (x, v);
    x += 1 / 4000.0;
  }
  return 0;
}
