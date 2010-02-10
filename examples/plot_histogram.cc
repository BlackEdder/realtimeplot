#include "cairo_plot/plot.h"

using namespace cairo_plot;

int main() {
	std::vector<double> data;
	for (int i=0; i<10; ++i) {
		data.push_back( 1 );
		data.push_back( 2 );
		data.push_back( 2.1 );
	}

	Histogram hist = Histogram();
	hist.set_data( data );

	sleep( 1 );

	std::vector<double> values;
	std::vector<int> counts;
	values.push_back( 1 );
	values.push_back( 2 );
	values.push_back( 2.1 );
	counts.push_back( 15 );
	counts.push_back( 10 );
	counts.push_back( 10 );

	hist.set_counts_data( values, counts );
}
