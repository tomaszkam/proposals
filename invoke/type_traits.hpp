#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP

#include <type_traits>
#include "viable_reference.hpp"

namespace type_traits
{
  namespace detail
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
  }

  //Is reference to pointer target or derived
  template<typename Object, typename Pointer>
  struct is_target_reference :
    public std::integral_constant<
             bool,
             std::is_reference<Object>::value &&
             std::is_base_of<
               typename detail::target_type<Pointer>::type,       
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
      return pred || predicate_or(std::forward<Preds>(preds)...);
    }

    //Predicate checking
    template<typename Object, typename Type>
    struct is_wrapper_compatible_with_member_pointer_impl : public std::false_type
    {};

    template<typename Object, typename... ViableRefs>
    struct is_wrapper_compatible_with_member_pointer_impl<Object, std::tuple<ViableRefs...>>
      : std::integral_constant<bool, predicate_or(std::is_convertible<Object, ViableRefs>{}...)>
    {};
  }

  template<typename Object, typename Pointer>
  struct is_wrapper_compatible_with_member_pointer
    : detail::is_wrapper_compatible_with_member_pointer_impl<Object, functional::viable_references_t<Pointer>>
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
