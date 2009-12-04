#include "cairo_plot.h"

using namespace cairo_plot;

int main() {
	PlotConfig config = PlotConfig();
    Plot pl = Plot( &config );
    for (int y=0; y<10000; ++y) {
        if (float(std::rand())/RAND_MAX < 0.01) {
            usleep(100000);
        }
        pl.plot_point( 50+10*float(std::rand())/RAND_MAX, y );
    }
	return 0;
}
