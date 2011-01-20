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

		void testVertexEqual()
		{
			Vertex v = Vertex( 0, 1 );
			Vertex v2 = Vertex( 0, 1 );
			TS_ASSERT_EQUALS( v, v2 );
		}

		void testVertexScaling()
		{
			Vertex v = Vertex( 2, 1 );
			TS_ASSERT_EQUALS( v.scalar(2) , Vertex( 4, 2 ) );
		}

		void testVertexAddition() {
			TS_ASSERT_EQUALS( Vertex(1,2)+Vertex(3,4), Vertex(4,6) );
		}

		void testEdge() {
			boost::shared_ptr<Vertex> pV0( new Vertex(0,1));
			boost::shared_ptr<Vertex> pV1( new Vertex(2,2));
			Edge e = Edge( pV0, pV1 );
			TS_ASSERT_EQUALS( (*e.pV0.get()), Vertex( 0,1 ) );
			TS_ASSERT_EQUALS( (*e.pV1.get()), Vertex( 2,2 ) );
			e = Edge( pV1, pV0 );
			TS_ASSERT_EQUALS( (*e.pV0.get()), Vertex( 0,1 ) );
			TS_ASSERT_EQUALS( (*e.pV1.get()), Vertex( 2,2 ) );
		}

		void testEdgeIntersect() {
			Edge e1 = Edge( boost::shared_ptr<Vertex>( new Vertex( 0,0 ) ),
				boost::shared_ptr<Vertex>( new Vertex( 1,1 ) ) );
			Edge e2 = Edge( boost::shared_ptr<Vertex>( new Vertex( 0,1 ) ),
				boost::shared_ptr<Vertex>( new Vertex( 1,0 ) ) );
			Edge e3 = Edge( boost::shared_ptr<Vertex>( new Vertex( 0,1 ) ),
				boost::shared_ptr<Vertex>( new Vertex( 0.4,0.6 ) ) );
			TS_ASSERT( e1.intersect( e2 ) );
			TS_ASSERT( !e1.intersect( e3 ) );
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

			TS_ASSERT_EQUALS( d.corners[0]->next, d.corners[1] );
			TS_ASSERT_EQUALS( d.corners[0]->previous, d.corners[2] );
			TS_ASSERT_EQUALS( d.corners[1]->next, d.corners[2] );
			TS_ASSERT_EQUALS( d.corners[1]->previous, d.corners[0] );
			
		}

		void testDelaunayFindTriangleFirstPoint()
		{
			Delaunay d = Delaunay( 0,1, 0,1 );
			boost::shared_ptr<Vertex> v( new Vertex( 0.5, 0.4 ) );
			TS_ASSERT_EQUALS( d.findTriangle( v ), d.triangles[0] );
		}

		void testDelaunayCreateNewTriangles()
		{
			Delaunay d = Delaunay( 0,1, 0,1 );
			boost::shared_ptr<Vertex> pVertex( new Vertex( 0.5, 0.4 ) );
			d.createNewTriangles( pVertex, d.findTriangle( pVertex ) );
			TS_ASSERT_EQUALS( d.triangles.size(), 3 );
			TS_ASSERT_EQUALS( d.vertices.size(), 4 );
			TS_ASSERT_EQUALS( d.corners.size(), 9 );
		}
};
