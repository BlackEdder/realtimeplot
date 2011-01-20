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
			if (x5<x1 || x5<x3 || x5>x2 || x5>x4)
				return false;
			//normalize ys
			if (y1>y2) {
				float tmp_y = y1; y1 = y2; y2 = tmp_y;
			}
			if (y3>y4) {
				float tmp_y = y3; y3 = y4; y4 = tmp_y;
			}
			if (y5<y1 || y5<y3 || y5>y2 || y5>y4)
				return false;
			return true;
		}


		Delaunay::Delaunay( float min_x, float max_x, float min_y, float max_y )
		{
			//Setup gVertices vTriangles oOpposites
			float dx = (max_x - min_x);
			float dy = (max_y - min_y);
			vertices.push_back( boost::shared_ptr<Vertex>( new Vertex( min_x - 0.5*dx, min_y ) ) );
			vertices.push_back( boost::shared_ptr<Vertex>( new Vertex( max_x - 0.5*dx, max_y + dy ) ) );
			vertices.push_back( boost::shared_ptr<Vertex>( new Vertex( max_x + 0.5*dx, min_y ) ) );
			
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

			//Else go on as in the article
			return tr;
		}
		void Delaunay::createNewTriangles( boost::shared_ptr<Vertex> vertex,
			boost::shared_ptr<Triangle> triangle ) {
			vertices.push_back( vertex );
			boost::shared_ptr<Vertex> old_vertex = triangle->corners[0]->vertex;
			triangle->corners[0]->vertex = vertex;

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
			c1p->vertex = old_vertex;
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
			c2n->vertex = triangle->corners[0]->next->vertex;
			c2n->triangle = tr2;
			tr2->corners.push_back( c2n );
			c2->next = c2n;
			c2n->previous = c2;
			boost::shared_ptr<Corner> c2p( new Corner() );
			c2p->vertex = old_vertex;
			c2p->triangle = tr1;
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
				triangle->corners[0]->next->opposite = c1->opposite;
			}
			if (triangle->corners[0]->previous->opposite) {
				c2->opposite = triangle->corners[0]->previous->opposite;
				triangle->corners[0]->previous->opposite = c2->opposite;
			}

			triangle->corners[0]->previous->opposite = c2p;
			c2p->opposite = triangle->corners[0]->previous;
			triangle->corners[0]->next->opposite = c1p;
			c1p->opposite = triangle->corners[0]->next;
			c1n->opposite = c2n;
			c2n->opposite = c1n;

		}
	};
};

