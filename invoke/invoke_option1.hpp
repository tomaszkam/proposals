//        Copyright Tomasz Kamiński 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef INVOKE_OPTION1_HPP
#define INVOKE_OPTION1_HPP

#include "type_traits.hpp"
#include "to_reference.hpp"

namespace functional
{
  namespace option1
  {
    template<typename Functor, typename Object, typename... Args>
    constexpr auto invoke(Functor&& functor, Object&& object, Args&&... args)
      ->  typename std::enable_if<
            std::is_member_function_pointer<
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_wrapper_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype((to_target_reference<Functor>{}(std::forward<Object>(object)).*functor)(std::forward<Args>(args)...))
          >::type
    {
      return (to_target_reference<Functor>{}(std::forward<Object>(object)).*functor)(std::forward<Args>(args)...);
    }

    template<typename Functor, typename Object, typename... Args>
    constexpr auto invoke(Functor&& functor, Object&& object, Args&&... args)
      ->  typename std::enable_if<
            std::is_member_function_pointer<
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_pointer_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype((to_target_reference<Functor>{}(*std::forward<Object>(object)).*functor)(std::forward<Args>(args)...))
          >::type
    {
      return (to_target_reference<Functor>{}(*std::forward<Object>(object)).*functor)(std::forward<Args>(args)...);
    }

    template<typename Functor, typename Object>
    constexpr auto invoke(Functor&& functor, Object&& object)
      ->  typename std::enable_if<
            std::is_member_object_pointer<
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_wrapper_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype(to_target_reference<Functor>{}(std::forward<Object>(object)).*functor)
          >::type
    {
      return to_target_reference<Functor>{}(std::forward<Object>(object)).*functor;
    }

    template<typename Functor, typename Object>
    constexpr auto invoke(Functor&& functor, Object&& object)
      ->  typename std::enable_if<
            std::is_member_object_pointer<
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_pointer_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype(to_target_reference<Functor>{}(*std::forward<Object>(object)).*functor)
          >::type
    {
      return to_target_reference<Functor>{}(*std::forward<Object>(object)).*functor;
    }

    template<typename Functor, typename... Args>
    constexpr auto invoke(Functor&& functor, Args&&... args)
      ->  typename std::enable_if<
            !std::is_member_pointer<
              typename std::decay<Functor>::type
            >::value,
            decltype(std::forward<Functor>(functor)(std::forward<Args>(args)...))
          >::type
    {
      return std::forward<Functor>(functor)(std::forward<Args>(args)...);
    }
  }
}

#endif //INVOKE_OPTION1_HPP
