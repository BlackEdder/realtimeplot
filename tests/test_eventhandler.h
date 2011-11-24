/*
  -------------------------------------------------------------------
  
  Copyright (C) 2011, Edwin van Leeuwen
  
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
#include <cxxtest/TestSuite.h>

#include "realtimeplot/events.h"

using namespace realtimeplot;

class TestEventHandler : public CxxTest::TestSuite 
{
	public:
		PlotConfig conf;
		void setUp() {
			conf = PlotConfig();
			conf.display = false;
		}
		void testOpenClose() {
			boost::shared_ptr<EventHandler> pEventHandler( 
					new EventHandler() );
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenPlotEvent( conf, pEventHandler ) ) );
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new FinalEvent(pEventHandler, false ) ) );
		}
};
	
