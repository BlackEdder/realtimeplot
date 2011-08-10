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
pl.point( 5,50 )
pl.point_with_color( 5,80, RealTimePlot::Color.red )
pl.line_add( 1, 50 )
pl.line_add( 5, 80 )
pl.line_add_with_color( 4, 50, 1, RealTimePlot::Color.red )
pl.line_add_with_color( 6, 80, 1, RealTimePlot::Color.red)

sleep( 1 )
plot_conf = RealTimePlot::PlotConfig.new
plot_conf.max_x = 1
plot_conf.max_y = 1
pl.reset( plot_conf )
pl.point( 0.5, 0.5 )

sleep( 1 )
hm = RealTimePlot::HeightMap.new( 0, 1, 0, 1 )
50.times do |x|
	x = x/50.0
	50.times do |y|
		y = y/50.0
		hm.add_data( x, y, ((((x-0.5)*(y-0.5))**2)**0.5), false );
	end
end
x = 0; y=0
hm.add_data( x, y, ((((x-0.5)*(y-0.5))**2)**0.5), true );
hm.calculate_height_scaling();

sleep(1)
hist = RealTimePlot::Histogram.new
hist.add_data( 1 )
# This will throw an error with some versions of rbplusplus
# since the conversion of array into c++ vector is not working
# in those versions
hist.set_data( [1,2,3,1,1.1] )

