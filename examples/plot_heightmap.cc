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
#include "math.h"

using namespace realtimeplot;

int main() {
	HeightMap hm = HeightMap(0,1,2,4);

	for (size_t i=0; i<10000; ++i) {
		float x = float(std::rand())/RAND_MAX;
		float y = 2+2*float(std::rand())/RAND_MAX;
		//std::cout << x << " " << y << " " << pow(((x-0.5)*(y-3)),2) << std::endl;
		hm.add_data( x, y, pow(pow(((x-0.5)*(y-3)),2),0.5), false );
	}
	float x = float(std::rand())/RAND_MAX;
	float y = 2+2*float(std::rand())/RAND_MAX;
	//std::cout << x << " " << y << " " << pow(((x-0.5)*(y-3)),2) << std::endl;
	hm.add_data( x, y, pow(pow(((x-0.5)*(y-3)),2),0.5), true );


	// A regular grid, which can lead to numerical problems in delaunay algorithms.
	// Our algorithm deals reasonably well with this case, but not perfectly.
	// If you want to do this it is better/ideal to actually make the 
	// grid slightly randomized, i.e. by adding small random values to each x/y 
	// coordinate
	HeightMap hm2 = HeightMap(0,1,0,1);

	for (size_t i=0; i<50; ++i) {
		for (size_t j=0; j<50; ++j) {
			float x = 1.0/49*i;
			float y = 1.0/49*j;
			//std::cout << x << " " << y << std::endl;
			bool show = false;
			if (i==49 && j==49)
				show = true;
			hm2.add_data( x, y, pow(pow(((x-0.5)*(y-0.5)),2),0.5), show );
			//sleep(1);
		}
	}
	sleep(1);
	//Optimize color distribution/usage
	hm2.calculate_height_scaling();
	hm.calculate_height_scaling();

	return 0;
}
