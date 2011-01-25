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

#include "realtimeplot/plot.h"

using namespace realtimeplot;

int main() {
	HeightMap hm = HeightMap();

	for (size_t i=0; i<100; ++i) {
		float x = 8*float(std::rand())/RAND_MAX;
		float y = 50*float(std::rand())/RAND_MAX;
		std::cout << x << " " << y << std::endl;
		hm.add_data( x, y, x*y, false );
	}
	hm.add_data( 8*float(std::rand())/RAND_MAX, 50*float(std::rand())/RAND_MAX, 1 );
	

	/*hm.add_data( 5.1, 20, 1 );
	sleep(1);
	hm.add_data( 9.1, 2.2, 1 );
	sleep(1);
	hm.add_data( 5, 15, 1 );*/
	/*HeightMap hm2 = HeightMap();

	for (size_t i=0; i<5; ++i) {
		for (size_t j=0; j<5; ++j) {
		float x = 5.0/5*i + 0.0001*float(std::rand())/RAND_MAX;
		float y = 50.0/5*j + 0.0001*float(std::rand())/RAND_MAX;
		std::cout << x << " " << y << std::endl;
		hm2.add_data( x, y, 1, true );
		sleep(1);
		}
	}
	hm2.add_data( 8*float(std::rand())/RAND_MAX, 50*float(std::rand())/RAND_MAX, 1 );*/
	
	return 0;
}
