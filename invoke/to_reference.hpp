#ifndef TO_REFERENCE_HPP
#define TO_REFERENCE_HPP

#include <utility>
#include "type_traits.hpp"

namespace functional
{
  template<typename Type>
  struct to_reference
  {
    Type& operator()(Type& t) const
    {
      return t;
    }
    
    Type const& operator()(Type const& t) const
    {
      return t;
    }

    Type&& operator()(Type&& t) const
    {
      return std::move(t);
    }

    Type const&& operator()(Type const&& t) const
    {
      return std::move(t);
    }
  };

  template<>
  struct to_reference<void>
  {};

  template<typename Pointer>
  struct to_target_reference
    : to_reference<typename type_traits::target_type<typename std::decay<Pointer>::type>::type>
  {};
}

#endif //TO_REFERENCE_HPP
