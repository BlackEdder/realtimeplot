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
			}
			triangles[0]->corners = corners;
		}
	};
};

