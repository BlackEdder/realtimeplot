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
	std::vector<double> data;
	for (int i=0; i<10; ++i) {
		data.push_back( 1 );
		data.push_back( 2 );
		data.push_back( 2.1 );
	}

	Histogram hist = Histogram();
	Histogram hist2 = Histogram();
	hist2.frequency = true;


	//hist.frequency = true;
	hist.set_data( data );
	hist.add_data( 1 );
	hist.add_data( 5 );

	sleep( 1 );


	hist2.add_data( 1 );
	hist2.add_data( 5 );
	hist2.add_data( 5 );
}
