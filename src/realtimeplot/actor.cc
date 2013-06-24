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

#include "realtimeplot/actor.hh"

namespace realtimeplot {
	using namespace cppa;
	Actor::Actor() {}

	void Actor::init() {
		become (
			on(atom("open"), arg_match ) >> [this] ( const std::string &type ) {
				PlotConfig conf = PlotConfig(); // Default config
				conf.area = 50*50;
				conf.min_x = -5;
				conf.max_x = 5;
				conf.min_y = -5;
				conf.max_y = 5;
				conf.margin_x = 20;
				conf.margin_y = 20;
				conf.display = false;
				if (type == "plot") {
					pBPlot = boost::shared_ptr<BackendPlot>( new BackendPlot( conf, 
						boost::shared_ptr<EventHandler>()  ) );
				}
			},
			on(atom("save"), arg_match ) >> [this] ( const std::string &filename ) {
				pBPlot->save( filename );
			},
			on(atom("point"), arg_match ) >> [this] ( const int &x, const int &y ) {
				pBPlot->point( x, y );
			},
			on(atom("point"), arg_match ) >> [this] ( const double &x, const double &y ) {
				pBPlot->point( x, y );
			},
			on(atom("close")) >> [=]() { reply(atom("DONE")); },
			others() >> [] { aout << "Message not understood" << std::endl; }
		);
	}
};
