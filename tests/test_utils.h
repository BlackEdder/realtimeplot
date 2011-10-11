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
#include <cxxtest/TestSuite.h>

#include "realtimeplot/utils.h"

using namespace realtimeplot::utils;

class TestUtils : public CxxTest::TestSuite {
	public:
		void testBinId() {
			TS_ASSERT_EQUALS( 0, bin_id( 0, 0.1, 0 ) );
			TS_ASSERT_EQUALS( 0, bin_id( 0, 0.1, 0.09 ) );
			TS_ASSERT_EQUALS( 1, bin_id( 0, 0.1, 0.1 ) );
			TS_ASSERT_EQUALS( 1, bin_id( 0, 0.1, 0.11 ) );
			TS_ASSERT_EQUALS( 1, bin_id( 0, 1.1, 1.11 ) );
		}

		//std::vector<double> bins( double min_x, double max_x, size_t no_bins, 
		//		const std::vector<double> &data )
		void testBins() {
			std::vector<double> data;
			data.push_back( 0.0999999 );
			data.push_back( 0.1000001 );
			data.push_back( 0.1 );
			data.push_back( 0.0 );
			data.push_back( 0.1999999 );
			std::vector<double> bins = calculate_bins( 0, 0.2, 2, data );
			TS_ASSERT_EQUALS( bins.size(), 2 );
			TS_ASSERT_EQUALS( bins[0], 2 );
			TS_ASSERT_EQUALS( bins[1], 3 );
		}
};

