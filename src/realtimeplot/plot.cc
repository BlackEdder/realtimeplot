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

	bool Color::operator==(Color color)
	{ 
		if (color.r == r && color.g == g && color.b == b && color.a == a)
			return true;
		else
			return false;
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
	/*
	 * ColorMap
	 */
	ColorMap::ColorMap() :
	 	alpha(-1), beta(-1), scaling( false ) 
	{}

	Color ColorMap::operator()( double proportion ) {
		proportion = scale( proportion );
		float r, g, b;
		if (proportion < 0.5) {
			r = 1;
			g = 1-2*proportion;
			b = 0;
		} else {
			r = 1-2*(proportion-0.5);
			g = 0;
			b = 0;
		}
		return Color( r, g, b, 1 );
	}

	void ColorMap::calculate_height_scaling( double mean, double var ) {
		if (var > 0) {
			alpha = mean*((mean*(1-mean))/var-1);
			beta = (1-mean)*((mean*(1-mean))/var-1);
		}

		// Sometimes this doesn't work properly -> no scaling
		if (alpha <=0 || beta <=0)
			scaling = false;
		else 
			scaling = true;
	}

	double ColorMap::scale( double proportion ) {
		if (!scaling)
			return proportion;
		else
			return boost::math::ibeta(alpha, beta, proportion);
	}


	void PlotConfig::setDefaults() {
		scaling = false;
		display = true;
		area = 500*500;
		margin_x = 0;
		bottom_margin = 60;
		margin_y = 0;
		left_margin = 60;
		top_margin = 10;
		right_margin = 10;
		min_x = 0;
		max_x = 1;
		min_y = 0;
		max_y = 1;
		nr_of_ticks = 10;
		ticks_length = 7;
		xlabel = "x";
		ylabel = "y";
		font = "sans";
		label_font_size = 12;
		numerical_labels_font_size = 10;
		overlap = 0.1;
		aspect_ratio = 1;
		fixed_plot_area = false;
		point_size = 4;
		title = "RealTimePlot";
	};

	Plot::Plot()
		: config( PlotConfig() ),
		pEventHandler( new EventHandler() ), detach( false )
	{ 
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenPlotEvent( config, pEventHandler ) ) );
	}

	Plot::Plot(bool open)
		: config( PlotConfig() ),
		pEventHandler( new EventHandler() ), detach( false )
	{ 
		if (open)
			pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
						pEventHandler ) ) );
	}


	Plot::Plot( PlotConfig conf )
		: config( conf ),
		pEventHandler( new EventHandler() ), detach( false )
	{ 
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenPlotEvent( config, 
						pEventHandler ) ) );
	}

	Plot::~Plot() {
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new FinalEvent(pEventHandler, false ) ) );
		if (detach)
			pEventHandler->pEventProcessingThrd->detach();
		else
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
		std::vector<boost::shared_ptr<Event> > events(2);
		events[0] = boost::shared_ptr<Event>( new SetColorEvent( color ) );
		events[1] = boost::shared_ptr<Event>( new PointEvent( x, y ) );
		//events[2] = boost::shared_ptr<Event>( new RestoreEvent() );
		pEventHandler->add_event( 
				boost::shared_ptr<Event>( new MultipleEvents( events ) ));
	}

	void Plot::rectangle( float min_x, float min_y, float width_x, float width_y,
			bool fill, Color color ) {
		//std::vector<boost::shared_ptr<Event> > events(2);
		//events[0] = boost::shared_ptr<Event>( new SetColorEvent( color ) );
		//events[1] = boost::shared_ptr<Event>( new RectangleEvent( 
		//			min_x, min_y, width_x, width_y ) );
		//events[2] = boost::shared_ptr<Event>( new RestoreEvent() );
		pEventHandler->add_event( 
				boost::shared_ptr<Event>( new RectangleEvent( min_x, min_y, width_x, width_y,
						fill, color ) ));
	}

	/*void Plot::line_add( float x, float y, int id ) {
		line_add( x, y, id, Color::Black() );
		}*/

	void Plot::line_add( float x, float y, int id ) {
		this->line_add( x, y, id, Color::black() );
	}

	void Plot::line_add( float x, float y, int id, Color color ) {
		boost::shared_ptr<Event> pEvent( new LineAddEvent( x, y, id, color ) );
		pEventHandler->add_event( pEvent );
	}

	void Plot::title( std::string title ) {
		pEventHandler->add_event( boost::shared_ptr<Event> ( new TitleEvent( title ) ) );
	}

	void Plot::text( float x, float y, std::string text ) {
		boost::shared_ptr<Event> pEvent( new TextEvent( x, y, text ) );
		pEventHandler->add_event( pEvent );
	}

	void Plot::save( std::string filename ) {
		boost::shared_ptr<Event> pEvent( new SaveEvent( filename ));
		pEventHandler->add_event( pEvent );
	}

	//For now just closes old plot window and opens a new one
	void Plot::reset( PlotConfig conf ) {
		/*std::vector<boost::shared_ptr<Event> > events(2);
		events[0] = boost::shared_ptr<Event>( new CloseWindowEvent() );
		//delete pEventHandler;
		config = conf;
		events[1] = boost::shared_ptr<Event>( new OpenPlotEvent( config, 
					pEventHandler ) );
		//pEventHandler.reset( new EventHandler( config ) );
		pEventHandler->add_event( 
				boost::shared_ptr<Event>( new MultipleEvents( events ) ));*/
		pEventHandler->add_event( boost::shared_ptr<Event>( new ResetEvent( conf ) ) );
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

	Histogram::Histogram( size_t no_bins, bool frequency )
		: Plot( false )
	{
		config.fixed_plot_area = false;
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenHistogramEvent( config, 
						frequency, no_bins, pEventHandler ) ) );
	}

	Histogram::Histogram( PlotConfig config, size_t no_bins, bool frequency )
		: Plot( false ) 
	{
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenHistogramEvent( config, 
						frequency, no_bins, pEventHandler ) ) );
	}

	Histogram::Histogram( double min_x, double max_x, size_t no_bins, bool frequency )
		:	Plot( false )
	{ 
		config.fixed_plot_area = true;
		config.min_x = min_x;
		config.max_x = max_x;
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenHistogramEvent( config, 
						frequency, no_bins, pEventHandler ) ) );
	}

	void Histogram::set_data( std::vector<double> the_data, bool show ) {
		for (size_t i=0; i<the_data.size(); ++i) {
			add_data( the_data[i], false );
		}
		if (show)
			plot();
	}

	void Histogram::add_data( double new_data, bool show ) {
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new HistDataEvent( new_data ) ) ); 
		if (show)
			plot();
	}

	void Histogram::plot() {
		pEventHandler->add_event( boost::shared_ptr<Event>( new HistPlotEvent() ) );
	}

	void Histogram::optimize_bounds( double proportion ) {
		pEventHandler->add_event( boost::shared_ptr<Event>(
				 new HistOptimizeEvent(proportion) ) );
	}

	/*
	 * Histogram3D
	 */

	Histogram3D::Histogram3D( size_t no_bins_x, size_t no_bins_y )
		: Plot( false )
	{
		config.fixed_plot_area = false;
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenHistogram3DEvent( config, 
						no_bins_x, no_bins_y, pEventHandler ) ) );
	}

	Histogram3D::Histogram3D( PlotConfig config,
		 	size_t no_bins_x, size_t no_bins_y )
		: Plot( false ) 
	{
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenHistogram3DEvent( config, 
						no_bins_x, no_bins_y, pEventHandler ) ) );
	}

	Histogram3D::Histogram3D( double min_x, double max_x, 
			double min_y, double max_y,
			size_t no_bins )
		:	Plot( false )
	{ 
		config.fixed_plot_area = true;
		config.min_x = min_x;
		config.max_x = max_x;
		config.min_y = min_y;
		config.max_y = max_y;
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenHistogram3DEvent( config, 
						no_bins, no_bins, pEventHandler ) ) );
	}

	void Histogram3D::add_data( double x, double y, bool show ) {
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new Hist3DDataEvent( x, y ) ) ); 
		if (show)
			plot();
	}

	void Histogram3D::plot() {
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new Hist3DPlotEvent() ) );
	}


	void Histogram3D::calculate_height_scaling() {
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new Hist3DHeightScalingEvent() ) ); 
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

	HeightMap::HeightMap() : Plot(false)
	{ 
		//config = PlotConfig();
		config.fixed_plot_area = true;
		//pEventHandler.reset( new EventHandler( config ) );
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new OpenHeightMapEvent( config, 
						pEventHandler ) ) );
	}

	HeightMap::HeightMap( float min_x, float max_x, float min_y, float max_y ) : Plot(false)
	{ 
		config.min_x = min_x;
		config.max_x = max_x;
		config.min_y = min_y;
		config.max_y = max_y;
		config.fixed_plot_area = true;
		//pEventHandler.reset( new EventHandler( config ) );
		pEventHandler->add_event( boost::shared_ptr<Event>( new OpenHeightMapEvent( config, 
						pEventHandler ) ) );
	}

	void HeightMap::add_data( float x, float y, float z, bool show ) {
		pEventHandler->add_event( boost::shared_ptr<Event>( new HMDataEvent( x, y, z, show ) ) ); 
	}

	void HeightMap::calculate_height_scaling() {
		pEventHandler->add_event( boost::shared_ptr<Event>( 
					new HMHeightScalingEvent() ) ); 
	}


}
