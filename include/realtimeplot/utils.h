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
#ifndef UTILS_H
#define UTILS_H

#include<vector>

namespace realtimeplot {
	/**
	 * \brief Some useful classes
	 */
	namespace utils {

		/**
		 * \brief Function to bin data
		 */
		std::vector<double> calculate_bins( double min_x, double max_x, size_t no_bins, 
				const std::vector<double> &data );

		/**
		 * \brief Return bin id/counter of a data point 
		 */
		size_t bin_id( double min_x, double bin_width, double data );

	};
};
#endif
