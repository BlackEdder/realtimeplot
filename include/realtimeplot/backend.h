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

#ifndef REALTIMEPLOT_BACKEND_H
#define REALTIMEPLOT_BACKEND_H

#include <vector>
#include <boost/thread/mutex.hpp>

// Needs to be before cairomm, due to Xlib.h macros
#include <pangomm/init.h>
#include <pangomm/context.h>
#include <pangomm/layout.h>

#include <cairomm/context.h>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "realtimeplot/plot.h"
#include "realtimeplot/eventhandler.h"
#include "realtimeplot/delaunay.h"
#include "realtimeplot/xcbhandler.h"
#include "realtimeplot/plotarea.h"

class TestBackend;

namespace realtimeplot {
	/**
		\brief BackendPlot that waits for events and then plots them

		Users should almost never create an object based on this class, but
		should use a frontend class or if they need more flexibility create an event 
		handler object and send that custom events.
		*/
	class BackendPlot {
		public:
			friend class BackendHeightMap;

			//temporary surface used when plotting stuff
			Cairo::RefPtr<Cairo::ImageSurface> temporary_display_surface;
			//Cairo::RefPtr<Cairo::XcbSurface> xSurface;
			Cairo::RefPtr<Cairo::Surface> xSurface;
			Cairo::RefPtr<Cairo::Context> xContext;

			boost::shared_ptr<PlotArea> pPlotArea;
			boost::shared_ptr<AxesArea> pAxesArea;

			int x_surface_width, x_surface_height;

			//config class, that keeps track op min_x etc
			PlotConfig config;

			//pointer to the eventhandler class, so we can check the queue
			boost::shared_ptr<EventHandler> pEventHandler;

			//last_update_time (display at least every second)
			boost::posix_time::ptime time_of_last_update;

			//set a flag when display shouldn't be updated (plotting still runs on)
			bool pause_display;
			/*
			 * Methods
			 */

			//Constructor, takes a config pointer
			//creates the surfaces and contexts
			BackendPlot( PlotConfig config, boost::shared_ptr<EventHandler> pEventHandler );

			//Destructor, wait for event_loop thread to finish (join)
			virtual ~BackendPlot();

			//draw_axes_surface
			void draw_axes_surface();

			//display the surface on xlib surface
			void display();

			//clears the plot
			virtual void clear();

			/**
			 * \brief Resets the plot to use a new config
			 */
			virtual void reset( PlotConfig config );

			//set_background_color
			//takes the context to do it to
			void set_background_color( Cairo::RefPtr<Cairo::Context> pContext );

			/**
			 * \brief Set default foreground color on any pContext
			 */
			void set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext );

			/**
			 * \brief Set default foreground color on plot_context
			 */
			void set_foreground_color();

			void set_alpha( float alpha );

			/**
			 * \brief Set the foreground color of the plot area
			 *
			 * Should often be followed by a restore, such that the color is changed
			 * back
			 */
			void set_color( Color color );

			/**
			 * \brief Restores previously saved settings of plot_context 
			 *
			 * Is called by many events after set_color, to change the color back
			 * to the previous value.
			 */
			void restore();

			//point
			//draw point on surface
			void point( float x, float y );

			/**
				\brief Add a point to a line 

				If no line exists with the specified id a new line will be added
				*/
			void line_add( float x, float y, int id, Color color );

			void title( std::string &title );

			/** 
			 * \brief Add text to plot at point x, y.
			 *
			 * Currently always left justified
			 */
			virtual void text( float x, float y, std::string &text );

			//! Draw rectangle to the surface
			void rectangle( float min_x, float min_y, float width_x, float width_y, 
					bool fill = true, Color color = Color::black() );

			void save( std::string fn );
			void save( std::string fn, Cairo::RefPtr<Cairo::ImageSurface> pSurface );

			//Moved to xcbhandler
			//void handle_xevent( xcb_generic_event_t *e );

			/**
			 * \brief Close the current xwindow
			 */
			void close_window();

			//rolling_update
			//moves the plot bounds to include the point x, y
			void rolling_update( float x, float y );

			//within_plot_bounds
			//check that a point lies within the bounds of the plot
			virtual bool within_plot_bounds( float x, float y );

			//plot_bounds_within_surface_bounds
			//check that the plot bounds are within surface bounds of the surface
			bool plot_bounds_within_surface_bounds( );

			/**
			 * \brief Call this when config is updated
			 *
			 * Would be better to do it automatically, but this works for now
			 *
			 * Needs more safety checks to see if the new values aren't bogus
			 */
			void update_config();

			/**
			 * \brief Call when xwindow changes size to scale it up or down
			 */
			void scale_xsurface( double width, double height );

			/**
			 * \brief Move the plotting area a number of onscreen pixels (xwindow pixels)
			 */
			void move_pixels( int pixels_x, int pixels_y );

			/**
			 * \brief Move the plotting area around in increments of 5%
			 */
			void move( double direction_x, double direction_y );

			/**
			 * \brief Zoom in or out according to the given scale
			 */
			void zoom( double scale );
			friend class ::TestBackend;
		private:
			DisplayHandler *pDisplayHandler;
			size_t win;

			Cairo::RefPtr<Cairo::ImageSurface> create_temporary_surface();
			
			static boost::mutex global_mutex;

			//! Check that the config values are valid
			void checkConfig();
	};


	/**
	 * \brief 3D Triangle for use in HeightMap
	 */
	class Vertex3D : public delaunay::Vertex
	{
		public:
			float z;
			Vertex3D( float x, float y, float z ) : 
				delaunay::Vertex( x, y ), z(z) {}
			boost::shared_ptr<Vertex3D> crossProduct( boost::shared_ptr<Vertex3D> pV ) {
				return boost::shared_ptr<Vertex3D>( new Vertex3D( y*pV->z-z*pV->y, 
							z*pV->x-x*pV->z, x*pV->y-y*pV->x ) );
			}
	};

	class Triangle3D 
	{
		public:
			Triangle3D() {};
			Triangle3D( boost::shared_ptr<delaunay::Triangle> pTriangle ) {
				for (size_t i=0; i<3; ++i) {
					vertices.push_back( boost::static_pointer_cast<Vertex3D, delaunay::Vertex>(
								pTriangle->corners[i]->vertex ) );
				}
			}

			std::vector<boost::shared_ptr<Vertex3D> > gradientVector();
			std::vector<boost::shared_ptr<Vertex3D> > vertices;
	};
	/**
	 * \brief Provides backend functions specific for Histogram
	 */
	
	class BackendHistogram : public BackendPlot {
		public:
			size_t no_bins;
			std::vector<double> data;

			//! If true plot frequencies, instead of counts
			bool frequency;
			bool rebin;

			double data_min, data_max;

			std::vector<double> bins_y;
			/**
			 * \brief Creates a histogram
			 *
			 * If frequency is true then relative counts will be used. If config.fixed_plot_are is true
			 * then the provided config.min_x, config.max_x will be used, otherwise they will be
			 * adjusted on the fly.
			 */
			BackendHistogram( PlotConfig config, bool frequency, size_t no_bins, 
					boost::shared_ptr<EventHandler> pEventHandler );

			double bin_width();
			double min();
			double max();

			/**
			 * \brief Add a new measurement/data
			 */
			void add_data( double data );

			/**
			 * \brief Optimize bounds based on current added data
			 *
			 * Tries to calculate min_x, max_x to capture most data, while ingnoring
			 * outliers.
			 * Should probably only be called after most/all data is added.
			 */
			void optimize_bounds( double proportion );

			void rebin_data();

			void plot();

		};

	/**
	 * \brief Provides backend functions for Histogram3D plots
	 */

	class BackendHistogram3D : public BackendPlot {
		public:
			size_t no_bins_x, no_bins_y;
			std::vector<delaunay::Vertex> data;

			double data_min_x, data_min_y, data_max_x, data_max_y;
			double max_z;

			bool rebin;

			std::vector<size_t> bins_xy;

			ColorMap color_map;

			BackendHistogram3D(PlotConfig cfg, 
				boost::shared_ptr<EventHandler> pEventHandler,
				size_t no_bins_x = 10, size_t no_bins_y = 10 );

			/**
			 * \brief Return the array index associated with x index and y index
			 */
			size_t xytoindex( size_t x, size_t y );

			/**
			 * \brief Return x and y index associated with data index
			 */
			std::vector<size_t> indextoxy( size_t index );

			double bin_width_x();
			double bin_width_y();

			double min_x();
			double min_y();

			double max_x();
			double max_y();

			/**
			 * \brief Add a new measurement/data
			 */
			void add_data( double x, double y );

			void rebin_data();

			void plot();

			void calculate_height_scaling();
	};

	/**
	 * \brief Provides backend functions specific for HeightMap plots
	 *
	 * Relies heavily on Delaunay.h
	 */
	class BackendHeightMap : public BackendPlot {
		public:
			BackendHeightMap(PlotConfig config, boost::shared_ptr<EventHandler> pEventHandler);

			/**
			 * \brief Adds a new point to the existing map
			 *
			 * If show then the plot is immediately redrawn with the new point
			 */
			void add_data( float x, float y, float z, bool show );

			void plot();

			/**
			 * \brief Tries to calculate ideal scaling of the height parameter
			 *
			 * Such that most of the colors in the colormap cover the spot where most
			 * of the data is
			 */
			void calculate_height_scaling();
		private:
			float zmin, zmax;
			delaunay::Delaunay delaunay;

			ColorMap color_map;

			inline Color colorMap( float z );
	};

}
#endif

