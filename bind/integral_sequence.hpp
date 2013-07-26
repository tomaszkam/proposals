#ifndef INTEGRAL_SEQUENCE_HPP
#define INTEGRAL_SEQUENCE_HPP

#include <type_traits>

namespace type_traits
{
  template<typename T, T... values>
  struct integral_sequence {};

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

    template<typename T1, typename T2>
    struct concat_impl<T1, T2>;

    template<typename T, T... values1, T... values2>
    struct concat_impl<integral_sequence<T, values1...>,
                       integral_sequence<T, values2...>>
    {
      using type = integral_sequence<T, values1..., values2...>; 
    };
  }

  template<typename... IntegralSequences>
  using concat = typename detail::concat_impl<IntegralSequences...>::type;
  
  namespace detail
  {
    template<typename T, T begin, T end, bool = (end - begin == 1)>
    struct make_integral_range_impl
    {
      static constexpr T middle = (begin + end)/2;
      using type = concat<typename make_integral_range_impl<T, begin, middle>::type,
                          typename make_integral_range_impl<T, middle, end>::type>;
    };

    template<typename T, T begin, T end>
    struct make_integral_range_impl<T, begin, end, true>
    {
      using type = integral_sequence<T, begin>;
    };

    template<typename T, T value>
    struct make_integral_range_impl<T, value, value, false>
    {
      using type = integral_sequence<T>;
    };
  } 

  template<typename T, T begin, T end>
  using make_integral_range = typename detail::make_integral_range_impl<T, begin, end>::type;

  template<typename T, T end>
  using make_integral_sequence = make_integral_range<T, 0, end>;
}

#endif //INTEGRAL_SEQUENCE_HPP
