//
// Copyright Mathieu Champlon 2008
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://turtle.sf.net for documentation.

#ifndef MOCK_ROOT_HPP_INCLUDED
#define MOCK_ROOT_HPP_INCLUDED

#include "group.hpp"
#include "context.hpp"
#include "parent.hpp"
#include "child.hpp"
#include "function.hpp"
#include <boost/test/utils/trivial_singleton.hpp>
#include <ostream>
#include <map>

namespace mock
{
namespace detail
{
    class root_t : public boost::unit_test::singleton< root_t >,
                   public detail::context
    {
    public:
        virtual void add( const void* p, verifiable& v,
            boost::unit_test::const_string instance,
            const boost::optional< type_name >& type,
            boost::unit_test::const_string name )
        {
            children_it it = children_.lower_bound( &v );
            if( it == children_.end() ||
                children_.key_comp()( &v, it->first ) )
                it = children_.insert( it,
                    std::make_pair( &v, counter_child( parents_, p ) ) );
            it->second.update( instance, type, name );
        }
        virtual void add( verifiable& v )
        {
            group_.add( v );
        }

        virtual void remove( verifiable& v )
        {
            group_.remove( v );
            children_.erase( &v );
        }

        bool verify() const
        {
            return group_.verify();
        }
        void reset()
        {
            group_.reset();
        }

        virtual void serialize( std::ostream& s, const verifiable& v ) const
        {
            children_cit it = children_.find( &v );
            if( it != children_.end() )
                s << it->second;
            else
                s << "?";
        }

    private:
        typedef std::map< const void*,
            std::pair< parent, std::size_t > > parents_t;
        typedef parents_t::iterator parents_it;

        class counter_child
        {
        public:
            counter_child( parents_t& parents, const void* p )
                : parents_( &parents )
                , it_( parents.insert(
                    std::make_pair( p, parents_t::mapped_type() ) ).first )
            {
                ++it_->second.second;
            }
            counter_child( const counter_child& rhs )
                : parents_( rhs.parents_ )
                , it_( rhs.it_ )
                , child_( rhs.child_ )
            {
                ++it_->second.second;
            }
            ~counter_child()
            {
                if( --it_->second.second == 0 )
                    parents_->erase( it_ );
            }
            void update( boost::unit_test::const_string instance,
                const boost::optional< type_name >& type,
                boost::unit_test::const_string name )
            {
                child_.update( it_->second.first, instance, type, name );
            }
            friend std::ostream& operator<<( std::ostream& s,
                const counter_child& c )
            {
                return s << c.child_;
            }

        private:
            counter_child& operator=( const counter_child& );

            parents_t* parents_;
            parents_it it_;
            child child_;
        };

        typedef std::map< const verifiable*, counter_child > children_t;
        typedef children_t::const_iterator children_cit;
        typedef children_t::iterator children_it;

        parents_t parents_;
        children_t children_;
        group group_;

    private:
        BOOST_TEST_SINGLETON_CONS( root_t );
    };
    BOOST_TEST_SINGLETON_INST( root )
}

    inline bool verify()
    {
        return mock::detail::root.verify();
    }
    inline void reset()
    {
        mock::detail::root.reset();
    }
}

#endif // MOCK_ROOT_HPP_INCLUDED
