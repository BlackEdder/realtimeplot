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
#include <limits>
#include "realtimeplot/utils.h"

#include <sstream>
namespace realtimeplot {
	namespace utils {
		std::vector<double> calculate_bins( double min_x, double max_x, size_t no_bins, 
				const std::vector<double> &data ) {
			std::vector<double> binned(no_bins);
			double bin_width = (max_x-min_x)/no_bins;
			for (size_t i=0; i<data.size(); ++i) {
				if (data[i]>=min_x && data[i]<max_x)
					++binned[bin_id(min_x, bin_width, data[i])];
			}
			return binned;
		}

		bintype calculate_bintype( double min_x, double max_x, size_t no_bins, 
				const std::vector<double> &data ) {
			bintype binned;
			binned.min = min_x;
			binned.width = (max_x-min_x)/no_bins;
			binned.bins = calculate_bins( min_x, max_x, no_bins, data );
			return binned;
		}


		size_t bin_id( double min_x, double bin_width, double data ) {
			return (data-min_x)/bin_width;
		}

		std::vector<size_t> range_of_bins_covering( double percentage,
				std::vector<double> bins ) {
			std::vector<size_t> range;
			double total_covered = 0;
			double sum = 0;
			size_t max_id = 0;
			// Find biggest bin and calculate sum;
			for( size_t i=0; i<bins.size(); ++i ) {
				if (bins[i]>bins[max_id]) {
					max_id = i; 
				}
				sum += bins[i];
			}

			if (sum == 0) {
				range.push_back( 0 );
				range.push_back( bins.size()-1 );
				return range;
			}

			total_covered = bins[max_id]/sum;
			size_t count = 1;
			size_t first = max_id; size_t last = max_id;
			while (total_covered < percentage) {
				if (max_id>count) {
					total_covered += bins[max_id-count]/sum;
					first = max_id - count;

				}
				if (max_id+count < bins.size()) {
					total_covered += bins[max_id+count]/sum;
					last = max_id + count;
				}
				++count;
			}

			if (first == last)
				range.push_back( first );
			else {
				range.push_back( first );
				range.push_back( last );
			}
			return range;
		}

		std::string stringify(double x)
		{
			std::ostringstream o;
			o.precision( 2 );
			o << x;
			return o.str();
		}   


	};
};
