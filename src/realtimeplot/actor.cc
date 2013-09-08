/*
	 -------------------------------------------------------------------

	 Copyright (C) 2013, Edwin van Leeuwen

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

#include "realtimeplot/actor.hh"
#include "realtimeplot/backend.h"

namespace realtimeplot {
	using namespace cppa;

	CacheActor::CacheActor( actor_ptr actor ) 
		: _actor( actor ) {}

	void CacheActor::init() {
		become(
			on(atom("resend")) >> [&]() { 
				for ( auto & tuple : _cache ) {
					_actor << tuple;
				}
			},
			on(atom("close")) >> [&] {
				forward_to( _actor );
				unbecome(); 
			},
			others() >> [&] {
				forward_to( _actor );
				_cache.push_back( self->last_dequeued()  ); 
			}
		);
	}

 	/**
	 * Actor
	 */
	Actor::Actor() : new_line_id( -2 ) {
		announce<Color>( &Color::r, &Color::g, &Color::b, &Color::a );
	}

	void Actor::init() {
		become (
			on(atom("poll")) >> [this]() { 
				delayed_send(self, std::chrono::milliseconds(100), atom("poll"));
				pBPlot->display(); // Force redraw/draw
			},
			on(atom("open"), arg_match ) >> [this] ( const std::string &type,
				const bool &window ) {
				PlotConfig conf = PlotConfig(); // Default config
				conf.display = window;
				if (type == "plot") {
					pBPlot = boost::shared_ptr<BackendPlot>( new BackendPlot( conf, 
						boost::shared_ptr<EventHandler>()  ) );
				}
				delayed_send( self, std::chrono::milliseconds(100), atom("poll"));
			},
			on(atom("open"), arg_match ) >> [this] ( const std::string &type ) {
				PlotConfig conf = PlotConfig(); // Default config
				if (type == "plot") {
					pBPlot = boost::shared_ptr<BackendPlot>( new BackendPlot( conf, 
						boost::shared_ptr<EventHandler>()  ) );
				}
				delayed_send( self, std::chrono::milliseconds(100), atom("poll"));
			},
			on(atom("plot")) >> [this] () {
				pBPlot->display(); // Force redraw/draw
			},
			on(atom("save"), arg_match ) >> [this] ( const std::string &filename ) {
				pBPlot->save( filename );
			},
			on(atom("point"), arg_match ) >> [this] ( const int &x, const int &y ) {
				pBPlot->point( x, y );
			},
			on(atom("point"), arg_match ) >> [this] ( const double &x, 
					const double &y ) {
				pBPlot->point( x, y );
			},
			on(atom("text"), arg_match ) >> [this] ( const int &x, const int &y,
					const std::string &txt ) {
				pBPlot->text( x, y, txt );
			},
			on(atom("text"), arg_match ) >> [this] ( const double &x, 
					const double &y, const std::string &txt ) {
				pBPlot->text( x, y, txt );
			},
			on(atom("new_line")) >> [this] () {
				pBPlot->current_line = new_line_id;
				--new_line_id;
			},
			on(atom("line_id"), arg_match ) >> [this] ( const int id  ) {
				pBPlot->current_line = id;
			},
			on(atom("line_add"), arg_match ) >> [this] ( const int &x, 
					const int &y ) {
				pBPlot->line_add( x, y );
			},
			on(atom("line_add"), arg_match ) >> [this] ( const double &x, 
					const double &y ) {
				pBPlot->line_add( x, y );
			},
			on(atom("color"), arg_match ) >> [this] ( const Color &color ) {
				pBPlot->set_color( color );
			},
			on(atom("title"), arg_match ) >> [this] ( const std::string &title ) {
				pBPlot->title( title );
			},
			on(atom("xrange"), arg_match ) >> [this] ( const double &min, 
					const double &max ) {
				pBPlot->xrange( min, max );
			},
			on(atom("yrange"), arg_match ) >> [this] ( const double &min, 
					const double &max ) {
				pBPlot->yrange( min, max );
			},
			on(atom("rectangle"), arg_match ) >> [this] ( const double &x, 
					const double &y, const double &width, const double &height,
					const bool &fill ) {
				pBPlot->rectangle_at( x, y, width, height, fill );
			},
			on(atom("area_size"), arg_match ) >> [this] ( const int &no_pixels ) {
				pBPlot->area_size( no_pixels );
			},
			on(atom("margin_x"), arg_match ) >> [this] ( const int &no_pixels ) {
				pBPlot->margin_x( no_pixels );
			},
			on(atom("margin_y"), arg_match ) >> [this] ( const int &no_pixels ) {
				pBPlot->margin_y( no_pixels );
			},
			on(atom("ylabel"), arg_match ) >> [this] ( const std::string &title ) {
			},
			on(atom("xlabel"), arg_match ) >> [this] ( const std::string &title ) {
				pBPlot->xlabel( title );
			},
			on(atom("ylabel"), arg_match ) >> [this] ( const std::string &title ) {
				pBPlot->ylabel( title );
			},
			on(atom("close")) >> [&]() {
				if (pBPlot != nullptr)
					pBPlot->close_window();
				reply( atom("close") );
				unbecome(); 
			},
			others() >> [] { aout << "Message not understood" << std::endl; }
		);
	}
};
