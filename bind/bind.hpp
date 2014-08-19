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
    enum class argument_type : int
    {
      call = 0,
      stored = 1,
      ref = 2,
      bind = 3
    };

    constexpr int combine(int position, argument_type type)
    {
      return position * 4 + static_cast<int>(type);
    }

    constexpr int position(int combined)
    {
      return combined / 4;
    }

    constexpr argument_type argument(int combined)
    {
      return static_cast<argument_type>(combined % 4);
    }

    template<typename Arg, int Position, int CallArgsCount,
             bool = (is_placeholder<Arg>::value > 0),
             bool = std::is_bind_expression<Arg>::value>
    struct positions_for;

    template<typename Arg, int Position, int CallArgsCount>
    struct positions_for<Arg, Position, CallArgsCount, false, false>
    {
      using sequence = type_traits::integer_sequence<int, combine(Position, argument_type::stored)>;
    };

    template<typename T, int Position, int CallArgsCount>
    struct positions_for<std::reference_wrapper<T>, Position, CallArgsCount, false, false>
    {
      using sequence = type_traits::integer_sequence<int, combine(Position, argument_type::ref)>;
    };

    template<typename Bind, int Position, int CallArgsCount>
    struct positions_for<Bind, Position, CallArgsCount, false, true>
    {
      using sequence = type_traits::integer_sequence<int, combine(Position, argument_type::bind)>;
    };

    template<int... Positions>
    type_traits::integer_sequence<int, combine(Positions, argument_type::call)...> combine(type_traits::integer_sequence<int, Positions...>);

    template<typename Placeholder, int Position, int CallArgsCount>
    struct positions_for<Placeholder, Position, CallArgsCount, true, false>
    {
      using sequence = decltype(combine(parameter_indexes<Placeholder, CallArgsCount>{}));
    };

    template<int, typename, typename...>
    struct combine_positions;

    template<int CallArgsCount, int... Positions, typename... StoredArgs>
    struct combine_positions<CallArgsCount, type_traits::integer_sequence<int, Positions...>, StoredArgs...>
    {
      using sequence = type_traits::concat<typename positions_for<StoredArgs, Positions, CallArgsCount>::sequence...>;
    };

    template<int CallArgsCount, typename... StoredArgs>
    using positions = typename combine_positions<CallArgsCount,
                         type_traits::make_integer_sequence<int, sizeof...(StoredArgs)>,
                         StoredArgs...>::sequence;

    template<argument_type type, int Position>
    struct argument_resolver_impl;
  
    template<int Position>
    struct argument_resolver_impl<argument_type::call, Position>
    {
      template<typename StoredArgs, typename CallArgs>
      auto operator()(StoredArgs&&, CallArgs&& callArgs)
         -> decltype(std::get<Position-1>(std::forward<CallArgs>(callArgs)))
      {
        return std::get<Position-1>(std::forward<CallArgs>(callArgs));
      }
    };

    template<int Position>
    struct argument_resolver_impl<argument_type::stored, Position>
    {
      template<typename StoredArgs, typename CallArgs>
      auto operator()(StoredArgs&& storedArgs, CallArgs&&)
        -> decltype(std::get<Position>(std::forward<StoredArgs>(storedArgs)))
      {
        return std::get<Position>(std::forward<StoredArgs>(storedArgs));
      }
    };

    template<int Position>
    struct argument_resolver_impl<argument_type::ref, Position>
    {
      template<typename StoredArgs, typename CallArgs>
      auto operator()(StoredArgs&& storedArgs, CallArgs&&)
        -> decltype(std::get<Position>(std::forward<StoredArgs>(storedArgs)).get())
      {
        return std::get<Position>(std::forward<StoredArgs>(storedArgs)).get();
      }
    };

    template<int Position>
    struct argument_resolver_impl<argument_type::bind, Position>
    {
    private:
      template<typename F, int... Is, typename CallArgs>
      auto unpack(F&& f, type_traits::integer_sequence<int, Is...>, CallArgs&& callArgs)
        -> decltype(std::forward<F>(f)(std::get<Is>(std::forward<CallArgs>(callArgs))...))
      {
        return std::forward<F>(f)(std::get<Is>(std::forward<CallArgs>(callArgs))...);
      }

    public:
      template<typename StoredArgs, typename CallArgs>
      auto operator()(StoredArgs&& storedArgs, CallArgs&& callArgs)
        -> decltype(this->unpack(std::get<Position>(std::forward<StoredArgs>(storedArgs)), 
                                 type_traits::make_integer_sequence<int, std::tuple_size<typename std::decay<CallArgs>::type>::value>{},
                                 std::forward<CallArgs>(callArgs)))
      {
        return this->unpack(std::get<Position>(std::forward<StoredArgs>(storedArgs)), 
                            type_traits::make_integer_sequence<int, std::tuple_size<typename std::decay<CallArgs>::type>::value>{},
                            std::forward<CallArgs>(callArgs));
      }
    };

    template<int Combined>
    using argument_resolver = argument_resolver_impl<argument(Combined), position(Combined)>;

    template<int... Combined, typename Function, typename StoredArgs, typename CallArgs>
    inline auto bind_invoke(type_traits::integer_sequence<int, Combined...>, Function&& function, StoredArgs&& storedArgs, CallArgs&& callArgs)
      -> decltype(INVOKE_MODEL::invoke(std::forward<Function>(function), argument_resolver<Combined>{}(std::forward<StoredArgs>(storedArgs), std::forward<CallArgs>(callArgs))...))
    { 
      return INVOKE_MODEL::invoke(std::forward<Function>(function), argument_resolver<Combined>{}(std::forward<StoredArgs>(storedArgs), std::forward<CallArgs>(callArgs))...);
    }

    template<typename Function, typename... Args>
    class bind_functor
    {
      Function            function;
      std::tuple<Args...> storedArgs;

    public:
      template<typename F, typename... As>
      bind_functor(F&& f, As&&... as)
       : function(std::forward<F>(f))
       , storedArgs(std::forward<As>(as)...)
      {}

      template<typename... CallArgs>
      auto operator()(CallArgs&&... callArgs)
        -> decltype(bind_invoke(positions<sizeof...(CallArgs), Args...>{}, function, storedArgs, std::forward_as_tuple(std::forward<CallArgs>(callArgs)...)))
      {
        return bind_invoke(positions<sizeof...(CallArgs), Args...>{}, function, storedArgs, std::forward_as_tuple(std::forward<CallArgs>(callArgs)...));
      }

      template<typename... CallArgs>
      auto operator()(CallArgs&&... callArgs) const
        -> decltype(bind_invoke(positions<sizeof...(CallArgs), Args...>{}, function, storedArgs, std::forward_as_tuple(std::forward<CallArgs>(callArgs)...)))
      {
        return bind_invoke(positions<sizeof...(CallArgs), Args...>{}, function, storedArgs, std::forward_as_tuple(std::forward<CallArgs>(callArgs)...));
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
  template<typename Function, typename... Args>
  struct is_bind_expression<functional::detail::bind_functor<Function, Args...>>
    : true_type
  {};
}

#endif //BIND_HPP
