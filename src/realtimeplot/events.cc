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

    void ConfigEvent::execute( BackendPlot *pBPlot ) {
        pBPlot->config = config;
        pBPlot->update_config();
    }

	PointEvent::PointEvent( float x, float y, Color color ) {
		x_crd = x;
		y_crd = y;
        colour = color;
	}

	void PointEvent::execute( BackendPlot *pBPlot ) {
		pBPlot->point( x_crd, y_crd, colour );
	}

	LineAddEvent::LineAddEvent( float x, float y, int id_value ) {
		x_crd = x;
		y_crd = y;
        id = id_value;
	}

	void LineAddEvent::execute( BackendPlot *pBPlot ) {
		pBPlot->line_add( x_crd, y_crd, id );
	}


	NumberEvent::NumberEvent( float x, float y, float i ) {
		x_crd = x;
		y_crd = y;
		nr = i;
	}

	void NumberEvent::execute( BackendPlot *pBPlot ) {
		pBPlot->number( x_crd, y_crd, nr );
	}

	PointTransparentEvent::PointTransparentEvent( float x, float y, float a ) {
		x_crd = x;
		y_crd = y;
		alpha = a;
	}

	void PointTransparentEvent::execute( BackendPlot *pBPlot ) {
        Color color = Color::black();
        color.a = alpha;
		pBPlot->point( x_crd, y_crd, color );
		pBPlot->set_alpha( 1 );
	}

	SaveEvent::SaveEvent( std::string fn ) {
		filename = fn;
	}

	void SaveEvent::execute( BackendPlot *pBPlot ) {
		pBPlot->save(filename);
	}

	ClearEvent::ClearEvent() {
	}

	void ClearEvent::execute( BackendPlot *pBPlot ) {
		pBPlot->clear();
	}

}


