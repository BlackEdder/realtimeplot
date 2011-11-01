/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, 2011 Edwin van Leeuwen

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

#include "realtimeplot/plotarea.h"

namespace realtimeplot {
	PlotArea::PlotArea( PlotConfig &config ) {

		//calculate minimum plot area width/height based on aspect ratio
		double x = sqrt(config.area)/sqrt(config.aspect_ratio);
		plot_area_width = round( config.aspect_ratio*x );
		plot_area_height = round( x );

		//plot_surface, the shown part of this surface is by default 250000 pixels (default 500x500)
		//The rest is for when plotting outside of the area
		double ratio_surface_to_area = 5;

		plot_surface_width = ratio_surface_to_area*plot_area_width;
		plot_surface_height = ratio_surface_to_area*plot_area_height;

		double xratio = (ratio_surface_to_area-1)/2.0;
		double yratio = (ratio_surface_to_area-1)/2.0;
		min_x = config.min_x-xratio*(config.max_x-config.min_x);
		max_x = config.max_x+xratio*(config.max_x-config.min_x);
		min_y = config.min_y-yratio*(config.max_y-config.min_y);
		max_y = config.max_y+yratio*(config.max_y-config.min_y);

		surface = 
			Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					plot_surface_width, plot_surface_height );

		//Create context to draw background color
		context = Cairo::Context::create(surface);

		//give the plot its background color
		context->set_source_rgba(1, 1, 1, 1);
		context->rectangle( 0, 0,
				surface->get_width(), surface->get_height() );
		context->fill();
	}	
};
