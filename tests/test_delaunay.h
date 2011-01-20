// MyTestSuite.h
#include <cxxtest/TestSuite.h>

#include "realtimeplot/delaunay.h"

using namespace realtimeplot::delaunay;

class TestDelaunay : public CxxTest::TestSuite 
{
	public:
		void testVertex()
		{
			Vertex v = Vertex( 0, 1 );
			TS_ASSERT_EQUALS( v.x, 0 );
			TS_ASSERT_EQUALS( v.y, 1 );
		}

		void testTriplet()
		{
			Triplet t = Triplet( 1, 2, 3 );
			TS_ASSERT_EQUALS( t[0], 1 );
			TS_ASSERT_EQUALS( t[1], 2 );
			TS_ASSERT_EQUALS( t[2], 3 );
		}

		void testTripletLoops()
		{
			Triplet t = Triplet( 1, 2, 3 );
			TS_ASSERT_EQUALS( t[-1], 3 );
			TS_ASSERT_EQUALS( t[-4], 3 );
			TS_ASSERT_EQUALS( t[3], 1 );
			TS_ASSERT_EQUALS( t[4], 2 );
		}


		void testDelaunaySetup()
		{
			Delaunay d = Delaunay( 0,1, 0,1 );
			TS_ASSERT_EQUALS( d.gVertices[0].x, -0.5  );
			TS_ASSERT_EQUALS( d.gVertices[0].y, 0  );
			TS_ASSERT_EQUALS( d.gVertices[1].x, 0.5  );
			TS_ASSERT_EQUALS( d.gVertices[1].y, 2  );
			TS_ASSERT_EQUALS( d.gVertices[2].x, 1.5  );
			TS_ASSERT_EQUALS( d.gVertices[2].y, 0  );

			TS_ASSERT_EQUALS( d.vTriangles[0][0], 0 );
			TS_ASSERT_EQUALS( d.vTriangles[0][1], 1 );
			TS_ASSERT_EQUALS( d.vTriangles[0][2], 2 );

			TS_ASSERT_EQUALS( d.oOpposites[0][0], -1 );
			TS_ASSERT_EQUALS( d.oOpposites[0][1], -1 );
			TS_ASSERT_EQUALS( d.oOpposites[0][2], -1 );
		}
};
