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
			std::string xlabel, ylabel;

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

			//axes_surface, surface that contains the axes + labels, used as a mask on the plot_surface when showing the plot
			//axes_context, the corresponding context
			Cairo::RefPtr<Cairo::ImageSurface> axes_surface;
			Cairo::RefPtr<Cairo::Context> axes_context;

			//config, pointer to config class, that keeps track op min_x etc
			PlotConfig *pConf;

			//Thread that contains the event loop
			boost::shared_ptr<boost::thread> pEvent_thrd;

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
	};
}
#endif
