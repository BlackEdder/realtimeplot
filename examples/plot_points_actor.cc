/*
	 -------------------------------------------------------------------

	 Copyright (C) 2013, Edwin van Leeuwen

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

#include "realtimeplot/realtimeplot.h"

using namespace realtimeplot;

int main() {
	actor_ptr actor = spawn<Actor>();
	actor << make_any_tuple( atom("open"), "plot" );
	actor << make_any_tuple( atom("xrange"), -1.0, 1.0 );
	actor << make_any_tuple( atom("yrange"), -21.0, 26.0 );

	Color color = Color::black();
	for (int i=0; i<10;++i) {
		actor << make_any_tuple( atom("color"), color );
		actor << make_any_tuple( atom("point"), (i-1)*0.1, (i-4)*5.0);
		sleep(1);
		if (i==2) {
			color = Color::red();
		}
		if (i==3)
			color = Color::blue();
		if (i==4)
			color = Color::green();

	}
	sleep(5);
	actor << make_any_tuple( atom("close") );
	await_all_others_done();
	return 0;
}
