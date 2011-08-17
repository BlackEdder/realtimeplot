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
	PlotConfig config = PlotConfig();
	config.min_x = -10;
	config.max_x = 90;
	config.min_y = -20;
	config.max_y = 30;
	for (size_t i=0; i<1; ++i) {
		Plot pl = Plot();
		pl.point( 1, 10 );
		std::cout << "Wait till plot gets closed" << std::endl;
	}
	std::cout << "Continuing" << std::endl;
	for (size_t i=0; i<5; ++i) {
		Plot pl = Plot();
		pl.point( 1, 10 );
		pl.detach = true;
	}
	std::cout << "Continuing" << std::endl;

	sleep( 5 );
	
	return 0;
}
