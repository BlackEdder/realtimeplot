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
#ifndef REALTIMEPLOT_EVENTS_H
#define REALTIMEPLOT_EVENTS_H
#include "realtimeplot/eventhandler.h"
#include "realtimeplot/backend.h"
#include "realtimeplot/adaptive.h"


namespace realtimeplot {

    class PlotConfig;
    /**
     * \brief Event that is send when the config gets updated
     */
    class ConfigEvent : public Event {
        public:
            ConfigEvent( PlotConfig new_config );
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
        private:
            PlotConfig config;
     };

    /**
     * \brief Event that can contain multiple events
     *
     * Should be used when one wants to be certain that the events
     * added are processed directly after each other. For example if
     * one wants to set a color, plot a point and unset the color, 
     * without the danger that another process suddenly plots something
     * else in the mean time.
		 */
    class MultipleEvents : public Event {
        public:
            MultipleEvents( std::vector<boost::shared_ptr<Event> > events );
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
        private:
            std::vector<boost::shared_ptr<Event> > events;
    };
		
		/**
		 * \brief Opens a plot
		 */
		class OpenPlotEvent : public Event {
			public:
				OpenPlotEvent( PlotConfig plot_conf, boost::shared_ptr<EventHandler> pEventHandler );
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
			private:
				PlotConfig plot_conf;
				boost::shared_ptr<EventHandler> pEventHandler;
		};

		/**
		 * \brief Resets a plot with a new config
		 */
		class ResetEvent : public Event {
			public:
				ResetEvent( PlotConfig plot_conf );
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
			private:
				PlotConfig plot_conf;
		};


    /**
     * \brief Sets the color used to draw on the plot
     *
     * Can be followed by a restore event to restore the color to its previous value
     */
    class SetColorEvent : public Event {
        public:
            SetColorEvent( Color color );
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
        private:
            Color color;
    };

    /**
     * \brief Restores the plot to former (saved) settings
     *
     * Can (for example) be used after SetColorEvent to restore former Color
     */
    class RestoreEvent : public Event {
        public:
            RestoreEvent() {}
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {pBPlot->restore();}
    };

    /**
      \brief Event that draws a point at x, y with Color color
      */
    class PointEvent : public Event {
        public:
            PointEvent( float x, float y );
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
        private:
            float x_crd, y_crd;
    };
 
		/**
      \brief Event that scales the surface size
      */
    class ScaleXSurfaceEvent : public Event {
        public:
            ScaleXSurfaceEvent( size_t width, size_t height );
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
        private:
            size_t width, height;
    };

		class PauseEvent : public Event {
			public:
				PauseEvent() {};
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
					if (pBPlot->pause_display) {
						pBPlot->pause_display = false;
						pBPlot->display();
					}
					else 
						pBPlot->pause_display = true;
				}
		};

		class MoveEvent : public Event {
			public:
				MoveEvent( int direction_x, int direction_y ) :
				dx( direction_x ), dy(direction_y)
			{
			}
			virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
				pBPlot->move( dx, dy );
			}
			private:
				int dx, dy;
		};

		class MovePixelsEvent : public Event {
			public:
				MovePixelsEvent( int direction_x, int direction_y ) :
				dx( direction_x ), dy(direction_y)
			{
			}
			virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
				pBPlot->move_pixels( dx, dy );
			}
			private:
				int dx, dy;
		};


		class ZoomEvent : public Event {
			public:
				ZoomEvent( double scale ) :
				scale( scale )
			{
			}
			virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
				pBPlot->zoom( scale );
			}
			private:
				double scale;
		};

		class ZoomAroundEvent : public Event {
			public:
				ZoomAroundEvent( double scale, float x, float y ) :
				scale( scale ), x( x ), y( y )
			{
			}
			virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
				pBPlot->zoom_around( scale, x, y );
			}
			private:
				double scale, x, y;
		};



    class RectangleEvent : public Event {
        public:
            RectangleEvent( float min_x, float min_y, float width_x, float width_y,
								bool fill, Color color );
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
        private:
						float min_x, min_y, width_x, width_y;
						bool fill;
						Color color;
    };

 		/**
		 \brief Event that adds a point to an existing line

		 If no line exists yet a new one will be started with starting point x, y
         @param id can be any int and identifies to which line a point belongs

		 */
     class LineAddEvent : public Event {
        public:
            LineAddEvent( float x, float y, int id, Color color );
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
        private:
            float x_crd, y_crd;
            int id;
						Color color;
    };

    class TitleEvent : public Event {
        public:
            TitleEvent( std::string title ) 
							: title(title) {}
						virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
							pBPlot->title( title );
						}
        private:
						std::string title;
    };

     class TextEvent : public Event {
        public:
            TextEvent( float x, float y, std::string text ) 
							: x(x), y(y), text(text) {}
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
							pBPlot->text( x, y, text );
						}
        private:
            float x, y;
						std::string text;
    };
 
    /**
      \brief Event to save the current plot to the specified file
      */
    class SaveEvent : public Event {
        public:
            SaveEvent( std::string filename );
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
        private:
            std::string filename;
    };

		/**
		 \brief Event to redraw the current plot onto the xSurface
		 */
    class DisplayEvent : public Event {
        public:
            DisplayEvent() {};
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
							pBPlot->display();
						}
    };

		/**
		 \brief Event to clear the current plot
		 */
    class ClearEvent : public Event {
        public:
            ClearEvent();
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
    };
		/**
		 \brief Event to close the current plot (window)
		 */
    class CloseWindowEvent : public Event {
        public:
            CloseWindowEvent();
            virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
    };

		/**
		 * \brief Stop processing events after this events
		 *
		 * By default this causes the program to keep open if the xwindow is still open.
		 * Alternatively, it can force the thread to stop listening to new events and
		 * therefore causes the xwindow to be closed immediately.
		 */
		class FinalEvent : public Event {
			public:
				FinalEvent(boost::shared_ptr<EventHandler> pEventHandler, bool force = false);
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
			private:
				boost::shared_ptr<EventHandler> pEventHandler;
				bool force;
		};

		/*
		 * Histogram specific events
		 */

		/**
		 * \brief Opens a Histogram 
		 */
		class OpenHistogramEvent : public Event {
			public:
				OpenHistogramEvent( PlotConfig plot_conf, bool frequency, 
						size_t no_bins, boost::shared_ptr<EventHandler> pEventHandler ) 
					: plot_conf( plot_conf ), frequency( frequency ), no_bins( no_bins ),
						pEventHandler( pEventHandler )
			{}

				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
					pBPlot.reset( new BackendHistogram( plot_conf, 
								frequency, no_bins, pEventHandler ) ); 
				}

			private:
				PlotConfig plot_conf;
				bool frequency;
				size_t no_bins;
				boost::shared_ptr<EventHandler> pEventHandler;
		};

		class HistDataEvent : public Event {
			public:
				HistDataEvent( double new_data )
					: new_data( new_data )
			{}
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
					boost::static_pointer_cast<BackendHistogram, 
						BackendPlot>(pBPlot)->add_data( new_data );
				}
			private:
				double new_data;
		};

		class HistOptimizeEvent : public Event {
			public:
				HistOptimizeEvent( double proportion ) 
				: proportion( proportion ) 
				{};
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
					boost::static_pointer_cast<BackendHistogram, 
						BackendPlot>(pBPlot)->optimize_bounds( proportion );
				}
			private:
				double proportion;
		};

		class HistPlotEvent : public Event {
			public:
				HistPlotEvent() {};
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
					boost::static_pointer_cast<BackendHistogram, 
						BackendPlot>(pBPlot)->plot();
				}
		};

		/*
		 * Histogram3D specific events
		 */

		/**
		 * \brief Opens a 3D Histogram 
		 */
		class OpenHistogram3DEvent : public Event {
			public:
				OpenHistogram3DEvent( PlotConfig plot_conf, 
						size_t no_bins_x, size_t no_bins_y,
						boost::shared_ptr<EventHandler> pEventHandler ) 
					: plot_conf( plot_conf ), no_bins_x( no_bins_x ),
						no_bins_y( no_bins_y ), 
						pEventHandler( pEventHandler )
			{}

				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
					pBPlot.reset( new BackendHistogram3D( plot_conf, 
								pEventHandler, no_bins_x, no_bins_y ) ); 
				}

			private:
				PlotConfig plot_conf;
				size_t no_bins_x, no_bins_y;
				boost::shared_ptr<EventHandler> pEventHandler;
		};

		class Hist3DDataEvent : public Event {
			public:
				Hist3DDataEvent( double x, double y )
					: x( x ), y( y )
			{}
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
					boost::static_pointer_cast<BackendHistogram3D, 
						BackendPlot>(pBPlot)->add_data( x, y );
				}
			private:
				double x, y;
		};

		class Hist3DPlotEvent : public Event {
			public:
				Hist3DPlotEvent() {};
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
					boost::static_pointer_cast<BackendHistogram3D, 
						BackendPlot>(pBPlot)->plot();
				}
		};
		/**
		 * \brief Causes Histogram3D to calculate it's optimal coloring scheme
		 */
		class Hist3DHeightScalingEvent	: public Event {
			public:
				Hist3DHeightScalingEvent( ) {};
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
				boost::static_pointer_cast<BackendHistogram3D, BackendPlot>( 
						pBPlot )->calculate_height_scaling();
				}
		};

		/*
		 * Adaptive specific events
		 */

		/**
		 * \brief Opens a Adaptive 
		 */
		class AdaptiveOpenPlotEvent : public Event {
			public:
				AdaptiveOpenPlotEvent( PlotConfig plot_conf, boost::shared_ptr<EventHandler> pEventHandler );
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
			private:
				PlotConfig plot_conf;
				boost::shared_ptr<EventHandler> pEventHandler;
		};

		/*
		 * HeightMap specific events
		 */

		/**
		 * \brief Opens a HeightMap 
		 */
		class OpenHeightMapEvent : public Event {
			public:
				OpenHeightMapEvent( PlotConfig plot_conf, boost::shared_ptr<EventHandler> pEventHandler );
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
			private:
				PlotConfig plot_conf;
				boost::shared_ptr<EventHandler> pEventHandler;
		};

		/**
		 * \brief Adds new data to the heightmap
		 */
		class HMDataEvent : public Event {
			public:
				HMDataEvent( float x, float y, float z, bool show );
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot );
			private:
				float x, y, z;
				bool show;
		};

		/**
		 * \brief Causes HeightMap to calculate it's optimal coloring scheme
		 */
		class HMHeightScalingEvent	: public Event {
			public:
				HMHeightScalingEvent( ) {};
				virtual void execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
				boost::static_pointer_cast<BackendHeightMap, BackendPlot>( pBPlot )->calculate_height_scaling();
				}
		};
 }
#endif

