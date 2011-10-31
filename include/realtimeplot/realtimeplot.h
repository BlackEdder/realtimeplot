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
#ifndef REALTIMEPLOT_H
#define REALTIMEPLOT_H
#include "realtimeplot/plot.h"

/**
 * \brief RealTimePlot namespace
 *
 * For normal usage one should use the classes defined in realtimeplot.h. 
 * These are the frontend classes that are used for different types of plots,
 * e.g. normal plot, histogram, heightmap or surfaceplot. These classes will
 * then call the backend classes to actually perform the plotting (in a separate
 * thread). See the files in the examples folder for more details.
 */
namespace realtimeplot {
};
#endif
