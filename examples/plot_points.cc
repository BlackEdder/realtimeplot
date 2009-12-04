#include "cairo_plot.h"

using namespace cairo_plot;

int main() {
	PlotConfig config = PlotConfig();
    Plot pl = Plot( &config );
	pl.plot_point( 0.5, 0.5 );
    sleep(1);
	pl.plot_point( 33, 5 );
    sleep(1);
	pl.plot_point( 50, 50 );
    sleep(1);
	pl.plot_point( 50, 60 );
	pl.plot_point( 50, 80 );
	return 0;

}
