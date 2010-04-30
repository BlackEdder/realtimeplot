#
# -------------------------------------------------------------------
# 
# Copyright (C) 2010, Edwin van Leeuwen
# 
# This file is part of RealTimePlot.
# 
# RealTimePlot is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# RealTimePlot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with RealTimePlot. If not, see <http://www.gnu.org/licenses/>.
#
# -------------------------------------------------------------------
#

cmake_minimum_required(VERSION 2.6)
project(RealTimePlot)

SET (CMAKE_CXX_FLAGS "-O2 -pipe -ffast-math -fomit-frame-pointer")
find_library( pangomm-1.4, REQUIRED )
find_library( pango-1.0, REQUIRED )
find_library( cairomm-1.0, REQUIRED )
find_library( glibmm-2.4, REQUIRED )
find_library( glib-2.0, REQUIRED )
find_library( freetype2, REQUIRED )
find_library( sigc++-2.0, REQUIRED )
find_library( boost_thread, REQUIRED )
find_library( boost_date_time, REQUIRED )

include_directories("include")
include_directories("/usr/include/pangomm-1.4")
include_directories("/usr/include/pango-1.0")
include_directories("/usr/include/glibmm-2.4")
include_directories("/usr/lib/glibmm-2.4/include")
include_directories("/usr/include/glib-2.0")
include_directories("/usr/lib/glib-2.0/include")
include_directories("/usr/include/cairomm-1.0")
include_directories("/usr/include/cairo")
include_directories("/usr/include/freetype2")
include_directories("/usr/include/sigc++-2.0")
include_directories("/usr/lib/sigc++-2.0/include")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
add_library( realtimeplot SHARED src/realtimeplot/plot.cc
	src/realtimeplot/backend.cc src/realtimeplot/eventhandler.cc
	src/realtimeplot/events.cc )
target_link_libraries(realtimeplot pangomm-1.4 boost_thread boost_date_time)

#Examples.. should be under own build target -> make examples
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
add_executable(plot_points examples/plot_points.cc)
target_link_libraries(plot_points realtimeplot)

#Install the library
SET (CMAKE_INSTALL_PREFIX /usr)
install (TARGETS realtimeplot DESTINATION lib)
install (FILES include/realtimeplot/backend.h
	include/realtimeplot/eventhandler.h
	include/realtimeplot/events.h include/realtimeplot/plot.h DESTINATION 
	include/realtimeplot)

#Documentation
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(UseDoxygen)
