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
		int a, b;
    Cairo::RefPtr<Cairo::ImageSurface> surface =
        Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 600, 400);

    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

    cr->set_source_rgb(0.86, 0.85, 0.47);
    cr->paint();    // fill image with the color

		Pango::init();
		Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create (cr);
		cr->move_to(300,200);
    cr->set_source_rgb(0, 0, 0);
		pangoLayout->set_text("text");
		pangoLayout->get_pixel_size( a, b );
		std::cout << a << " " << b << std::endl;
		pangoLayout->update_from_cairo_context(cr);  //gets cairo cursor position
		pangoLayout->add_to_cairo_context(cr);       //adds text to cairos stack of stuff to be drawn
		//pangoLayout->set_alignment( Pango::ALIGN_CENTER );
		cr->move_to(300,220);
		pangoLayout->set_text("centered");
		pangoLayout->get_pixel_size( a, b );
		pangoLayout->set_text("");
		std::cout << a << " " << b << std::endl;
		pangoLayout->update_from_cairo_context(cr);  //gets cairo cursor position
		pangoLayout->add_to_cairo_context(cr);       //adds text to cairos stack of stuff to be drawn
			cr->stroke();


#ifdef CAIRO_HAS_PNG_FUNCTIONS
    std::string filename = "image.png";
    surface->write_to_png(filename);

    std::cout << "Wrote png file \"" << filename << "\"" << std::endl;

#else
    std::cout << "You must compile cairo with PNG support for this example to work."
        << std::endl;

#endif
}
