#include "cairo_plot.h"

using namespace cairo_plot;

int main() {
	PlotConfig config = PlotConfig();
    Plot pl = Plot( &config );
    for (int y=0; y<10000; ++y) {
        usleep(1000);
        pl.plot_point( 50, y );
    }
	return 0;
}
