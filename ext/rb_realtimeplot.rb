#
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

require 'rubygems'
require 'rbplusplus'
require 'fileutils'
include RbPlusPlus

ROOT = File.expand_path(File.join(File.dirname(__FILE__), ".."))

Extension.new "rb_realtimeplot" do |e|
	e.working_dir = File.join( ROOT, "ext", "generated" )
	e.sources [
			File.join( ROOT, "include/realtimeplot/plot.h" )
		],
			:include_paths => File.join( ROOT, "include" ),
			:libraries => "realtimeplot"

	e.module "RealTimePlot" do |m|
		node = m.namespace "realtimeplot"

		node.classes( "Color" ).use_constructor(
			node.classes( "Color" ).constructors.find( :arguments => [] )
		)

		node.classes( "Plot" ).use_constructor(
			node.classes( "Plot" ).constructors.find( :arguments => [] )
		)

		node.classes( "Plot" ).methods( "line_add" ).ignore

		node.classes( "PlotConfig" ).use_constructor(
			node.classes( "PlotConfig" ).constructors.find( :arguments => [] )
		)

		node.classes( "Histogram" ).use_constructor(
			node.classes( "Histogram" ).constructors.find( :arguments => [] )
		)
		node.classes( "SurfacePlot" ).use_constructor(
			node.classes( "SurfacePlot" ).constructors.find( :arguments => [nil,nil,nil,nil,nil] )
		)
		node.classes( "HeightMap" ).use_constructor(
			node.classes( "HeightMap" ).constructors.find( :arguments => [nil,nil,nil,nil,nil] )
		)
	end
		
end

