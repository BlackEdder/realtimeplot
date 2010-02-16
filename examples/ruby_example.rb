require "rb_cairo_plot"

pl = Plot.new
pl.point( 10,10 )
pl.point( 10,80 )
1000.times do |i|
    pl.point( 45+20*rand(), i*(0.1) )
    sleep(0.1)
end
