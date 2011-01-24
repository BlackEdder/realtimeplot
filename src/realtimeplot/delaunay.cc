/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, Edwin van Leeuwen

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

#include "realtimeplot/delaunay.h"
#include "ostream"
namespace realtimeplot {
	namespace delaunay {
		bool Edge::intersect( Edge& e ) {
			//Calculate intersection point
			float x1 = pV0->x;
			float x2 = pV1->x;
			float y1 = pV0->y;
			float y2 = pV1->y;
			float x3 = e.pV0->x;
			float x4 = e.pV1->x;
			float y3 = e.pV0->y;
			float y4 = e.pV1->y;

			//Intersection point (see wikipedia line-line intersection)
			float x5 = ((x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4))/
				((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
			float y5 = ((x1*y2-y1*x2)*(y3-y4)-(y1-y2)*(x3*y4-y3*x4))/
				((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
			if (x5<=x1 || x5<=x3 || x5>=x2 || x5>=x4)
				return false;
			//normalize ys
			if (y1>y2) {
				float tmp_y = y1; y1 = y2; y2 = tmp_y;
			}
			if (y3>y4) {
				float tmp_y = y3; y3 = y4; y4 = tmp_y;
			}
			if (y5<=y1 || y5<=y3 || y5>=y2 || y5>=y4)
				return false;
			return true;
		}

		bool Triangle::inTriangle( boost::shared_ptr<Vertex> pV ) {
			// Compute vectors        
			Vertex v0 = (*corners[2]->vertex.get())-(*corners[0]->vertex.get());
			Vertex v1 = (*corners[1]->vertex.get())-(*corners[0]->vertex.get());
			Vertex v2 = (*pV.get())-(*corners[0]->vertex.get());
			// Compute dot products
			float dot00 = v0.dot( v0 );
			float dot01 = v0.dot( v1 );
			float dot02 = v0.dot( v2 );
			float dot11 = v1.dot( v1 );
			float dot12 = v1.dot( v2 );

			// Compute barycentric coordinates
			float invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
			float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
			float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

			// Check if point is in triangle
			return (u > 0) && (v > 0) && (u + v < 1);
		}

		bool Triangle::inCircumCircle( boost::shared_ptr<Vertex> pV ) {
			// For now recalculate everything everytime, since vertices of corners
			// can change between calls
			//
			// Should be adapted to deal with cases where all vertices are on same line
			

			float x0 = corners[0]->vertex->x;
			float y0 = corners[0]->vertex->y;

			float x1 = corners[1]->vertex->x;
			float y1 = corners[1]->vertex->y;

			float x2 = corners[2]->vertex->x;
			float y2 = corners[2]->vertex->y;

			float y10 = y1 - y0;
			float y21 = y2 - y1;

			float m0 = - (x1 - x0) / y10;
			float m1 = - (x2 - x1) / y21;

			float mx0 = (x0 + x1) * .5F;
			float my0 = (y0 + y1) * .5F;

			float mx1 = (x1 + x2) * .5F;
			float my1 = (y1 + y2) * .5F;

			float x = (m0 * mx0 - m1 * mx1 + my1 - my0) / (m0 - m1);
			float y = m0 * (x - mx0) + my0;
			Vertex center = Vertex( x, y );

			float dx = x0 - center.x;
			float dy = y0 - center.y;
			float m_R2 = dx * dx + dy * dy;	// the radius of the circumcircle, squared

			Vertex dist = (*pV)-center;		// the distance between v and the circle center
			float dist2 = dist.x * dist.x + dist.y * dist.y;		// squared
			return dist2 < m_R2;
		}


		Delaunay::Delaunay( float min_x, float max_x, float min_y, float max_y )
		{
			//Setup super triangle (slightly larger than plotting area)
			float dx = (max_x - min_x);
			float dy = (max_y - min_y);
			vertices.push_back( boost::shared_ptr<Vertex>( new Vertex( min_x - 0.501*dx, 
							max_y - 1.001*dy ) ) );
			vertices.push_back( boost::shared_ptr<Vertex>( new Vertex( max_x - 0.5*dx, 
							max_y + 1.001*dy ) ) );
			vertices.push_back( boost::shared_ptr<Vertex>( new Vertex( max_x + 0.501*dx, 
							max_y - 1.001*dy ) ) );
			
			triangles.push_back( boost::shared_ptr<Triangle>( new Triangle ) );

			for (size_t i = 0; i<3; ++i) {
				boost::shared_ptr<Corner> corner (new Corner());
				corner->vertex = vertices[i];
				corner->triangle = triangles[0];
				corners.push_back( corner );
				triangles[0]->corners.push_back( corner );
			}

			for (size_t i = 0; i<3; ++i) {
				int j = i+1;
				if (j>=3)
					j = j%3;
				corners[i]->next = corners[j];
				j = i-1;
				if (j<0)
					j = 3+j;
				corners[i]->previous = corners[j];
			}
		}

		void Delaunay::add_data( boost::shared_ptr<Vertex> vertex ) {
			boost::shared_ptr<Triangle> triangle =
				findTriangle( vertex );
			createNewTriangles( vertex, triangle );
		}

		boost::shared_ptr<Triangle> 
			Delaunay::findTriangle( boost::shared_ptr<Vertex> vertex ) 
		{
			//Choose random triangle
			boost::shared_ptr<Triangle> tr = triangles[0];

			//Vertex in that triangle (using the barycenter)
			Vertex start_v = tr->corners[0]->vertex->scalar( 1/3.0 ) + 
				tr->corners[1]->vertex->scalar( 1/3.0 ) +
				tr->corners[2]->vertex->scalar( 1/3.0 );
			boost::shared_ptr<Vertex> pStart_v( new Vertex( start_v.x, start_v.y ) );

			//Find edge that a line between that vertex and the provided vertex passes through
			//(if none found, then return current triangle)
			size_t i = 0;
			bool passed = false;
			boost::shared_ptr<Corner> current_corner;
			Edge eline = Edge( pStart_v, vertex );
			while (i<3 && !passed) {
				Edge etriangle = Edge( tr->corners[i]->vertex, tr->corners[i]->next->vertex );
				if (etriangle.intersect( eline )) {
					passed = true;
					current_corner = tr->corners[i];
				}
				++i;
			}

			if (!passed) {
				//The point is in the current triangle
				return tr;
			}
			current_corner = current_corner->previous->opposite;
			tr = current_corner->triangle;

			while (passed) {
				//Is it to the right?
				/*std::cout << "eline: " << eline << std::endl;
				std::cout << (*vertex) << std::endl;
				std::cout << (*current_corner->vertex) << std::endl;*/
				Edge etriangle = Edge( current_corner->vertex, current_corner->next->vertex );
				/*std::cout << etriangle << std::endl;
				std::cout << (*current_corner->triangle) << std::endl;
				std::cout << (*current_corner->next->triangle) << std::endl;*/
				if (etriangle.intersect( eline )) {
					current_corner = current_corner->previous->opposite;
					tr = current_corner->triangle;
				} else {
					etriangle = Edge( current_corner->vertex, 
							current_corner->previous->vertex );
					//Is it to the left?
					if (etriangle.intersect( eline )) {
						current_corner = current_corner->next->opposite;
						tr = current_corner->triangle;
					} else {
						passed = false;
					}

				}
			}

			return tr;
		}

		void Delaunay::createNewTriangles( boost::shared_ptr<Vertex> vertex,
			boost::shared_ptr<Triangle> triangle ) {
			vertices.push_back( vertex );
			boost::shared_ptr<Vertex> old_vertex = triangle->corners[0]->vertex;
			triangle->corners[0]->vertex = vertex;

			// Lot's of housekeeping. Most should be abstracted away by the Triangle/Corner
			// classes, but isn't yet.
			boost::shared_ptr<Triangle> tr1( new Triangle() );
			boost::shared_ptr<Corner> c1( new Corner() );
			c1->vertex = vertex;
			c1->triangle = tr1;
			tr1->corners.push_back( c1 );
			boost::shared_ptr<Corner> c1n( new Corner() );
			c1n->vertex = triangle->corners[0]->previous->vertex;
			c1n->triangle = tr1;
			tr1->corners.push_back( c1n );
			c1->next = c1n;
			c1n->previous = c1;
			boost::shared_ptr<Corner> c1p( new Corner() );
			c1p->vertex =  old_vertex;
			c1p->triangle = tr1;
			tr1->corners.push_back( c1p );
			c1->previous = c1p;
			c1p->next = c1;
			c1n->next = c1p;
			c1p->previous = c1n;

			boost::shared_ptr<Triangle> tr2( new Triangle() );
			boost::shared_ptr<Corner> c2( new Corner() );
			c2->vertex = vertex;
			c2->triangle = tr2;
			tr2->corners.push_back( c2 );
			boost::shared_ptr<Corner> c2n( new Corner() );
			c2n->vertex = old_vertex;		
			c2n->triangle = tr2;
			tr2->corners.push_back( c2n );
			c2->next = c2n;
			c2n->previous = c2;
			boost::shared_ptr<Corner> c2p( new Corner() );
			c2p->vertex =  triangle->corners[0]->next->vertex; 
			c2p->triangle = tr2;
			tr2->corners.push_back( c2p );
			c2->previous = c2p;
			c2p->next = c2;
			c2n->next = c2p;
			c2p->previous = c2n;

			triangles.push_back( tr1 );
			triangles.push_back( tr2 );
			corners.push_back( c1 );
			corners.push_back( c1n );
			corners.push_back( c1p );
			corners.push_back( c2 );
			corners.push_back( c2n );
			corners.push_back( c2p );

			//Opposites
			if (triangle->corners[0]->next->opposite) {
				c1->opposite = triangle->corners[0]->next->opposite;
				triangle->corners[0]->next->opposite->opposite = c1;
			}
			if (triangle->corners[0]->previous->opposite) {
				c2->opposite = triangle->corners[0]->previous->opposite;
				triangle->corners[0]->previous->opposite->opposite = c2;
			}

			triangle->corners[0]->previous->opposite = c2n;
			c2n->opposite = triangle->corners[0]->previous;
			triangle->corners[0]->next->opposite = c1p;
			c1p->opposite = triangle->corners[0]->next;
			c1n->opposite = c2p;
			c2p->opposite = c1n;
			flipEdgesRecursively( triangle->corners[0] );
			flipEdgesRecursively( c1 );
			flipEdgesRecursively( c2 );
		}

		void Delaunay::flipEdgesRecursively( boost::shared_ptr<Corner> pC ) {
			if (!pC->opposite) {
			} else {
			// Check if pC->opposite is inside the circumcircle of pC->triangle
			if (pC->triangle->inCircumCircle( pC->opposite->vertex ) )
			{
				// If so then flip (lot's of housekeeping again)
				boost::shared_ptr<Corner> pCP = pC->previous;
				boost::shared_ptr<Corner> pCN = pC->next;
				boost::shared_ptr<Corner> pCO = pC->opposite;
				boost::shared_ptr<Corner> pCON = pC->opposite->next;
				boost::shared_ptr<Corner> pCOP = pC->opposite->previous;

				boost::shared_ptr<Corner> pCOR = pC->opposite->previous->opposite;
				boost::shared_ptr<Corner> pCOL = pC->opposite->next->opposite;
				boost::shared_ptr<Corner> pCR = pC->previous->opposite;
				boost::shared_ptr<Corner> pCL = pC->next->opposite;

				pCN->vertex = pCO->vertex;
				pCON->vertex = pC->vertex;


				pC->opposite = pCOR;
				if (pCOR) pCOR->opposite = pC;
				pCP->opposite = pCOP;
				pCOP->opposite = pCP;
				pCON->opposite = pCOL;
				if (pCOL) pCOL->opposite = pCON;
				pCO->opposite = pCR;
				if (pCR) pCR->opposite = pCO;
				pCN->opposite = pCL;
				if (pCL) pCL->opposite = pCN;
	
			// Call flipEdgesRecursively for pC (with new triangle) 
			// and old pC.o.n (now pC.p.o.p)
				flipEdgesRecursively( pC );
				flipEdgesRecursively( pCON );
			}
			}
		}
	};
};

std::ostream & operator<<(std::ostream &out,
		const realtimeplot::delaunay::Vertex &v ) {
	out << "(" << v.x << "," << v.y << ")";
}

std::ostream & operator<<(std::ostream &out,
		const realtimeplot::delaunay::Edge &e ) {
	out << (*e.pV0) << "--" << (*e.pV1);

}
std::ostream & operator<<(std::ostream &out,
		const realtimeplot::delaunay::Triangle &t ) {
	for (size_t i=0; i<2; ++i) {
		out << (*t.corners[i]->vertex) << "--"; 
	}
	out << (*t.corners[2]->vertex); 
}

