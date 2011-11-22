/*
  -------------------------------------------------------------------
  
  Copyright (C) 2010, Edwin van Leeuwen
  
  This file is part of RealTimePlot.
  
  RealTimePlot is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  RealTimePlot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with RealTimePlot. If not, see <http://www.gnu.org/licenses/>.

  -------------------------------------------------------------------
*/

#include <cstdlib>
#include <cmath>

#include "realtimeplot/plot.h"

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

int main() {
	//Histogram3D pl = Histogram3D( -1, 1, -1, 1, 50 );
	Histogram3D pl = Histogram3D( 50, 60 );
	for( size_t i=0; i<100000; ++i ) {
		pl.add_data( rnorm( 0, 0.2 ), rnorm( 0, 0.4 ), false );
		/*std::cout << rnorm( 0, 0.1 ) << std::endl;
		std::cout << rnorm( 0, 0.2 ) << std::endl;*/
	}
	pl.add_data( rnorm( 0, 0.2 ), rnorm( 0, 0.4 ), true );
	sleep(1);
	pl.calculate_height_scaling();
	return 0;
}
