#include <string>
#include <iostream>
#include <cairomm/cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <pangomm/init.h>
#include <pangomm/context.h>
#include <pangomm/layout.h>

/* M_PI is defined in math.h in the case of Microsoft Visual C++, Solaris,
 * et. al.
 */
#if defined(_MSC_VER)
#define _USE_MATH_DEFINES
#endif 
#ifdef HAVE_MATH_H
# include <math.h>
#endif
int main()
{
    Cairo::RefPtr<Cairo::ImageSurface> surface =
        Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 600, 400);

    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

    cr->set_source_rgb(0.86, 0.85, 0.47);
    cr->paint();    // fill image with the color

		Pango::init();
		Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create (cr);
		cr->move_to(300,200);
		pangoLayout->set_text("text");
		pangoLayout->update_from_cairo_context(cr);  //gets cairo cursor position
		std::cout << "bla" << std::endl;
		pangoLayout->add_to_cairo_context(cr);       //adds text to cairos stack of stuff to be drawn
		std::cout << "bla" << std::endl;
		cr->stroke();


		cr->paint();


#ifdef CAIRO_HAS_PNG_FUNCTIONS
    std::string filename = "image.png";
    surface->write_to_png(filename);

    std::cout << "Wrote png file \"" << filename << "\"" << std::endl;

#else
    std::cout << "You must compile cairo with PNG support for this example to work."
        << std::endl;

#endif
}
