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
#ifndef PLOTAREA_H
#define PLOTAREA_H

#include <cairomm/context.h>
#include "realtimeplot/plot.h"

namespace realtimeplot {
	/**
	 * \brief Class that is used to keep stats of existing lines
	 */
	class LineAttributes {
		public:
			int id;
			float current_x, current_y;

			//context used for drawing lines
			Cairo::RefPtr<Cairo::Context> context;

			LineAttributes( float x, float y, int id ) 
				: id( id ), current_x( x ), current_y( y )	{
			}
	};

	/**
	 * \brief Manage the actual area used for plotting
	 *
	 * The actual PlotArea is by default 25 times the size of the shown plot area.
	 * This is too facilitate things such as move, scale, zoom etc. This class should
	 * not have to be aware of this though.
	 */
	class PlotArea {
		public:

			/*Cairo::RefPtr<Cairo::ImageSurface> plot_surface;
			Cairo::RefPtr<Cairo::Context> plot_context;*/
			Cairo::RefPtr<Cairo::ImageSurface> surface;
			Cairo::RefPtr<Cairo::Context> context;

			size_t plot_area_width, plot_area_height;

			size_t point_size;

			//Keep track of the maximum/minimum values of the plot_surface in plot units
			//float plot_surface_max_x, plot_surface_min_x;
			//float plot_surface_max_y, plot_surface_min_y;
			float max_x, min_x;
			float max_y, min_y;
			/// Device units (pixels)
			float width, height;

			PlotArea( PlotConfig &config );

			void setup( PlotConfig &config );	

			void transform_to_plot_units();

			void transform_to_device_units();
			
			void set_color( Color color );

			void rectangle( float min_x, float min_y, float width_x, float width_y, 
				bool fill );

			void point( float x, float y );

			void line_add( float x, float y, int id );

			void clear();
		//private:
			//Keep track to lines
			std::list<boost::shared_ptr<LineAttributes> > lines;
	};

	/**
	 * \brief Manage the area containing the axes
	 */
	class AxesArea {
	};
};
#endif
