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

CFLAGS="-O2 -pipe -ffast-math"
EXAMPLE_PARS = " -L./lib -lcairo_plot -lcairomm-1.0 -lglib-2.0 -lpangomm-1.4 -I/usr/include/pangomm-1.4 -I/usr/include/pango-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/cairomm-1.0 -I/usr/include/cairo -I/usr/include/freetype2 -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I./include -lboost_thread -lboost_date_time #{CFLAGS}"
LIBRARY_PARS = " -Wall -I/usr/include/pangomm-1.4 -I/usr/include/pango-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/cairomm-1.0 -I/usr/include/cairo -I/usr/include/freetype2 -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I./include #{CFLAGS}"

file "src/cairo_plot/plot.o" => ["src/cairo_plot/plot.cc",
    "include/cairo_plot/plot.h",
    "include/cairo_plot/eventhandler.h"] do |t|
    sh "g++ -c -fPIC -o #{t.name} src/cairo_plot/plot.cc #{LIBRARY_PARS}"
end

file "src/cairo_plot/eventhandler.o" => ["src/cairo_plot/eventhandler.cc",
    "include/cairo_plot/plot.h",
    "include/cairo_plot/eventhandler.h"] do |t|
    sh "g++ -c -fPIC -o #{t.name} src/cairo_plot/eventhandler.cc #{LIBRARY_PARS}"
end

file "lib/libcairo_plot.so" => ["src/cairo_plot/eventhandler.o", 
    "src/cairo_plot/plot.o"] do |t|
    sh "g++ -shared -o #{t.name} #{t.prerequisites.to_text}"
end

FileList['examples/plot_*.cc'].each do |fn|
	name = File.basename(fn).sub('.cc','')
	file "bin/#{name}" => [fn, "lib/libcairo_plot.so"] do |t|
    sh "g++ -o #{t.name} #{fn} #{EXAMPLE_PARS}"
	end
	task name => "bin/#{name}" do |t|
    ENV['LD_LIBRARY_PATH']="lib"
		sh "bin/#{name}"
	end
end

task :build => ["lib/libcairo_plot.so"]

task :install => :build do
	sh "cp lib/libcairo_plot.so /usr/lib"
	sh "cp -r include/* /usr/include/"
end

task :default => :plot_points
