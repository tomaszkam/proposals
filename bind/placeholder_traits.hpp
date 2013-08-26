#ifndef PLACEHOLDER_TRAITS_HPP
#define PLACEHOLDER_TRAITS_HPP

#include <functional>
#include "integer_sequence.hpp"

namespace functional
{
  template<typename T>
  struct is_placeholder 
   : std::is_placeholder<T> 
  {};

  template<typename Placeholder, int ArgCount>
  struct parameter_indexes
    : type_traits::integer_sequence<int, is_placeholder<Placeholder>::value>
  {
    static_assert(is_placeholder<Placeholder>::value > 0, "Provided type must be an placeholder.");
    static_assert(is_placeholder<Placeholder>::value <= ArgCount, "Placeholder position is greater the argument count.");
  };
}

#endif //PLACEHOLDER_TRAITS_HPP
