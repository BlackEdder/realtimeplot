#include "realtimeplot/realtimeplot.h"

using namespace realtimeplot;

inline double rnorm( double mean, double sigma ) {
	double x1, x2, w;
	do {
		double rnd1 = double(rand())/RAND_MAX;
		double rnd2 = double(rand())/RAND_MAX;
		x1 = 2.0 * rnd1 - 1.0;
		x2 = 2.0 * rnd2 - 1.0;
		w = x1 * x1 + x2 * x2;
	} while ( w >= 1.0 || w == 0 );

	w = sqrt( (-2.0 * log( w ) ) / w );
	return mean + sigma * x1 * w;
}


void plot() {
	std::vector<boost::shared_ptr<Histogram3D> > hists;
	for (size_t i = 0; i<1; ++i) {

		hists.push_back( boost::shared_ptr<Histogram3D>( new Histogram3D( -1, 1, -1, 1, 40 ) ) );
		hists.push_back( boost::shared_ptr<Histogram3D>( new Histogram3D( -1, 1, -1, 1, 40 ) ) );
		hists.push_back( boost::shared_ptr<Histogram3D>( new Histogram3D( -1, 1, -1, 1, 40 ) ) );
	}
	for (size_t i = 0; i<1000; ++i) {
		for (size_t j=0; j<hists.size(); ++j) {
			hists[j]->add_data( rnorm(0,0.1), rnorm( 0,0.1 ), false );
		}
	}
	for (size_t j=0; j<hists.size(); ++j) {
		hists[j]->plot();
	}
}

int main() {
	plot(); 
	plot(); 
	plot(); 
	return 0;
}
