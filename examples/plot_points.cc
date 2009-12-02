#include "cairo_plot.h"

using namespace cairo_plot;

int main() {
	Plot pl = Plot( 10, 5 );
	pl.plot_point( 0.5, 0.5 );
	return 0;

}
