/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, Edwin van Leeuwen

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

#ifndef REALTIMEPLOT_THREAD_QUEUE_H
#define REALTIMEPLOT_THREAD_QUEUE_H
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include<queue>
namespace realtimeplot {
	/**
	 * \brief Thread safe version of std::queue with blocking push and pop methods
	 * 
	 * If the thread is empty the read element will block. Similarly if queue reaches
	 * it max size push will block
	 */
	template <class T>
	class ThreadQueue {
		public:
			ThreadQueue( size_t max_size ) : m_size( max_size ) {}

			ThreadQueue( const ThreadQueue& tq ) { throw;} 

			size_t size() {
				boost::mutex::scoped_lock lock( queue_mutex );
				size_t dim = queue.size();
				return dim;
			}

			size_t max_size() {
				boost::mutex::scoped_lock lock( queue_mutex );
				size_t dim = m_size;
				return dim;
			}

			void set_max_size(size_t msize) {
				/*if (msize < 1)
					msize = 1;*/
				boost::mutex::scoped_lock lock( queue_mutex );
				m_size = msize;
				cond.notify_one();
			}

			/**
			 * \brief Block till the queue is empty
			 *
			 * Usefull when working with multiple threads
			 */
			void wait_till_empty() {
				boost::mutex::scoped_lock lock( m_mutex );
				while (size() > 0 )
					cond.wait( lock );
			}

			void wait_till_full() {
				queue_mutex.lock();
				size_t msize = m_size;
				queue_mutex.unlock();

				boost::mutex::scoped_lock lock( m_mutex );
				while (size() < msize )
					cond.wait( lock );
			}



			void push( const T& element ) {
				boost::mutex::scoped_lock lock( m_mutex );
				while (size() >= max_size())
					cond.wait( lock );
				queue_push( element );
				cond.notify_one();
			}
			
			/**
			 * \brief Return front element of the queue and deletes it from the queue
			 */
			T pop() {
				boost::mutex::scoped_lock lock( m_mutex );
				while (size() == 0)
					cond.wait( lock );
				T el = queue_pop();
				cond.notify_one();
				return el;
			}

		protected:
			std::queue<T> queue;
			size_t m_size;
			boost::mutex m_mutex;
			boost::mutex queue_mutex;
			boost::condition cond;

			void queue_push( const T& element ) {
				boost::mutex::scoped_lock lock( queue_mutex );
				queue.push( element );
			}

			T queue_pop() {
				boost::mutex::scoped_lock lock( queue_mutex );
				T el = queue.front();
				queue.pop();
				return el;
			}
	
	};
};
#endif


