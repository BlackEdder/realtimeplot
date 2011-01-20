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
#ifndef DELAUNAY_H
#define DELAUNAY_H
#include <iostream>
#include <ostream>
#include <vector>
#include <boost/shared_ptr.hpp>
namespace realtimeplot {
	namespace delaunay {
		/*
		 * This implementation follows the "corner method" as described in:
		 * Venkates and Lee, An imporved incremental Delaunay Triangulation Algorithm
		 */
		class Vertex {
			public:
				float x, y;
				Vertex( float x, float y ) :
					x(x), y(y)
			{}
		};

		class Corner;
		class Triangle {
			public:
				std::vector<boost::shared_ptr<Corner> > corners;
		};

		class Corner {
			public:
				boost::shared_ptr<Vertex> vertex;
				boost::shared_ptr<Corner> next;
				boost::shared_ptr<Corner> previous;
				boost::shared_ptr<Corner> opposite;
				boost::shared_ptr<Triangle> triangle;
				Corner() {};
		};

		class Delaunay {
			public:
				//For testing
				Delaunay( float xmin, float xmax, float ymin, float ymax );

			/*private:
				friend class TestDelaunay;*/
				std::vector<boost::shared_ptr<Vertex> > vertices;
				std::vector<boost::shared_ptr<Triangle> > triangles;
				std::vector<boost::shared_ptr<Corner> > corners;
		};
	};
};
#endif
