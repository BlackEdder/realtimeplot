#include "cairo_plot.h"

using namespace cairo_plot;

int main() {
	PlotConfig config = PlotConfig();
 	config.min_x = 0;
	config.max_x = 1000;
	config.min_y = 0;
	config.max_y = 1000;
	config.fixed_plot_area = false;

  Plot pl = Plot( &config );
    for (int y=0; y<10000; ++y) {
        if (float(std::rand())/RAND_MAX < 0.01) {
            usleep(10000);
        }
        pl.point( -y, 50+900*float(std::rand())/RAND_MAX );
    }
	return 0;
}
