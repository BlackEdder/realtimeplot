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
#ifndef CAIRO_PLOT_PLOT_H
#define CAIRO_PLOT_PLOT_H

/** \file plot.h
	 	\brief File containing frontend and backend plotting classes
*/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

/// Needs to be before cairomm, due to Xlib.h macros
#include <pangomm/init.h>
#include <pangomm/context.h>
#include <pangomm/layout.h>

#include <cairomm/context.h>
#include <cairomm/xlib_surface.h>

#include "realtimeplot/eventhandler.h"

#include "boost/date_time/posix_time/posix_time.hpp"
namespace realtimeplot {

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
		 \brief Class that keeps track of all the config variables used in a plot
     */
    class PlotConfig {
        public:
            /// All the needed variables
            int pixel_width, pixel_height;
            int margin_x, margin_y;
            int nr_of_ticks, ticks_length;
            int point_size;
            float min_x, max_x;
            float min_y, max_y;
            float overlap;
            float aspect_ratio;
            std::string xlabel, ylabel, font;
            bool fixed_plot_area;

            /// Constructor that sets default values
            PlotConfig() {
								margin_x = 50;
								margin_y = 50;
                min_x = 0;
                max_x = 90;
                min_y = 0;
                max_y = 50;
                nr_of_ticks = 10;
                ticks_length = 7;
                xlabel = "x";
                ylabel = "y";
								font = "sans 8";
                overlap = 0.1;
                aspect_ratio = 1;
                fixed_plot_area = false;
                point_size = 4;
            }
    };

		/**
		 \brief Event that draws a point at x, y
		 */
    class PointEvent : public Event {
        public:
            PointEvent( float x, float y );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd;
    };

 		/**
		 \brief Event that adds a point to an existing line

		 If no line exists yet a new one will be started with starting point x, y
         @param id can be any int and identifies to which line a point belongs

		 */
     class LineAddEvent : public Event {
        public:
            LineAddEvent( float x, float y, int id );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd;
            int id;
    };

		/**
		 \brief Event that plots a number (float) at the specified x,y coordinate
		 */
    class NumberEvent : public Event {
        public:
            NumberEvent( float x, float y, float i );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd, nr;
    };

		/**
		 \brief Event that plots an transparent point
		 
		 @param alpha takes  a value between 0 and 1, with 0 completely 
		 transparent and 1 not transparent at all.

		 \future Separate event to set transparency and plot a point. This depends on
		 the ability to lock the eventhandler, so we can be certain events are done in
		 a specific order
		 */ 
    class PointTransparentEvent : public Event {
        public:
            PointTransparentEvent( float x, float y, float alpha );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd, alpha;
    };

		/**
		 \brief Event to save the current plot to the specified file
		 */
    class SaveEvent : public Event {
        public:
            SaveEvent( std::string filename );
            virtual void execute( BackendPlot *bPl );
        private:
            std::string filename;
    };

		/**
		 \brief Event to clear the current plot
		 */
    class ClearEvent : public Event {
        public:
            ClearEvent();
            virtual void execute( BackendPlot *bPl );
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
            Plot();
            Plot( PlotConfig conf );
            ~Plot();

            void point( float x, float y );
            void line_add( float x, float y, int id );
            void number( float x, float y, float i );
            void point_transparent( float x, float y, float a );
            void save( std::string filename );
            void clear();
        private:
            EventHandler *pEventHandler;
    };

		/**
		 \brief Class to produce histograms from data, will calculate range etc

		 Currently calls plot itself. It might be better to actually just integrate it
		 into the plot class itself or to inherit the plot class.
		 */
		class Histogram {
			public:
				int no_bins, max_y;
				std::vector<double> data;
				std::vector<double> bins_x;
				std::vector<double> bins_y;
				double bin_width;

				PlotConfig config;
				Plot *pHistogram;

				Histogram();
				~Histogram();
				void set_data( std::vector<double> data );
				void set_counts_data( std::vector<double> values, 
						std::vector<int> counts );
			private:
				void fill_bins();
				void plot();
		};


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

		\future Document the way the plot works (with the different plotting surfaces etc)
		
     */
    class BackendPlot {
        public:
            //plot_surface, an imagesurface that contains the plotted points
            //plot_context, the corresponding context
            Cairo::RefPtr<Cairo::ImageSurface> plot_surface;
            Cairo::RefPtr<Cairo::Context> plot_context;

            //temporary surface used when plotting stuff
            Cairo::RefPtr<Cairo::ImageSurface> temporary_display_surface;
            Cairo::RefPtr<Cairo::XlibSurface> xSurface;
            Cairo::RefPtr<Cairo::Context> xContext;
            Window win;
            Display *dpy;

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
            EventHandler *pEventHandler;

            //Keep track of the maximum/minimum values of the plot_surface in plot units
            float plot_surface_max_x, plot_surface_min_x;
            float plot_surface_max_y, plot_surface_min_y;
            float alpha;

            //last_update_time (display at least every second)
            boost::posix_time::ptime time_of_last_update;

            /*
             * Methods
             */

            //Constructor, takes a config pointer
            //creates the surfaces and contexts
            //starts the event_loop
            BackendPlot( PlotConfig config, EventHandler *pEventHandler );

            //Destructor, wait for event_loop thread to finish (join)
            ~BackendPlot();

            //create_plot_surface
            //creates a new plot surface and sets helper variables such as plot_surface_min_x
            void create_plot_surface();

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

            //set_foreground_color
            void set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext );

            void set_alpha( float alpha );

            //point
            //draw point on surface
            void point( float x, float y);

            /**
            \brief Add a point to a line 

            If no line exists with the specified id a new line will be added
            */
            void line_add( float x, float y, int id );

            void save( std::string fn );
            void save( std::string fn, Cairo::RefPtr<Cairo::ImageSurface> pSurface );

            //handle_xevent
            //Called by event handler when an xevent happens
            void handle_xevent( XEvent report );

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

        private:
            //Keep track to lines
            std::list<LineAttributes*> lines;

            //set a flag when display shouldn't be updated (plotting still runs on)
            bool pause_display;

            //function to calculate the "optimal" tick values/positions
            std::vector<float> axes_ticks( float min, float max, int nr );

            Cairo::RefPtr<Cairo::ImageSurface> create_temporary_surface();

            //move the plotting area around in increments of 5%
            void move( int direction_x, int direction_y );
    };
}
#endif
