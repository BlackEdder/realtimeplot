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
		setup( config );
	}	

	void PlotArea::setup( PlotConfig &config ) {
		point_size = config.point_size;

		//calculate minimum plot area width/height based on aspect ratio
		double x = sqrt(config.area)/sqrt(config.aspect_ratio);
		plot_area_width = round( config.aspect_ratio*x );
		plot_area_height = round( x );

		//plot_surface, the shown part of this surface is by default 250000 pixels (default 500x500)
		//The rest is for when plotting outside of the area
		double ratio_surface_to_area = 5;

		width = ratio_surface_to_area*plot_area_width;
		height = ratio_surface_to_area*plot_area_height;

		double xratio = (ratio_surface_to_area-1)/2.0;
		double yratio = (ratio_surface_to_area-1)/2.0;
		min_x = config.min_x-xratio*(config.max_x-config.min_x);
		max_x = config.max_x+xratio*(config.max_x-config.min_x);
		min_y = config.min_y-yratio*(config.max_y-config.min_y);
		max_y = config.max_y+yratio*(config.max_y-config.min_y);

		surface = 
			Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					width, height );

		//Create context to draw background color
		context = Cairo::Context::create(surface);

		transform_to_plot_units();

		//give the plot its background color
		clear();	}

	void PlotArea::transform_to_plot_units() {
		transform_to_device_units();
		context->translate( 0, height );
		context->scale( width/(max_x-min_x),
				-height/(max_y-min_y) );
		context->translate( -min_x, -min_y );
	}

	void PlotArea::transform_to_device_units() {
		context->set_identity_matrix();
	}

	void PlotArea::set_color( Color color ) {
		context->set_source_rgba( color.r, color.g, color.b, color.a );
	}

	void PlotArea::rectangle( float rect_min_x, float rect_min_y,
		 float width, float height, bool fill ) {
		context->save();
		context->rectangle( rect_min_x, rect_min_y, width, height );
		// So strange that this is needed, but fill seems 
		// somehow to convert back to device units 
		transform_to_device_units();
		if (fill) 
			context->fill_preserve();
		context->stroke();
		context->restore();
	}

	void PlotArea::point( float x, float y ) {
		double dx = point_size;
		double dy = point_size;
		context->device_to_user_distance(dx,dy);
		rectangle( x-0.5*dx, y-0.5*dy, dx, dy, true );
	}

	void PlotArea::clear() {
		context->save();
		set_color( Color::white() );
		rectangle( min_x, min_y, max_x-min_x, max_y-min_y,
				true );
		context->restore();
	}
};
