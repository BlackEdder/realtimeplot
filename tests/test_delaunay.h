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


		void testTriangleVertexInTriangle() {
			boost::shared_ptr<Corner> c1( new Corner );
			boost::shared_ptr<Corner> c2( new Corner );
			boost::shared_ptr<Corner> c3( new Corner );
			c1->vertex = boost::shared_ptr<Vertex>( new Vertex( 0,0 ) );
			c2->vertex = boost::shared_ptr<Vertex>( new Vertex( 0,1 ) );
			c3->vertex = boost::shared_ptr<Vertex>( new Vertex( 1,0 ) );

			Triangle tr = Triangle();
			tr.corners.push_back( c1 );
			tr.corners.push_back( c2 );
			tr.corners.push_back( c3 );

			TS_ASSERT( tr.inTriangle( boost::shared_ptr<Vertex>( new Vertex( 0.4, 0.4 ) ) ) );
			TS_ASSERT( !tr.inTriangle( boost::shared_ptr<Vertex>( new Vertex( 1, 1 ) ) ));
		}

		void testTriangleInCircumCircle() {
			//Not a very thorough test

			boost::shared_ptr<Corner> c1( new Corner );
			boost::shared_ptr<Corner> c2( new Corner );
			boost::shared_ptr<Corner> c3( new Corner );
			c1->vertex = boost::shared_ptr<Vertex>( new Vertex( 0,0 ) );
			c2->vertex = boost::shared_ptr<Vertex>( new Vertex( 0,1 ) );
			c3->vertex = boost::shared_ptr<Vertex>( new Vertex( 1,0 ) );

			Triangle tr = Triangle();
			tr.corners.push_back( c1 );
			tr.corners.push_back( c2 );
			tr.corners.push_back( c3 );

			TS_ASSERT( 
					tr.inCircumCircle( boost::shared_ptr<Vertex>( new Vertex( 0.4, 0.4 ) ) ) );
			TS_ASSERT( 
					!tr.inCircumCircle( boost::shared_ptr<Vertex>( new Vertex( 1.1, 1.1 ) ) ));
		}

		// Used to check consistency of Delaunay state
		void checkTriangleConsistency( boost::shared_ptr<Triangle> tr ) {
			TS_ASSERT_EQUALS( tr->corners[0]->next, tr->corners[1] )
			TS_ASSERT_EQUALS( tr->corners[1]->next, tr->corners[2] )
			TS_ASSERT_EQUALS( tr->corners[2]->next, tr->corners[0] )
			TS_ASSERT_EQUALS( tr->corners[0], tr->corners[1]->previous )
			TS_ASSERT_EQUALS( tr->corners[1], tr->corners[2]->previous )
			TS_ASSERT_EQUALS( tr->corners[2], tr->corners[0]->previous )
			TS_ASSERT_EQUALS( tr->corners[0]->triangle, tr )
			TS_ASSERT_EQUALS( tr->corners[1]->triangle, tr )
			TS_ASSERT_EQUALS( tr->corners[2]->triangle, tr )
		}

		void checkOppositesConsistency( boost::shared_ptr<Corner> pC ) {
			TS_ASSERT_EQUALS( pC->previous->vertex, pC->opposite->next->vertex )
			TS_ASSERT_EQUALS( pC->next->vertex, pC->opposite->previous->vertex )
		}

		void checkDelaunayConsistency( Delaunay &d ) {
			//Should start with some general stats like number of vertices etc
			TS_ASSERT_EQUALS( d.triangles.size(), 2*d.vertices.size() - 2 - 3  );
			TS_ASSERT_EQUALS( d.corners.size(), d.triangles.size()*3);

			if (d.triangles.size()>1) {
				std::map<boost::shared_ptr<Vertex>, size_t> no_triangles_per_vertex;
				std::map<boost::shared_ptr<Vertex>, size_t>::iterator ntpv_iter;
				for (size_t i=0; i<d.vertices.size(); ++i) {
					no_triangles_per_vertex[d.vertices[i]] = 0;
				}
				for (size_t i=0; i<d.corners.size(); ++i) {
					++no_triangles_per_vertex[d.corners[i]->vertex];
				}
				for (ntpv_iter = no_triangles_per_vertex.begin(); 
						ntpv_iter != no_triangles_per_vertex.end(); ++ntpv_iter) {
					TS_ASSERT( ntpv_iter->second > 1 );
				}
			}

			// Triangles/next/previous
			for (size_t i=0; i<d.triangles.size(); ++i) {
				checkTriangleConsistency( d.triangles[i] );
			}

			// Opposites
			size_t lacking_opposites = 0;
			for (size_t i=0; i<d.corners.size(); ++i) {
				if (!d.corners[i]->opposite)
					++lacking_opposites;
				else
					checkOppositesConsistency( d.corners[i] );
			}
			TS_ASSERT_EQUALS( lacking_opposites, 3 );
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

			checkDelaunayConsistency( d );
			
		}

		void testDelaunayAddData() {
			Delaunay d = Delaunay( 0,10, 0,50 );
			checkDelaunayConsistency( d );
			boost::shared_ptr<Vertex> vertex( new Vertex( 6.7215, 19.7191 ) );
			boost::shared_ptr<Triangle> triangle =
				d.findTriangle( vertex );
			TS_ASSERT( triangle->inTriangle( vertex ) );
			checkDelaunayConsistency( d );
			d.createNewTriangles( vertex, triangle );
			checkDelaunayConsistency( d );
			vertex.reset( new Vertex( 3.20183, 44.5765 ) );
			triangle =
				d.findTriangle( vertex );
			TS_ASSERT( triangle->inTriangle( vertex ) );
			checkDelaunayConsistency( d );
			d.createNewTriangles( vertex, triangle );
			checkDelaunayConsistency( d );
	}

		void testDelaunayFindTriangleFirstPoint()
		{
			Delaunay d = Delaunay( 0,1, 0,1 );
			boost::shared_ptr<Vertex> v( new Vertex( 0.5, 0.4 ) );
			TS_ASSERT_EQUALS( d.findTriangle( v ), d.triangles[0] );
			checkDelaunayConsistency( d );

			Delaunay d2 = Delaunay( 0,10, 0,50 );
			checkDelaunayConsistency( d2 );
			boost::shared_ptr<Vertex> pV0( new Vertex( 5.1, 20 ) );
			d2.add_data( pV0 );
			checkDelaunayConsistency( d2 );
			boost::shared_ptr<Vertex> pV1( new Vertex( 9.1, 2.2 ) );
			TS_ASSERT( d2.findTriangle(pV1)->inTriangle( pV1 ) );
			d2.add_data( pV1 );
			boost::shared_ptr<Vertex> pV2( new Vertex( 5, 15 ) );
			TS_ASSERT( d2.findTriangle(pV2)->inTriangle( pV2 ) );
			d2.add_data( pV2 );
			checkDelaunayConsistency( d2 );

		}

		void testDelaunayCreateNewTriangles()
		{
			Delaunay d = Delaunay( 0,1, 0,1 );
			boost::shared_ptr<Vertex> pVertex( new Vertex( 0.5, 0.4 ) );
			d.createNewTriangles( pVertex, d.findTriangle( pVertex ) );
			TS_ASSERT_EQUALS( d.triangles.size(), 3 );
			TS_ASSERT_EQUALS( d.vertices.size(), 4 );
			TS_ASSERT_EQUALS( d.corners.size(), 9 );
			checkDelaunayConsistency( d );
		}

		void testDelaunayManyTimes()
		{
			Delaunay d = Delaunay( 0,10, 0,50 );
			for (size_t i=0; i<1; ++i) {
				float x = 8*float(std::rand())/RAND_MAX;
				float y = 50*float(std::rand())/RAND_MAX;
				boost::shared_ptr<Vertex> vertex( new Vertex( x, y ) );
				boost::shared_ptr<Triangle> triangle =
					d.findTriangle( vertex );
				checkDelaunayConsistency( d );
				TS_ASSERT( triangle->inTriangle( vertex ) );
				d.createNewTriangles( vertex, triangle );
				checkDelaunayConsistency( d );
			}
		}
	
};
