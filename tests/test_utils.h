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

		void testBins() {
			std::vector<double> data;
			data.push_back( 0.1999999 );
			data.push_back( 0.2000001 );
			data.push_back( 0.2 );
			data.push_back( 0.1 );
			data.push_back( 0.2999999 );
			data.push_back( 0.3999999 );
			std::vector<double> bins = calculate_bins( 0.1, 0.3, 2, data );
			TS_ASSERT_EQUALS( bins.size(), 2 );
			TS_ASSERT_EQUALS( bins[0], 2 );
			TS_ASSERT_EQUALS( bins[1], 3 );
		}

		void testRangeCover() {
			std::vector<double> bins(10);
			std::vector<size_t> range;
			range = range_of_bins_covering( 0.8, bins );
			TS_ASSERT_EQUALS( range[0], 0 );
			TS_ASSERT_EQUALS( range.back(), 9 );

			bins[3] = 4;
			range = range_of_bins_covering( 0.8, bins );
			TS_ASSERT_EQUALS( range[0], 3 );
			TS_ASSERT_EQUALS( range.size(), 1 );

			bins[5] = 4;
			range = range_of_bins_covering( 0.8, bins );
			TS_ASSERT_EQUALS( range[0], 1 );
			TS_ASSERT_EQUALS( range[1], 5 );
			TS_ASSERT_EQUALS( range.size(), 2 );
			bins[6] = 1;
			range = range_of_bins_covering( 0.8, bins );
			TS_ASSERT_EQUALS( range[0], 1 );
			TS_ASSERT_EQUALS( range[1], 5 );
			TS_ASSERT_EQUALS( range.size(), 2 );
		}

		void testStringify() {
			TS_ASSERT_EQUALS( stringify( 1 ), "1" );
			TS_ASSERT_EQUALS( stringify( 10 ), "10" );
			TS_ASSERT_EQUALS( stringify( 3 ), "3" );
		}
};

