#include "cairo_plot.h"

using namespace cairo_plot;

int main() {
	PlotConfig config = PlotConfig();
	config.min_x = -10;
	config.max_x = 100;
	config.min_y = -20;
	config.max_y = 30;

	Plot pl = Plot( &config );
	pl.axes_surface->write_to_png("axes.png");
	std::cout << "Written" << std::endl;
	for (int i=0; i<10;++i) {
		pl.point( (i-1)*10, (i-4)*5 );
		sleep(1);
	}
	pl.point( 10, 10 );
	pl.plot_surface->write_to_png("plot.png");
	std::cout << "Written" << std::endl;
	return 0;
}
