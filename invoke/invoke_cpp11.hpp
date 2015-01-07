//        Copyright Tomasz Kamiński 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef INVOKE_CPP11_HPP
#define INVOKE_CPP11_HPP

#include "type_traits.hpp"

namespace functional
{
  namespace cpp11
  {
    template<typename Functor, typename Object, typename... Args>
    constexpr auto invoke(Functor&& functor, Object&& object, Args&&... args)
      ->  typename std::enable_if<
            std::is_member_function_pointer<
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_target_reference<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype((object.*functor)(std::forward<Args>(args)...))
          >::type
    {
      return (object.*functor)(std::forward<Args>(args)...);
    }

    template<typename Functor, typename Object, typename... Args>
    constexpr auto invoke(Functor&& functor, Object&& object, Args&&... args)
      ->  typename std::enable_if<
            std::is_member_function_pointer<
              typename std::decay<Functor>::type
            >::value &&
            !type_traits::is_target_reference<
              Object&&,
              typename std::decay<Functor>::type
            >::value, 
            decltype(((*std::forward<Object>(object)).*functor)(std::forward<Args>(args)...))
          >::type
    {
      return ((*std::forward<Object>(object)).*functor)(std::forward<Args>(args)...);
    }

    template<typename Functor, typename Object>
    constexpr auto invoke(Functor&& functor, Object&& object)
      ->  typename std::enable_if<
            std::is_member_object_pointer<
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_target_reference<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype(object.*functor)
          >::type
    {
      return object.*functor;
    }

    template<typename Functor, typename Object>
    constexpr auto invoke(Functor&& functor, Object&& object)
      ->  typename std::enable_if<
            std::is_member_object_pointer<
              typename std::decay<Functor>::type
            >::value &&
            !type_traits::is_target_reference<
              Object&&,
              typename std::decay<Functor>::type
            >::value, 
            decltype((*std::forward<Object>(object)).*functor)
          >::type
    {
      return (*std::forward<Object>(object)).*functor;
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

#endif //INVOKE_CPP11_HPP
