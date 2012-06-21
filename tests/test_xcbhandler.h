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
			DisplayHandler *xcb1 = XcbHandler::Instance();
			DisplayHandler *xcb2 = XcbHandler::Instance();
			TS_ASSERT_EQUALS( xcb1, xcb2 );
		}

		/*
		 * Opening windows
		 */
		void testXcbOpenWindow() {
			DisplayHandler *xcb = XcbHandler::Instance();
			xcb->open_window(500,500);
			//size_t win = xcb->open_window(500,500);
		}
};
	
