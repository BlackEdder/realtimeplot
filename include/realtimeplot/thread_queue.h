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
	 * If the queue is empty the read element will block. Similarly if queue reaches
	 * it max size push will block
	 */
	template <class T>
	class ThreadQueue {
		public:
			ThreadQueue( size_t max_size ) : m_size( max_size ) {}

			ThreadQueue( const ThreadQueue& tq ) { throw;} 

			/**
			 * \brief Return current size of the queue
			 */
			size_t size() {
				boost::mutex::scoped_lock lock( queue_mutex );
				size_t dim = queue.size();
				return dim;
			}

			/**
			 * \brief Return max size of the queue
			 */
			size_t max_size() {
				boost::mutex::scoped_lock lock( max_size_mutex );
				size_t dim = m_size;
				return dim;
			}

			/**
			 * \brief Change the max size of the queue
			 *
			 * If the current size is larger than the new max size then push will block
			 * till the size is lower than the new max size
			 */
			void set_max_size(size_t msize) {
				boost::mutex::scoped_lock lock( max_size_mutex );
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

			/**
			 * \brief Block till the queue is empty
			 *
			 * Usefull when working with multiple threads and can be useful for unit testing
			 */
			void wait_till_full() {
				max_size_mutex.lock();
				size_t msize = m_size;
				max_size_mutex.unlock();

				boost::mutex::scoped_lock lock( m_mutex );
				while (size() < msize )
					cond.wait( lock );
			}

			/**
			 * \brief Add new element to the queue. Will block if the queue is full
			 */
			void push( const T& element ) {
				boost::mutex::scoped_lock lock( m_mutex );
				while (size() >= max_size())
					cond.wait( lock );
				queue_push( element );
				cond.notify_one();
			}
			
			/**
			 * \brief Return front element of the queue and deletes it from the queue. Will block if the queue is empty
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
			boost::mutex max_size_mutex;
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

	/**
	 * \brief Thread safe version of queue with two separate queues, a priority queue and a normal one
	 * 
	 * If both queues are empty the read element will block. Similarly if queue reaches
	 * it max size low priority push will block. High priority push will still push to the 
	 * high priority queue.
	 */
	template <class T>
	class ThreadSplitQueue {
		public:
			ThreadSplitQueue( size_t max_size ) : m_size( max_size ) {}

			ThreadSplitQueue( const ThreadSplitQueue& tq ) { throw;} 

			/**
			 * \brief Return current size of the queue
			 */
			size_t size() {
				return normal_size() + priority_size();
			}

			size_t normal_size() {
				boost::mutex::scoped_lock lock( queue_mutex );
				size_t dim = queue.size();
				return dim;
			}

			size_t priority_size() {
				boost::mutex::scoped_lock lock( priority_queue_mutex );
				size_t dim = priority_queue.size();
				return dim;
			}


			/**
			 * \brief Return max size of the low priority queue
			 */
			size_t max_size() {
				boost::mutex::scoped_lock lock( max_size_mutex );
				size_t dim = m_size;
				return dim;
			}

			/**
			 * \brief Change the max size of the low priority queue
			 *
			 * If the current size is larger than the new max size then push will block
			 * till the size is lower than the new max size
			 */
			void set_max_size(size_t msize) {
				boost::mutex::scoped_lock lock( max_size_mutex );
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

			/**
			 * \brief Block till the queue is full 
			 *
			 * Note that we can still write to priority queue even when queue is full
			 * Usefull when working with multiple threads and can be useful for unit testing
			 */
			void wait_till_full() {
				size_t msize = max_size();

				boost::mutex::scoped_lock lock( m_mutex );
				while (size() < msize )
					cond.wait( lock );
			}

			/**
			 * \brief Add new element to the queue.
			 *
			 * Blocks for low priority elements when the queue is full
			 */
			void push( const T& element, bool priority = false ) {
				boost::mutex::scoped_lock lock( m_mutex );
				if (priority)
					priority_queue_push( element );
				else {
					while (size() >= max_size()) {
						cond.wait( lock );
					}
					queue_push( element );
				}
				cond.notify_one();
			}
			
			/**
			 * \brief Return front element of the queue and deletes it from the queue. Will block if the queue is empty
			 *
			 * Will first return high priority elements, before returning low priority elements.
			 */
			T pop() {
				boost::mutex::scoped_lock lock( m_mutex );
				while (size() == 0)
					cond.wait( lock );

				T el;
				if (priority_size() > 0)
					el = priority_queue_pop();
				else
					el = queue_pop();
				cond.notify_one();
				return el;
			}

		protected:
			std::queue<T> queue;
			std::queue<T> priority_queue;
			size_t m_size;
			boost::mutex m_mutex;
			boost::mutex queue_mutex;
			boost::mutex priority_queue_mutex;
			boost::mutex max_size_mutex;
			boost::condition cond;

			void queue_push( const T& element ) {
				boost::mutex::scoped_lock lock( queue_mutex );
				queue.push( element );
			}

			void priority_queue_push( const T& element ) {
				boost::mutex::scoped_lock lock( priority_queue_mutex );
				priority_queue.push( element );
			}


			T queue_pop() {
				boost::mutex::scoped_lock lock( queue_mutex );
				T el = queue.front();
				queue.pop();
				return el;
			}

			T priority_queue_pop() {
				boost::mutex::scoped_lock lock( priority_queue_mutex );
				T el = priority_queue.front();
				priority_queue.pop();
				return el;
			}
	};

};
#endif


