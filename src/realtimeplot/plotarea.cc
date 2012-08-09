/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, 2011 Edwin van Leeuwen

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

#include "realtimeplot/plotarea.h"
#include "realtimeplot/utils.h"

#include <pangomm/init.h>
#include <pangomm/context.h>
#include <pangomm/layout.h>


namespace realtimeplot {
	void Area::transform_to_device_units() {
		context->set_identity_matrix();
	}

	void Area::set_color( Color color ) {
		context->set_source_rgba( color.r, color.g, color.b, color.a );
	}

	PlotArea::PlotArea( PlotConfig &config ) : Area() {
		setup( config );
	}	

	void PlotArea::setup( PlotConfig &config ) {
		point_size = config.point_size;

		//calculate minimum plot area width/height based on aspect ratio
		double x = sqrt(config.area)/sqrt(config.aspect_ratio);
		plot_area_width = round( config.aspect_ratio*x );
		plot_area_height = round( x );

		//plot_surface, the shown part of this surface is by default 250000 pixels (default 500x500)
		//The rest is for when plotting outside of the area
		double ratio_surface_to_area = 5;

		width = ratio_surface_to_area*plot_area_width;
		height = ratio_surface_to_area*plot_area_height;

		double xratio = (ratio_surface_to_area-1)/2.0;
		double yratio = (ratio_surface_to_area-1)/2.0;
		min_x = config.min_x-xratio*(config.max_x-config.min_x);
		max_x = config.max_x+xratio*(config.max_x-config.min_x);
		min_y = config.min_y-yratio*(config.max_y-config.min_y);
		max_y = config.max_y+yratio*(config.max_y-config.min_y);

		surface = 
			Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					width, height );

		//Create context to draw background color
		context = Cairo::Context::create(surface);

		transform_to_plot_units();

		//give the plot its background color
		clear();
	}

	void PlotArea::transform_to_plot_units() {
		transform_to_device_units();
		context->translate( 0, height );
		context->scale( ((double) width)/(max_x-min_x),
				-((double) height)/(max_y-min_y) );
		context->translate( -min_x, -min_y );
	}

	void PlotArea::rectangle( float rect_min_x, float rect_min_y,
		 float width, float height, bool fill ) {
		context->save();
		context->rectangle( rect_min_x, rect_min_y, width, height );
		// So strange that this is needed, but fill seems 
		// somehow to convert back to device units 
		transform_to_device_units();
		if (fill) 
			context->fill_preserve();
		context->stroke();
		context->restore();
	}

	void PlotArea::point( float x, float y ) {
		double dx = point_size;
		double dy = point_size;
		context->device_to_user_distance(dx,dy);
		rectangle( x-0.5*dx, y-0.5*dy, dx, dy, true );
	}

	void PlotArea::line_add( float x, float y, int id ) {
		boost::shared_ptr<LineAttributes> line( new LineAttributes( x, y, id ) );

		//check if line already exists
		bool exists = false;
		std::list<boost::shared_ptr<LineAttributes> >::iterator i;
		for (i=lines.begin(); i != lines.end(); ++i) {
			if ((*i)->id == id) {
				line = (*i);
				exists = true;
				break;
			}
		}

		if (!exists) {
			//Push to the front assuming that new lines are more likely to added to
			//and the check if line already exists will be quicker
			lines.push_front( line );
		} else {
			context->save();
			transform_to_plot_units();
			context->move_to( line->current_x, line->current_y );
			context->line_to( x, y );
			transform_to_device_units();
			context->stroke();
			context->restore();

			line->current_x = x;
			line->current_y = y;
		}
	}

	void PlotArea::reposition( float x, float y ) {
		double dx = x - (min_x + (max_x-min_x)/2.0);
		double dy = y - (min_y + (max_y-min_y)/2.0);
		double old_plot_min_x = min_x;
		double old_plot_max_y = max_y;
		min_x += dx;
		max_x += dx;
		min_y += dy;
		max_y += dy;

		Cairo::RefPtr<Cairo::ImageSurface> old_surface = surface;

		surface = 
			Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					width, height );
		//Create context to draw background color
		context = Cairo::Context::create(surface);

		transform_to_plot_units();

		//give the plot its background color (don't use clear(), since it also clears the lines cache)
		context->save();
		set_color( Color::white() );
		rectangle( min_x, min_y, max_x-min_x, max_y-min_y,
				true );
		context->restore();

		context->save();
		context->user_to_device(old_plot_min_x, old_plot_max_y);
		transform_to_device_units();
		context->set_source( old_surface, old_plot_min_x, old_plot_max_y );
		context->paint();
		context->restore();
	}

	void PlotArea::clear() {
		context->save();
		set_color( Color::white() );
		rectangle( min_x, min_y, max_x-min_x, max_y-min_y,
				true );
		context->restore();
		lines.clear();
	}

	/*
	 * AxesArea
	 */
	AxesArea::AxesArea() 
		: Area() {
		Pango::init();
	}

	AxesArea::AxesArea( PlotConfig &config, size_t width, size_t height  ) 
		: Area() {
		Pango::init();
		setup( config, width, height );
	}

	// FIXME: Should be split up for change in w, h vs change in config etc.
	void AxesArea::setup( PlotConfig &config, size_t w, size_t h ) {
		width = w; height = h;
		bottom_margin = config.bottom_margin;
		left_margin = config.left_margin;
		top_margin = config.top_margin;
		right_margin = config.right_margin;
		min_x = config.min_x;
	 	max_x = config.max_x;
		min_y = config.min_y;
	 	max_y = config.max_y;
		
		std::vector<double> xaxis_ticks;
		std::vector<double> yaxis_ticks;


		surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
					width, height );
		context = Cairo::Context::create(surface);

		int text_width, text_height;
		Glib::RefPtr<Pango::Layout> pango_layout = Pango::Layout::create(context);
		Pango::FontDescription pango_font = Pango::FontDescription(config.font);


		//plot background color outside the axes (to cover points plotted outside)
		set_color( Color::white() );
		transform_to_device_units();
		context->move_to( 0, 0 );
		context->line_to( 0, height );
		context->line_to( width, height );
		context->line_to( width, 0 );
		transform_to_plot_units();
		context->move_to( config.min_x, config.min_y );
		context->line_to( config.min_x, config.max_y );
		context->line_to( config.max_x, config.max_y );
		context->line_to( config.max_x, config.min_y );
		context->line_to( config.min_x, config.min_y );
		context->fill();

		//Plot the main axes lines
		set_color( Color::black() );
		context->move_to( config.min_x, config.min_y );
		context->line_to( config.min_x, config.max_y );
		context->move_to( config.min_x, config.min_y );
		context->line_to( config.max_x, config.min_y );

		//Plot the ticks + tick labels
		xaxis_ticks = axes_ticks( config.min_x, config.max_x, config.nr_of_ticks );
		yaxis_ticks = axes_ticks( config.min_y, config.max_y, config.nr_of_ticks );

		double length_tick_x = config.ticks_length;
		double length_tick_y = -config.ticks_length;
		context->device_to_user_distance( length_tick_x, length_tick_y );

		//set font size. 
		//Thought pango_font.set_absolute_size would work, like this 
		//(i.e. wouldn't need Pango::SCALE, but apparently not)
		pango_font.set_size( config.numerical_labels_font_size*Pango::SCALE );
		pango_layout->set_font_description( pango_font );

		for (unsigned int i = 0; i < xaxis_ticks.size(); ++i) {
			context->move_to( xaxis_ticks[i], config.min_y );
			context->rel_line_to( 0, length_tick_y );
			transform_to_device_units();
			pango_layout->set_text( utils::stringify( xaxis_ticks[i] ) );
			pango_layout->get_pixel_size( text_width, text_height );
			context->rel_move_to( -0.5*text_width, 1*text_height );
			//pango_layout->add_to_cairo_context(context); //adds text to cairos stack of stuff to be drawn
			pango_layout->show_in_cairo_context( context );
			transform_to_plot_units();
		}

		for (unsigned int i = 0; i < yaxis_ticks.size(); ++i) {
			context->move_to( config.min_x, yaxis_ticks[i] );
			context->rel_line_to( length_tick_x, 0 );

			transform_to_device_units();
			context->rotate_degrees( -90 );
			pango_layout->set_text( utils::stringify( yaxis_ticks[i] ) );
			pango_layout->get_pixel_size( text_width, text_height );
			context->rel_move_to( -0.5*text_width, -2*text_height );
			pango_layout->show_in_cairo_context( context );
			transform_to_plot_units();
		}


		pango_font.set_size( config.label_font_size*Pango::SCALE );
		pango_layout->set_font_description( pango_font );

		pango_layout->set_text( config.ylabel );
		pango_layout->get_pixel_size( text_width, text_height );

		context->move_to( min_x, min_y+(0.5*(max_y-min_y)) ); 
		transform_to_device_units();
		context->rel_move_to( -2.5*text_height, 0.5*text_width );
		context->save();
		context->rotate_degrees( -90 );
		pango_layout->show_in_cairo_context( context );
		context->restore();

		pango_layout->set_text( config.xlabel );
		pango_layout->get_pixel_size( text_width, text_height );
		transform_to_plot_units();
		context->move_to( min_x + (0.5*(max_x-min_x) ), min_y );
		transform_to_device_units();
		context->rel_move_to( -0.5*text_width, 1.5*text_height );
		pango_layout->show_in_cairo_context( context );

		context->stroke();
	}

	void AxesArea::setup_with_plot_size( PlotConfig &config, 
					size_t width, size_t height ) {
		setup( config, width + config.left_margin + config.right_margin,
				height + config.bottom_margin + config.top_margin );
	}	

	void AxesArea::transform_to_plot_units() {
		transform_to_device_units();
		context->translate( left_margin, height-bottom_margin );
		context->scale( (width-left_margin-right_margin)/((max_x-min_x)),
				-((double) height-bottom_margin-top_margin)/((max_y-min_y)) );
		context->translate( -min_x, -min_y );
	}

	int AxesArea::power_of_step( float step ) {
		int power = 0;
		//Using straightforward method. There might be a better way 
		if (step <= 1) {
			while (step/pow(10,power)<=1) {
				--power;
			}
		} else if (step >=10) {
			while (step/pow(10,power)>=10) {
				++power;
			}
		}
		return power;
	}

	std::vector<double> AxesArea::axes_ticks( float min, float max, int nr ) {
		std::vector<double> ticks;
		double tick;
		double step = (max-min)/nr;
		int power = power_of_step( step );

		//round our step
		step = round(step/pow(10,power)) * pow(10,power);

		//first tick is rounded version of min
		tick = round(min/pow(10,power)) * pow(10,power);

		while (tick <= max ) {
			if (tick >= min) {
				ticks.push_back( tick );
			}
			tick += step;
		}
		return ticks;
	}
};
