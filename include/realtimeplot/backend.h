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

/// Needs to be before cairomm, due to Xlib.h macros
#include <pangomm/init.h>
#include <pangomm/context.h>
#include <pangomm/layout.h>

#include <cairomm/context.h>
#include <cairomm/xcb_surface.h>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "realtimeplot/plot.h"
#include "realtimeplot/eventhandler.h"
#include "realtimeplot/delaunay.h"

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

			LineAttributes( float x, float y, int id_value ) {
				id = id_value;
				current_x = x;
				current_y = y;
			}
	};

	/**
		\brief BackendPlot that waits for events and then plots them

		Users should almost never create an object based on this class themself, but
		should use a frontend class or if they need more flexibility create a event 
		handler and send that custom events.

		Large parts of the class are accessible from events (including the image 
		surfaces), which should allow one to do everything possible they want using
		custom events (as long as one knows what's going on in this class :) )

		\future Document the way the plot works (with the different plotting 
		surfaces etc)
		*/
	class BackendPlot {
		public:
			friend class BackendHeightMap;
			//plot_surface, an imagesurface that contains the plotted points
			//plot_context, the corresponding context
			Cairo::RefPtr<Cairo::ImageSurface> plot_surface;
			Cairo::RefPtr<Cairo::Context> plot_context;

			//temporary surface used when plotting stuff
			Cairo::RefPtr<Cairo::ImageSurface> temporary_display_surface;
			Cairo::RefPtr<Cairo::XcbSurface> xSurface;
			Cairo::RefPtr<Cairo::Context> xContext;
			xcb_connection_t *dpy;
			xcb_drawable_t win;
			xcb_screen_t *screen;

			//keep track of plot area pixels
			int plot_area_width, plot_area_height;

			//axes_surface, surface that contains the axes + labels, 
			//used as a mask on the plot_surface when showing the plot
			//axes_context, the corresponding context
			Cairo::RefPtr<Cairo::ImageSurface> axes_surface;
			Cairo::RefPtr<Cairo::Context> axes_context;

			//config class, that keeps track op min_x etc
			PlotConfig config;

			//pointer to the eventhandler class, so we can check the queue
			boost::shared_ptr<EventHandler> pEventHandler;

			//Keep track of the maximum/minimum values of the plot_surface in plot units
			float plot_surface_max_x, plot_surface_min_x;
			float plot_surface_max_y, plot_surface_min_y;
			/// Device units (pixels)
			float plot_surface_width, plot_surface_height;

			//last_update_time (display at least every second)
			boost::posix_time::ptime time_of_last_update;

			/*
			 * Methods
			 */

			//Constructor, takes a config pointer
			//creates the surfaces and contexts
			BackendPlot( PlotConfig config, boost::shared_ptr<EventHandler> pEventHandler );

			//Destructor, wait for event_loop thread to finish (join)
			~BackendPlot();

			/**
			 * \brief returns a blank (background color) create_plot_surface
			 *
			 * Also sets helper variables, such as plot_surface_min_x
			 */
			Cairo::RefPtr<Cairo::ImageSurface> create_plot_surface();

			//draw_axes_surface
			void draw_axes_surface();

			//create_xlib_window
			void create_xlib_window();

			//display the surface on xlib surface
			void display();

			//clears the plot
			void clear();

			//transform_to_plot_units
			//rescale image to plot scale
			void transform_to_plot_units( );
			void transform_to_plot_units( Cairo::RefPtr<Cairo::Context> pContext );

			//transform_to_plot_units_with_origin
			//rescale image to plot scale, taking into account the origin
			//Used for axes, and keeping 50 device pixels at each side 
			//for the axes + labels
			void transform_to_plot_units_with_origin( 
					Cairo::RefPtr<Cairo::ImageSurface> pSurface,
					Cairo::RefPtr<Cairo::Context> pContext,
					int origin_x, int origin_y );

			//transform_to_device_units
			//rescales context to device units
			void transform_to_device_units( Cairo::RefPtr<Cairo::Context> pContext );

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

			//! Draw rectangle to the surface
			void rectangle( float min_x, float min_y, float width_x, float width_y, 
					bool fill = true, Color color = Color::black() );

			void save( std::string fn );
			void save( std::string fn, Cairo::RefPtr<Cairo::ImageSurface> pSurface );

			//handle_xevent
			//Called by event handler when an xevent happens
			//void handle_xevent( XEvent report );

			/**
			 * \brief Close the current xwindow
			 */
			void close_window();

			//number
			//draws a number as text on the surface
			void number( float x, float y, float i );

			//rolling_update
			//moves the plot bounds to include the point x, y
			void rolling_update( float x, float y );

			//within_plot_bounds
			//check that a point lies within the bounds of the plot
			bool within_plot_bounds( float x, float y );

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
		private:
			//Keep track to lines
			std::list<boost::shared_ptr<LineAttributes> > lines;

			//set a flag when display shouldn't be updated (plotting still runs on)
			bool pause_display;

			//function to calculate the "optimal" tick values/positions
			std::vector<float> axes_ticks( float min, float max, int nr );

			Cairo::RefPtr<Cairo::ImageSurface> create_temporary_surface();
			
			xcb_visualtype_t *get_root_visual_type(xcb_screen_t *s);

			//move the plotting area around in increments of 5%
			void move( int direction_x, int direction_y );
			static boost::mutex global_mutex;
	};

	/**
	 * \brief 3D Triangle for use in HeightMap
	 */
	class Vertex3D : public delaunay::Vertex
	{
		public:
			float z;
			Vertex3D( float x, float y, float z ) : 
				z(z), delaunay::Vertex( x, y ) {}
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

			std::vector<boost::shared_ptr<Vertex3D> > gradientVector() {
				std::vector<boost::shared_ptr<Vertex3D> > v;
				//Find lowest and highest Vertex
				v.push_back( boost::shared_ptr<Vertex3D>( new Vertex3D( vertices[0]->x, 
								vertices[0]->y, vertices[0]->z ) ) );
				v.push_back( boost::shared_ptr<Vertex3D>( new Vertex3D( vertices[0]->x, 
								vertices[0]->y, vertices[0]->z ) ) );
				for (size_t i=1; i<3; ++i) {
					if (vertices[i]->z < v[0]->z)
						v[0].reset( new Vertex3D( vertices[i]->x, vertices[i]->y, vertices[i]->z ) );
					if (vertices[i]->z > v[1]->z)
						v[1].reset( new Vertex3D( vertices[i]->x, vertices[i]->y, vertices[i]->z ) );
				}

				boost::shared_ptr<Vertex3D> pEV1( new Vertex3D( vertices[1]->x-vertices[0]->x,
							vertices[1]->y-vertices[0]->y,
							vertices[1]->z-vertices[0]->z ) );
				boost::shared_ptr<Vertex3D> pEV2( new Vertex3D( vertices[2]->x-vertices[0]->x,
							vertices[2]->y-vertices[0]->y,
							vertices[2]->z-vertices[0]->z ) );
				float x2; float y2; float z2;
				boost::shared_ptr<Vertex3D> pNormal;
				pNormal = pEV1->crossProduct( pEV2 );
				y2 = pNormal->y/pNormal->x;
				z2 = -(pow(pNormal->y,2)+pow(pNormal->x,2))/(pNormal->x*pNormal->z);

				float scalar = (v[1]->z-v[0]->z)/z2;
				v[1]->x = v[0]->x+scalar;
				v[1]->y = v[0]->y+scalar*y2;
				v[1]->z = v[0]->z+scalar*z2;
				return v;
			}

			std::vector<boost::shared_ptr<Vertex3D> > vertices;
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
			float zmin, zmax,;
			delaunay::Delaunay delaunay;

			double alpha, beta;
			bool scale;

			inline Color colorMap( float z );
			/*delaunay::vertex vSuper[3];

			//! All the vertices
			std::set<delaunay::vertex> vertices;
			//! All the triangles
			std::multiset<delaunay::triangle> triangles;
			//! All the edges (used for plotting)
			//std::set<delaunay::edge> edges;*/
	};

}
#endif

