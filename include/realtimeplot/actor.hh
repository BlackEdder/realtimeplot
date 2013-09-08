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
#include "boost/shared_ptr.hpp"

namespace realtimeplot {
	using namespace cppa;
	class BackendPlot; // Forward declaration

	/**
	 * \brief Cache messages before forwarding messages to another actor
	 *
	 * Will resend all the messages on a atom("resend")
	 */
	class CacheActor : public event_based_actor {
		public:
			CacheActor( actor_ptr actor );

			void init();	

		protected:
			std::vector<any_tuple> _cache;
			actor_ptr _actor;
	};



	class Actor : public cppa::event_based_actor {
		public:
			Actor();

			void init();

			boost::shared_ptr<BackendPlot> pBPlot;
		private:
			int new_line_id; // line id used when new line is called
	};
};
#endif
