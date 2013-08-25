#ifndef PLACEHOLDERS_HPP
#define PLACEHOLDERS_HPP

#include <functional>
#include <type_traits>
#include "placeholder_traits.hpp"

namespace functional
{
  struct all_placeholder {};

  template<std::size_t N>
  struct from_placeholder {};
  
  namespace placeholders
  {
    using namespace std::placeholders;

    all_placeholder _all;

    //Should be variable template
    //But gcc 4.8.1 does not support it
    template<std::size_t N>
    using _from = from_placeholder<N>;
  }

  template<>
  struct is_placeholder<all_placeholder>
   : std::integral_constant<int, 1>
  {};

  template<std::size_t ArgCount>
  struct placeholder_positions<all_placeholder, ArgCount>
    : type_traits::make_integral_range<std::size_t, 1, ArgCount + 1>
  {};

  template<std::size_t N>
  struct is_placeholder<from_placeholder<N>>
    : std::integral_constant<int, 1>
  {};

  template<std::size_t N, std::size_t ArgCount>
  struct placeholder_positions<from_placeholder<N>, ArgCount>
    : type_traits::make_integral_range<std::size_t, N, ArgCount + 1>
  {
    static_assert(N <= ArgCount + 1, "To few argument provided.");
  };
}

#endif //PLACEHOLDERS_HPP
