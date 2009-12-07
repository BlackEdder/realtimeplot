#include "cairo_plot/plot.h"

namespace cairo_plot {

    Plot::Plot( PlotConfig conf ) {
        pEvent_Handler = new EventHandler( conf );
    }

    Plot::~Plot() {
        delete pEvent_Handler;
    }

    void Plot::point( float x, float y ) {
        Event *pEvent = new PointEvent( x, y );
        pEvent_Handler->add_event( pEvent );
    }

    void Plot::number( float x, float y, float i ) {
        Event *pEvent = new NumberEvent( x, y, i );
        pEvent_Handler->add_event( pEvent );
    }

    /*
     * BackendPlot
     */

    BackendPlot::BackendPlot(PlotConfig conf, EventHandler *pEH) {
        config = conf;
        pEventHandler = pEH;

        //create the surface to draw on
        create_plot_surface();

        //draw initial axes etc
        draw_axes_surface();

        //create_xlib_window
        create_xlib_window();

        time_of_last_update = time(0)-2;

        display();
    }

    BackendPlot::~BackendPlot() {
        XCloseDisplay(dpy);
    }

    void BackendPlot::display() {
        //Only do this if event queue is empty or last update was more than a second ago
        if (pEventHandler->event_queue.size() < 1 
                || (time(0)-time_of_last_update)>=1)  {
            transform_to_plot_units();
            double x = config.min_x;
            double y = config.max_y;
            plot_context->user_to_device( x, y );
            //xContext->set_source( axes_surface, 0, 0 );
            //xContext->paint();
            xContext->rectangle(50,0,plot_area_width, plot_area_height);
            xContext->set_source( plot_surface, -x+50, -y );
            xContext->fill();
            xContext->set_source( axes_surface, 0, 0 );
            xContext->paint();
            time_of_last_update = time(0);
            //only sleep if no more events are coming
            if (pEventHandler->event_queue.size() < 1) {
               usleep(100000);
            }
        }
    }

    void BackendPlot::handle_xevent( XEvent report ) {
        switch( report.type ) {
            case ConfigureNotify:
                xSurface->set_size( report.xconfigure.width,
                        report.xconfigure.height );
                //std::cout << report.xconfigure.width << std::endl;
                //std::cout << report.xconfigure.height << std::endl;
                xContext = Cairo::Context::create( xSurface );
                xContext->scale( float(xSurface->get_width())/(plot_area_width+50),
                        float(xSurface->get_height())/(plot_area_height+50) );
                display();
                break;
            case Expose:
                display();
                break;
        }
    }

    void BackendPlot::create_plot_surface() {
        //calculate minimum plot area width/height based on aspect ratio
        double x = 500/sqrt(config.aspect_ratio);
        plot_area_width = round( config.aspect_ratio*x );
        plot_area_height = round( x );
        //create the surfaces and contexts
        //
        //plot_surface, the shown part of this surface is 500 by 500
        //The rest is for when plotting outside of the area
        plot_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                5*plot_area_width, 5*plot_area_height );
        plot_context = Cairo::Context::create(plot_surface);
        //give the plot its background color
        set_background_color( plot_context );
        plot_context->rectangle( 0, 0,
                plot_surface->get_width(), plot_surface->get_height() );
        plot_context->fill();

        //set helper variables
        plot_surface_min_x = config.min_x-2*(config.max_x-config.min_x);
        plot_surface_max_x = config.max_x+2*(config.max_x-config.min_x);
        plot_surface_min_y = config.min_y-2*(config.max_y-config.min_y);
        plot_surface_max_y = config.max_y+2*(config.max_y-config.min_y);
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
                plot_area_width+50, plot_area_height+50, // size
                0, black, // border
                white );
        
        XStoreName(dpy, win, "hello");
        XMapWindow(dpy, win);
        XSelectInput( dpy, win, StructureNotifyMask | ExposureMask );
        xSurface = Cairo::XlibSurface::create( dpy, win , DefaultVisual(dpy, 0), 
                plot_area_width+50, plot_area_height+50);
        xContext = Cairo::Context::create( xSurface );
    }

    void BackendPlot::transform_to_plot_units( ) {
        transform_to_plot_units( plot_context );
    }

    void BackendPlot::transform_to_plot_units( Cairo::RefPtr<Cairo::Context> pContext ) {
        transform_to_device_units( pContext );
        pContext->translate( 0, plot_area_height*5 );
        pContext->scale( plot_area_width*5/(plot_surface_max_x-plot_surface_min_x),
                -plot_area_height*5/(plot_surface_max_y-plot_surface_min_y) );
        pContext->translate( -plot_surface_min_x, -plot_surface_min_y );
    }

    void BackendPlot::transform_to_plot_units_with_origin( 
            Cairo::RefPtr<Cairo::ImageSurface> pSurface, 
            Cairo::RefPtr<Cairo::Context> pContext, int origin_x, int origin_y ) {
        transform_to_device_units( pContext );
        pContext->translate( origin_x, pSurface->get_height()-origin_y );
        pContext->scale( plot_area_width/((config.max_x-config.min_x)),
                -plot_area_height/((config.max_y-config.min_y)) );
        pContext->translate( -config.min_x, -config.min_y );
    }

    void BackendPlot::transform_to_device_units(Cairo::RefPtr<Cairo::Context> pContext) {
        pContext->set_identity_matrix();
    }

    void BackendPlot::draw_axes_surface() {
        Cairo::RefPtr<Cairo::ToyFontFace> font =
            Cairo::ToyFontFace::create("Bitstream Charter",
                    Cairo::FONT_SLANT_ITALIC,
                    Cairo::FONT_WEIGHT_BOLD);
        //axes_surface, extra 50 pixels for axes and labels
        axes_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                plot_area_width+50, plot_area_height+50 );
        axes_context = Cairo::Context::create(axes_surface);
        transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );
        //Plot background for axes (remember this will be used as a mask and plotted points
        //should not appear outside the axes
        set_background_color( axes_context );
        double dx=50;
        double dy=-50;
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
        double dtick_x = (config.max_x-config.min_x)/config.nr_of_ticks;
        double dtick_y = (config.max_y-config.min_y)/config.nr_of_ticks;
        double length_tick_x = config.ticks_length;
        double length_tick_y = -config.ticks_length;
        axes_context->device_to_user_distance( length_tick_x, length_tick_y );
        for (int i = 0; i < config.nr_of_ticks; ++i) {
            axes_context->move_to( config.min_x+i*(dtick_x), config.min_y );
            axes_context->line_to( config.min_x+i*(dtick_x), config.min_y+length_tick_y );
            axes_context->move_to( config.min_x+i*(dtick_x), config.min_y-2*length_tick_y );
            transform_to_device_units( axes_context );
            axes_context->show_text( stringify( config.min_x+i*dtick_x ) );
            transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );

            axes_context->move_to( config.min_x, config.min_y+i*(dtick_y) );
            axes_context->line_to( config.min_x+length_tick_x, config.min_y+i*(dtick_y) );
            axes_context->move_to( config.min_x-3*length_tick_x, config.min_y+i*(dtick_y) );
            transform_to_device_units( axes_context );
            axes_context->show_text( stringify( config.min_y+i*dtick_y ) );
            transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );

        }

        transform_to_device_units( axes_context );

        axes_context->move_to( 20, round(0.5*plot_area_height+25) );
        axes_context->show_text( config.ylabel );
        axes_context->move_to( round(0.5*plot_area_width+25), plot_area_height+25 );
        axes_context->show_text( config.xlabel );
        axes_context->stroke();
    }

    void BackendPlot::set_background_color( Cairo::RefPtr<Cairo::Context> pContext ) {
        pContext->set_source_rgb(1,1,1);
    }

    void BackendPlot::set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext ) {
        pContext->set_source_rgb(0,0,0);
    }

    void BackendPlot::point( float x, float y) {
        if (!within_plot_bounds(x,y)) {
            if (!config.fixed_plot_area)
                rolling_update(x, y);
        }
        double dx = 5;
        double dy = 5;
        set_foreground_color( plot_context );
        transform_to_plot_units(); 
        plot_context->device_to_user_distance(dx,dy);
        plot_context->rectangle( x-0.5*dx, y-0.5*dy, dx, dy );
        transform_to_device_units( plot_context );
        plot_context->stroke();

        display();
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
            Cairo::RefPtr<Cairo::ImageSurface> old_plot_surface = plot_surface;
            double old_plot_max_y = plot_surface_max_y;
            double old_plot_min_x = plot_surface_min_x;
            //create new plot surface
            create_plot_surface();
            //copy old plot surface onto new plot surface
            transform_to_plot_units();
            plot_context->user_to_device( old_plot_min_x, old_plot_max_y );
            transform_to_device_units( plot_context );
            plot_context->set_source( old_plot_surface, old_plot_min_x, old_plot_max_y );
            plot_context->reset_clip();
            plot_context->paint();
            set_background_color( plot_context );
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
}
