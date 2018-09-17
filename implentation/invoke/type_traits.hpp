//        Copyright Tomasz Kamiński 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP

#include <type_traits>

namespace type_traits
{
  template<typename T>
  struct target_type
  {
    typedef void type;
  };

  template<typename Class, typename Member>
  struct target_type<Member Class::*>
  {
    typedef Class type;
  };

  //Is reference to pointer target or derived
  template<typename Object, typename Pointer>
  struct is_target_reference :
    public std::integral_constant<
             bool,
             std::is_reference<Object>::value &&
             std::is_base_of<
               typename target_type<Pointer>::type,       
               typename std::decay<Object>::type
             >::value 
           >
  {};

  namespace detail
  {
    //MPL or
    constexpr bool predicate_or()
    {
      return false;
    }

    template<typename Pred, typename... Preds>
    constexpr bool predicate_or(Pred&& pred, Preds&&... preds)
    {
      return pred || predicate_or(preds...);
    }

    template<typename Object, typename TargetType>
    struct is_wrapper_compatible_with_member_pointer_impl
      : std::integral_constant<bool, predicate_or(
           std::is_convertible<Object, TargetType&>{},
           std::is_convertible<Object, TargetType const&>{},
           std::is_convertible<Object, TargetType&&>{},
           std::is_convertible<Object, TargetType const&&>{})>
    {};

    template<typename Object>
    struct is_wrapper_compatible_with_member_pointer_impl<Object, void>
      : std::false_type
    {};
  }

  template<typename Object, typename Pointer>
  struct is_wrapper_compatible_with_member_pointer
    : detail::is_wrapper_compatible_with_member_pointer_impl<Object, typename target_type<Pointer>::type>
  {};

  namespace detail
  {
    template<typename Object, typename Pointer>
    constexpr bool is_pointer_compatible_with_member_pointer_impl(typename std::decay<decltype(*std::declval<Object>())>::type*)
    {
      return is_wrapper_compatible_with_member_pointer<decltype(*std::declval<Object>()), Pointer>::value;
    }

    template<typename Object, typename Pointer>
    constexpr bool is_pointer_compatible_with_member_pointer_impl(...)
    {
      return false;
    }
  }

  template<typename Object, typename Pointer>
  struct is_pointer_compatible_with_member_pointer
   : public std::integral_constant<bool, detail::is_pointer_compatible_with_member_pointer_impl<Object, Pointer>(0)>
  {};
}

#endif //TYPE_TRAITS_HPP
