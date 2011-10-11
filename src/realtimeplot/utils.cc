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
#include "realtimeplot/utils.h"
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

		size_t bin_id( double min_x, double bin_width, double data ) {
			return (data-min_x)/bin_width;
		}
	};
};
