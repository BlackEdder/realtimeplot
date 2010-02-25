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
#include "realtimeplot/backend.h"

namespace realtimeplot {
    /*
     * BackendPlot
     */

    BackendPlot::BackendPlot(PlotConfig conf, EventHandler *pEH) {
        config = conf;
        pEventHandler = pEH;

        //calculate minimum plot area width/height based on aspect ratio
        double x = 500/sqrt(config.aspect_ratio);
        plot_area_width = round( config.aspect_ratio*x );
        plot_area_height = round( x );
        //create the surfaces and contexts
        //
        //plot_surface, the shown part of this surface is 250000 pixels (default 500x500)
        //The rest is for when plotting outside of the area
        plot_surface_width = 5*plot_area_width;
        plot_surface_height = 5*plot_area_height;

        //create the surface to draw on
        plot_surface = create_plot_surface();
        plot_context = Cairo::Context::create(plot_surface);
        set_foreground_color();

        //create_xlib_window
        create_xlib_window();

        //draw initial axes etc
        draw_axes_surface();

        time_of_last_update = boost::posix_time::microsec_clock::local_time() - 
            boost::posix_time::microseconds(500000);

        pause_display = false;

        display();
    }

    BackendPlot::~BackendPlot() {
        //delete pEventHandler;
        XCloseDisplay(dpy);
    }

    void BackendPlot::display() {
        //Has the display been paused?
        if (!pause_display ) {
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
            //Only do this if event queue is empty 
            //or last update was more than a 0.5 seconds ago
            if  (pEventHandler->get_queue_size() < 1 
                    || (( now-time_of_last_update )>( boost::posix_time::microseconds(500000))))  {
                temporary_display_surface = create_temporary_surface();
                //copy the temporary surface onto the xlib surface
                xContext->set_source( temporary_display_surface, 0, 0 );
                xContext->paint();

                time_of_last_update = boost::posix_time::microsec_clock::local_time();
                //only sleep if no more events are coming
                //if (pEventHandler->get_queue_size() < 1) {
                //	usleep(100000);
                //}
            }
        }
    }

    void BackendPlot::clear() {
        //give the plot its background color
        transform_to_device_units( plot_context );
        set_background_color( plot_context );
        plot_context->rectangle( 0, 0,
                plot_surface->get_width(), plot_surface->get_height() );
        plot_context->fill();
        set_foreground_color( plot_context );
        display();
    }

    void BackendPlot::handle_xevent( XEvent report ) {
        switch( report.type ) {
            case ConfigureNotify:
                scale_xsurface( report.xconfigure.width, report.xconfigure.height );
                display();
                break;
            case Expose:
                display();
                break;
            case KeyPress:
                if (XLookupKeysym(&report.xkey, 0) == XK_space)  {
                    if (pause_display) {
                        pause_display = false;
                        display();
                    }
                    else
                        pause_display = true;
                } else if (XLookupKeysym(&report.xkey, 0) == XK_w)  {
                    save( "realtimeplot.png", temporary_display_surface );
                } else if (XLookupKeysym(&report.xkey, 0) == XK_Left) {
                    move( -1, 0 );
                } else if (XLookupKeysym(&report.xkey, 0) == XK_Right) {
                    move( 1, 0 );
                } else if (XLookupKeysym(&report.xkey, 0) == XK_Up) {
                    move( 0, 1 );
                } else if (XLookupKeysym(&report.xkey, 0) == XK_Down) {
                    move( 0, -1 );
                } else if (XLookupKeysym(&report.xkey, 0) == XK_KP_Add) { 
                    double xrange = config.max_x-config.min_x;
                    config.min_x+=0.05*xrange;
                    config.max_x-=0.05*xrange;
                    double yrange = config.max_y-config.min_y;
                    config.min_y+=0.05*yrange;
                    config.max_y-=0.05*yrange;
                    update_config();
                } else if (XLookupKeysym(&report.xkey, 0) == XK_KP_Subtract) { 
                    double xrange = config.max_x-config.min_x;
                    config.min_x-=0.05*xrange;
                    config.max_x+=0.05*xrange;
                    double yrange = config.max_y-config.min_y;
                    config.min_y-=0.05*yrange;
                    config.max_y+=0.05*yrange;
                    update_config();
                }
                break;
                //close window
            case ClientMessage:
                pEventHandler->plot_closed();
                //XCloseDisplay(dpy);
                break;
        }
    }


    Cairo::RefPtr<Cairo::ImageSurface> BackendPlot::create_plot_surface() {
     
        Cairo::RefPtr<Cairo::ImageSurface> surface = 
            Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                plot_surface_width, plot_surface_height );

        //Create context to draw background color
        Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);
        
        //give the plot its background color
        set_background_color( context );
        context->rectangle( 0, 0,
                surface->get_width(), surface->get_height() );
        context->fill();

        //set helper variables
        double xratio = ((double(plot_surface_width)/plot_area_width)-1)/2.0;
        double yratio = ((double(plot_surface_height)/plot_area_height)-1)/2.0;
        plot_surface_min_x = config.min_x-xratio*(config.max_x-config.min_x);
        plot_surface_max_x = config.max_x+xratio*(config.max_x-config.min_x);
        plot_surface_min_y = config.min_y-yratio*(config.max_y-config.min_y);
        plot_surface_max_y = config.max_y+yratio*(config.max_y-config.min_y);
        return surface;
    }

    void BackendPlot::create_xlib_window() {
        Window rootwin;
        int scr, white, black;
        if(!(dpy=XOpenDisplay(NULL))) {
            fprintf(stderr, "ERROR: Could not open display\n");
            throw;
        }

        scr = DefaultScreen(dpy);
        rootwin = RootWindow(dpy, scr);
        white = WhitePixel(dpy,scr);
        black = BlackPixel(dpy,scr);
        win = XCreateSimpleWindow(dpy,
                rootwin,
                0, 0,   // origin
                plot_area_width+config.margin_y, plot_area_height+config.margin_x, // size
                0, black, // border
                white );

        XStoreName(dpy, win, "hello");
        XMapWindow(dpy, win);
        XSelectInput( dpy, win, KeyPressMask | StructureNotifyMask | ExposureMask );

        Atom wmDelete=XInternAtom(dpy, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(dpy, win, &wmDelete, 1);
        xSurface = Cairo::XlibSurface::create( dpy, win, DefaultVisual(dpy, 0), 
                plot_area_width+config.margin_y, plot_area_height+config.margin_x);
        xContext = Cairo::Context::create( xSurface );
    }

    void BackendPlot::transform_to_plot_units( ) {
        transform_to_plot_units( plot_context );
    }

    void BackendPlot::transform_to_plot_units( Cairo::RefPtr<Cairo::Context> pContext ) {
        transform_to_device_units( pContext );
        pContext->translate( 0, plot_surface_height );
        pContext->scale( plot_surface_width/(plot_surface_max_x-plot_surface_min_x),
                -plot_surface_height/(plot_surface_max_y-plot_surface_min_y) );
        pContext->translate( -plot_surface_min_x, -plot_surface_min_y );
    }

    void BackendPlot::transform_to_plot_units_with_origin( 
            Cairo::RefPtr<Cairo::ImageSurface> pSurface, 
            Cairo::RefPtr<Cairo::Context> pContext, int margin_x, int margin_y ) {
        transform_to_device_units( pContext );
        //pContext->translate( margin_y, pSurface->get_height()-margin_x );
        pContext->translate( margin_y, pSurface->get_height()-margin_x );
        pContext->scale( (pSurface->get_width()-margin_y)/((config.max_x-config.min_x)),
                -(pSurface->get_height()-margin_x)/((config.max_y-config.min_y)) );
        pContext->translate( -config.min_x, -config.min_y );
    }

    void BackendPlot::transform_to_device_units(
            Cairo::RefPtr<Cairo::Context> pContext) {
        pContext->set_identity_matrix();
    }

    void BackendPlot::draw_axes_surface() {
        //draw them non transparent (else we get weird interactions that when 
        //drawing a transparent point and a rolling update happens the axes 
        //become transparent as well)
        std::vector<float> xaxis_ticks;
        std::vector<float> yaxis_ticks;

        Pango::init();

        //axes_surface, extra margin_x/margin_y pixels for axes and labels
        axes_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                xSurface->get_width(), 
                xSurface->get_height() );
        axes_context = Cairo::Context::create(axes_surface);

        int text_width, text_height;
        Glib::RefPtr<Pango::Layout> pango_layout = Pango::Layout::create(axes_context);
        Pango::FontDescription pango_font = Pango::FontDescription(config.font);
        pango_font.set_weight( Pango::WEIGHT_ULTRALIGHT );
        pango_layout->set_font_description( pango_font );

        /*Cairo::FontOptions font_options = Cairo::FontOptions();
          font_options.set_hint_metrics( Cairo::HINT_METRICS_OFF );
          font_options.set_hint_style( Cairo::HINT_STYLE_NONE );
          font_options.set_antialias( Cairo::ANTIALIAS_NONE );
          pango_layout->get_context()->set_cairo_font_options( font_options );*/


        transform_to_plot_units_with_origin( axes_surface, axes_context, 
                config.margin_x, config.margin_y );
        //plot background color outside the axes (to cover points plotted outside)
        set_background_color( axes_context );
        double dx=config.margin_x;
        double dy=-config.margin_y;
        axes_context->device_to_user_distance( dx, dy );
        axes_context->move_to( config.min_x, config.min_y );
        axes_context->line_to( config.min_x, config.max_y );
        axes_context->line_to( config.min_x-dx, config.max_y );
        axes_context->line_to( config.min_x-dx, config.min_y-dy );
        axes_context->line_to( config.max_x, config.min_y-dy );
        axes_context->line_to( config.max_x, config.min_y );
        axes_context->move_to( config.min_x, config.min_y );
        axes_context->fill();

        //Plot the main axes lines
        set_foreground_color( axes_context );
        axes_context->move_to( config.min_x, config.min_y );
        axes_context->line_to( config.min_x, config.max_y );
        axes_context->move_to( config.min_x, config.min_y );
        axes_context->line_to( config.max_x, config.min_y );

        //Plot the ticks + tick labels
        xaxis_ticks = axes_ticks( config.min_x, config.max_x, config.nr_of_ticks );
        yaxis_ticks = axes_ticks( config.min_y, config.max_y, config.nr_of_ticks );

        double length_tick_x = config.ticks_length;
        double length_tick_y = -config.ticks_length;
        axes_context->device_to_user_distance( length_tick_x, length_tick_y );

        for (unsigned int i = 0; i < xaxis_ticks.size(); ++i) {
            axes_context->move_to( xaxis_ticks[i], config.min_y );
            axes_context->rel_line_to( 0, length_tick_y );
            transform_to_device_units( axes_context );
            pango_layout->set_text( stringify( xaxis_ticks[i] ) );
            pango_layout->get_pixel_size( text_width, text_height );
            axes_context->rel_move_to( -0.5*text_width, 1*text_height );
            //pango_layout->add_to_cairo_context(axes_context); //adds text to cairos stack of stuff to be drawn
            pango_layout->show_in_cairo_context( axes_context );
            transform_to_plot_units_with_origin( axes_surface, axes_context, 
                    config.margin_x, config.margin_y );
        }

        for (unsigned int i = 0; i < yaxis_ticks.size(); ++i) {
            axes_context->move_to( config.min_x, yaxis_ticks[i] );
            axes_context->rel_line_to( length_tick_x, 0 );

            transform_to_device_units( axes_context );
            axes_context->rotate_degrees( -90 );

            pango_layout->set_text( stringify( yaxis_ticks[i] ) );
            pango_layout->get_pixel_size( text_width, text_height );
            axes_context->rel_move_to( -0.5*text_width, -2*text_height );
            pango_layout->show_in_cairo_context( axes_context );
            axes_context->rotate_degrees( 90 ); //think the tranform_to_plot_units also unrotates
            transform_to_plot_units_with_origin( axes_surface, axes_context, 
                    config.margin_x, config.margin_y );
        }

        transform_to_device_units( axes_context );

        pango_layout->set_text( config.ylabel );
        pango_layout->get_pixel_size( text_width, text_height );

        axes_context->move_to( config.margin_y-3*text_height, 
                0.5*axes_surface->get_height()+0.5*text_width );
        axes_context->save();
        axes_context->rotate_degrees( -90 );
        pango_layout->show_in_cairo_context( axes_context );
        axes_context->restore();

        pango_layout->set_text( config.xlabel );
        pango_layout->get_pixel_size( text_width, text_height );
        axes_context->move_to( 
                config.margin_y+0.5*axes_surface->get_width()-0.5*text_width, 
                axes_surface->get_height()-config.margin_x+1.5*text_height );
        pango_layout->show_in_cairo_context( axes_context );

        axes_context->stroke();
    }

    void BackendPlot::set_background_color( Cairo::RefPtr<Cairo::Context> pContext ) {
        pContext->set_source_rgba(1, 1, 1, 1);
    }

    void BackendPlot::set_foreground_color() {
        set_foreground_color( plot_context );
    }
    void BackendPlot::set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext ) {
        pContext->set_source_rgba(0, 0, 0, 1);
    }

    void BackendPlot::set_color( Color color ) {
        plot_context->set_source_rgba( color.r, color.g, color.b, color.a );
    }

    void BackendPlot::point( float x, float y ) {
        if (!within_plot_bounds(x,y)) {
            if (!config.fixed_plot_area)
                rolling_update(x, y);
        }
        double dx = config.point_size;
        double dy = config.point_size;
        transform_to_plot_units(); 
        plot_context->device_to_user_distance(dx,dy);
        plot_context->rectangle( x-0.5*dx, y-0.5*dy, dx, dy );
        transform_to_device_units( plot_context );
        plot_context->fill();
        display();
    }

    void BackendPlot::line_add( float x, float y, int id ) {
        if (!within_plot_bounds(x,y)) {
            if (!config.fixed_plot_area)
                rolling_update(x, y);
        }

        LineAttributes *line = new LineAttributes( x, y, id );

        //check if line already exists
        bool exists = false;
        std::list<LineAttributes*>::iterator i;
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
            //plot_surface might have been updated, for example due to rolling_update
            line->context = Cairo::Context::create( plot_surface );

            transform_to_device_units( line->context );
            set_foreground_color( line->context );
            transform_to_plot_units( line->context );
            line->context->move_to( line->current_x, line->current_y );
            line->context->line_to( x, y );
            transform_to_device_units( line->context );
            line->context->stroke();
            line->current_x = x;
            line->current_y = y;
            display();
        }
    }

    void BackendPlot::save( std::string fn ) {
        Cairo::RefPtr<Cairo::ImageSurface> surface = create_temporary_surface();
        save(fn, surface );
    }

    void BackendPlot::save( std::string fn, 
            Cairo::RefPtr<Cairo::ImageSurface> pSurface ) {
        pSurface->write_to_png( fn );
    }



    void BackendPlot::number( float x, float y, float i) {
        if (!within_plot_bounds(x,y)) {
            if (!config.fixed_plot_area)
                rolling_update(x, y);
        }
        transform_to_plot_units(); 
        plot_context->move_to( x, y );
        transform_to_device_units( plot_context );
        set_foreground_color( plot_context );
        plot_context->show_text( stringify( i ) );

        display();
    }


    void BackendPlot::rolling_update( float x, float y ) {
        std::vector<int> direction;
        direction.push_back( 0 );
        direction.push_back( 0 );
        if (x>config.max_x) {
            direction[0] = 1;
        } else if (x<config.min_x) {
            direction[0] = -1;
        } else if (y>config.max_y) {
            direction[1] = 1;
        } else if (y<config.min_y) {
            direction[1] = -1;
        }

        //update min_x etc
        double xrange = config.max_x-config.min_x;
        config.min_x += direction[0]*xrange*(1-config.overlap);
        config.max_x = config.min_x+xrange;
        double yrange = config.max_y-config.min_y;
        config.min_y += direction[1]*yrange*(1-config.overlap);
        config.max_y = config.min_y+yrange;

        if (!plot_bounds_within_surface_bounds()) {
            //copy old plot surface
            double old_plot_max_y = plot_surface_max_y;
            double old_plot_min_x = plot_surface_min_x;

            //create new blank surface
            Cairo::RefPtr<Cairo::ImageSurface> surface = create_plot_surface();
            Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);

            //copy old plot surface onto the blank surface
            transform_to_plot_units( context );
            context->user_to_device( old_plot_min_x, old_plot_max_y );
            transform_to_device_units( context );
            context->set_source( plot_surface, old_plot_min_x, old_plot_max_y );
            context->paint();

            //copy the newly created surface over the old plot
            plot_context->set_source( surface, 0, 0 );
            plot_context->paint();

            set_foreground_color();
        }
        //be recursive about it :)
        if (within_plot_bounds( x, y )) {
            draw_axes_surface();
        } else {
            rolling_update( x, y );
        }
    }

    bool BackendPlot::within_plot_bounds( float x, float y ) {
        if ( x < config.min_x || x > config.max_x ||
                y < config.min_y || y > config.max_y )
            return false;
        else
            return true;
    }

    bool BackendPlot::plot_bounds_within_surface_bounds( ) {
        if ( config.min_x <= plot_surface_min_x || config.max_x >= plot_surface_max_x ||
                config.min_y <= plot_surface_min_y || config.max_y >= plot_surface_max_y )
            return false;
        else
            return true;
    }

    std::vector<float> BackendPlot::axes_ticks( float min, float max, int nr ) {
        std::vector<float> ticks;
        int power = 0;
        float tick;
        float step = (max-min)/nr;

        //Calculate power of step (i.e. 0.01 -> power is -2)
        //Using straightforward method. Is probably much easier way
        if (step <= 1) {
            while (step/pow(10,power)<=1) {
                --power;
            }
        } else if (step >=10) {
            while (step/pow(10,power)<=1) {
                ++power;
            }
        }

        //round our step
        step = round(step/pow(10,power))*pow(10,power);

        //first tick is rounded version of min
        tick = round(min/pow(10,power))*pow(10,power);

        while (tick <= max ) {
            if (tick>=min) {
                ticks.push_back( tick );
            }
            tick += step;
        }
        return ticks;
    }

    /** \brief Create an temporary imagesurface 
     *
     * (using a temp surface gets rid of flickering we get if we 
     * plot plot_surface and then axes_surface
     * directly onto xlibsurface
     */
    Cairo::RefPtr<Cairo::ImageSurface> BackendPlot::create_temporary_surface() {

        Cairo::RefPtr<Cairo::ImageSurface> surface = 
            Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                    xSurface->get_width(), xSurface->get_height() );
        Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( surface );


        double x = plot_surface_min_x;
        double y = plot_surface_max_y;
        transform_to_plot_units_with_origin( surface, context,
                config.margin_x, config.margin_y );
        context->user_to_device( x, y );

        transform_to_device_units( context );
        context->translate( x, y );
        context->scale( double(xSurface->get_width()-config.margin_y)/plot_area_width,
                double(xSurface->get_height()-config.margin_x)/plot_area_height );

        //copy the plot onto our temporary image surface
        context->set_source( plot_surface, 0, 0 );
        context->paint();
        //copy the axes onto our temporary image surface
        transform_to_device_units( context );
        context->set_source( axes_surface, 0, 0 );
        context->paint();
        return surface;
    }

    void BackendPlot::move( int direction_x, int direction_y ) {
        double xrange = config.max_x-config.min_x;
        config.min_x += 0.05*direction_x*xrange;
        config.max_x = config.min_x+xrange;
        double yrange = config.max_y-config.min_y;
        config.min_y += 0.05*direction_y*yrange;
        config.max_y = config.min_y+yrange;

        //don't move outside of the plot_surface, since we don't have that data anymore
        if (config.max_x>plot_surface_max_x) {
            config.max_x = plot_surface_max_x;
            config.min_x = config.max_x-xrange;
        } else if (config.min_x<plot_surface_min_x) {
            config.min_x = plot_surface_min_x;
            config.max_x = config.min_x+xrange;
        }
        if (config.max_y>plot_surface_max_y) {
            config.max_y = plot_surface_max_y;
            config.min_y = config.max_y-yrange;
        } else if (config.min_y<plot_surface_min_y) {
            config.min_y = plot_surface_min_y;
            config.max_y = config.min_y+yrange;
        }

        draw_axes_surface();
        display();
    }

    void BackendPlot::update_config() { 
        //Check that it can be done:
        if (config.max_x > plot_surface_max_x)
            config.max_x = plot_surface_max_x;
        if (config.min_x < plot_surface_min_x)
            config.min_x = plot_surface_min_x;
        if (config.max_y > plot_surface_max_y)
            config.max_y = plot_surface_max_y;
        if (config.min_y < plot_surface_min_y)
            config.min_y = plot_surface_min_y;
        //Temporary put here, should only be done when min_x/max_x change
        //recalculate plot_area_width
        transform_to_plot_units( plot_context );
        double width,height;
        width = config.max_x-config.min_x;
        height = config.max_y-config.min_y;
        plot_context->user_to_device_distance( width, height );
        transform_to_device_units( plot_context );
        plot_area_width = round(width);
        plot_area_height = round(-height);
        width = xSurface->get_width();
        height = xSurface->get_height();
        xSurface = Cairo::XlibSurface::create( dpy, win, DefaultVisual(dpy, 0), 
                plot_area_width+config.margin_y, plot_area_height+config.margin_x);
        scale_xsurface( width, height );
        draw_axes_surface();
        display();
    }

    void BackendPlot::scale_xsurface( double width, double height ) {
        xSurface->set_size( width, height );
        xContext = Cairo::Context::create( xSurface );
        draw_axes_surface();
        //xContext->scale( float(xSurface->get_width())/(plot_area_width+config.margin_y),
        //        float(xSurface->get_height())/(plot_area_height+config.margin_x) );
    }

}

