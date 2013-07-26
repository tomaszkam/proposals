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
    template<typename Arg,
             bool = (is_placeholder<Arg>::value > 0),
             bool = std::is_bind_expression<Arg>::value>
    class argument_passer;

    template<typename T>
    class argument_passer<T, false, false>
    {
    public:
      template<typename Invoker, typename Arg, typename... ActualArgs>
      auto operator()(const Invoker& invoker, Arg&& arg, ActualArgs&&... actualArgs)
        -> decltype(invoker(std::forward<Arg>(arg), std::forward<ActualArgs>(actualArgs)...))
      {
        return invoker(std::forward<ActualArgs>(actualArgs)..., std::forward<Arg>(arg)); 
      }
    };

    template<typename T>
    class argument_passer<std::reference_wrapper<T>, false, false>
    {
    public:
      template<typename Invoker, typename... ActualArgs>
      auto operator()(const Invoker& invoker, const std::reference_wrapper<T>& ref, ActualArgs&&... actualArgs)
        -> decltype(invoker(ref.get(), std::forward<ActualArgs>(actualArgs)...))
      {
        return invoker(std::forward<ActualArgs>(actualArgs)..., ref.get()); 
      }
    };

    template<typename Placeholder>
    class argument_passer<Placeholder, true, false>
    {
      template<typename Invoker, std::size_t... Positions, typename... ActualArgs>
      auto pass_selected(const Invoker& invoker, type_traits::integral_sequence<std::size_t, Positions...>, ActualArgs&&... actualArgs)
        -> decltype(invoker(std::forward<ActualArgs>(actualArgs)..., std::get<Positions-1>(invoker.call_args())...))
      {
        return invoker(std::forward<ActualArgs>(actualArgs)..., std::get<Positions-1>(invoker.call_args())...);
      }

    public:
      template<typename Invoker, typename... ActualArgs>
      auto operator()(const Invoker& invoker, const Placeholder&, ActualArgs&&... actualArgs)
        -> decltype(this->pass_selected(invoker, placeholder_positions<Placeholder, Invoker::call_args_size()>{}, std::forward<ActualArgs>(actualArgs)...))
      {
        return pass_selected(invoker, placeholder_positions<Placeholder, Invoker::call_args_size()>{}, std::forward<ActualArgs>(actualArgs)...);
      }
    };

    template<typename Bind>
    class argument_passer<Bind, false, true>
    {
      template<typename Invoker, typename Function, std::size_t... Indexes, typename... ActualArgs>
      auto pass_result(const Invoker& invoker, Function&& function, type_traits::integral_sequence<std::size_t, Indexes...>, ActualArgs&&... actualArgs)
        -> decltype(invoker(std::forward<ActualArgs>(actualArgs)..., std::forward<Function>(function)(std::get<Indexes>(invoker.call_args())...)))
      {
        return invoker(std::forward<ActualArgs>(actualArgs)..., std::forward<Function>(function)(std::get<Indexes>(invoker.call_args())...));
      }

    public:
      template<typename Invoker, typename Function, typename... ActualArgs>
      auto operator()(const Invoker& invoker, Function&& function, ActualArgs&&... actualArgs)
        -> decltype(this->pass_result(invoker, std::forward<Function>(function), type_traits::make_integral_sequence<std::size_t, Invoker::call_args_size()>{}, std::forward<ActualArgs>(actualArgs)...))
      {
        return pass_result(invoker, std::forward<Function>(function), type_traits::make_integral_sequence<std::size_t, Invoker::call_args_size()>{}, std::forward<ActualArgs>(actualArgs)...);
      }
    };

    template<typename Invoker, typename Argument, typename... ActualArgs>
    auto pass_arguments(const Invoker& invoker, Argument&& argument, ActualArgs&&... actualArgs)
      -> decltype(argument_passer<typename std::decay<Argument>::type>{}(invoker, std::forward<Argument>(argument), std::forward<ActualArgs>(actualArgs)...))
    {
      return argument_passer<typename std::decay<Argument>::type>{}(invoker, std::forward<Argument>(argument), std::forward<ActualArgs>(actualArgs)...);
    } 

    template<typename StoredArgs, typename CallArgs>
    class bind_invoker_base
    {
      using StoredArgsD = typename std::decay<StoredArgs>::type;
      using CallArgsD = typename std::decay<CallArgs>::type;
   
    public:

      static constexpr std::size_t stored_args_size()
      {
        return std::tuple_size<StoredArgsD>::value;
      }

      static constexpr std::size_t call_args_size()
      {
        return std::tuple_size<CallArgsD>::value;
      }

      bind_invoker_base(StoredArgs _storedArgs, CallArgs _callArgs)
        : storedArgs(std::forward<StoredArgs>(_storedArgs)),
          callArgs(std::forward<CallArgs>(_callArgs))
      {}

      StoredArgs stored_args() const
      {
        return std::forward<StoredArgs>(storedArgs);
      }

      CallArgs call_args() const
      {
        return std::forward<CallArgs>(callArgs);
      }

    private:
      StoredArgs storedArgs;
      CallArgs   callArgs;
    };

    template<std::size_t ReversePosition, typename StoredArgs, typename CallArgs>
    class bind_invoker :
      public bind_invoker_base<StoredArgs, CallArgs>
    {
      using base = bind_invoker_base<StoredArgs, CallArgs>; 
 
      static constexpr std::size_t current_position()
      {
        return base::stored_args_size() - ReversePosition;
      }

      auto current_arg() const
        -> decltype(std::get<current_position()>(this->stored_args()))
      {
        return std::get<current_position()>(this->stored_args());
      }
     
      bind_invoker<ReversePosition-1, StoredArgs, CallArgs> next_invoker() const
      {
        return {this->stored_args(), this->call_args()};
      }

    public:
      //using base::base;
      bind_invoker(StoredArgs _storedArgs, CallArgs _callArgs)
        : base(std::forward<StoredArgs>(_storedArgs), std::forward<CallArgs>(_callArgs))
      {}

      template<typename... ActualArgs>
      auto operator()(ActualArgs&&... actualArgs) const
        -> decltype(pass_arguments(this->next_invoker(), this->current_arg(), std::forward<ActualArgs>(actualArgs)...))
      {
        return pass_arguments(next_invoker(), current_arg(), std::forward<ActualArgs>(actualArgs)...);
      }
    };

    template<typename StoredArgs, typename CallArgs>
    class bind_invoker<0, StoredArgs, CallArgs> : 
      public bind_invoker_base<StoredArgs, CallArgs>
    {
      using base = bind_invoker_base<StoredArgs, CallArgs>; 

    public:
      //using base::base;
      bind_invoker(StoredArgs _storedArgs, CallArgs _callArgs)
        : base(std::forward<StoredArgs>(_storedArgs), std::forward<CallArgs>(_callArgs))
      {}

      template<typename... ActualArgs>
      auto operator()(ActualArgs&&... actualArgs) const
        -> decltype(INVOKE_MODEL::invoke(std::get<0>(this->stored_args()), std::forward<ActualArgs>(actualArgs)...))
      {
        return INVOKE_MODEL::invoke(std::get<0>(this->stored_args()), std::forward<ActualArgs>(actualArgs)...);
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

      template<typename... Args>
      auto operator()(Args&&... args) const
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
