#include <cxxtest/TestSuite.h>

#include "realtimeplot/xcbhandler.h"

using namespace realtimeplot;

class TestXcbHandler : public CxxTest::TestSuite 
{
	public:
		/*
		 * Make sure we can only create one object
		 */
		void testSingleton() {
			XcbHandler xcb1 = XcbHandler::create();
			XcbHandler xcb2 = XcbHandler::create();
			TS_ASSERT_EQUALS( xcb1, xcb2 );
		}
		
		void testConstructorPrivate() {}
		/*
		 * x connection
		 */
		void testXcbConnection() {}

		/*
		 * Opening windows
		 */
		void testXcbOpenWindow() {}
};
	
