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
CLEAN.include("doc/realtimeplot/html/*")
CLEAN.include("doc/realtimeplot/latex/*")

CFLAGS="-O2 -pipe -ffast-math"
EXAMPLE_PARS = " -L./lib -lrealtimeplot -lcairomm-1.0 -lglib-2.0 -lpangomm-1.4 -I/usr/include/pangomm-1.4 -I/usr/include/pango-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/cairomm-1.0 -I/usr/include/cairo -I/usr/include/freetype2 -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I./include -lboost_thread -lboost_date_time #{CFLAGS}"
LIBRARY_PARS = " -Wall -I/usr/include/pangomm-1.4 -I/usr/include/pango-1.0 -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/cairomm-1.0 -I/usr/include/cairo -I/usr/include/freetype2 -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I./include #{CFLAGS}"

directory "lib"
directory "bin"

file "src/realtimeplot/plot.o" => ["src/realtimeplot/plot.cc",
    "include/realtimeplot/plot.h",
    "include/realtimeplot/eventhandler.h"] do |t|
    sh "g++ -c -fPIC -o #{t.name} src/realtimeplot/plot.cc #{LIBRARY_PARS}"
end

file "src/realtimeplot/eventhandler.o" => ["src/realtimeplot/eventhandler.cc",
    "include/realtimeplot/plot.h",
    "include/realtimeplot/eventhandler.h"] do |t|
    sh "g++ -c -fPIC -o #{t.name} src/realtimeplot/eventhandler.cc #{LIBRARY_PARS}"
end

file "lib/librealtimeplot.so" => ["src/realtimeplot/eventhandler.o", 
    "src/realtimeplot/plot.o", "lib"] do |t|
    sh "g++ -shared -o #{t.name} src/realtimeplot/eventhandler.o src/realtimeplot/plot.o"
end

FileList['examples/plot_*.cc'].each do |fn|
	name = File.basename(fn).sub('.cc','')
	file "bin/#{name}" => ["bin", fn, "lib/librealtimeplot.so"] do |t|
    sh "g++ -o #{t.name} #{fn} #{EXAMPLE_PARS}"
	end
	task name => "bin/#{name}" do |t|
    ENV['LD_LIBRARY_PATH']="lib"
		sh "bin/#{name}"
	end
end

task :build => ["lib/librealtimeplot.so"]

task :install => :build do
	sh "cp lib/librealtimeplot.so /usr/lib"
	sh "mkdir -p /usr/include/realtimeplot/"
	sh "cp include/realtimeplot/* /usr/include/realtimeplot/"
end

desc "Generate documentation using doxygen"
task :doc do
	sh "doxygen doc/realtimeplot/Doxyfile"
end

task :default => :plot_points


#### Ruby
RUBYLIBPATH="/usr/lib/ruby/site_ruby/1.8/x86_64-linux/"

file "ext/rb_realtimeplot.o" => [:build, "ext/rb_realtimeplot.cc"] do |t|
    sh "g++ -c -fPIC -o #{t.name} ext/rb_realtimeplot.cc -I/usr/lib64/ruby/1.8/x86_64-linux -I/usr/lib64/ruby/gems/1.8/gems/rice-1.3.1/ruby/lib/include  #{LIBRARY_PARS}"
end

file "ext/rb_realtimeplot.so" => [:build, "ext/rb_realtimeplot.o"] do |t|
    sh "g++ -shared -o #{t.name} ext/rb_realtimeplot.o  -L/usr/lib64/ruby/gems/1.8/gems/rice-1.3.1/ruby/lib/lib -lrice -lruby18 -lrt -ldl -lcrypt -lm -lc #{EXAMPLE_PARS}"
end

desc "Build ruby extension"
task :ruby_extension => ["ext/rb_realtimeplot.so"]

task :ruby_example => :ruby_extension do |t|
    sh "ruby -Iext examples/ruby_example.rb"
end

#Should find the path to install it to, but not sure how
desc "Install ruby extension to #{RUBYLIBPATH}"
task :ruby_install => :ruby_extension do |t|
    sh "cp ext/rb_realtimeplot.so #{RUBYLIBPATH}"
end

