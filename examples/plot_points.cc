#include "cairo_plot.h"

using namespace cairo_plot;

int main() {
	
    Plot pl = Plot( 100, 100 );
	pl.plot_point( 0.5, 0.5 );
    sleep(5);
	return 0;

}
