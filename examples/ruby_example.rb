###
#  -------------------------------------------------------------------
#  
#  Copyright (C) 2010, Edwin van Leeuwen
#  
#  This file is part of RealTimePlot.
#  
#  RealTimePlot is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#  
#  RealTimePlot is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with RealTimePlot. If not, see <http://www.gnu.org/licenses/>.
#
#  -------------------------------------------------------------------
#
require "ext/rb_realtimeplot.so"

pl = RealTimePlot::Plot.new
pl.point_0( 5,50 )
pl.point_1( 5,80, RealTimePlot::Color.red )
#100.times do |i|
#    pl.point_0( 45+20*rand(), i*(0.1) )
#    sleep(0.1)
#end

sleep( 5 )
plot_conf = RealTimePlot::PlotConfig.new
plot_conf.max_x = 1
plot_conf.max_y = 1
pl.reset( plot_conf )
pl.point_0( 0.5, 0.5 )
