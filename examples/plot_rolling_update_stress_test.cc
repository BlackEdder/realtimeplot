#include "cairo_plot/plot.h"

using namespace cairo_plot;

int main() {
    PlotConfig config = PlotConfig();
    config.min_x = 0;
    config.max_x = 1000;
    config.min_y = 0;
    config.max_y = 1000;
    config.fixed_plot_area = false;
    config.overlap = 0.99;

    Plot pl = Plot( config );
    int y = 0;
    while (1) {
        if (float(std::rand())/RAND_MAX < 0.01) {
            //usleep(10000);
        }
        pl.point( -y, 50+900*float(std::rand())/RAND_MAX );
        ++y;
    }
    return 0;
}
