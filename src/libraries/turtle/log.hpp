//
//  Copyright Mathieu Champlon 2011
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MOCK_LOG_HPP_INCLUDED
#define MOCK_LOG_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>
#include <boost/detail/container_fwd.hpp>
#include <boost/function_types/is_callable_builtin.hpp>
#include <ostream>
#include <string>

namespace boost
{
namespace assign_detail
{
    template< typename T > class generic_list;
}
}

namespace mock
{
    struct stream
    {
        explicit stream( std::ostream& s )
            : s_( &s )
        {}
        std::ostream* s_;
    };

#ifdef MOCK_USE_CONVERSIONS

namespace detail3
{
    struct sink
    {
        template< typename T >
        sink( const T& )
        {}
    };

    inline std::ostream& operator<<( std::ostream& s, const sink& )
    {
        return s << "?";
    }

    struct holder
    {
        virtual ~holder()
        {}
        virtual void serialize( std::ostream& s ) const = 0;
    };

    template< typename T >
    struct holder_imp : holder
    {
        explicit holder_imp( const T& t )
            : t_( &t )
        {}
        virtual void serialize( std::ostream& s ) const
        {
            // if it fails here because ambiguous stuff
            // -> add operator<< to mock::stream for T
            s << *t_;
        }
        const T* t_;
    };

    struct data
    {
        template< typename T >
        data( const T& t )
            : h_( new holder_imp< T >( t ) )
        {}
        ~data()
        {
            delete h_;
        }
        holder* h_;
    };
}

    inline stream& operator<<( stream& s, const detail3::data& d )
    {
        d.h_->serialize( *s.s_ );
        return s;
    }

#else // MOCK_USE_CONVERSIONS

namespace detail3
{
    template< typename S, typename T >
    S& operator<<( S &s, const T& )
    {
        return s << "?";
    }
}

    template< typename T >
    BOOST_DEDUCED_TYPENAME boost::disable_if<
        boost::function_types::is_callable_builtin< T >, stream&
    >::type
    operator<<( stream& s, const T& t )
    {
        using namespace detail3;
        *s.s_ << t;
        return s;
    }

#endif // MOCK_USE_CONVERSIONS

namespace detail2
{
    template< typename T >
    void serialize( stream& s, const T& t )
    {
        // if it fails here because ambiguous stuff
        // -> add operator<< to mock::stream for T
        s << t;
    }
    inline void serialize( stream& s, bool b )
    {
        s << (b ? "true" : "false");
    }
    template< typename C, typename T, typename A >
    void serialize( stream& s, const std::basic_string< C, T, A >& str )
    {
        s << '"' << str << '"';
    }
    inline void serialize( stream& s, const char* str )
    {
        s << '"' << str << '"';
    }

    template< typename T >
    struct formatter
    {
        explicit formatter( const T& t )
            : t_( &t )
        {}
        void serialize( stream& s ) const
        {
            detail2::serialize( s, *t_ );
        }
        const T* t_;
    };

    template< typename T >
    stream& operator<<( stream& s, const formatter< T >& ser )
    {
        ser.serialize( s );
        return s;
    }

    template< typename T >
    std::ostream& operator<<( std::ostream& s, const formatter< T >& ser )
    {
        stream ss( s );
        ser.serialize( ss );
        return s;
    }
}

    template< typename T >
    detail2::formatter< T > format( const T& t )
    {
        return detail2::formatter< T >( t );
    }

    template< typename T1, typename T2 >
    stream& operator<<( stream& s, const std::pair< T1, T2 >& p )
    {
        return s << '(' << mock::format( p.first )
            << ',' << mock::format( p.second ) << ')';
    }

namespace detail
{
    template< typename T >
    void serialize( stream& s, const T& begin, const T& end )
    {
        s << '(';
        for( T it = begin; it != end; ++it )
            s << (it == begin ? "" : ",") << mock::format( *it );
        s << ')';
    }
}

    template< typename T, typename A >
    stream& operator<<( stream& s, const std::deque< T, A >& t )
    {
        detail::serialize( s, t.begin(), t.end() );
        return s;
    }
    template< typename T, typename A >
    stream& operator<<( stream& s, const std::list< T, A >& t )
    {
        detail::serialize( s, t.begin(), t.end() );
        return s;
    }
    template< typename T, typename A >
    stream& operator<<( stream& s, const std::vector< T, A >& t )
    {
        detail::serialize( s, t.begin(), t.end() );
        return s;
    }
    template< typename K, typename T, typename C, typename A >
    stream& operator<<( stream& s, const std::map< K, T, C, A >& t )
    {
        detail::serialize( s, t.begin(), t.end() );
        return s;
    }
    template< typename K, typename T, typename C, typename A >
    stream& operator<<( stream& s, const std::multimap< K, T, C, A >& t )
    {
        detail::serialize( s, t.begin(), t.end() );
        return s;
    }
    template< typename T, typename C, typename A >
    stream& operator<<( stream& s, const std::set< T, C, A >& t )
    {
        detail::serialize( s, t.begin(), t.end() );
        return s;
    }
    template< typename T, typename C, typename A >
    stream& operator<<( stream& s, const std::multiset< T, C, A >& t )
    {
        detail::serialize( s, t.begin(), t.end() );
        return s;
    }
    template< typename T >
    stream& operator<<( stream& s,
        const boost::assign_detail::generic_list< T >& t )
    {
        detail::serialize( s, t.begin(), t.end() );
        return s;
    }

    template< typename T >
    BOOST_DEDUCED_TYPENAME boost::enable_if<
        boost::function_types::is_callable_builtin< T >,
        stream&
    >::type
    operator<<( stream& s, const T& )
    {
         return s << '?';
    }
}

#endif // #ifndef MOCK_LOG_HPP_INCLUDED