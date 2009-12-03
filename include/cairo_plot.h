#ifndef CAIRO_PLOT_H
#define CAIRO_PLOT_H

#include <cairomm/context.h>
#include <cairomm/xlib_surface.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>


namespace cairo_plot {
	/*
	 * Class controls the plotting
	 */
	class Plot {
        public:
            Cairo::RefPtr<Cairo::ImageSurface> surface;
            Cairo::RefPtr<Cairo::Context> context;
            boost::shared_ptr<boost::thread> pEvent_thrd;

            int width;
            int height;

            // Should open cairo surface
            Plot( int max_x, int max_y );
            ~Plot();
            // Plots a point to the surface
            void plot_point( float x, float y );
            void event_loop();
    };
}
#endif
