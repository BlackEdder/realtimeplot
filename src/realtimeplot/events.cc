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

#include "realtimeplot/events.h"

namespace realtimeplot {
    ConfigEvent::ConfigEvent( PlotConfig new_config ) {
        config = new_config;
    }

    void ConfigEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
        pBPlot->config = config;
        pBPlot->update_config();
    }

    MultipleEvents::MultipleEvents( std::vector<boost::shared_ptr<Event> > event_vector ) {
        events = event_vector;
    }

    void MultipleEvents::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
        for (std::vector<boost::shared_ptr<Event> >::iterator it = events.begin(); 
                it!=events.end(); ++it) {
            (*it)->execute( pBPlot );
        }
    }

		OpenPlotEvent::OpenPlotEvent( PlotConfig plot_conf, 
				boost::shared_ptr<EventHandler> pEventHandler ) :
			plot_conf( plot_conf ),
			pEventHandler( pEventHandler->shared_from_this() )
		{
		}

		void OpenPlotEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
			pBPlot.reset( new BackendPlot( plot_conf, pEventHandler ) );
		}

		ResetEvent::ResetEvent( PlotConfig plot_conf ) :
			plot_conf( plot_conf )
		{
		}

		void ResetEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
			pBPlot->reset( plot_conf );
		}

    SetColorEvent::SetColorEvent( Color colour ) {
        color = colour;
    }

    void SetColorEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
        pBPlot->set_color( color );
    }

	PointEvent::PointEvent( float x, float y ) {
		x_crd = x;
		y_crd = y;
	}

	void PointEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
		pBPlot->point( x_crd, y_crd );
	}

	ScaleXSurfaceEvent::ScaleXSurfaceEvent( size_t width, size_t height ) 
		: width( width), height( height )
	{ }

	void ScaleXSurfaceEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
		pBPlot->scale_xsurface( width, height );
	}


	RectangleEvent::RectangleEvent( float min_x, float min_y, float width_x, float width_y )
		: min_x( min_x ),
			min_y( min_y ),
			width_x( width_x ),
			width_y( width_y )
	{}

	void RectangleEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
		pBPlot->rectangle( min_x, min_y, width_x, width_y );
	}


	LineAddEvent::LineAddEvent( float x, float y, int id_value, Color col ) {
		x_crd = x;
		y_crd = y;
    id = id_value;
		color = col;
	}

	void LineAddEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
		pBPlot->line_add( x_crd, y_crd, id, color );
	}


	SaveEvent::SaveEvent( std::string fn ) {
		filename = fn;
	}

	void SaveEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
		pBPlot->save(filename);
	}

	ClearEvent::ClearEvent() {
	}

	void ClearEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
		pBPlot->clear();
	}

	CloseWindowEvent::CloseWindowEvent() {
	}
	void CloseWindowEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
		pBPlot->close_window();
	}

	FinalEvent::FinalEvent( boost::shared_ptr<EventHandler> pEventHandler,
			bool force ) :
		pEventHandler( pEventHandler->shared_from_this() ),
		force( force )
	{
	}

	void FinalEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
		pEventHandler->processing_events = false;
		if (force)
			pEventHandler->force_close = true;
	}

	/*
	 * HeightMap specific
	 */

	OpenHeightMapEvent::OpenHeightMapEvent( PlotConfig plot_conf, 
			boost::shared_ptr<EventHandler> pEventHandler ) :
		plot_conf( plot_conf ),
		pEventHandler( pEventHandler->shared_from_this() )
	{
	}

	void OpenHeightMapEvent::execute( boost::shared_ptr<BackendPlot> &pBPlot ) {
		pBPlot.reset( new BackendHeightMap( plot_conf, pEventHandler ) );
	}

	HMDataEvent::HMDataEvent( float x, float y, float z, bool show )
		: x(x), y(y), z(z), show( show )
	{}

	void HMDataEvent::execute(boost::shared_ptr<BackendPlot> &pBPlot ) {
		boost::static_pointer_cast<BackendHeightMap, BackendPlot>(pBPlot)->add_data( x, y, z, show );
	}

}


