#ifndef BIND_HPP
#define BIND_HPP

#include <tuple>
#include <type_traits>
#include "../invoke/invoke_cpp11.hpp"
#include "../invoke/invoke_option1.hpp"
#include "../invoke/invoke_option2.hpp"
#include "placeholder_traits.hpp"

#ifndef INVOKE_MODEL
#define INVOKE_MODEL cpp11
#endif 

namespace functional
{
  namespace detail
  {
    template<std::size_t RN, typename StoredArgs, typename CallArgs>
    class bind_invoker
    {
      using StoredArgsD = typename std::decay<StoredArgs>::type;
      using CallArgsD = typename std::decay<CallArgs>::type;

      static constexpr std::size_t StoredArgsCount = std::tuple_size<StoredArgsD>::value;
      static constexpr std::size_t CallArgsCount = std::tuple_size<CallArgsD>::value;

      template<std::size_t I>
      using CallArg = typename std::tuple_element<I, CallArgsD>::type;

      static constexpr std::size_t N  = StoredArgsCount - RN;
      using StoredArg = typename std::tuple_element<N, StoredArgsD>::type;
      static constexpr std::integral_constant<bool, is_placeholder<StoredArg>::value> IsPlaceholder{};
      static constexpr std::integral_constant<bool, std::is_bind_expression<StoredArg>::value> IsBindExpression{};       
      static_assert(!IsPlaceholder || !IsBindExpression, "Type cannot be marked as placehodler and bind expression.");

      StoredArgs storedArgs;
      CallArgs   callArgs;

      bind_invoker<RN-1, StoredArgs, CallArgs> next_invoker() const
      {
        return {std::forward<StoredArgs>(storedArgs), std::forward<CallArgs>(callArgs)};
      }

      template<std::size_t... Positions, typename... ActualArgs>
      auto handle_placeholder(type_traits::integral_sequence<std::size_t, Positions...>, ActualArgs&&... actualArgs)
        -> decltype(this->next_invoker()(std::forward<ActualArgs>(actualArgs)..., std::get<Positions-1>(std::forward<CallArgs>(callArgs))...))
      {
        return next_invoker()(std::forward<ActualArgs>(actualArgs)..., std::get<Positions-1>(std::forward<CallArgs>(callArgs))...);
      }

      template<typename Functor, std::size_t... Indexes, typename... ActualArgs>
      auto handle_bind(Functor&& functor, type_traits::integral_sequence<std::size_t, Indexes...>, ActualArgs&&... actualArgs)
        -> decltype(this->next_invoker()(std::forward<ActualArgs>(actualArgs)..., std::forward<Functor>(functor)(std::get<Indexes>(std::forward<CallArgs>(callArgs))...)))
      {
        return next_invoker()(std::forward<ActualArgs>(actualArgs)..., std::forward<Functor>(functor)(std::get<Indexes>(std::forward<CallArgs>(callArgs))...));
      }

      template<typename CurrentArg, typename... ActualArgs>
      auto pass_arg(std::false_type, std::false_type, CurrentArg&& currentArg, ActualArgs&&... actualArgs)
        -> decltype(this->next_invoker()(std::forward<ActualArgs>(actualArgs)..., std::forward<CurrentArg>(currentArg)))
      {
        return next_invoker()(std::forward<ActualArgs>(actualArgs)..., std::forward<CurrentArg>(currentArg));
      }

      template<typename T, typename... ActualArgs>
      auto pass_arg(std::false_type, std::false_type, std::reference_wrapper<T> ref, ActualArgs&&... actualArgs)
        -> decltype(this->next_invoker()(std::forward<ActualArgs>(actualArgs)..., ref.get()))
      {
        return next_invoker()(std::forward<ActualArgs>(actualArgs)..., ref.get());
      }
     
      template<typename Placeholder, typename... ActualArgs>
      auto pass_arg(std::true_type, std::false_type, Placeholder&&, ActualArgs&&... actualArgs)
        -> decltype(this->handle_placeholder(placeholder_positions<StoredArg, CallArgsCount>{}, std::forward<ActualArgs>(actualArgs)...))
      {
        return handle_placeholder(placeholder_positions<StoredArg, CallArgsCount>{}, std::forward<ActualArgs>(actualArgs)...);
      }

      template<typename Functor, typename... ActualArgs>
      auto pass_arg(std::false_type, std::true_type, Functor&& functor, ActualArgs&&... actualArgs)
        -> decltype(this->handle_bind(std::forward<Functor>(functor), type_traits::make_integral_sequence<std::size_t, CallArgsCount>{}, std::forward<ActualArgs>(actualArgs)...))
      {
        return handle_bind(std::forward<Functor>(functor), type_traits::make_integral_sequence<std::size_t, CallArgsCount>{}, std::forward<ActualArgs>(actualArgs)...);
      }

    public:
      bind_invoker(StoredArgs _storedArgs, CallArgs _callArgs)
        : storedArgs(std::forward<StoredArgs>(_storedArgs)),
          callArgs(std::forward<CallArgs>(_callArgs))
      {}

      template<typename... ActualArgs>
      auto operator()(ActualArgs&&... actualArgs)
       -> decltype(this->pass_arg(IsPlaceholder, IsBindExpression, std::get<N>(std::forward<StoredArgs>(storedArgs)),
                                  std::forward<ActualArgs>(actualArgs)...))
      {
        return pass_arg(IsPlaceholder, IsBindExpression, std::get<N>(std::forward<StoredArgs>(storedArgs)),
                        std::forward<ActualArgs>(actualArgs)...);
      }
    };

    template<typename StoredArgs, typename CallArgs>
    class bind_invoker<0, StoredArgs, CallArgs>
    {
      StoredArgs storedArgs;
      CallArgs   callArgs;

    public:
      bind_invoker(StoredArgs _storedArgs, CallArgs _callArgs)
        : storedArgs(std::forward<StoredArgs>(_storedArgs)),
          callArgs(std::forward<CallArgs>(_callArgs))
      {}

      template<typename... ActualArgs>
      auto operator()(ActualArgs&&... actualArgs)
        -> decltype(INVOKE_MODEL::invoke(std::get<0>(std::forward<StoredArgs>(storedArgs)), std::forward<ActualArgs>(actualArgs)...))
      {
        return INVOKE_MODEL::invoke(std::get<0>(std::forward<StoredArgs>(storedArgs)), std::forward<ActualArgs>(actualArgs)...);
      }
    };


    template<typename StoredArgs, typename CallArgs>
    inline bind_invoker<
      std::tuple_size<typename std::decay<StoredArgs>::type>::value-1, 
      StoredArgs&&, CallArgs&&
    > make_bind_invoker(StoredArgs&& storedArgs, CallArgs&& callArgs)
    {
      return {std::forward<StoredArgs>(storedArgs), std::forward<CallArgs>(callArgs)};
    }

    template<typename... StoredArgs>
    class bind_functor
    {
      std::tuple<StoredArgs...> storedArgs;

    public:
      template<typename... Args>
      bind_functor(Args&&... args)
       : storedArgs(std::forward<Args>(args)...)
      {}

      template<typename... Args>
      auto operator()(Args&&... args)
        -> decltype(make_bind_invoker(storedArgs, std::forward_as_tuple(std::forward<Args>(args)...))())
      {
        return make_bind_invoker(storedArgs, std::forward_as_tuple(std::forward<Args>(args)...))();
      }
    };
  }

  template<typename Function, typename... Args>
  inline detail::bind_functor<
    typename std::decay<Function>::type,
    typename std::decay<Args>::type...
  > bind(Function&& f, Args&&... args)
  {
    return {std::forward<Function>(f), std::forward<Args>(args)...};
  }
}

namespace std
{
  template<typename... StoredArgs>
  struct is_bind_expression<functional::detail::bind_functor<StoredArgs...>>
    : true_type
  {};
}

#endif //BIND_HPP
