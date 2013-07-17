#ifndef VIABLE_REFERENCE_HPP
#define VIABLE_REFERENCE_HPP

#include <tuple>

namespace functional
{
  namespace detail
  {
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
  }

  template<typename T>
  struct viable_references
  {
    typedef std::tuple<> type;
  };

  template<typename Class, typename Member>
  struct viable_references<Member Class::*>
  {
    typedef typename detail::viable_references_impl<Class, Member>::type type;
  };

  template<typename T>
  using viable_references_t = typename viable_references<T>::type;
}

#endif //VIABLE_REFERENCE_HPP
