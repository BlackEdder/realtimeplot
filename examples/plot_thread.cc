/*
  -------------------------------------------------------------------
  
  Copyright (C) 2010, 2011, Edwin van Leeuwen
  
  This file is part of RealTimePlot.
  
  RealTimePlot is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  RealTimePlot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with RealTimePlot. If not, see <http://www.gnu.org/licenses/>.

  -------------------------------------------------------------------
*/

extern "C" {
#include <pthread.h>
}

#include "realtimeplot/plot.h"

using namespace realtimeplot;

static long sleep_millis = 0;
static int  nthreads = 3;

static
void*
do_plot (void* data)
{
  int i = 0;
  Plot* pl = static_cast<Plot*> (data);
  while (true) {
    pl->line_add ((i - 1) * 10, (i - 4) * 5);
    pl->line_add ((i - 1) * 10, (i - 5) * 5, 2, Color::grey());
    ::usleep (sleep_millis * 1000);
    ++i;
  }
  delete pl;
}

int
main (int argc, char** argv) {
  if (argc > 1) {
    nthreads = ::atoi (argv[1]);
  }

  if (argc > 2) {
    sleep_millis = ::strtol (argv[1], 0, 10);
  }

  PlotConfig config = PlotConfig();
  config.min_x = -10;
  config.max_x = 90;
  config.min_y = -20;
  config.max_y = 30;
  config.area = 250 * 250;

  std::cout << "#Threads: " << nthreads << std::endl;
  std::vector<pthread_t> threads;
  
  for (int i = 0; i < nthreads; ++i) {
    Plot* plot = new Plot (config);
    pthread_t thread;

    pthread_create (&thread, 0, do_plot, plot);
    threads.push_back (thread);
  }

  std::vector<pthread_t>::iterator it;
  it = threads.begin ();
  while (it != threads.end ()) {
    pthread_join (*it, 0);
    ++it;
  }
  
  
  return 0;
}
