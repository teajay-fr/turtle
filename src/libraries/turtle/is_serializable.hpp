//
//  Copyright Mathieu Champlon 2011
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MOCK_IS_SERIALIZABLE_HPP_INCLUDED
#define MOCK_IS_SERIALIZABLE_HPP_INCLUDED

#include "yes_no_type.hpp"
#include "sink.hpp"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4913 )
#endif

namespace mock
{
namespace detail
{
namespace protect
{
    template< typename S >
    yes_type operator<<( S&, sink );

    template< typename S, typename T >
    struct is_serializable
    {
        static S* s;
        static T* t;
        // if an error is generated by the line below it means T has more than
        // one conversion to other types which are serializable : the easiest
        // solution would be to add a format function for T.
        enum { value = sizeof( yes_type(), (*s << *t), yes_type() ) == sizeof( yes_type ) };
    };
}
}
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif // #ifndef MOCK_IS_SERIALIZABLE_HPP_INCLUDED
