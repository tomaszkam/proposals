#ifndef INVOKE_OPTION2_HPP
#define INVOKE_OPTION2_HPP

#include "type_traits.hpp"
#include "to_viable_reference.hpp"

namespace functional
{
  namespace option2
  {
    template<typename Functor, typename Object, typename... Args>
    inline auto invoke(Functor&& functor, Object&& object, Args&&... args)
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
    inline auto invoke(Functor&& functor, Object&& object, Args&&... args)
      ->  typename std::enable_if<
            std::is_member_function_pointer<
              typename std::decay<Functor>::type
            >::value &&
            !type_traits::is_target_reference<
              Object&&,
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_pointer_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype((to_viable_ref<Functor>{}(*std::forward<Object>(object)).*functor)(std::forward<Args>(args)...))
          >::type
    {
      return (to_viable_ref<Functor>{}(*std::forward<Object>(object)).*functor)(std::forward<Args>(args)...);
    }

    template<typename Functor, typename Object, typename... Args>
    inline auto invoke(Functor&& functor, Object&& object, Args&&... args)
      ->  typename std::enable_if<
            std::is_member_function_pointer<
              typename std::decay<Functor>::type
            >::value &&
            !type_traits::is_target_reference<
              Object&&,
              typename std::decay<Functor>::type
            >::value &&
            !type_traits::is_pointer_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_wrapper_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype((to_viable_ref<Functor>{}(std::forward<Object>(object)).*functor)(std::forward<Args>(args)...))
          >::type
    {
      return (to_viable_ref<Functor>{}(std::forward<Object>(object)).*functor)(std::forward<Args>(args)...);
    }

    template<typename Functor, typename Object>
    inline auto invoke(Functor&& functor, Object&& object)
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
    inline auto invoke(Functor&& functor, Object&& object)
      ->  typename std::enable_if<
            std::is_member_object_pointer<
              typename std::decay<Functor>::type
            >::value &&
            !type_traits::is_target_reference<
              Object&&,
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_pointer_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype(to_viable_ref<Functor>{}(*std::forward<Object>(object)).*functor)
          >::type
    {
      return to_viable_ref<Functor>{}(*std::forward<Object>(object)).*functor;
    }

    template<typename Functor, typename Object>
    inline auto invoke(Functor&& functor, Object&& object)
      ->  typename std::enable_if<
            std::is_member_object_pointer<
              typename std::decay<Functor>::type
            >::value &&
            !type_traits::is_target_reference<
              Object&&,
              typename std::decay<Functor>::type
            >::value &&
            !type_traits::is_pointer_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value &&
            type_traits::is_wrapper_compatible_with_member_pointer<
              Object&&,
              typename std::decay<Functor>::type
            >::value,
            decltype(to_viable_ref<Functor>{}(std::forward<Object>(object)).*functor)
          >::type
    {
      return to_viable_ref<Functor>{}(std::forward<Object>(object)).*functor;
    }

    template<typename Functor, typename... Args>
    inline auto invoke(Functor&& functor, Args&&... args)
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

#endif //INVOKE_OPTION2_HPP
