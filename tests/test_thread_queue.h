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
			for (size_t n = 0; n < iter; ++n)
			{
				tqueue.push(n);
			}
		}

		void read_helper( size_t iter ) {
			for (size_t n = 0; n < iter; ++n)
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

class Q2ReaderWriter {
	public:
		ThreadSplitQueue<size_t> tqueue;
		
		Q2ReaderWriter( size_t max_size ) : tqueue( max_size ) {
		}

		void write_helper( size_t iter, bool priority ) {
			for (size_t n = 0; n < iter; ++n)
			{
				tqueue.push(n, priority);
			}
		}

		void read_helper( size_t iter ) {
			for (size_t n = 0; n < iter; ++n)
			{
				tqueue.pop();
			}
		}


		void write( size_t iter, bool priority = false ) {
			boost::thread thrd( boost::bind( &Q2ReaderWriter::write_helper, this, iter, priority ) );
		}

		void write_join( size_t iter, bool priority = false ) {
			boost::thread thrd( boost::bind( &Q2ReaderWriter::write_helper, this, iter, priority ) );
			thrd.join();
		}

		void read( size_t iter ) {
			boost::thread thrd( boost::bind( &Q2ReaderWriter::read_helper, this, iter ) );
		}

		void read_join( size_t iter ) {
			boost::thread thrd( boost::bind( &Q2ReaderWriter::read_helper, this, iter ) );
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
			qrw.tqueue.wait_till_full();
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
			qrw.tqueue.wait_till_full();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
			qrw.tqueue.set_max_size( 0 );
			//sleep( 2 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.tqueue.set_max_size( 10 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}

		void testWaitTillEmpty() { 
			QReaderWriter qrw = QReaderWriter( 10000 );
			qrw.write( 10000 );
			qrw.tqueue.wait_till_full();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 10000 );
			qrw.read( 10000 );
			qrw.tqueue.wait_till_empty();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.write_join( 1 );
			qrw.read_join( 1 );
		}

		void testQ2Basics() { 
			ThreadSplitQueue<size_t> tq = ThreadSplitQueue<size_t>( 100 );
			tq.push( 0 );
			tq.push( 1 );
			tq.push( 2 );
			TS_ASSERT_EQUALS( tq.pop(), 0 );
			TS_ASSERT_EQUALS( tq.pop(), 1 );
			TS_ASSERT_EQUALS( tq.pop(), 2 );
		}

		void testQ2PriorityBasics() { 
			ThreadSplitQueue<size_t> tq = ThreadSplitQueue<size_t>( 100 );
			tq.push( 0 );
			tq.push( 1 );
			tq.push( 2 );
			tq.push( 3, true );
			tq.push( 4, true );
			TS_ASSERT_EQUALS( tq.pop(), 3 );
			TS_ASSERT_EQUALS( tq.pop(), 4 );
			TS_ASSERT_EQUALS( tq.pop(), 0 );
			TS_ASSERT_EQUALS( tq.pop(), 1 );
			TS_ASSERT_EQUALS( tq.pop(), 2 );
		}


		void testQ2Write() { 
			Q2ReaderWriter qrw = Q2ReaderWriter( 100 );
			qrw.write_join( 10 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 10 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
		}

		void testQ2SplitWrite() {
			Q2ReaderWriter qrw = Q2ReaderWriter( 100 );
			qrw.write_join( 10 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 10 );
			qrw.write_join( 5, true );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 15 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 10 );
			qrw.read_join( 10 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}

		void testQ2Size() { 
			Q2ReaderWriter qrw = Q2ReaderWriter( 5 );
			qrw.write( 10 );
			qrw.tqueue.wait_till_full();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
			qrw.read_join( 5 );
			usleep( 100 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}

		void testQ2SplitSize() { 
			Q2ReaderWriter qrw = Q2ReaderWriter( 5 );
			qrw.write( 10 );
			qrw.tqueue.wait_till_full();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
			qrw.write_join( 5, true );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 10 );
			TS_ASSERT_EQUALS( qrw.tqueue.priority_size(), 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.normal_size(), 5 );
			qrw.read_join( 5 );
			qrw.tqueue.wait_till_full();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
			qrw.read_join( 5 );
			qrw.tqueue.wait_till_full();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 5 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}


		void testQ2ReadBlocking() { 
			Q2ReaderWriter qrw = Q2ReaderWriter( 10 );
			qrw.read( 10 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.write_join( 5 );
			usleep( 100 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.write_join( 5 );
			usleep( 100 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}

		void testQ2SplitReadBlocking() { 
			Q2ReaderWriter qrw = Q2ReaderWriter( 10 );
			qrw.read( 15 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.write_join( 5 );
			qrw.write_join( 5, true );
			qrw.tqueue.wait_till_empty();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.write_join( 5 );
			qrw.tqueue.wait_till_empty();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}


		void testQ2SizeChange() { 
			Q2ReaderWriter qrw = Q2ReaderWriter( 5 );
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

		void testQ2SplitSizeChange() { 
			Q2ReaderWriter qrw = Q2ReaderWriter( 5 );
			qrw.write( 10 );
			qrw.tqueue.wait_till_full();
			qrw.write_join( 5, true );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 10 );
			qrw.tqueue.set_max_size( 0 );
			qrw.read_join( 10 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.tqueue.set_max_size( 10 );
			qrw.read_join( 5 );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}


		void testQ2WaitTillEmpty() { 
			Q2ReaderWriter qrw = Q2ReaderWriter( 10000 );
			qrw.write( 10000 );
			qrw.tqueue.wait_till_full();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 10000 );
			qrw.read( 10000 );
			qrw.tqueue.wait_till_empty();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.write_join( 1 );
			qrw.read_join( 1 );
		}

		void testQ2SplitWaitTillEmpty() { 
			Q2ReaderWriter qrw = Q2ReaderWriter( 10000 );
			qrw.write_join( 10000 );
			qrw.write_join( 5000, true );
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 15000 );
			qrw.read( 15000 );
			qrw.tqueue.wait_till_empty();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
			qrw.write_join( 1 );
			qrw.read_join( 1 );
		}

		void testQ2StressTestSmallQueue() {
			Q2ReaderWriter qrw = Q2ReaderWriter( 100 );
			qrw.write( 5000 );
			qrw.read( 10000 );
			qrw.write( 5000, true );
			qrw.write( 5000, true );
			qrw.write( 5000 );
			qrw.read( 10000 );
			qrw.tqueue.wait_till_empty();
			qrw.write( 100000 );
			qrw.read_join( 100000 );
			qrw.tqueue.wait_till_empty();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}

		void testQ2StressTestLargeQueue() {
			Q2ReaderWriter qrw = Q2ReaderWriter( 20000 );
			qrw.write( 5000 );
			qrw.read( 10000 );
			qrw.write( 5000, true );
			qrw.write( 5000, true );
			qrw.write( 5000 );
			qrw.read( 10000 );
			qrw.tqueue.wait_till_empty();
			qrw.write( 100000 );
			qrw.read_join( 100000 );
			qrw.tqueue.wait_till_empty();
			TS_ASSERT_EQUALS( qrw.tqueue.size(), 0 );
		}



};

