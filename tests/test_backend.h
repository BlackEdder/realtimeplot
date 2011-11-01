/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, 2011 Edwin van Leeuwen

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
#include <cxxtest/TestSuite.h>

#include "realtimeplot/backend.h"

using namespace realtimeplot;

class TestBackend : public CxxTest::TestSuite 
{
	public:
		void testPlotConfig() {
			PlotConfig conf = PlotConfig();
			TS_ASSERT_EQUALS( conf.max_x, 1 );
			TS_ASSERT_EQUALS( conf.min_x, 0 );
			TS_ASSERT_EQUALS( conf.max_y, 1 );
			TS_ASSERT_EQUALS( conf.min_y, 0 );
		}

		void testVertex3DCrossProduct() {
			boost::shared_ptr<Vertex3D> pV1( new Vertex3D( 1,0,0 ) );
			boost::shared_ptr<Vertex3D> pV2( new Vertex3D( 0,1,0 ) );
			boost::shared_ptr<Vertex3D> pV3 = pV1->crossProduct( pV2 );
			TS_ASSERT_EQUALS( pV3->x, 0 );
			TS_ASSERT_EQUALS( pV3->y, 0 );
			TS_ASSERT_EQUALS( pV3->z, 1 );
		}

		void testTriangle3DgradientVector() {
			boost::shared_ptr<Vertex3D> pV1( new Vertex3D( 1,0,0 ) );
			boost::shared_ptr<Vertex3D> pV2( new Vertex3D( 0,1,0 ) );
			boost::shared_ptr<Vertex3D> pV3( new Vertex3D( 1,1,1 ) );

			Triangle3D tr = Triangle3D();
			tr.vertices.push_back( pV1 );
			tr.vertices.push_back( pV2 );
			tr.vertices.push_back( pV3 );
			std::vector<boost::shared_ptr<Vertex3D> > v;
			v = tr.gradientVector();
			TS_ASSERT_EQUALS( v[0]->x, 1 );
			TS_ASSERT_EQUALS( v[0]->y, 0 );
			TS_ASSERT_EQUALS( v[0]->z, 0 );
			TS_ASSERT_EQUALS( v[1]->x, 1.5 );
			TS_ASSERT_EQUALS( v[1]->y, 0.5 );
			TS_ASSERT_EQUALS( v[1]->z, 1 );

			TS_ASSERT_EQUALS( pV3->x, 1 );
			TS_ASSERT_EQUALS( pV3->y, 1 );
			TS_ASSERT_EQUALS( pV3->z, 1 );

			Triangle3D tr2 = Triangle3D();
			tr2.vertices.push_back( pV3 );
			tr2.vertices.push_back( pV1 );
			tr2.vertices.push_back( pV2 );
			v = tr2.gradientVector();
			TS_ASSERT_EQUALS( v[0]->x, 1 );
			TS_ASSERT_EQUALS( v[0]->y, 0 );
			TS_ASSERT_EQUALS( v[0]->z, 0 );
			TS_ASSERT_EQUALS( v[1]->x, 1.5 );
			TS_ASSERT_EQUALS( v[1]->y, 0.5 );
			TS_ASSERT_EQUALS( v[1]->z, 1 );
		}
};
	
