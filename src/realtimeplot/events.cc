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

    void ConfigEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
        pBPlot->config = config;
        pBPlot->update_config();
    }

    MultipleEvents::MultipleEvents( std::vector<boost::shared_ptr<Event> > event_vector ) {
        events = event_vector;
    }

    void MultipleEvents::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
        for (std::vector<boost::shared_ptr<Event> >::iterator it = events.begin(); 
                it!=events.end(); ++it) {
            (*it)->execute( pBPlot );
        }
    }

		OpenPlotEvent::OpenPlotEvent() {}
		void OpenPlotEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
			pBPlot->reset( new Plot( plot_conf ) );
		}


    SetColorEvent::SetColorEvent( Color colour ) {
        color = colour;
    }

    void SetColorEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
        pBPlot->set_color( color );
    }

	PointEvent::PointEvent( float x, float y ) {
		x_crd = x;
		y_crd = y;
	}

	void PointEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
		pBPlot->point( x_crd, y_crd );
	}

	RectangleEvent::RectangleEvent( float min_x, float min_y, float width_x, float width_y )
		: min_x( min_x ),
			min_y( min_y ),
			width_x( width_x ),
			width_y( width_y )
	{}

	void RectangleEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
		pBPlot->rectangle( min_x, min_y, width_x, width_y );
	}


	LineAddEvent::LineAddEvent( float x, float y, int id_value, Color col ) {
		x_crd = x;
		y_crd = y;
    id = id_value;
		color = col;
	}

	void LineAddEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
		pBPlot->line_add( x_crd, y_crd, id, color );
	}


	NumberEvent::NumberEvent( float x, float y, float i ) {
		x_crd = x;
		y_crd = y;
		nr = i;
	}

	void NumberEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
		pBPlot->number( x_crd, y_crd, nr );
	}

	SaveEvent::SaveEvent( std::string fn ) {
		filename = fn;
	}

	void SaveEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
		pBPlot->save(filename);
	}

	ClearEvent::ClearEvent() {
	}

	void ClearEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
		pBPlot->clear();
	}

	CloseEvent::CloseEvent() {
	}
	void CloseEvent::execute( boost::shared_ptr<BackendPlot> pBPlot ) {
		pBPlot->close_window();
	}


}


