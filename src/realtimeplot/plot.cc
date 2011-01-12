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

#include "realtimeplot/plot.h"
#include "realtimeplot/events.h"

namespace realtimeplot {

	Color::Color() {
		r = 1;
		g = 1;
		b = 1;
		a = 0;
	}

	Color::Color( double red, double green, double blue, double alpha ) {
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}

	Color Color::black() {
		return Color( 0, 0, 0, 1 );
	}

	Color Color::white() {
		return Color( 1, 1, 1, 1 );
	}

	Color Color::red() {
		return Color( 1, 0, 0, 1 );
	}

	Color Color::green() {
		return Color( 0, 1, 0, 1 );
	}

	Color Color::blue() {
		return Color( 0, 0, 1, 1 );
	}

	Color Color::yellow() {
		return Color( 1, 1, 0, 1 );
	}

	Color Color::purple() {
		return Color( 1, 0, 1, 1 );
	}

	Color Color::grey() {
		return Color( 0.5, 0.5, 0.5, 1 );
	}

	Color Color::darkblue() {
		return Color( 0, 0, 0.545098, 1 );
	}

	Color Color::cyan() {
		return Color( 0, 1, 1, 1 );
	}

	Color Color::indigo() {
		return Color( 0.294118, 0, 0.509804, 1 );
	}

	Color Color::orange() {
		return Color( 1, 0.647059, 0, 1 );
	}

	Color Color::brown() {
		return Color( 0.647059, 0.164706, 0.164706, 1 );
	}

	Color Color::cadetblue() {
		return Color( 0.372549, 0.619608, 0.627451, 1 );
	}

	Color Color::by_id(size_t id) {
		std::vector<Color> colors = Color::colors();
		return colors[ id%colors.size() ];
	}

	std::vector<Color> Color::colors() {
		std::vector<Color> colors;
		colors.push_back( Color::black() );
		colors.push_back( Color::red() );
		colors.push_back( Color::green() );
		colors.push_back( Color::blue() );
		colors.push_back( Color::yellow() );
		colors.push_back( Color::purple() );
		colors.push_back( Color::grey() );
		colors.push_back( Color::brown() );
		colors.push_back( Color::darkblue() );
		colors.push_back( Color::cyan() );
		colors.push_back( Color::indigo() );
		colors.push_back( Color::orange() );
		colors.push_back( Color::cadetblue() );
		return colors;
	}

	Plot::Plot()
		: config( PlotConfig() ),
		pEventHandler( new EventHandler( config ) )
	{ 
		pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
						pEventHandler ) ) );
	}

	Plot::Plot( PlotConfig conf )
		: config( conf ),
		pEventHandler( new EventHandler( config ) )
	{ 
		pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
						pEventHandler ) ) );
	}

	Plot::~Plot() {
		pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
		pEventHandler->pEventProcessingThrd->join();
	}


	void Plot::point( float x, float y ) {
		pEventHandler->add_event( boost::shared_ptr<Event>( new PointEvent(x, y) ) ); 
	}

	/**
	 * \brief Function to plot a point of a specific color
	 *
	 * Default color of the point is black.
	 * We send a vector of the events needed (set the color, plot the point
	 * and restore color to its former value). A vector is used to make sure
	 * that these events are processed directly following each other (thread
	 * safety).
	 */
	void Plot::point( float x, float y, Color color ) {
		std::vector<boost::shared_ptr<Event> > events(3);
		events[0] = boost::shared_ptr<Event>( new SetColorEvent( color ) );
		events[1] = boost::shared_ptr<Event>( new PointEvent( x, y ) );
		events[2] = boost::shared_ptr<Event>( new RestoreEvent() );
		pEventHandler->add_event( 
				boost::shared_ptr<Event>( new MultipleEvents( events ) ));
	}

	void Plot::rectangle( float min_x, float min_y, float width_x, float width_y,
			bool fill, Color color ) {
		std::vector<boost::shared_ptr<Event> > events(3);
		events[0] = boost::shared_ptr<Event>( new SetColorEvent( color ) );
		events[1] = boost::shared_ptr<Event>( new RectangleEvent( 
					min_x, min_y, width_x, width_y ) );
		events[2] = boost::shared_ptr<Event>( new RestoreEvent() );
		pEventHandler->add_event( 
				boost::shared_ptr<Event>( new MultipleEvents( events ) ));
	}

	/*void Plot::line_add( float x, float y, int id ) {
		line_add( x, y, id, Color::Black() );
		}*/

	void Plot::line_add( float x, float y, int id, Color color ) {
		boost::shared_ptr<Event> pEvent( new LineAddEvent( x, y, id, color ) );
		pEventHandler->add_event( pEvent );
	}

	void Plot::number( float x, float y, float i ) {
		boost::shared_ptr<Event> pEvent( new NumberEvent( x, y, i ) );
		pEventHandler->add_event( pEvent );
	}

	void Plot::save( std::string filename ) {
		boost::shared_ptr<Event> pEvent( new SaveEvent( filename ));
		pEventHandler->add_event( pEvent );
	}

	//For now just closes old plot window and opens a new one
	void Plot::reset( PlotConfig conf ) {
		std::vector<boost::shared_ptr<Event> > events(2);
		events[0] = boost::shared_ptr<Event>( new CloseWindowEvent() );
		//delete pEventHandler;
		config = conf;
		events[1] = boost::shared_ptr<Event>( new OpenPlotEvent( config, 
					pEventHandler ) );
		//pEventHandler.reset( new EventHandler( config ) );
		pEventHandler->add_event( 
				boost::shared_ptr<Event>( new MultipleEvents( events ) ));
	}

	void Plot::clear() {
		boost::shared_ptr<Event> pEvent( new ClearEvent() );
		pEventHandler->add_event( pEvent );
	}

	void Plot::update_config() {
		boost::shared_ptr<Event> pEvent( new ConfigEvent( config ) );
		pEventHandler->add_event( pEvent );
	}

	void Plot::close() {
		//pEventHandler->plot_closed();
		boost::shared_ptr<Event> pEvent( new CloseWindowEvent() );
		pEventHandler->add_event( pEvent );
		pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, true ) ) );
	}

	/*
	 * Histogram
	 */

	Histogram::Histogram()
		: no_bins( 4 ),
		max_y( 0 ),
		min_x( 0 ),
		max_x( -1 ),
		frequency( false ),
		frozen_bins_x( false ),
		Plot()
	{
	}

	Histogram::Histogram( double min_x, double max_x, size_t no_bins )
		: no_bins( no_bins ),
		max_y( 0 ),
		min_x( min_x ),
		max_x( max_x ),
		frequency( false ),
		frozen_bins_x( true ),
		Plot()
	{ 
		bin_width = (max_x-min_x)/(no_bins-1);
		for (int i=0; i<no_bins; ++i) {
			bins_x.push_back( min_x+i*bin_width );
			bins_y.push_back( 0 );
		}
	}


	Histogram::~Histogram() {
	}

	void Histogram::set_data( std::vector<double> the_data, bool show ) {
		data = the_data;
		fill_bins();
		if (show)
			plot();
	}

	void Histogram::add_data( double new_data, bool show ) {
		data.push_back( new_data );
		//First check that data is not smaller or larger than the current range
		if (new_data < min_x) {
			fill_bins();
		} else if (new_data > max_x) {
			fill_bins();
		} else if (data.size() == 1) {
			fill_bins();
		} else {
			unsigned int current_bin;
			for (current_bin=0; current_bin<bins_x.size(); ++current_bin) {
				if (new_data < bins_x[current_bin]+0.5*bin_width)
					break;
			}
			++bins_y[current_bin];
			if (bins_y[current_bin]>max_y)
				max_y = bins_y[current_bin];
		}
		if (show)
			plot();
	}

	void Histogram::fill_bins() {
		sort( data.begin(), data.end() );

		if (!frozen_bins_x) {
			bins_x.clear();

			if (data.front() != data.back() ) {
				bin_width = (data.back()-data.front())/(no_bins-1);
				for (int i=0; i<no_bins; ++i) {
					bins_x.push_back( data.front()+i*bin_width );
				}

				min_x = data.front()-0.5*bin_width;
				max_x = data.back()+0.5*bin_width;
			} else {
				//choose arbitrary bin_width
				bin_width = 1;
				for (int i=0; i<no_bins; ++i) {
					bins_x.push_back( data.front()+(i-no_bins/2)*bin_width );
				}
				min_x = data.front();
				max_x = data.front();
			}
		}
		bins_y.clear();

		for (size_t i=0; i<bins_x.size(); ++i) {
			bins_y.push_back( 0 );
		}
		max_y = 0;

		int current_bin = 0;
		//should use iterator
		for (size_t i=0; i<data.size(); ++i) {
			while (data[i] > bins_x[current_bin]+0.5*bin_width) {
				++current_bin;
			}
			if (current_bin<bins_x.size()) {
				++bins_y[current_bin];
				if (bins_y[current_bin]>max_y)
					max_y = bins_y[current_bin];
			}
		}
	}

	void Histogram::set_counts_data( std::vector<double> values,
			std::vector<int> counts, bool show ) {
		bins_x.clear();
		for (unsigned int i=0;i<values.size();++i) {
			bins_x.push_back( values[i] ); //This assumes that values are sorted!
			for (int j=0;j<counts[i];++j) {
				data.push_back( values[i] );
			}
		}
		bin_width = bins_x[1]-bins_x[0];
		frozen_bins_x = true;
		fill_bins();
		if (show)
			plot();
	}

	void Histogram::plot() {
		if ( (min_x == max_x) || 
				!(frequency && config.max_y >= max_y && config.max_y <= 2*max_y) ||
				!(config.max_x >= max_x && config.max_x <= max_x + 4*bin_width	) ||
				!(config.min_x <= min_x && config.min_x >= min_x - 4*bin_width	) ) {
			PlotConfig new_config = PlotConfig(config);
			new_config.min_x = bins_x.front()-bin_width;
			new_config.max_x = bins_x.back()+bin_width;
			if (!frequency)
				new_config.max_y = 1.1*max_y;
			else
				new_config.max_y = 1.1;

			reset( new_config );
		} else {
			clear();
		}
		for (unsigned int i=0; i<bins_x.size(); ++i) {
			double height = bins_y[i];
			if (frequency)
				height/=data.size();
			line_add( bins_x[i]-0.5*bin_width, 0, -1 );
			line_add( bins_x[i]-0.5*bin_width, height, -1 );
			line_add( bins_x[i]+0.5*bin_width, height, -1 );
			line_add( bins_x[i]+0.5*bin_width, 0, -1 );
		}
	}

	/*
	 * SurfacePlot
	 */
	SurfacePlot::SurfacePlot( float min_x, float max_x, float min_y, float max_y,
			size_t resolution )
		: resolution( resolution ),
		data( resolution*resolution ), 
		max_z( 1 ), 
		Plot()
	{ 
		PlotConfig new_config = PlotConfig();
		new_config.min_x = min_x;
		new_config.max_x = max_x;
		new_config.min_y = min_y;
		new_config.max_y = max_y;
		new_config.fixed_plot_area = true;
		reset( new_config );
		width_x = (max_x-min_x)/(resolution-1);
		width_y = (max_y-min_y)/(resolution-1);
		for (size_t i=0; i<(resolution); ++i) {
			bins_x.push_back( min_x+width_x*i );
			bins_y.push_back( min_y+width_y*i );
			for (size_t j=0; j<(resolution); ++j) {
				data[i*resolution+j] = 0;
			}
		}
	}

	void SurfacePlot::add_data( float x, float y, bool show )
	{
		if (x > bins_x[0] && x < bins_x.back() && 
				y > bins_y[0] && y < bins_y.back() ) {
			for (size_t i=1; i<bins_x.size(); ++i) {
				if (x < bins_x[i]) {
					for (size_t j=1; j<bins_y.size(); ++j) {
						if (y < bins_y[j]) {
							++data[(i-1)*resolution+(j-1)];
							if (data[(i-1)*resolution+(j-1)]>max_z)
								++max_z;
							break;
						}
					}
					break;
				}
			}
		}
		if (show)
			plot();
	}

	void SurfacePlot::plot() {
		for (size_t i=0; i<bins_x.size(); ++i) {
			for (size_t j=0; j<bins_y.size(); ++j) {
				float shade = 1-double(data[i*resolution+j])/max_z;
				Color color = Color( shade, shade, shade, 1 );
				rectangle( bins_x[i], bins_y[j], width_x, width_y, true, color );

			}
		}
	}

	HeightMap::HeightMap()
	{ 
		config = PlotConfig();
		pEventHandler.reset( new EventHandler( config ) );
		pEventHandler->add_event( boost::shared_ptr<Event>( new OpenHeightMapEvent( config, 
						pEventHandler ) ) );
	}

	void HeightMap::add_data( float x, float y, float z, bool show ) {
		point( x, y );
	}

}
