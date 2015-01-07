//        Copyright Tomasz Kamiński 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef INTEGER_SEQUENCE_HPP
#define INTEGER_SEQUENCE_HPP

#include <type_traits>

namespace type_traits
{
  template<typename T, T... values>
  struct integer_sequence 
  {
    using value_type = T;

    static constexpr std::size_t size()
    {
      return sizeof...(values);
    }
  };

  namespace detail
  {
    template<typename IntegralSequence, 
             typename... IntegralSequences>
    struct concat_impl
    {
      using type = typename concat_impl<
                     IntegralSequence,
                     typename concat_impl<IntegralSequences...>::type
                   >::type;
    };

    template<typename T1>
    struct concat_impl<T1>
    {
      using type = T1;
    };

    template<typename T1, typename T2>
    struct concat_impl<T1, T2>;

    template<typename T, T... values1, T... values2>
    struct concat_impl<integer_sequence<T, values1...>,
                       integer_sequence<T, values2...>>
    {
      using type = integer_sequence<T, values1..., values2...>; 
    };
  }

  template<typename... IntegralSequences>
  using concat = typename detail::concat_impl<IntegralSequences...>::type;
 
  namespace detail
  {
    template<typename Sequence, typename Sequence::value_type v>
    struct shift_impl;

    template<typename T, T... values, T v>
    struct shift_impl<integer_sequence<T, values...>, v>
    {
      using type = integer_sequence<T, values + v...>;
    };
  }

  template<typename Sequence, typename Sequence::value_type v>
  using shift = typename detail::shift_impl<Sequence, v>::type; 
 
  namespace detail
  {
    template<typename T, T end, bool even = (end % 2 == 0), bool zero = (end / 2 == 0)>
    struct make_integer_sequence_impl;

    template<typename T, T end /*= 0*/>
    struct make_integer_sequence_impl<T, end, true, true>
    {
      using type = integer_sequence<T>;
    };

    template<typename T, T end /* = 1*/>
    struct make_integer_sequence_impl<T, end, false, true>
    {
      using type = integer_sequence<T, 0>;
    };

    template<typename T, T end>
    struct make_integer_sequence_impl<T, end, true, false>
    {
      static constexpr T middle = end / 2;
      using left_part = typename make_integer_sequence_impl<T, middle>::type;
      using rigth_part = shift<left_part, middle>;
      using type = concat<left_part, rigth_part>;
    };

    template<typename T, T end>
    struct make_integer_sequence_impl<T, end, false, false>
    {
      static constexpr T middle = end / 2;
      using left_part = typename make_integer_sequence_impl<T, middle>::type;
      using rigth_part = shift<left_part, middle+1>;
      using type = concat<left_part, 
                          integer_sequence<T, middle>,
                          rigth_part>;
    };
  } 
  template<typename T, T end>
  using make_integer_sequence = typename detail::make_integer_sequence_impl<T, end>::type;

  template<typename T, T begin, T end>
  using make_integer_range = shift<make_integer_sequence<T, end-begin>, begin>;
}

#endif //INTEGER_SEQUENCE_HPP
