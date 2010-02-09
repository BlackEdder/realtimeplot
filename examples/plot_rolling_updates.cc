#include "cairo_plot/plot.h"

using namespace cairo_plot;

int main() {
	PlotConfig config = PlotConfig();
	config.min_x = -10;
	config.max_x = 90;
	config.min_y = -20;
	config.max_y = 30;

	Plot pl = Plot( config );
	pl.point( 80, 0 );
	for (int i=0; i<30;++i) {
		pl.number( 10+10*float(std::rand())/RAND_MAX, (i)*10, i );
		usleep(500000);
	}
	return 0;
}
