//
// Copyright Mathieu Champlon 2011
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://turtle.sf.net for documentation.

#ifndef MOCK_DEFAULT_ERROR_POLICY_HPP_INCLUDED
#define MOCK_DEFAULT_ERROR_POLICY_HPP_INCLUDED

#include <iostream>

namespace mock
{
    struct exception
    {};

    template< typename Result >
    struct default_error_policy
    {
        static Result abort()
        {
            throw exception();
        }

        static void checkpoint( const char* /*file*/, int /*line*/ )
        {}

        template< typename Context >
        static void fail(
            const char* message, const Context& context,
            const char* file = "unknown location", int line = 0 )
        {
            std::cerr << file << '(' << line << "): "
                << message << ": " << context << std::endl;
        }

        template< typename Context >
        static void expected_call( const Context& /*context*/,
            const char* /*file*/, int /*line*/ )
        {}

        template< typename Context >
        static void missing_action( const Context& context,
            const char* file, int line )
        {
            fail( "missing action", context, file, line );
        }
        template< typename Context >
        static void unexpected_call( const Context& context )
        {
            fail( "unexpected call", context );
        }
        template< typename Context >
        static void sequence_failed( const Context& context,
            const char* file, int line )
        {
            fail( "sequence failed", context, file, line );
        }
        template< typename Context >
        static void verification_failed( const Context& context,
            const char* file, int line )
        {
            fail( "verification failed", context, file, line );
        }
        template< typename Context >
        static void untriggered_expectation( const Context& context,
            const char* file, int line )
        {
            fail( "untriggered expectation", context, file, line );
        }
    };
}

#endif // MOCK_DEFAULT_ERROR_POLICY_HPP_INCLUDED
