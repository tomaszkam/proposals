#include "type_traits.hpp"
#include "to_viable_reference.hpp"
#include "test_classes.hpp"

namespace functional
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


using namespace test_classes;

Base base;
Class clazz;
Derived derived;
Class* rawPointer;
Pointer smartPointer;
ValueConversion valueConversion;
ReferenceConversion referenceConversion;
Mixed mixed;

int main()
{
  using functional::invoke;
 
  invoke(&Class::normal, Class{});
  //invoke(&Class::normal, base);
  invoke(&Class::normal, clazz);
  invoke(&Class::normal, derived);
  invoke(&Class::normal, rawPointer);
  invoke(&Class::normal, smartPointer);
  invoke(&Class::normal, valueConversion);
  invoke(&Class::normal, referenceConversion);
  invoke(&Class::normal, mixed);
 
  /* Wont work on gcc 4.8.1 and clang 3.2 because of bug with in implementation
     of is_member_function_pointer for ref qualified methods.
     See: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57388
          http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57825
  //invoke(&Class::reference, Class{});
  //invoke(&Class::reference, base);
  invoke(&Class::reference, clazz);
  invoke(&Class::reference, derived);
  invoke(&Class::reference, rawPointer);
  invoke(&Class::reference, smartPointer);
  invoke(&Class::reference, valueConversion);
  invoke(&Class::reference, referenceConversion);
  invoke(&Class::reference, mixed);*/

  invoke(&Class::member, Class{});
  //invoke(&Class::member, base);
  invoke(&Class::member, clazz);
  invoke(&Class::member, derived);
  invoke(&Class::member, rawPointer);
  invoke(&Class::member, smartPointer);
  invoke(&Class::member, valueConversion);
  invoke(&Class::member, referenceConversion);
  invoke(&Class::member, mixed);

  invoke([](int) {}, 1);
}
