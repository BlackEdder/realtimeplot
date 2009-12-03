#include "cairo_plot.h"

using namespace cairo_plot;

int main() {
	
    Plot pl = Plot( PlotConfig() );
    sleep(5);
	pl.plot_point( 0.5, 0.5 );
    sleep(5);
	pl.plot_point( 33, 5 );
    sleep(5);
	pl.plot_point( 50, 25 );
	return 0;

}
