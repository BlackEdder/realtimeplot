#ifndef CAIRO_PLOT_H
#define CAIRO_PLOT_H
#include <iostream>
#include <sstream>
#include <string>

#include <cairomm/context.h>
#include <cairomm/xlib_surface.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>


namespace cairo_plot {

	inline std::string stringify(double x)
	{
		std::ostringstream o;
		o << x;
		return o.str();
	}   

	/*
	 * Class keeps track of all the config variables
	 */

	class PlotConfig {
		public:
			//all the needed variables
			int pixel_width, pixel_height;
			int origin_x, origin_y;
			int nr_of_ticks, ticks_length;
			float min_x, max_x;
			float min_y, max_y;
			float overlap;
			float aspect_ratio;
			std::string xlabel, ylabel;
			bool fixed_plot_area;

			//Constructor that sets default values
			PlotConfig() {
				origin_x = 50;
				origin_y = 50;
				min_x = -10;
				max_x = 90;
				min_y = -10;
				max_y = 50;
				nr_of_ticks = 10;
				ticks_length = 7;
				xlabel = "x";
				ylabel = "y";
				overlap = 0.1;
				aspect_ratio = 1;
				fixed_plot_area = false;
			}
	};

	/*
	 * Main class to control the plotting
	 */
	class Plot {
		public:
			/*
			 * Instance methods
			 */
			//plot_surface, an imagesurface that contains the plotted points
			//plot_context, the corresponding context
			Cairo::RefPtr<Cairo::ImageSurface> plot_surface;
			Cairo::RefPtr<Cairo::Context> plot_context;

			//keep track of plot area pixels
			int plot_area_width, plot_area_height;

			//axes_surface, surface that contains the axes + labels, used as a mask on the plot_surface when showing the plot
			//axes_context, the corresponding context
			Cairo::RefPtr<Cairo::ImageSurface> axes_surface;
			Cairo::RefPtr<Cairo::Context> axes_context;

			//config, pointer to config class, that keeps track op min_x etc
			PlotConfig *pConf;

			//keep track when plot_surface was updated:
			bool plot_surface_update;

			//Thread that contains the event loop
			boost::shared_ptr<boost::thread> pEvent_thrd;

			//Keep track of the maximum/minimum values of the plot_surface in plot units
			float plot_surface_max_x, plot_surface_min_x;
			float plot_surface_max_y, plot_surface_min_y;

			/*
			 * Methods
			 */

			//Constructor, takes a config pointer
			//creates the surfaces and contexts
			//starts the event_loop
			Plot( PlotConfig *config );

			//Destructor, wait for event_loop thread to finish (join)
			~Plot();

			//event_loop, draws plot to screen (and keeps redrawing it)
			void event_loop();

			//create_plot_surface
			//creates a new plot surface and sets helper variables such as plot_surface_min_x
			void create_plot_surface();

			//transform_to_plot_units
			//rescale image to plot scale, basically calls transform_to_plot_units_with_origin
			//with origin (1000,1000) (should be used for plot_context only)!
			void transform_to_plot_units( );

			//transform_to_plot_units_with_origin
			//rescale image to plot scale, taking into account the origin
			//Used for axes, and keeping 50 device pixels at each side for the axes + labels
			void transform_to_plot_units_with_origin( Cairo::RefPtr<Cairo::ImageSurface> pSurface,
					Cairo::RefPtr<Cairo::Context> pContext,
					int origin_x, int origin_y );

			//transform_to_device_units
			//rescales context to device units
			void transform_to_device_units( Cairo::RefPtr<Cairo::Context> pContext );

			//draw_axes_surface
			//Draws the axes_surface (also used as a mask)
			void draw_axes_surface();

			//set_background_color
			//takes the context to do it to
			void set_background_color( Cairo::RefPtr<Cairo::Context> pContext );

			//set_foreground_color
			void set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext );

			//point
			//draw point on surface
			void point( float x, float y);

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

};
}
#endif
