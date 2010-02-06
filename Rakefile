require 'rake/clean'

class Array
  def to_text
    str = ""
    each do |s|
      str += " #{s}"
    end
    str
  end
end

CLEAN.include("**/*.o")
CLEAN.include("bin/*")
CLEAN.include("lib/*")


EXAMPLE_PARS = " -L./lib -lcairo_plot -lcairomm-1.0 -lglib-2.0 -lpangomm-1.4 -I/usr/include/pangomm-1.4 -I/usr/include/pango-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/cairomm-1.0 -I/usr/include/cairo -I/usr/include/freetype2 -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I./include -lboost_thread -lboost_date_time"
GPP_PARS = " -Wall -I/usr/include/pangomm-1.4 -I/usr/include/pango-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/cairomm-1.0 -I/usr/include/cairo -I/usr/include/freetype2 -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I./include"

file "src/cairo_plot/plot.o" => ["src/cairo_plot/plot.cc",
    "include/cairo_plot/plot.h",
    "include/cairo_plot/eventhandler.h"] do |t|
    sh "g++ -c -fPIC -o #{t.name} src/cairo_plot/plot.cc #{GPP_PARS}"
end

file "src/cairo_plot/eventhandler.o" => ["src/cairo_plot/eventhandler.cc",
    "include/cairo_plot/plot.h",
    "include/cairo_plot/eventhandler.h"] do |t|
    sh "g++ -c -fPIC -o #{t.name} src/cairo_plot/eventhandler.cc #{GPP_PARS}"
end

file "lib/libcairo_plot.so" => ["src/cairo_plot/eventhandler.o", 
    "src/cairo_plot/plot.o"] do |t|
    sh "g++ -shared -o #{t.name} #{t.prerequisites.to_text}"
end

file "bin/plot_points" => ["examples/plot_points.cc", 
    "lib/libcairo_plot.so" ] do |t|
    sh "g++ -o bin/plot_points examples/plot_points.cc #{EXAMPLE_PARS}"
end

task :plot_points => ["bin/plot_points"] do
    ENV['LD_LIBRARY_PATH']="lib"
    sh "bin/plot_points"
end

task :default => :plot_points
