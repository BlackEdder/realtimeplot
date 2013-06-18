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

#include <iostream>
#include<vector>

namespace realtimeplot {
	/**
	 * \brief Some useful classes
	 */
	namespace utils {
		/**
		 * \brief Struct to hold information that defines the bins
		 */
		struct bintype {
			double min;
			double width;
			std::vector<double> bins;
		};

		/**
		 * \brief Function to bin data
		 */
		std::vector<double> calculate_bins( double min_x, double max_x, size_t no_bins, 
				const std::vector<double> &data );

		/**
		 * \brief Return bin id/counter of a data point 
		 */
		size_t bin_id( double min_x, double bin_width, double data );

		/**
		 * \brief Combine two sets of bins into one that has the given bin width
		 *
		 * Will cover the entire set of data;
		 */
		bintype combine_bins( const bintype &bins1, const bintype &bins2, double width );

		/**
		 * \brief Return a minimal range of bins that together cover at least the given percentage of data
		 * 
		 * Will start around the  bin that currently has most data in it.
		 *
		 */
		std::vector<size_t> range_of_bins_covering( double percentage,
				std::vector<double> bins );

		/**
			\brief Util function to turn doubles into strings
			*/
		std::string stringify(double x);
	};
};
#endif
