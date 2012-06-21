/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, 2011 Edwin van Leeuwen

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
#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H
#include <iostream>
#include <fstream>
#include <string>

#include "realtimeplot/eventhandler.h"
#include "realtimeplot/backend.h"
#include "realtimeplot/adaptive.h"
#include "realtimeplot/utils.h"

using namespace realtimeplot;
class MockBackendPlot : public realtimeplot::BackendPlot {
	public:
		std::string state;

		MockBackendPlot( PlotConfig conf, 
				boost::shared_ptr<EventHandler> pEH ) : BackendPlot( conf, pEH ) {};

		void text( float x, float y, std::string &text ) {
			state += text;
		}
};

class MockEvent : public realtimeplot::Event {
	public: 
		MockEvent( size_t id ) : realtimeplot::Event(), id( id ) {}
    virtual void execute( boost::shared_ptr<realtimeplot::BackendPlot> &pBPlot ) {
			std::string s_id = realtimeplot::utils::stringify( id );
			pBPlot->text(0, 0, s_id );
		}
	private:
		size_t id;
};

class MockOpenPlotEvent : public Event {
	public:
		MockOpenPlotEvent( boost::shared_ptr<MockBackendPlot> pl ) :
			pl( pl )
		{}

		virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
			pBPlot = boost::static_pointer_cast< 
				BackendPlot, MockBackendPlot >( pl );
		}
	private:
		boost::shared_ptr<MockBackendPlot> pl;

};

class MockAdaptiveEventHandler : public AdaptiveEventHandler {
	public:
		MockAdaptiveEventHandler() : AdaptiveEventHandler(), no_reprocess( 0 ) {
			processing_events = false; 
			window_closed = true;
			pEventProcessingThrd->join();
			processing_events = true;
			window_closed = false;
		}

		void add_event(	boost::shared_ptr< Event > 	pEvent, bool 	high_priority = false ) {
			pEvent->execute( pBPlot );
			processed_events.push_back( pEvent );
		}

		void reprocess() {
			++no_reprocess;
			AdaptiveEventHandler::reprocess();
		}

		size_t no_reprocess;
};


bool compare_files( std::string fn1, std::string fn2 ) {
	std::ifstream file1 (fn1.c_str(), std::ios::in|std::ios::binary);
	std::ifstream file2 (fn2.c_str(), std::ios::in|std::ios::binary);

  if (file1.is_open() && file2.is_open())
  {
    while ( file1.good() && file2.good() )
    {
			if (file1.get() != file2.get())
				return false;
      
    }
		if (file1.good() != file2.good())
			return false;
  } else {
		return false;
	}
	return true;
}

bool check_plot( std::string plotname ) {
	return compare_files( "tests/tmp_plots/test_" + plotname + ".png", 
		"tests/data/correct_plots/test_" + plotname + ".png" ); 
}

std::string fn( std::string plotname ) {
	return "tests/tmp_plots/test_" + plotname + ".png";
}
#endif

