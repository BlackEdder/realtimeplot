/*
  -------------------------------------------------------------------
  
  Copyright (C) 2012, Edwin van Leeuwen
  
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

template< typename Result >
struct custom_policy
{
   static Result abort()
   {
		 std::cout << "Unimplemented 1" << std::endl;
		 throw;
       // ...
   }
   static void checkpoint( const char* file, int line )
   {
		 // Ignore for now, seems to be called even when everything is ok.
 		 //std::cout << "Unimplemented 2" << std::endl;
   }
   template< typename Context >
   static void unexpected_call( const Context& context )
   {
 		 std::cout << "Unimplemented 3" << std::endl;
		 throw;
        // ...
   }
   template< typename Context >
   static void expected_call( const Context& context, const char* file, int line )
   {
		 // Ignore for now, seems to be called even when everything is ok.
 		 //std::cout << "Unimplemented 2" << std::endl;
   }
   template< typename Context >
   static void missing_action( const Context& context, const char* file, int line )
   {
 		 std::cout << "Unimplemented 5" << std::endl;
		 throw;
        // ...
   }
   template< typename Context >
   static void sequence_failed( const Context& context, const char* file, int line )
   {
 		 std::cout << "Unimplemented 6" << std::endl;
		 throw;
        // ...
   }
   template< typename Context >
   static void verification_failed( const Context& context, const char* file, int line )
   {
 		 std::cout << "Unimplemented 7" << std::endl;
		 throw;
        // ...
   }
   template< typename Context >
   static void untriggered_expectation( const Context& context, const char* file, int line )
   {
		 std::ostringstream out;
		 out << file << ":" << line << ": Error: " << context;
		 TS_FAIL(out.str());
   }
};

#define MOCK_ERROR_POLICY custom_policy
#include "turtle/mock.hpp"

#include "realtimeplot/eventhandler.h"
#include "realtimeplot/adaptive.h"

using namespace realtimeplot;

MOCK_BASE_CLASS( MockEvent, realtimeplot::Event )
{
	MOCK_METHOD( execute, 1 );
};

// This makes it easy to ensure that events are directly executed (no threading)
class MockAdaptiveEventHandler2 : public AdaptiveEventHandler {
	public:
		MockAdaptiveEventHandler2() : AdaptiveEventHandler(), no_reprocess( 0 ) {
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



