#ifndef CAIRO_PLOT_H
#define CAIRO_PLOT_H

namespace cairo_plot {
	class Plot {
		public: 
		Plot( float max_x, float max_y );
		void plot_point( float x, float y );
	};
}
#endif
