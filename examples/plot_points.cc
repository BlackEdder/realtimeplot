#include "cairo_plot/plot.h"

using namespace cairo_plot;

int main() {
	PlotConfig config = PlotConfig();
	config.min_x = -0.111;
	config.max_x = 1.04;
	config.min_y = -20;
	config.max_y = 30;
	//config.margin_x = 200;
	//config.font = "sans 12";
	config.aspect_ratio = 0.5;
	config.point_size = 2;
	config.xlabel = "Very Long Long label";

	Plot pl = Plot( config );
	for (int i=0; i<10;++i) {
		pl.point( (i-1)*0.1, (i-4)*5 );
		sleep(1);
        if (i==5) {
            pl.clear();
            sleep(1);
        }
	}
	pl.point( 0.02, 10 );
    pl.save( "plot_points.png" );
	return 0;
}
