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

#ifndef REALTIMEPLOT_XCBHANDLER_H
#define REALTIMEPLOT_XCBHANDLER_H


namespace realtimeplot {
	/**
	 *	\brief Singular class that maintains an x_connection and handles xevents
	 *
	 * This class is used in the following way. Each plot will request a window from
	 * this class. With that request it will also give provide a ptr to itself.
	 *
	 * The handler starts a thread that handles all x events and sends those events to
	 * the plot that belongs with the window. 
	 *
	 * When a plot is closed it will need to tell the xcb handler that, which will need to
	 * remove the window/plot from its list of windows/plots. Or (probably better) xcbhandler
	 * can keep an eye out for unmap/destroy events and remove it itself.
	 */
	class XcbHandler {
	};
}
#endif
