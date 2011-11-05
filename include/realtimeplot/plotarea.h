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

	class Area {
		public:
			Cairo::RefPtr<Cairo::ImageSurface> surface;
			Cairo::RefPtr<Cairo::Context> context;
			size_t width, height;
	};

	/**
	 * \brief Manage the actual area used for plotting
	 *
	 * The actual PlotArea is by default 25 times the size of the shown plot area.
	 * This is too facilitate things such as move, scale, zoom etc.	
	 */
	class PlotArea : public Area {
		public:
			size_t plot_area_width, plot_area_height;

			size_t point_size;

			//Keep track of the maximum/minimum values of the plot_surface in plot units
			float max_x, min_x;
			float max_y, min_y;

			PlotArea( PlotConfig &config );

			void setup( PlotConfig &config );	

			void transform_to_plot_units();

			void transform_to_device_units();
			
			void set_color( Color color );

			void rectangle( float min_x, float min_y, float width_x, float width_y, 
				bool fill );

			void point( float x, float y );

			void line_add( float x, float y, int id );
			
			/**
			 * \brief Reposition to a new center
			 *
			 * Needs to copy old surface on top of the newer
			 */
			void reposition( float center_x, float center_y);

			void clear();
		//private:
			//Keep track to lines
			std::list<boost::shared_ptr<LineAttributes> > lines;
	};

	/**
	 * \brief Manage the area containing the axes
	 */
	class AxesArea : public Area {
		public:
			AxesArea( PlotConfig &config, size_t width, size_t height );
			void setup( PlotConfig &config, size_t width, size_t height  );

			/**
			 * \brief Calculate power of step (i.e. 0.01 -> power is -2)
			 */
			int power_of_step( float step );
	};
};
#endif
