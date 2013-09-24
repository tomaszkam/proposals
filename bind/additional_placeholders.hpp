#ifndef ADDITIONAL_PLACEHOLDERS_HPP
#define ADDITIONAL_PLACEHOLDERS_HPP

#include <functional>
#include <type_traits>
#include "placeholder_traits.hpp"

namespace functional
{
  template<int B, int E>
  struct between_placeholder 
  {
    static_assert(B > 0 && E > 0, "Parameter indexes must be positive.");
    static_assert(E >= B, "Stating parameter index must be lower of equal to end index.");
  };
  
  namespace placeholders
  {
    //Should be variable template
    //But gcc 4.8.1 does not support it
    template<int B, int E>
    using _between = between_placeholder<B, E>;
  }

  template<int B, int E>
  struct is_placeholder<between_placeholder<B, E>>
    : std::integral_constant<int, (E >= B)>
  {};

  template<int B, int E,  int ArgCount>
  struct parameter_indexes<between_placeholder<B, E>, ArgCount>
    : type_traits::make_integer_range<int, B, E + 1>
  {
    static_assert(ArgCount > 0, "Argument count must be positive");
    static_assert(E <= ArgCount + 1, "To few argument provided.");
  };
}

#endif //ADDITIONAL_PLACEHOLDERS_HPP
