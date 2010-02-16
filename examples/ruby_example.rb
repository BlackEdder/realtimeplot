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
require "rb_realtimeplot"

pl = Plot.new
pl.point( 10,10 )
pl.point( 10,80 )
100.times do |i|
    pl.point( 45+20*rand(), i*(0.1) )
    sleep(0.1)
end
