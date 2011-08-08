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
#ifndef REALTIMEPLOT_PLOT_H
#define REALTIMEPLOT_PLOT_H

/** \file plot.h
	\brief File containing frontend and backend plotting classes
	*/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <boost/shared_ptr.hpp>

//#include "realtimeplot/eventhandler.h"

namespace realtimeplot {

	class EventHandler;
	/**
		\brief Util function to turn doubles into strings

		\future Move to util file at some point?
		*/
	inline std::string stringify(double x)
	{
		std::ostringstream o;
		o << x;
		return o.str();
	}   

	/**
	 * \brief Class used to set color data

	 @param alpha takes  a value between 0 and 1, with 0 completely 
	 transparent and 1 not transparent at all.
	 */

	class Color {
		public:
			double r, g, b, a;
			/**
			 * \brief Default constructer, sets a white/transparent color
			 *
			 * More colors can be found here: 
			 * http://www.avatar.se/molscript/doc/colour_names.html
			 */
			Color();
			Color( double red, double green, double blue, double alpha );
			~Color() {};
			static Color black();
			static Color white();
			static Color red();
			static Color green();
			static Color blue();
			static Color yellow();
			static Color purple();
			static Color grey();
			static Color brown();
			static Color darkblue();
			static Color cyan();
			static Color indigo();
			static Color orange();
			static Color cadetblue();
			/**
			 * \brief Gets a color based on an id
			 *
			 * Easy if you need to get number of different colors, 
			 * but don't care which colors they are. 
			 *
			 * If id is greater than number of available colors it 
			 * will loop back to the first color.
			 */
			static Color by_id(size_t id);

			/** 
			 * \brief Gets a vector with available colors
			 */
			static std::vector<Color> colors();
	};


	/**
		\brief Class that keeps track of all the config variables used in a plot
		*/
	class PlotConfig {
		public:
			/// All the needed variables
			//! Scale to window size?
			bool scaling;
			int margin_x, margin_y;
			int nr_of_ticks, ticks_length;
			//! Size of a point (4 by default)
			int point_size;

			//! Total area in number of pixels (default = 500*500)
			size_t area;
			float min_x, max_x;
			float min_y, max_y;
			float overlap;
			float aspect_ratio;
			std::string xlabel, ylabel, font, title;
			bool fixed_plot_area;
			int label_font_size, numerical_labels_font_size;

			/// Constructor that sets default values
			PlotConfig()
				: scaling( false )
			{
				area = 500*500;
				margin_x = 60;
				margin_y = 60;
				min_x = 0;
				max_x = 10;
				min_y = 0;
				max_y = 50;
				nr_of_ticks = 10;
				ticks_length = 7;
				xlabel = "x";
				ylabel = "y";
				font = "sans";
				label_font_size = 12;
				numerical_labels_font_size = 10;
				overlap = 0.1;
				aspect_ratio = 1;
				fixed_plot_area = false;
				point_size = 4;
				title = "RealTimePlot";
			}
	};

	/**
		\brief Frontend class that opens a plot

		This is the main class people will use. Most functions will create an event
		that will be added to the event queue managed by the event handler. Then this
		event will be executed by the backend plotting object, which is running in a 
		separate process.
		*/
	class Plot {
		public:
			PlotConfig config;

			Plot();
			Plot( PlotConfig conf );
			~Plot();

			void point( float x, float y );
			void point( float x, float y, Color color );
			void rectangle( float x, float y, float width_x, float width_y,
					bool fill = true, Color color = Color::black() );
			//void line_add( float x, float y, int id=-1 );
			void line_add( float x, float y, int id=-1 );
			void line_add( float x, float y, int id, Color color );
			void number( float x, float y, float i );
			void save( std::string filename );
			void clear();
			/**
			 * \brief Creates a new plot (new surfaces etc) based on the given config
			 *
			 * Used by Histogram whenever a the new histogram is outside the 
			 * original range.
			 */
			void reset( PlotConfig config );
			/**
			 * \brief Call this when config is updated (BROKEN/DANGEROUS
			 *
			 * Would be better to do it automatically, but this works for now.
			 *
			 * Is currently only save for changing labels etc. Changing max_x etc will
			 * lead to wrong plots
			 */
			void update_config();

			/**
			 * \brief Close the plot
			 *
			 * By default the plot stays open, to allow one to view a plot after
			 * the program has finished. This closes the plot if one wants to.
			 */
			void close();
			//private:
			boost::shared_ptr<EventHandler> pEventHandler;
		protected:
			//! Constructor which doesn't immediately open a plot (only used by children at the moment)
			Plot( bool open );
	};

	/**
		\brief Class to produce histograms from data, will calculate range etc

		Also allows to add a new data point to the histogram on the fly. Histograms are 
		relatively costly in that they need to redraw everytime a new point is added. As
		such it can be usefull to let it redraw sometimes by mostly calling
		add_data( new_data, false ) and only calling add_data( new_data, true ) or plot()
		when you want to actually redraw.

		For the histogram we also need to recalculate the bins everytime a point is added that falls 
		outside the current range. Therefore, the class keeps around a vector
		containing all the old data. Which could in theory lead to memory problems. 

		\todo This could be solved by making an algorithm that updates range until things seem to "quiet" down and from that point onward only keeps bins and total counts around. If a point falls outside the range just or not plot it or add a bin.
		*/
	class Histogram : public Plot {
		public:
			int no_bins, max_y, min_x, max_x;
			std::vector<double> data;
			std::vector<double> bins_x;
			std::vector<double> bins_y;
			double bin_width;

			//! If true plot frequencies, instead of counts
			bool frequency;

			Histogram();
			//! Creates a histogram with a set min_x and max_x
			Histogram( double min_x, double max_x, size_t no_bins );
			~Histogram();
			/**
			 * \brief Set data based on a vector containing all the measurements
			 *
			 * Will automatically calculate the ranges. If show == true (default) it will
			 * immediately show the histogram.
			 */
			void set_data( std::vector<double> data, bool show = true );
			/**
			 * \brief Set data based on two vectors, one containing values and the other how often those values were found (counts)
			 *
			 * Will automatically calculate the ranges. If show == true (default) it will
			 * immediately show the histogram.
			 *
			 */
			void set_counts_data( std::vector<double> values, 
					std::vector<int> counts, bool show = true );

			/**
			 * \brief Add a new measurement/data
			 *
			 * Might be usefull to only set show true sometimes, since it can be costly to
			 * replot every time.
			 */

			void add_data( double data, bool show = true );
			void plot();
		private:
			bool frozen_bins_x;
			/**
			 * \brief Fill bins
			 *
			 * Terribly inefficient at the moment, since it always redos all data
			 * \todo Fix inefficiency.
			 *
			 * \todo Fix dropping data outside bin range when frozen_bins_x is set end
			 */
			void fill_bins();
	};

	/**
	 * \brief Produce surface plots
	 *
	 * Currently actually more of a "3d" histogram, where colors indicate number of points
	 * within a certain "rectangle" of values
	 *
	 * For now the implementation is quite limited, with always a gray scale palette.
	 */
	class SurfacePlot : public Plot {
		public:
			size_t resolution;

			//! Vector containing all the counts/data, with [resolution*x+y]
			std::vector<size_t> data;

			size_t max_z;

			SurfacePlot( float min_x, float max_x, float min_y, float max_y, 
					size_t resolution = 20 );
			void add_data( float x, float y, bool show=true );

			void plot();

		private:
			float width_x, width_y;
			//! Actually store lower bounds of the bins.
			std::vector<float> bins_x;
			std::vector<float> bins_y;
	};

	/**
	 * \brief Plot a height map from x,y,z points
	 *
	 * Using Delauney triangulation, so won't need to be a grid of x,y points
	 * Need an algorithm that allows us to add point for point (Bowyer-Watson)
	 */
	class HeightMap : public Plot {
		public:
			HeightMap(); //float min_x, float max_x, float min_y, float max_y );
			HeightMap( float min_x, float max_x, float min_y, float max_y );

			//! Sends an HeightMapData event to eventhandler
			void add_data( float x, float y, float z, bool show=true );

			/**
			 * \brief Calculates parameters that should lead to "optimal" colouring
			 *
			 * I.e. if most of the data is grouped (i.e. most values are low) then it will
			 * cause the data to be rescaled in such a way that most of the color range is 
			 * used for that part of the scale. 
			 *
			 * Assumes that the relative height is beta distributed and calculates alpha and
			 * beta values. Is a relatively costly procedure, so should not be used to often.
			 * For example only when all data is present, or once when a significant portions
			 * of the data has been added.
			 */
			void calculate_height_scaling();
	};


}
#endif
