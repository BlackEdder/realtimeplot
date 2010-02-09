require 'mkmf-rice'
dir_config('cairo_plot')
dir_config('pangomm')
dir_config('pango')
dir_config('glibmm_extra')
dir_config('glibmm')
dir_config('glib')
dir_config('glib_extra')
dir_config('cairomm')
dir_config('cairo')
dir_config('freetype2')
dir_config('sigc++_extra')
dir_config('sigc++')
$CPPFLAGS += ' -lcairo_plot'
$CFLAGS += ' -lcairo_plot'
create_makefile('rb_cairo_plot')

#ruby extconf.rb --with-cairo_plot-dir=.. --with-pangomm-include=/usr/include/pangomm-1.4 --with-pango-include=/usr/include/pango-1.0 --with-glibmm-include=/usr/include/glibmm-2.4 --with-glibmm_extra-include=/usr/lib/glibmm-2.4/include --with-glib-include=/usr/include/glib-2.0 --with-glib_extra-include=/usr/lib/glib-2.0/include --with-cairomm-include=/usr/include/cairomm-1.0 --with-cairo-include=/usr/include/cairo --with-freetype2-include=/usr/include/freetype2 --with-sigc++-include=/usr/include/sigc++-2.0 --with-sigc++_extra-include=/usr/lib/sigc++-2.0/include

