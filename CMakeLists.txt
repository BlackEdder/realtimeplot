#
# -------------------------------------------------------------------
# 
# Copyright (C) 2010, 2011, Edwin van Leeuwen, 
#								2011 Juan Carlos Vallejo
#
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

#SET (CMAKE_CXX_FLAGS "-O2 -march=native -pipe -ffast-math -fomit-frame-pointer -Wall ${CMAKE_CXX_FLAGS}")
SET (CMAKE_CXX_FLAGS "-O3 -std=c++11 ${CMAKE_CXX_FLAGS}")

if(NO_X)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_X")
endif()

#SET (CMAKE_CXX_FLAGS "-O1 -ggdb") 
INCLUDE(FindPkgConfig)
pkg_check_modules(PANGOMM REQUIRED pangomm-1.4)
pkg_check_modules(CAIROMM REQUIRED cairomm-1.0)
pkg_check_modules(GLIBMM REQUIRED glibmm-2.4)

set(BOOST_LIBS thread date_time math_tr1 )
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)

find_library( CPPA cppa )
if (NOT CPPA)
	message( FATAL_ERROR "libcppa not found" )
	endif()

SET(LIBS "${PANGOMM_LIBRARIES};${Boost_LIBRARIES};${CPPA}")
SET(SRC
	"src/realtimeplot/backend.cc;src/realtimeplot/eventhandler.cc;src/realtimeplot/events.cc;src/realtimeplot/delaunay.cc;src/realtimeplot/xcbhandler.cc;src/realtimeplot/utils.cc;src/realtimeplot/plotarea.cc;src/realtimeplot/plot.cc;src/realtimeplot/adaptive.cc;src/realtimeplot/actor.cc")

if(NOT NO_X)
	find_library( XCB-UTIL xcb-keysyms xcb-util )
	if(NOT XCB-UTIL)
		message( FATAL_ERROR "Could not find xcb-util(-keysyms) library\n You
		could compile without X support by adding NO_X=1 to your cmake command
		line:\ncmake . -DNO_X=1" )
	endif()
	SET(LIBS "${LIBS};${XCB-UTIL}")
	SET(SRC "${SRC};src/cairomm/xcb_surface.cc")
endif()

# Can be used by youcompleteme to lookup compile information
SET (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories( ${PANGOMM_INCLUDE_DIRS} )
include_directories( ${CAIROMM_INCLUDE_DIRS} )
include_directories( ${GLIBMM_INCLUDE_DIRS} )
include_directories( "/usr/lib/x86_64-linux-gnu/glibmm-2.4/include" )
include_directories( "/usr/lib/x86_64-linux-gnu/sigc++-2.0/include" )

include_directories("include")

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
add_library( realtimeplot SHARED ${SRC} )
target_link_libraries(realtimeplot ${LIBS})

#Examples.. should be under own build target -> make examples
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
add_executable(plot_points examples/plot_points.cc)
target_link_libraries(plot_points realtimeplot)

add_executable(plot_points_actor examples/plot_points_actor.cc)
target_link_libraries(plot_points_actor realtimeplot)

add_executable(plot_test examples/plot_test.cc)
target_link_libraries(plot_test realtimeplot)

add_executable(plot_line examples/plot_line.cc)
target_link_libraries(plot_line realtimeplot)

add_executable(plot_stream examples/plot_stream.cc)
target_link_libraries(plot_stream realtimeplot)

add_executable(plot_thread examples/plot_thread.cc)
target_link_libraries(plot_thread realtimeplot pthread)

add_executable(plot_histogram examples/plot_histogram.cc)
target_link_libraries(plot_histogram realtimeplot)

add_executable(plot_surface examples/plot_surface.cc)
target_link_libraries(plot_surface realtimeplot)

add_executable(plot_rolling_update_stress_test examples/plot_rolling_update_stress_test.cc)
target_link_libraries(plot_rolling_update_stress_test realtimeplot)

add_executable(plot_heightmap examples/plot_heightmap.cc)
target_link_libraries(plot_heightmap realtimeplot)

add_executable(plot_multiple examples/plot_multiple.cc)
target_link_libraries(plot_multiple realtimeplot)

# Refresh tags file after changes
FIND_PROGRAM(CTAGS ctags)
if(CTAGS)
	add_custom_command (TARGET realtimeplot POST_BUILD COMMAND ${CTAGS} -R --c++-kinds=+p
	--fields=+iaS --extra=+q .)
endif()

#Install the library
SET (CMAKE_INSTALL_PREFIX /usr)
install (TARGETS realtimeplot DESTINATION lib)
install (FILES include/realtimeplot/realtimeplot.h
	include/realtimeplot/backend.h
	include/realtimeplot/plotarea.h
	include/realtimeplot/eventhandler.h
	include/realtimeplot/thread_queue.h
	include/realtimeplot/xcbhandler.h
	include/realtimeplot/adaptive.h
	include/realtimeplot/utils.h
	include/realtimeplot/events.h
	include/realtimeplot/delaunay.h
	include/realtimeplot/actor.hh
 	include/realtimeplot/plot.h DESTINATION 
	include/realtimeplot)

#Documentation
set(DOXYFILE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc/realtimeplot")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(UseDoxygen)

# Tests
find_package(CxxTest)
if(CXXTEST_FOUND)
	file(MAKE_DIRECTORY tests/tmp_plots)
	include_directories(tests/include)
	enable_testing()

	CXXTEST_ADD_TEST(unittest_actor test_actor.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_actor.h)
	target_link_libraries(unittest_actor realtimeplot)
	CXXTEST_ADD_TEST(unittest_thread_queue test_thread_queue.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_thread_queue.h)
	target_link_libraries(unittest_thread_queue realtimeplot)
	CXXTEST_ADD_TEST(unittest_eventhandler test_eventhandler.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_eventhandler.h)
	target_link_libraries(unittest_eventhandler realtimeplot)
	CXXTEST_ADD_TEST(unittest_delaunay test_delaunay.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_delaunay.h)
	target_link_libraries(unittest_delaunay realtimeplot) # as needed
	CXXTEST_ADD_TEST(unittest_backend test_backend.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_backend.h)
	target_link_libraries(unittest_backend realtimeplot) # as needed
	CXXTEST_ADD_TEST(unittest_plotarea test_plotarea.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_plotarea.h)
	target_link_libraries(unittest_plotarea realtimeplot cairomm-1.0)
	CXXTEST_ADD_TEST(unittest_plot test_plot.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_plot.h)
	target_link_libraries(unittest_plot realtimeplot)
	CXXTEST_ADD_TEST(unittest_utils test_utils.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_utils.h)
	target_link_libraries(unittest_utils realtimeplot)
	CXXTEST_ADD_TEST(unittest_adaptive test_adaptive.cc
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test_adaptive.h)
	target_link_libraries(unittest_adaptive realtimeplot) # as needed
	if(NOT NO_X)
		CXXTEST_ADD_TEST(unittest_xcbhandler test_xcbhandler.cc
			${CMAKE_CURRENT_SOURCE_DIR}/tests/test_xcbhandler.h)
		target_link_libraries(unittest_xcbhandler realtimeplot) # as needed
	endif()
endif()

#
# Ruby library
#
#add_custom_target(rb_two_niches.so COMMAND ruby ruby/rb_two_niches.rb DEPENDS
#    two_niches)
add_custom_target(ruby_install ${CMAKE_COMMAND} -E chdir ext/generated/ make
	install
	DEPENDS ext/generated/Makefile __install )

# Currently impossible to depend on build in command, so need to define my own
# install: http://public.kitware.com/Bug/view.php?id=8438
add_custom_target(__install ${CMAKE_COMMAND} cmake -P cmake_install.cmake)

add_custom_target(ruby DEPENDS ext/rb_realtimeplot.so)

add_custom_command( OUTPUT ext/rb_realtimeplot.so ext/generated/Makefile
	COMMAND ruby ext/rb_realtimeplot.rb 
	COMMAND ${CMAKE_COMMAND} -E copy ext/generated/rb_realtimeplot.so ext/rb_realtimeplot.so
	DEPENDS ext/rb_realtimeplot.rb realtimeplot
	ext/utils/custom_from_to_ruby.h ext/utils/custom_from_to_ruby.cc)
