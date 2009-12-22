#include "cairo_plot/plot.h"

namespace cairo_plot {

    PointEvent::PointEvent( float x, float y ) {
        x_crd = x;
        y_crd = y;
    }

    void PointEvent::execute( BackendPlot *pBPlot ) {
        pBPlot->point( x_crd, y_crd );
    }

    LineAddEvent::LineAddEvent( float x, float y ) {
        x_crd = x;
        y_crd = y;
    }

    void LineAddEvent::execute( BackendPlot *pBPlot ) {
        pBPlot->line_add( x_crd, y_crd );
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
        pBPlot->set_alpha( alpha );
        pBPlot->point( x_crd, y_crd );
        pBPlot->set_alpha( 1 );
    }

    SaveEvent::SaveEvent( std::string fn ) {
        filename = fn;
    }

    void SaveEvent::execute( BackendPlot *pBPlot ) {
        pBPlot->save(filename);
    }


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

    void Plot::line_add( float x, float y ) {
        Event *pEvent = new LineAddEvent( x, y );
        pEvent_Handler->add_event( pEvent );
    }

    void Plot::number( float x, float y, float i ) {
        Event *pEvent = new NumberEvent( x, y, i );
        pEvent_Handler->add_event( pEvent );
    }

    void Plot::point_transparent( float x, float y, float a ) {
        Event *pEvent = new PointTransparentEvent( x, y, a );
        pEvent_Handler->add_event( pEvent );
    }

    void Plot::save( std::string filename ) {
        Event *pEvent = new SaveEvent( filename );
        pEvent_Handler->add_event( pEvent );
    }

    /*
     * BackendPlot
     */

    BackendPlot::BackendPlot(PlotConfig conf, EventHandler *pEH) {
        config = conf;
        alpha = 1;
        pEventHandler = pEH;

        //create the surface to draw on
        create_plot_surface();

        //draw initial axes etc
        draw_axes_surface();

        //create_xlib_window
        create_xlib_window();

        time_of_last_update = boost::posix_time::microsec_clock::local_time() - boost::posix_time::microseconds(200000);

        pause_display = false;

        display();
    }

    BackendPlot::~BackendPlot() {
        XCloseDisplay(dpy);
    }

    void BackendPlot::display() {
        //Has the display been paused?
        if (!pause_display ) {
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
            //Only do this if event queue is empty 
            //or last update was more than a 0.1 seconds ago
            if  (pEventHandler->get_queue_size() < 1 
                    || (( now-time_of_last_update )>( boost::posix_time::microseconds(200000))))  {
                temporary_display_surface = create_temporary_surface();
                //copy the temporary surface onto the xlib surface
                xContext->set_source( temporary_display_surface, 0, 0 );
                xContext->paint();

                time_of_last_update = boost::posix_time::microsec_clock::local_time();
                //only sleep if no more events are coming
                if (pEventHandler->get_queue_size() < 1) {
                    usleep(100000);
                }
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
            case KeyPress:
                if (XLookupKeysym(&report.xkey, 0) == XK_space)  {
                    if (pause_display) {
                        pause_display = false;
                        display();
                    }
                    else
                        pause_display = true;
                } else if (XLookupKeysym(&report.xkey, 0) == XK_w)  {
									save( "cairo_plot.png", temporary_display_surface );
								} /*else if (XLookupKeysym(&report.xkey, 0) == XK_arrow_left) {
										move( -1, 0 );
								} else if (XLookupKeysym(&report.xkey, 0) == XK_arrow_right) {
										move( 1, 0 );
								} else if (XLookupKeysym(&report.xkey, 0) == XK_arrow_up) {
										move( 0, 1 );
								} else if (XLookupKeysym(&report.xkey, 0) == XK_arrow_down) {
										move( 0, -1 );
								}*/
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
        XSelectInput( dpy, win, KeyPressMask | StructureNotifyMask | ExposureMask );
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
        //draw them non transparent (else we get weird interactions that when 
        //drawing a transparent point and a rolling update happens the axes 
        //become transparent as well)
        float old_alpha = alpha;
        Cairo::Matrix y_font_matrix;
        Cairo::Matrix x_font_matrix;
        std::vector<float> xaxis_ticks;
        std::vector<float> yaxis_ticks;
        alpha = 1;

        Cairo::RefPtr<Cairo::ToyFontFace> font =
            Cairo::ToyFontFace::create("Bitstream Charter",
                    Cairo::FONT_SLANT_ITALIC,
                    Cairo::FONT_WEIGHT_BOLD);
        //axes_surface, extra 50 pixels for axes and labels
        axes_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                plot_area_width+50, plot_area_height+50 );
        axes_context = Cairo::Context::create(axes_surface);
        transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );
        //plot background color outside the axes (to cover points plotted outside)
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

        axes_context->get_font_matrix(x_font_matrix);
        //this might be technically wrong, (.yy and .xx reversed) but normally
        //.xx==.yy so it doesn't matter
        y_font_matrix = Cairo::Matrix( 0, -x_font_matrix.yy, x_font_matrix.xx, 0, 0, 0 );


        //Plot the ticks + tick labels
        xaxis_ticks = axes_ticks( config.min_x, config.max_x, 10 );
        yaxis_ticks = axes_ticks( config.min_y, config.max_y, config.nr_of_ticks );
        
        double length_tick_x = config.ticks_length;
        double length_tick_y = -config.ticks_length;
        axes_context->device_to_user_distance( length_tick_x, length_tick_y );

        for (int i = 0; i < xaxis_ticks.size(); ++i) {
            axes_context->move_to( xaxis_ticks[i], config.min_y );
            axes_context->line_to( xaxis_ticks[i], config.min_y+length_tick_y );
            axes_context->move_to( xaxis_ticks[i], config.min_y-2*length_tick_y );
            transform_to_device_units( axes_context );
            axes_context->set_font_matrix( x_font_matrix );
            axes_context->show_text( stringify( xaxis_ticks[i] ) );
            transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );
         }

        for (int i = 0; i < yaxis_ticks.size(); ++i) {
            axes_context->move_to( config.min_x, yaxis_ticks[i] );
            axes_context->line_to( config.min_x+length_tick_x, yaxis_ticks[i] );
            axes_context->move_to( config.min_x-2*length_tick_x, yaxis_ticks[i] );
            transform_to_device_units( axes_context );
            axes_context->set_font_matrix( y_font_matrix );
            axes_context->show_text( stringify( yaxis_ticks[i] ) );
            transform_to_plot_units_with_origin( axes_surface, axes_context, 50, 50 );

        }

        transform_to_device_units( axes_context );

        axes_context->move_to( 20, round(0.5*plot_area_height+25) );
        axes_context->set_font_matrix( y_font_matrix );
        axes_context->show_text( config.ylabel );
        axes_context->move_to( round(0.5*plot_area_width+25), plot_area_height+25 );
        axes_context->set_font_matrix( x_font_matrix );
        axes_context->show_text( config.xlabel );
        axes_context->stroke();
        alpha = old_alpha;
    }

    void BackendPlot::set_background_color( Cairo::RefPtr<Cairo::Context> pContext ) {
        pContext->set_source_rgba(1, 1, 1, 1);
    }

    void BackendPlot::set_foreground_color( Cairo::RefPtr<Cairo::Context> pContext ) {
        pContext->set_source_rgba(0,0,0, alpha);
    }
    
    void BackendPlot::set_alpha( float a ) {
        alpha = a;
    }

    void BackendPlot::point( float x, float y) {
        if (!within_plot_bounds(x,y)) {
            if (!config.fixed_plot_area)
                rolling_update(x, y);
        }
        double dx = config.point_size;
        double dy = config.point_size;
        set_foreground_color( plot_context );
        transform_to_plot_units(); 
        plot_context->device_to_user_distance(dx,dy);
        plot_context->rectangle( x-0.5*dx, y-0.5*dy, dx, dy );
        transform_to_device_units( plot_context );
        plot_context->fill();

        display();
    }

    void BackendPlot::line_add( float x, float y ) {
        if (!within_plot_bounds(x,y)) {
            if (!config.fixed_plot_area)
                rolling_update(x, y);
        }
        if (line_context == NULL) {
            line_context = Cairo::Context::create( plot_surface );
            line_old_x = x;
            line_old_y = y;
        } else {
            //plot_surface might have been updated, by other actions
            transform_to_device_units( line_context );
            line_context = Cairo::Context::create( plot_surface );
            set_foreground_color( line_context );
            transform_to_plot_units( line_context );
            line_context->move_to( line_old_x, line_old_y );
            line_context->line_to( x, y );
            transform_to_device_units( line_context );
            line_context->stroke();
            line_old_x = x;
            line_old_y = y;
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

    Cairo::RefPtr<Cairo::ImageSurface> BackendPlot::create_temporary_surface() {
        //Create an temporary imagesurface (using a temp surface gets rid of
        //flickering we get if we plot plot_surface and then axes_surface
        //directly onto xlibsurface
        Cairo::RefPtr<Cairo::ImageSurface> surface = 
            Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                    50+plot_area_width, 50+plot_area_height );
        Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create( surface );

        transform_to_plot_units();
        double x = config.min_x;
        double y = config.max_y;
        plot_context->user_to_device( x, y );
        //copy the plot onto our temporary image surface
        context->set_source( plot_surface, -x+50, -y );
        context->paint();
        //copy the axes onto our temporary image surface
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

			display();
		}
}
