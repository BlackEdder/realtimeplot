#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"

#include "cairo_plot/plot.h"

using namespace Rice;
using namespace cairo_plot;

extern "C"
void Init_rb_cairo_plot()
{
    /*Data_Type<PlotConfig> rb_cPlotConfig =
    define_class<PlotConfig>("PlotConfig")
    .define_constructor(Constructor<PlotConfig>());*/
    //.define_method("hello", &Test::hello);
    //
    Data_Type<Plot> rb_cPlot =
        define_class<cairo_plot::Plot>("Plot")
        .define_constructor(Constructor<cairo_plot::Plot>())
				.define_method("clear", &cairo_plot::Plot::clear);
}
