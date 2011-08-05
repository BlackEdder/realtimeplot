/* Copyright (C) 2005 The cairomm Development Team and 
 * 2011 Edwin van Leeuwen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef XCB_SURFACE_H
#define XCB_SURFACE_H

#include <cairomm/surface.h>

// This header is not included by cairomm.h because it requires X headers that 
// tend to pollute the namespace with non-prefixed #defines and typedefs.
// You may include it directly if you need to use this API.

#ifdef CAIRO_HAS_XCB_SURFACE
#include <cairo-xcb.h> 
#endif 

namespace Cairo
{

#ifdef CAIRO_HAS_XCB_SURFACE

/** An XcbSurface provides a way to render to the X Window System using Xcb.
 * If you want to draw to the screen within an application that uses the X
 * Window system, you should use this Surface type.
 *
 * @note For this surface to be availabe, cairo must have been compiled with
 * support for Xcb Surfaces
 */
class XcbSurface : public Surface
{
public:

  /** Create a C++ wrapper for the C instance. This C++ instance should then be
   * given to a RefPtr.
  */
  explicit XcbSurface(cairo_surface_t* cobject, bool has_reference = false);
  virtual ~XcbSurface();

  /** Creates an Xcb surface that draws to the given drawable. The way that
   * colors are represented in the drawable is specified by the provided
   * visual.
   */
  static RefPtr<XcbSurface> create(xcb_connection_t *c, xcb_drawable_t drawable, xcb_visualtype_t *visual, int width, int height);

  /** Creates an Xcb surface that draws to the given bitmap. This will be
   * drawn to as a CAIRO_FORMAT_A1 object.
   *
   */
	static RefPtr<XcbSurface> create(xcb_connection_t *connection,
				xcb_screen_t *screen,
				xcb_pixmap_t  bitmap,
				int     width,
				int     height);

  /** Informs cairo of the new size of the X Drawable underlying the surface.
   * For a surface created for a Window (rather than a Pixmap), this function
   * must be called each time the size of the window changes. (For a subwindow,
   * you are normally resizing the window yourself, but for a toplevel window,
   * it is necessary to listen for ConfigureNotify events.)
   *
   * A Pixmap can never change size, so it is never necessary to call this
   * function on a surface created for a Pixmap.
   *
   * @param width	the new width of the surface
   * @param height	the new height of the surface
   */
//	void set_size(int width, int height);

  /** Informs cairo of a new X Drawable underlying the surface. The drawable
   * must match the display, screen and format of the existing drawable or the
   * application will get X protocol errors and will probably terminate. No
   * checks are done by this function to ensure this compatibility.
   *
   * @param drawable	the new drawable for the surface
   * @param width	the width of the new drawable
   * @param height	the height of the new drawable
   */
//  void set_drawable(Drawable drawable, int width, int height);

  /** gets the Drawable object associated with this surface */
//  Drawable get_drawable() const;

  /** Get the X Display for the underlying X Drawable. */
//  const Display* get_display() const;
  /** Get the X Display for the underlying X Drawable. */
//  Display* get_display();

  /** Get the X Screen for the underlying X Drawable */
//  Screen* get_screen();
  /** Get the X Screen for the underlying X Drawable */
//  const Screen* get_screen() const;

  /** Get the X Visual for the underlying X Drawable */
//  Visual* get_visual();
  /** Get the X Visual for the underlying X Drawable */
//  const Visual* get_visual() const;

  /** Get the number of bits used to represent each pixel value. */
//  int get_depth() const;

  /** Get the height in pixels of the X Drawable underlying the surface */
//  int get_height() const;

  /** Get the width in pixels of the X Drawable underlying the surface */
//  int get_width() const;

};

#endif // CAIRO_HAS_XCB_SURFACE

} // namespace Cairo

#endif //__CAIROMM_XLIB_SURFACE_H

// vim: ts=2 sw=2 et
