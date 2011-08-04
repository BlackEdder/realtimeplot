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
			XcbHandler *xcb1 = XcbHandler::Instance();
			XcbHandler *xcb2 = XcbHandler::Instance();
			TS_ASSERT_EQUALS( xcb1, xcb2 );
		}
		
		/*
		 * x connection
		 */
		void testXcbConnection() {
			XcbHandler *xcb = XcbHandler::Instance();
			xcb->connection;
		}


		/*
		 * Opening windows
		 */
		void testXcbOpenWindow() {
			XcbHandler *xcb = XcbHandler::Instance();
			xcb_drawable_t win = xcb->open_window(500,500);
		}
};
	
