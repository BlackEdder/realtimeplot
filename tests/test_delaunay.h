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

		void testDelaunaySetup()
		{
			Delaunay d = Delaunay( 0,1, 0,1 );
			TS_ASSERT_EQUALS( d.triangles.size(), 1 );
			TS_ASSERT_EQUALS( d.vertices.size(), 3 );
			TS_ASSERT_EQUALS( d.corners.size(), 3 );
			
			TS_ASSERT_EQUALS( d.corners[0]->triangle, d.triangles[0] );
			TS_ASSERT_EQUALS( d.corners[0]->vertex, d.vertices[0] );
			TS_ASSERT_EQUALS( d.triangles[0]->corners[1], d.corners[1] );
		}
};
