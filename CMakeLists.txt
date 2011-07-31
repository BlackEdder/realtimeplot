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

SET (CMAKE_CXX_FLAGS "-O2 -pipe -ffast-math -fomit-frame-pointer ${CMAKE_CXX_FLAGS}") 
find_library( PANGOMM pangomm-1.4 )
if( PANGOMM-NOTFOUND )
	message( FATAL_ERROR "Could not find pangomm library" )
	endif()

set(BOOST_LIBS thread date_time math_tr1 )
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)

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
	src/realtimeplot/events.cc src/realtimeplot/delaunay.cc
	src/cairomm/xcb_surface.cc )
target_link_libraries(realtimeplot ${PANGOMM} ${Boost_LIBRARIES})

#Examples.. should be under own build target -> make examples
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
add_executable(plot_points examples/plot_points.cc)
target_link_libraries(plot_points realtimeplot)

add_executable(plot_test examples/plot_test.cc)
target_link_libraries(plot_test realtimeplot)

add_executable(plot_line examples/plot_line.cc)
target_link_libraries(plot_line realtimeplot)

add_executable(plot_histogram examples/plot_histogram.cc)
target_link_libraries(plot_histogram realtimeplot)

add_executable(plot_surface examples/plot_surface.cc)
target_link_libraries(plot_surface realtimeplot)

add_executable(plot_rolling_update_stress_test examples/plot_rolling_update_stress_test.cc)
target_link_libraries(plot_rolling_update_stress_test realtimeplot)

add_executable(plot_heightmap examples/plot_heightmap.cc)
target_link_libraries(plot_heightmap realtimeplot)

#Testing for xcb
add_executable(xcb examples/xcb.cc src/cairomm/xcb_surface.cc)
target_link_libraries(xcb ${PANGOMM})

#Install the library
SET (CMAKE_INSTALL_PREFIX /usr)
install (TARGETS realtimeplot DESTINATION lib)
install (FILES include/realtimeplot/backend.h
	include/realtimeplot/eventhandler.h
	include/realtimeplot/events.h include/realtimeplot/plot.h DESTINATION 
	include/realtimeplot)

#Documentation
set(DOXYFILE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc/realtimeplot")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(UseDoxygen)

# Tests
find_package(CxxTest)
if(CXXTEST_FOUND)
	#include_directories(tests)
  enable_testing()

  CXXTEST_ADD_TEST(unittest_delaunay test_delaunay.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_delaunay.h)
  target_link_libraries(unittest_delaunay realtimeplot) # as needed
  CXXTEST_ADD_TEST(unittest_backend test_backend.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_backend.h)
  target_link_libraries(unittest_backend realtimeplot) # as needed
	#CXXTEST_ADD_TEST(unittest_backend_with_cairo test_backend_with_cairo.cc
	#	${CMAKE_CURRENT_SOURCE_DIR}/tests/test_backend_with_cairo.h)
	#target_link_libraries(unittest_backend_with_cairo realtimeplot) # as needed
endif()

#
# Ruby library
#
#add_custom_target(rb_two_niches.so COMMAND ruby ruby/rb_two_niches.rb DEPENDS
#    two_niches)
add_custom_target(ruby DEPENDS ext/rb_realtimeplot.so)

add_custom_command( OUTPUT ext/rb_realtimeplot.so
	COMMAND ruby ext/rb_realtimeplot.rb 
	COMMAND ${CMAKE_COMMAND} -E copy ext/generated/rb_realtimeplot.so ext/rb_realtimeplot.so
	DEPENDS ext/rb_realtimeplot.rb realtimeplot
	ext/utils/custom_from_to_ruby.h ext/utils/custom_from_to_ruby.cc)
