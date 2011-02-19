//
//  Copyright Mathieu Champlon 2008
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MOCK_CHECK_HPP_INCLUDED
#define MOCK_CHECK_HPP_INCLUDED

#include "is_functor.hpp"
#include "is_comparable.hpp"
#include "constraints.hpp"
#include "operators.hpp"
#include "format.hpp"
#include <boost/utility/enable_if.hpp>
#include <boost/concept_check.hpp>

namespace mock
{
namespace detail
{
    template< typename Functor, typename Actual >
    struct FunctorCompatible : private boost::noncopyable
    {
    public:
        BOOST_CONCEPT_USAGE( FunctorCompatible )
        {
            boost::require_boolean_expr(
                // if an error is generated by the line below it means
                // an argument passed to 'with' was of the wrong type.
                functor_accepts( actual_argument_type )
            );
        }
    private:
        FunctorCompatible( int ) {}
        Functor functor_accepts;
        Actual actual_argument_type;
    };

    template< typename Expected, typename Actual >
    struct EqualityComparable : private boost::noncopyable
    {
    public:
        BOOST_CONCEPT_USAGE( EqualityComparable )
        {
            boost::require_boolean_expr(
                // if an error is generated by the line below it means
                // an argument passed to 'with' was of the wrong type.
                actual_argument_type == expected_argument_type
            );
        }
    private:
        EqualityComparable( int ) {}
        Expected expected_argument_type;
        Actual actual_argument_type;
    };

    template< typename Actual >
    class check_base : boost::noncopyable
    {
    public:
        virtual ~check_base() {}

        virtual bool operator()( Actual ) const = 0;

        friend std::ostream& operator<<( std::ostream& s, const check_base& c )
        {
            c.format( s );
            return s;
        }
    private:
        virtual void format( std::ostream& ) const = 0;
    };

    template< typename Actual, typename Expected, typename Enable = void >
    class check : public check_base< Actual >
    {
    public:
        explicit check( Expected expected )
            : expected_( expected )
        {
            BOOST_CONCEPT_ASSERT(( EqualityComparable< Expected, Actual > ));
        }
    private:
        virtual bool operator()( Actual actual ) const
        {
            return actual == expected_;
        }
        virtual void format( std::ostream& s ) const
        {
            s << mock::format( expected_ );
        }
    private:
        Expected expected_;
    };

    template< typename Actual, typename Constraint >
    class check< Actual, mock::constraint< Constraint > >
        : public check_base< Actual >
    {
    public:
        explicit check( const constraint< Constraint >& c )
            : c_( c.f_ )
        {
            BOOST_CONCEPT_ASSERT(( FunctorCompatible< Constraint, Actual > ));
        }
    private:
        virtual bool operator()( Actual actual ) const
        {
            return c_( actual );
        }
        virtual void format( std::ostream& s ) const
        {
            s << mock::format( c_ );
        }
    private:
        Constraint c_;
    };

    template< typename Actual, typename Functor >
    class check< Actual, Functor,
        BOOST_DEDUCED_TYPENAME boost::enable_if<
            boost::mpl::or_<
                detail::is_functor< Functor >,
                boost::mpl::not_<
                    detail::is_comparable< Actual, Functor >
                >
            >
        >::type
    > : public check_base< Actual >
    {
    public:
        explicit check( const Functor& f )
            : f_( f )
        {
            BOOST_CONCEPT_ASSERT(( FunctorCompatible< Functor, Actual > ));
        }
    private:
        virtual bool operator()( Actual actual ) const
        {
            return f_( actual );
        }
        virtual void format( std::ostream& s ) const
        {
            s << mock::format( f_ );
        }
    private:
        Functor f_;
    };
}
}

#endif // #ifndef MOCK_CHECK_HPP_INCLUDED
