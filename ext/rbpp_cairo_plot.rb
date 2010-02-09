  require 'rbplusplus'
  include RbPlusPlus
	 Extension.new "extension" do |e|
		 e.sources "/usr/include/cairo_plot/plot.h",
			 :include_paths => ["/usr/include/pangomm-1.4","/usr/include/pango-1.0",
				 "/usr/include/glibmm-2.4", "/usr/lib/glibmm-2.4/include", 
				 "/usr/include/glib-2.0", "/usr/lib/glib-2.0/include", 
				 "/usr/include/cairomm-1.0", "/usr/include/cairo",
				 "/usr/include/freetype2", "/usr/include/sigc++-2.0", 
				 "/usr/lib/sigc++-2.0/include"]
		 e.module "cairo_plot" do |m|
			 m.namespace "cairo_plot"
			 m.include "Plot"
		 end
  end
