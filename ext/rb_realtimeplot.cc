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

#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"

#include "realtimeplot/plot.h"

using namespace Rice;
using namespace realtimeplot;

extern "C"
void Init_rb_realtimeplot()
{
    /*Data_Type<PlotConfig> rb_cPlotConfig =
    define_class<PlotConfig>("PlotConfig")
    .define_constructor(Constructor<PlotConfig>());*/
    //.define_method("hello", &Test::hello);
    //
    Data_Type<Plot> rb_cPlot =
        define_class<Plot>("Plot")
        .define_constructor(Constructor<Plot>())
            .define_method("clear", &Plot::clear)
            .define_method("point", &Plot::point);
}
