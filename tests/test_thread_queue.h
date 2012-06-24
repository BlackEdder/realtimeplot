/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, 2011 Edwin van Leeuwen

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
#include <cxxtest/TestSuite.h>
#include <boost/thread/thread.hpp>

#include "realtimeplot/thread_queue.h"
using namespace realtimeplot;

/**
 * \brief Helper class that uses threads to read and write
 */

class QReaderWriter {
	public:
		ThreadQueue<size_t> tqueue;
		
		QReaderWriter( size_t max_size ) : tqueue( max_size ) {
		}

		void write_helper( size_t iter ) {
			for (int n = 0; n < iter; ++n)
			{
				tqueue.push(n);
			}
		}

		void read_helper( size_t iter ) {
			for (int n = 0; n < iter; ++n)
			{
				tqueue.pop();
			}
		}


		void write( size_t iter ) {
			boost::thread thrd( boost::bind( &QReaderWriter::write_helper, this, iter ) );
		}

		void write_join( size_t iter ) {
			boost::thread thrd( boost::bind( &QReaderWriter::write_helper, this, iter ) );
			thrd.join();
		}

		void read( size_t iter ) {
			boost::thread thrd( boost::bind( &QReaderWriter::read_helper, this, iter ) );
		}

		void read_join( size_t iter ) {
			boost::thread thrd( boost::bind( &QReaderWriter::read_helper, this, iter ) );
			thrd.join();
		}


};

class TestThreadQueue : public CxxTest::TestSuite {
	public:
		void testBasics() { 
			ThreadQueue<size_t> tq = ThreadQueue<size_t>( 100 );
			tq.push( 0 );
			tq.push( 1 );
			tq.push( 2 );
			TS_ASSERT_EQUALS( tq.pop(), 0 );
			TS_ASSERT_EQUALS( tq.pop(), 1 );
			TS_ASSERT_EQUALS( tq.pop(), 2 );
		}

		void testWrite() { 
			QReaderWriter qrw = QReaderWriter( 100 );
			qrw.write_join( 10 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 10 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
		}

		void testSize() { 
			QReaderWriter qrw = QReaderWriter( 5 );
			qrw.write( 10 );
			usleep( 100 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
			//sleep( 2 );
			qrw.read_join( 5 );
			usleep( 100 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}

		void testReadBlocking() { 
			QReaderWriter qrw = QReaderWriter( 10 );
			qrw.read( 10 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.write_join( 5 );
			usleep( 100 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.write_join( 5 );
			usleep( 100 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}

		void testSizeChange() { 
			QReaderWriter qrw = QReaderWriter( 5 );
			qrw.write( 10 );
			usleep( 100 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
			qrw.tqueue.set_max_size( 0 );
			//sleep( 2 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.tqueue.set_max_size( 10 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}



};

