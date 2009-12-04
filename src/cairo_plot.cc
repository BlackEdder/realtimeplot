#include "cairo_plot.h"

namespace cairo_plot {

    /*
     * Implementation of PlotSurface class
     */

    PlotSurface::PlotSurface( PlotConfig *conf,
            Cairo::RefPtr<Cairo::ImageSurface> surface ) {
        pSurface = surface;
        config = conf;
    }

    /*
     * Draws the axes
     */
    void PlotSurface::paint( Cairo::RefPtr<Cairo::Context> pContext ) {
        Cairo::RefPtr<Cairo::ToyFontFace> font =
            Cairo::ToyFontFace::create("Bitstream Charter",
                    Cairo::FONT_SLANT_ITALIC,
                    Cairo::FONT_WEIGHT_BOLD);
        //empty outside of bounding box
        pContext->set_source_rgb(1, 1, 1);
        pContext->rectangle( 0, config->pixel_height-config->origin_y, config->pixel_width, config->origin_y );
        pContext->fill();
        pContext->rectangle( 0, 0, config->origin_x, config->pixel_height );
        pContext->fill();
 
        
        //bounding box
        pContext->set_source_rgb(0, 0, 0);
        pContext->rectangle( config->origin_x, 0,
                this->get_pixel_width(), 
                this->get_pixel_height() );

        //ticks
        pContext->set_font_face(font);
        pContext->set_font_size(10);
        for (int i=0; i<config->nr_of_ticks; ++i) {
            Coord coord_x = to_pixel_coord( 
                    Coord( config->min_x+i*(config->max_x-config->min_x)/(config->nr_of_ticks-1), config->min_y ) );
            pContext->move_to( coord_x.x, coord_x.y );
            pContext->line_to( coord_x.x, coord_x.y-config->ticks_length );
            pContext->move_to( coord_x.x, coord_x.y+3*config->ticks_length);
            pContext->show_text(
                    stringify(config->min_x+i*(config->max_x-config->min_x)/(config->nr_of_ticks-1)));
            Coord coord_y = to_pixel_coord( 
                    Coord( config->min_x, config->min_y+i*(config->max_y-config->min_y)/(config->nr_of_ticks-1) ) );
            pContext->move_to( coord_y.x, coord_y.y );
            pContext->line_to( coord_y.x+config->ticks_length, coord_y.y );
            pContext->move_to( coord_y.x-3*config->ticks_length, coord_y.y );
            pContext->show_text(stringify(config->min_y+i*(config->max_y-config->min_y)/(config->nr_of_ticks-1)));
        }

        //plot labels
        Coord tmp_coord = to_pixel_coord( 
                Coord( config->min_x, (config->max_y-config->min_y)/2.0+config->min_y ));
        pContext->move_to( tmp_coord.x-5*config->ticks_length, tmp_coord.y  );
        pContext->show_text( config->ylabel );
        tmp_coord = to_pixel_coord( 
                Coord( (config->max_x-config->min_x)/2.0+config->min_x , config->min_y ));
        pContext->move_to( tmp_coord.x, tmp_coord.y+5*config->ticks_length  );
        pContext->show_text( config->xlabel );

        pContext->stroke();
        pContext->set_source_rgb(1, 1, 1);
    }

    int PlotSurface::get_pixel_width() {
        return pSurface->get_width() - config->origin_x;
    }

    int PlotSurface::get_pixel_height() {
        return pSurface->get_height() - config->origin_y;
    }

    Coord PlotSurface::to_pixel_coord( Coord plot_coords ) {
        Coord pixel_coord = Coord( 
                this->get_pixel_width()*(plot_coords.x-config->min_x)/(config->max_x-config->min_x)+config->origin_x,
                pSurface->get_height()-(config->origin_y+this->get_pixel_height()*(plot_coords.y-config->min_y)/(config->max_y-config->min_y) ));
        return pixel_coord;
    }

    /*
     * Implementation of Plot class
     */
    Plot::Plot( PlotConfig *conf ) {
        loop_started = False;
        config = conf;

        surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                config->pixel_width, config->pixel_height );
        context = Cairo::Context::create(surface);

        plot_surface = PlotSurface( config, surface );
        plot_surface.paint( context );

        pEvent_thrd = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &cairo_plot::Plot::event_loop, this ) ) );
    }

    Plot::~Plot() { 
        pEvent_thrd->join(); 
    }

    void Plot::event_loop() {
        Window rootwin;
        int scr, white, black;
        Cairo::RefPtr<Cairo::XlibSurface> xSurface;
        Cairo::RefPtr<Cairo::Context> xContext;
        XEvent report;

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
                config->pixel_width, config->pixel_height, // size
                0, black, // border
                white );
        XStoreName(dpy, win, "hello");
        XMapWindow(dpy, win);
        XSelectInput( dpy, win, StructureNotifyMask | ExposureMask );
        //nu snap ik het weer. Maar bovenstaande kan mooier

        xSurface = Cairo::XlibSurface::create( dpy, win , DefaultVisual(dpy, 0), 
                config->pixel_width, config->pixel_height);
        xContext = Cairo::Context::create( xSurface );

        xContext->set_source( surface, 0, 0 );

        loop_started = True;

        while(1) {
            if (XPending(dpy)>0) {
                XNextEvent( dpy, &report ); 
                switch( report.type ) {
                    case ConfigureNotify:
                        xSurface->set_size( report.xconfigure.width,
                                report.xconfigure.height );
                        //std::cout << report.xconfigure.width << std::endl;
                        //std::cout << report.xconfigure.height << std::endl;
                        xContext = Cairo::Context::create( xSurface );
                        xContext->scale( float(xSurface->get_width())/surface->get_width(),
                                float(xSurface->get_height())/surface->get_height() );
                        xContext->set_source( surface, 0, 0 );
                        break;
                    case Expose:
                        if (report.xexpose.count<1) {
                            xContext->set_source( surface, 0, 0 );
                            xContext->paint();
                            usleep(100000); //max fps of 10
                        }
                        break;
                }
            } else {
                if (updated) {
                    xContext->set_source( surface, 0, 0 );
                    xContext->paint();
                    updated = false;
                    usleep(100000); 
                } else {
                    usleep(100000);
                }
            }
        }
        XCloseDisplay(dpy);
    }


    void Plot::plot_point( float x, float y ) {
        bool draw = true;
        if (!check_bounds( Coord(x,y) )) {
            if (!config->update_rolling)
                draw = false;
            else {
                update_bounds_rolling( Coord(x,y) );
            }
        } 
        
        if (draw) {
            Coord pixel_coord = plot_surface.to_pixel_coord( Coord(x, y) );
            context->set_source_rgb(0, 0, 0);
            context->rectangle( pixel_coord.x, pixel_coord.y, 1, 1 );
            context->stroke();
            context->set_source_rgb(1, 1, 1);
            updated = true;
        }
    }

    bool Plot::check_bounds( Coord crd ) {
        if ( crd.x < config->min_x || crd.x > config->max_x ||
                crd.y < config->min_y || crd.y > config->max_y )
            return false;
        else
            return true;
    }

    /*
     * Method responsible to set new bounds and redraw picture to include crd
     */
    void Plot::update_bounds_rolling( Coord crd ) {
        float range;
        float old_max_y = config->max_y;
        float old_min_x = config->min_x;
        float surface_new_x = 0;
        float surface_new_y = 0;
        //set new bounds
        //For now don't worry about case that point is twice as far away,
        //i.e. an update of bounds in the new direction will include crd
        if (crd.x > config->max_x) {
            range = config->max_x-config->min_x;
            config->min_x = config->max_x - config->rolling_overlap*range;
            config->max_x = config->min_x + range;
            surface_new_x = plot_surface.to_pixel_coord( 
                    Coord( old_min_x, config->min_y ) ).x-config->origin_x;
        } else if (crd.x < config->min_x) {
            range = config->max_x-config->min_x;
            config->max_x = config->min_x + config->rolling_overlap*range;
            config->min_x = config->max_x - range;
            surface_new_x = plot_surface.to_pixel_coord( 
                    Coord( old_min_x, config->min_y ) ).x-config->origin_x;
         }
        if (crd.y > config->max_y) {
            range = config->max_y-config->min_y;
            config->min_y = config->max_y - config->rolling_overlap*range;
            config->max_y = config->min_y + range;
            surface_new_y = plot_surface.to_pixel_coord( 
                    Coord( config->min_x, old_max_y ) ).y;
        } else if (crd.y < config->min_y) {
            range = config->max_y-config->min_y;
            config->max_y = config->min_y + config->rolling_overlap*range;
            config->min_y = config->max_y - range;
            surface_new_y = plot_surface.to_pixel_coord( 
                    Coord( config->min_x, old_max_y ) ).y;
        }

        //Create new surface, with part of old surface showing
        Cairo::RefPtr<Cairo::ImageSurface> new_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 
                config->pixel_width, config->pixel_height );
        Cairo::RefPtr<Cairo::Context> new_context = Cairo::Context::create(new_surface);

        new_context->set_source_rgb(1, 1, 1);
        new_context->rectangle(0,0,surface_new_x, surface_new_y);
        new_context->fill();
        new_context->stroke();
        new_context->set_source( surface, surface_new_x, surface_new_y );
        new_context->paint();
        
        surface = new_surface;
        context = new_context;


        //Plot new bounding box
        plot_surface.paint( context );
        updated = true;
    }
}
