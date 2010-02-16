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
CLEAN.include("**/*.so")
CLEAN.include("bin/*")

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
	sh "mkdir -p /usr/include/cairo_plot/"
	sh "cp include/cairo_plot/* /usr/include/cairo_plot/"
end

desc "Generate documentation using doxygen"
task :doc do
	sh "doxygen doc/cairo_plot/Doxyfile"
end

task :default => :plot_points


#### Ruby
file "ext/rb_cairo_plot.o" => [:build, "ext/rb_cairo_plot.cc"] do |t|
    sh "g++ -c -fPIC -o #{t.name} ext/rb_cairo_plot.cc -I/usr/lib64/ruby/1.8/x86_64-linux -I/usr/lib64/ruby/gems/1.8/gems/rice-1.3.1/ruby/lib/include  #{LIBRARY_PARS}"
end

file "ext/rb_cairo_plot.so" => [:build, "ext/rb_cairo_plot.o"] do |t|
    sh "g++ -shared -o #{t.name} ext/rb_cairo_plot.o  -L/usr/lib64/ruby/gems/1.8/gems/rice-1.3.1/ruby/lib/lib -lrice -lruby18 -lrt -ldl -lcrypt -lm -lc #{EXAMPLE_PARS}"
end

desc "Build ruby extension"
task :ruby_extension => ["ext/rb_cairo_plot.so"]

task :ruby_example => :ruby_extension do |t|
    sh "ruby -Iext examples/ruby_example.rb"
end

