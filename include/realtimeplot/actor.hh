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

#ifndef REALTIMEPLOT_ACTOR_H
#define REALTIMEPLOT_ACTOR_H

#include "cppa/cppa.hpp"

#include "realtimeplot/backend.h"

namespace realtimeplot {
	using namespace cppa;
	class Actor : public cppa::event_based_actor {
		public:
			Actor();

			void init();

			boost::shared_ptr<BackendPlot> pBPlot;

	};
};
#endif
