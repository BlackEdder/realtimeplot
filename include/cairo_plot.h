#ifndef CAIRO_PLOT_H
#define CAIRO_PLOT_H

#include <cairomm/context.h>
#include <cairomm/xlib_surface.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>


namespace cairo_plot {

    class PlotConfig {
        public:
            int pixel_width, pixel_height;
            int origin_x, origin_y;
            float min_x, max_x;
            float min_y, max_y;
            
            //set default values
            PlotConfig() {
                pixel_width = 400;
                pixel_height = 400;
                origin_x = 20;
                origin_y = 20;
                min_x = 0;
                max_x = 100;
                min_y = 0;
                max_y = 50;
            }
    };

    class Coord {
        public:
            float x, y;
            Coord( float tmp_x, float tmp_y ) {
                x = tmp_x;
                y = tmp_y;
            }
    };

    /*
     * Class that keeps track of the Plot area
     * i.e. its location and width and also
     * its xrange etc
     *
     * Is needed to translate x,y coordinates to pixel coordinates
     *
     * Watch out, cairo coordinates are from upper left corner instead
     * of lower left corner
     */
    class PlotSurface {
        public:
            int origin_x, origin_y;
            int pixel_widht, pixel_height;
            float min_x, max_x;
            float min_y, max_y;
            PlotConfig config;
            Cairo::RefPtr<Cairo::ImageSurface> pSurface;

            //Dummy to allow PlotSurface plot_surface
            PlotSurface() {};

            PlotSurface( PlotConfig config, 
                    Cairo::RefPtr<Cairo::ImageSurface> pSurface );

            //Draws axes etc
            void paint( Cairo::RefPtr<Cairo::Context> pContext );

            int get_pixel_width();
            int get_pixel_height();

            Coord to_pixel_coord( Coord plot_coords );
    };

	/*
	 * Class controls the plotting
	 */
	class Plot {
        public:
            Cairo::RefPtr<Cairo::ImageSurface> surface;
            Cairo::RefPtr<Cairo::Context> context;
            Cairo::RefPtr<Cairo::Context> xContext;
            PlotSurface plot_surface;
            PlotConfig config;
            Display *dpy;

            boost::shared_ptr<boost::thread> pEvent_thrd;

            int width;
            int height;

            // Should open cairo surface
            Plot( PlotConfig conf );
            ~Plot();
            // Plots a point to the surface
            void plot_point( float x, float y );
            void event_loop();
    };
}
#endif
