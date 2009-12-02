#include <cairomm/context.h>
#ifndef CAIRO_PLOT_H
#define CAIRO_PLOT_H

namespace cairo_plot {
	/*
	 * Class controls the plotting
	 */
	class Plot {
		public:
			Cairo::RefPtr<Cairo::Context> pContext;
			// Should open cairo surface
			Plot( float max_x, float max_y );
			// Plots a point to the surface
			void plot_point( float x, float y );
	};
}
#endif
