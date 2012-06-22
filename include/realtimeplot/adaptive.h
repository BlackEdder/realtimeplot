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

#ifndef REALTIMEPLOT_ADAPTIVE_H
#define REALTIMEPLOT_ADAPTIVE_H
#include "realtimeplot/backend.h"
class TestAdaptive;

namespace realtimeplot {
	/**
	 * \brief EventHandler that keeps no of events around.
	 *
	 * Instead of deleting events will keep them around
	 * for some time.
	 */
	class AdaptiveEventHandler : public EventHandler {
		public:
			bool adaptive;
			/**
			 * \brief Create AdaptiveEventHandler
			 *
			 * Optional parameter that defines the number of events to keep around
			 */
			AdaptiveEventHandler( size_t no_events = 100 ); 

			/**
			 * \brief Reprocess events in the processed_events list
			 * 
			 * Only called from within process_events_thread, so no locking needed
			 */
			virtual void reprocess();


			friend class ::TestAdaptive;
		protected:
			size_t max_no_events;
			std::list<boost::shared_ptr<Event> > processed_events;

			void process_events();
	};

	/**
	 * \brief Adaptive plot. Will automatically choose reasonable x/y range
	 *
	 * Works by keeping last x (default 100) events and whenever a new event
	 * falls outside the current region -> Redraw the whole plot
	 */
	class BackendAdaptivePlot : public BackendPlot {
		public:
			/**
			 * \brief Creates an Adaptive Plot 
			 *
			 * One can specify the default number of events to "remember". After more
			 * events than that we won't adapt any more.
			 */
			BackendAdaptivePlot( PlotConfig config, 
				boost::shared_ptr<EventHandler> pEventHandler );

			bool within_plot_bounds( float x, float y );

			/**
			 * \brief Adapt the plot to the new x/y coords
			 */
			void adapt();
			friend class ::TestAdaptive;
		private:
			double max_data_x, max_data_y, min_data_x, min_data_y;

			bool adapting;

			/**
			 * \brief Convenience method to convert EventHandler 
			 * 	pointer to AdaptiveEventHandler
			 */
			boost::shared_ptr<AdaptiveEventHandler> convert_to_adaptive( 
					boost::shared_ptr<EventHandler> pEventHandler );

	};


};
#endif
