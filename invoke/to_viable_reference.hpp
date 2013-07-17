#ifndef TO_VIABLE_REFERENCE_HPP
#define TO_VIABLE_REFERENCE_HPP

#include "viable_reference.hpp"

namespace functional
{
  namespace detail
  {
    //To viable ref functor
    template<typename... Ts>
    struct to_viable_ref_functor_combiner
    {};

    template<typename T>
    struct to_viable_ref_functor_combiner<T>
    {
      T operator()(T t) const
      {
        return std::forward<T>(t);
      }
    };

    template<typename T1, typename T2, typename... Ts>
    struct to_viable_ref_functor_combiner<T1, T2, Ts...>
      : to_viable_ref_functor_combiner<T1>,
        to_viable_ref_functor_combiner<T2, Ts...>
    {
      using to_viable_ref_functor_combiner<T1>::operator();
      using to_viable_ref_functor_combiner<T2, Ts...>::operator();
    };  

    template<typename T>
    struct to_viable_ref_impl
    {};

    template<typename... Ts>
    struct to_viable_ref_impl<std::tuple<Ts...>>
      : to_viable_ref_functor_combiner<Ts...>
    {};
  }

  template<typename Pointer>
  struct to_viable_ref 
   : detail::to_viable_ref_impl<functional::viable_references_t<typename std::decay<Pointer>::type>>
  {};
}

#endif //TO_VIABLE_REFERENCE_HPP
