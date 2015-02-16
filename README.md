Read the INSTALL file for installation instructions.

Usage
=================
A good way to learn how to use the library is to have a look at the examples in the examples directory. For now only a couple of examples are build automatically by installation (need to update CMakeLists.txt):
e.g.
bin/plot_points

Description
=================

RealTimePlot is a simple c++ library that provides plotting capabilities using cairo and pango. It is written in c++ and mainly meant for real time plotting of the outcome of numerical simulations. It is multithreaded and capable of handling large amounts of data. It should not slow down your main program at all, especially if you have a multicore computer. To accomplish this the library provides a front end plotting class, that one can use to send plotting events (such as add a point) to the background process that will plot it to the screen when it has the time. Ruby bindings will be added reasonably soon. 

What makes it different from other plotting libraries is that most libraries seem to expect all the data to be already produced and therefore are build around the fact that you send the data and only then plot it. This library is meant to display data, while it is being produced/calculated.

Features:

- Multithreaded
- Rolling updates (i.e. the plot will "shift" to include new data)
- Keyboard controlled (arrow keys to move plot around, w to write a png, space to pause plotting (will not block to process that¿s sending data)).

Feel free to contact me with any questions/suggestion/bugs (see AUTHORS file for my email or use gitorious/github to send me a message). Knowing that people actually use/are in interisted in my code will motivate me to clean up/better document the code.
