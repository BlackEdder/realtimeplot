#ifndef CAIRO_PLOT_PLOT_H
#define CAIRO_PLOT_PLOT_H
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <cairomm/context.h>
#include <cairomm/xlib_surface.h>

#include "cairo_plot/eventhandler.h"

#include "boost/date_time/posix_time/posix_time.hpp"

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
            int point_size;
            float min_x, max_x;
            float min_y, max_y;
            float overlap;
            float aspect_ratio;
            std::string xlabel, ylabel;
            bool fixed_plot_area;

            //Constructor that sets default values
            PlotConfig() {
                min_x = 0;
                max_x = 90;
                min_y = 0;
                max_y = 50;
                nr_of_ticks = 10;
                ticks_length = 7;
                xlabel = "x";
                ylabel = "y";
                overlap = 0.1;
                aspect_ratio = 1;
                fixed_plot_area = false;
                point_size = 4;
            }
    };

    //Event that draws a point at x,y
    class PointEvent : public Event {
        public:
            PointEvent( float x, float y );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd;
    };

    class LineAddEvent : public Event {
        public:
            LineAddEvent( float x, float y );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd;
    };

    class NumberEvent : public Event {
        public:
            NumberEvent( float x, float y, float i );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd, nr;
    };

    class PointTransparentEvent : public Event {
        public:
            PointTransparentEvent( float x, float y, float a );
            virtual void execute( BackendPlot *bPl );
        private:
            float x_crd, y_crd, alpha;
    };

    class SaveEvent : public Event {
        public:
            SaveEvent( std::string filename );
            virtual void execute( BackendPlot *bPl );
        private:
            std::string filename;
    };


    /*
     * This is a "frontend" plotting class, that sends events to the "backend"
     * Backend runs in a separate thread
     */
    class Plot {
        public:
            Plot( PlotConfig conf );
            ~Plot();
            void point( float x, float y );
            void line_add( float x, float y );
            void number( float x, float y, float i );
            void point_transparent( float x, float y, float a );
            void save( std::string filename );
        private:
            EventHandler *pEvent_Handler;
    };

    /*
     * BackendPlot that waits for events and then plots them
     */
    class BackendPlot {
        public:
            /*
             * Instance methods
             */
            //plot_surface, an imagesurface that contains the plotted points
            //plot_context, the corresponding context
            Cairo::RefPtr<Cairo::ImageSurface> plot_surface;
            Cairo::RefPtr<Cairo::Context> plot_context;
            
            //context used for drawing lines
            Cairo::RefPtr<Cairo::Context> line_context;

            //temporary surface used when plotting stuff
						Cairo::RefPtr<Cairo::ImageSurface> temporary_display_surface;
						Cairo::RefPtr<Cairo::XlibSurface> xSurface;
            Cairo::RefPtr<Cairo::Context> xContext;
            Window win;
            Display *dpy;

            //keep track of plot area pixels
            int plot_area_width, plot_area_height;

            //axes_surface, surface that contains the axes + labels, used as a mask on the plot_surface when showing the plot
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

            //transform_to_plot_units
            //rescale image to plot scale
            void transform_to_plot_units( );
            void transform_to_plot_units( Cairo::RefPtr<Cairo::Context> pContext );

            //transform_to_plot_units_with_origin
            //rescale image to plot scale, taking into account the origin
            //Used for axes, and keeping 50 device pixels at each side for the axes + labels
            void transform_to_plot_units_with_origin( Cairo::RefPtr<Cairo::ImageSurface> pSurface,
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

            //line_add
            //start a line at x, y or add to a current line
            void line_add( float x, float y);

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
            //previous x and y coordinate of the line point
            float line_old_x, line_old_y;

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
