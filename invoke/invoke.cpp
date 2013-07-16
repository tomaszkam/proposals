#include <memory>
#include <type_traits>
#include <tuple>

//Viable references getter
template<typename Class, typename Member>
struct viable_references_impl
{
  typedef std::tuple<
    Class&, Class&&,
    Class const&, Class const&&,
    Class volatile&, Class volatile&&,
    Class const volatile&, Class const volatile&&
  > type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...)>
{
  typedef std::tuple<Class&, Class&&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) const>
{
  typedef std::tuple<Class const&, Class const&&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) volatile>
{
  typedef std::tuple<Class volatile&, Class volatile&&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) const volatile>
{
  typedef std::tuple<Class const volatile&, Class const volatile&&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) &>
{
  typedef std::tuple<Class&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) const&>
{
  typedef std::tuple<Class const&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) volatile&>
{
  typedef std::tuple<Class volatile&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) const volatile&>
{
  typedef std::tuple<Class const volatile&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) &&>
{
  typedef std::tuple<Class&&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) const&&>
{
  typedef std::tuple<Class const&&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) volatile&&>
{
  typedef std::tuple<Class volatile&&> type;
};

template<typename Class, typename Ret, typename... Args>
struct viable_references_impl<Class, Ret(Args...) const volatile&&>
{
  typedef std::tuple<Class const volatile&&> type;
};

template<typename T>
struct viable_references
{
  typedef std::tuple<> type;
};

template<typename Class, typename Member>
struct viable_references<Member Class::*>
{
  typedef typename viable_references_impl<Class, Member>::type type;
};

template<typename T>
using viable_references_t = typename viable_references<T>::type;

//MPL or
constexpr bool predicate_or()
{
  return false;
};

template<typename Pred, typename... Preds>
constexpr bool predicate_or(Pred&& pred, Preds&&... preds)
{
  return pred || predicate_or(std::forward<Preds>(preds)...);
};

//Predicate checking
template<typename Object, typename Type>
struct is_wrapper_compatible_with_member_pointer_impl : public std::false_type
{};

template<typename Object, typename... ViableRefs>
struct is_wrapper_compatible_with_member_pointer_impl<Object, std::tuple<ViableRefs...>>
  : std::integral_constant<bool, predicate_or(std::is_convertible<Object, ViableRefs>{}...)>
{};

template<typename Object, typename Pointer>
struct is_wrapper_compatible_with_member_pointer
  : is_wrapper_compatible_with_member_pointer_impl<Object, viable_references_t<Pointer>>
{};

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

template<typename Object, typename Pointer>
struct is_pointer_compatible_with_member_pointer
 : public std::integral_constant<bool, is_pointer_compatible_with_member_pointer_impl<Object, Pointer>(0)>
{};

//Class target
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

template<typename Pointer>
struct to_viable_ref 
 : to_viable_ref_impl<viable_references_t<typename std::decay<Pointer>::type>>
{};


template<typename Functor, typename Object, typename... Args>
inline auto invoke(Functor&& functor, Object&& object, Args&&... args)
  ->  typename std::enable_if<
        std::is_member_function_pointer<
          typename std::decay<Functor>::type
        >::value &&
        is_target_reference<
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
        !is_target_reference<
          Object&&,
          typename std::decay<Functor>::type
        >::value &&
        is_pointer_compatible_with_member_pointer<
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
        !is_target_reference<
          Object&&,
          typename std::decay<Functor>::type
        >::value &&
        !is_pointer_compatible_with_member_pointer<
          Object&&,
          typename std::decay<Functor>::type
        >::value &&
        is_wrapper_compatible_with_member_pointer<
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
        is_target_reference<
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
        !is_target_reference<
          Object&&,
          typename std::decay<Functor>::type
        >::value &&
        is_pointer_compatible_with_member_pointer<
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
        !is_target_reference<
          Object&&,
          typename std::decay<Functor>::type
        >::value &&
        !is_pointer_compatible_with_member_pointer<
          Object&&,
          typename std::decay<Functor>::type
        >::value &&
        is_wrapper_compatible_with_member_pointer<
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

struct Base {};

struct Class : Base
{
  void normal() {}
  void reference()& {}
  int member;
};

struct Derived : Class {};

struct Pointer
{
  Class clazz;

  Class& operator*() { return clazz; }
};

struct ValueConversion
{
  operator Class() { return Class{}; }
};

struct ReferenceConversion
{
  Class clazz;

  operator Class&() { return clazz; }
};

struct Mixed
{
  Class clazz;

  Class& operator*() { return clazz; }
  operator Class() { throw std::logic_error("ReferenceConversion operator invoked for type with operator*"); }
};

static_assert(!is_target_reference<Base&, int Class::*>::value, "checking Base");
static_assert(!is_target_reference<Pointer&, int Class::*>::value, "checking Pointer");
static_assert(!is_target_reference<ValueConversion&, int Class::*>::value, "checking Pointer");
static_assert(!is_target_reference<ReferenceConversion&, int Class::*>::value, "checking Pointer");
static_assert(!is_target_reference<Mixed&, int Class::*>::value, "checking Mixed");

static_assert(is_target_reference<Class&, int Class::*>::value, "checking Class&");
static_assert(is_target_reference<Class const&, int Class::*>::value, "checking Class const&");
static_assert(is_target_reference<Class&&, int Class::*>::value, "checking Class&");
static_assert(is_target_reference<Class const&&, int Class::*>::value, "checking Class const&&");

static_assert(is_target_reference<Derived&, int Derived::*>::value, "checking Derived&");
static_assert(is_target_reference<Derived const&, int Derived::*>::value, "checking Derived const&");
static_assert(is_target_reference<Derived&&, int Derived::*>::value, "checking Derived&");
static_assert(is_target_reference<Derived const&&, int Derived::*>::value, "checking Derived const&&");

Base base;
Class clazz;
Derived derived;
Class* rawPointer;
Pointer smartPointer;
ValueConversion valueConversion;
ReferenceConversion referenceConversion;
Mixed mixed;

static_assert(!is_pointer_compatible_with_member_pointer<decltype(base), int Class::*>::value, "checking Base");
static_assert(!is_pointer_compatible_with_member_pointer<decltype(clazz), int Class::*>::value, "checking Class");
static_assert(!is_pointer_compatible_with_member_pointer<decltype(derived), int Class::*>::value, "checking Derived");
static_assert(is_pointer_compatible_with_member_pointer<decltype(rawPointer), int Class::*>::value, "checking RawPointer");
static_assert(is_pointer_compatible_with_member_pointer<decltype(smartPointer), int Class::*>::value, "checking SmartPointer");
static_assert(!is_pointer_compatible_with_member_pointer<decltype(valueConversion), int Class::*>::value, "checking ValueConversion");
static_assert(!is_pointer_compatible_with_member_pointer<decltype(referenceConversion), int Class::*>::value, "checking ReferenceConversion");
static_assert(is_pointer_compatible_with_member_pointer<decltype(mixed), int Class::*>::value, "checking Mixed");

static_assert(!is_wrapper_compatible_with_member_pointer<decltype(base), int Class::*>::value, "checking Base");
static_assert(is_wrapper_compatible_with_member_pointer<decltype(clazz), int Class::*>::value, "checking Class");
static_assert(is_wrapper_compatible_with_member_pointer<decltype(derived), int Class::*>::value, "checking Derived");
static_assert(!is_wrapper_compatible_with_member_pointer<decltype(rawPointer), int Class::*>::value, "checking RawPointer");
static_assert(!is_wrapper_compatible_with_member_pointer<decltype(smartPointer), int Class::*>::value, "checking SmartPointer");
static_assert(is_wrapper_compatible_with_member_pointer<decltype(valueConversion), int Class::*>::value, "checking ValueConversion");
static_assert(is_wrapper_compatible_with_member_pointer<decltype(referenceConversion), int Class::*>::value, "checking ReferenceConversion");
static_assert(is_wrapper_compatible_with_member_pointer<decltype(mixed), int Class::*>::value, "checking Mixed");


int main()
{
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
