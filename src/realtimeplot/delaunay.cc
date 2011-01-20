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
			gVertices.push_back( Vertex( min_x - 0.5*dx, min_y ) );
			gVertices.push_back( Vertex( max_x - 0.5*dx, max_y + dy ) );
			gVertices.push_back( Vertex( max_x + 0.5*dx, min_y ) );
			vTriangles.push_back( Triplet( 0, 1, 2 ) );

			//Only one triangle, so no opposites
			oOpposites.push_back( Triplet( -1, -1, -1 ) );
		}
	};
};

