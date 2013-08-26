#ifndef PLACEHOLDERS_HPP
#define PLACEHOLDERS_HPP

#include <functional>
#include <type_traits>
#include "placeholder_traits.hpp"

namespace functional
{
  struct all_placeholder {};

  template<int N>
  struct from_placeholder {};
  
  namespace placeholders
  {
    using namespace std::placeholders;

    all_placeholder _all;

    //Should be variable template
    //But gcc 4.8.1 does not support it
    template<int N>
    using _from = from_placeholder<N>;
  }

  template<>
  struct is_placeholder<all_placeholder>
   : std::integral_constant<int, 1>
  {};

  template<int ArgCount>
  struct parameter_indexes<all_placeholder, ArgCount>
    : type_traits::make_integer_range<int, 1, ArgCount + 1>
  {};

  template<int N>
  struct is_placeholder<from_placeholder<N>>
    : std::integral_constant<int, 1>
  {};

  template<int N, int ArgCount>
  struct parameter_indexes<from_placeholder<N>, ArgCount>
    : type_traits::make_integer_range<int, N, ArgCount + 1>
  {
    static_assert(N <= ArgCount + 1, "To few argument provided.");
  };
}

#endif //PLACEHOLDERS_HPP
