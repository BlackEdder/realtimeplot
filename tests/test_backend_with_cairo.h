#include <cxxtest/TestSuite.h>

#include <boost/shared_ptr.hpp>

#include "realtimeplot/backend.h"
#include "realtimeplot/eventhandler.h"
#include "realtimeplot/events.h"

using namespace realtimeplot;
namespace realtimeplot {
	class TestBackendWithCairo : public CxxTest::TestSuite 
	{
		public:
			Display *dpy;
			Window win;
			Cairo::RefPtr<Cairo::XlibSurface> xSurface;
			Cairo::RefPtr<Cairo::Context> xContext;

			void xtestOpenAndClosePlot() {
				PlotConfig config = PlotConfig();
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );
				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}

			void xtestDrawPoint() {
				PlotConfig config = PlotConfig();
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );
				pEventHandler->add_event( boost::shared_ptr<Event>( new PointEvent(5, 20) ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}

			void openWindow() {
				static int i = 0;
				Window rootwin;
				int scr, white, black;
				if(!(dpy=XOpenDisplay(NULL))) {
					fprintf(stderr, "ERROR: Could not open display\n");
					throw;
				}
				assert(dpy);

				scr = DefaultScreen(dpy);
				rootwin = RootWindow(dpy, scr);
				white = WhitePixel(dpy,scr);
				black = BlackPixel(dpy,scr);
				win = XCreateSimpleWindow(dpy,
						rootwin,
						0, 0,   // origin
						100, 100, // size
						0, black, // border
						white );

				if (i==0)
					XStoreName(dpy, win, "hello");
				else {
					XStoreName(dpy, win, "hello2");
				}

				XMapWindow(dpy, win);
				XSelectInput( dpy, win, KeyPressMask | StructureNotifyMask | ExposureMask );

				Atom wmDelete=XInternAtom(dpy, "WM_DELETE_WINDOW", True);
				XSetWMProtocols(dpy, win, &wmDelete, 1);
				//xSurface = Cairo::XlibSurface::create( dpy, win, DefaultVisual(dpy, 0), 
				//			100, 100);
				//xContext = Cairo::Context::create( xSurface );
				++i;

			}
			void testXlib() {
				openWindow();
				// With this sleep it works as expected
				sleep(1);
				//xContext.clear();
				//xSurface.clear();
				XCloseDisplay(dpy);
				openWindow();
				sleep(1);
			}

			void xtestAlternativeResetPlot() {
				/**
				 * Removed multiEvent to hunt down the bug
				 */

				PlotConfig config = PlotConfig();
				PlotConfig conf = PlotConfig();
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );

				pEventHandler->add_event( boost::shared_ptr<Event>( new CloseWindowEvent() ) );
				config = conf;
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
							pEventHandler ) ) );

				sleep(1);
				TS_ASSERT( pEventHandler->pBPlot.use_count() > 0 );
				TS_ASSERT( pEventHandler->pBPlot->xSurface );

				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}

			void xtestResetPlot() {
				/**
				 * Notes on hunting down this problem:
				 *
				 * 1) Tested if it has something to do with the MultiEvent. It hasn't
				 */

				PlotConfig config = PlotConfig();
				PlotConfig conf = PlotConfig();
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );

				std::vector<boost::shared_ptr<Event> > events(2);
				events[0] = boost::shared_ptr<Event>( new CloseWindowEvent() );
				config = conf;
				events[1] = boost::shared_ptr<Event>( new OpenPlotEvent( config, 
							pEventHandler ) );
				pEventHandler->add_event( 
						boost::shared_ptr<Event>( new MultipleEvents( events ) ));

				TS_ASSERT( pEventHandler->pBPlot.use_count() > 0 );

				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}


	};
};
	
