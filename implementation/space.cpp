#include <utility>
#include <cassert>

enum class cmp_result : int
{
    less = -2,
    neg_unordered = -1,
    equal = 0,
    non_equal = 1,
    pos_unordered = 1,
    greater = 2
};

class weak_equality;
class strong_equality;
class partial_ordering;
class weak_ordering;
class strong_ordering;

class weak_equality
{
  int value;

  explicit constexpr weak_equality(cmp_result v) noexcept : value(int(v)) {}
  explicit constexpr weak_equality(int v) noexcept : value(v) {}

public:
  static const weak_equality equivalent;
  static const weak_equality nonequivalent;

  friend constexpr bool operator==(weak_equality v, std::nullptr_t) { return v.value == 0; }
  friend constexpr bool operator!=(weak_equality v, std::nullptr_t) { return v.value != 0; }

  friend constexpr bool operator==(std::nullptr_t, weak_equality v) { return v.value == 0; }
  friend constexpr bool operator!=(std::nullptr_t, weak_equality v) { return v.value != 0; }

  friend constexpr weak_equality operator<=>(weak_equality v, std::nullptr_t) { return v; }
  friend constexpr weak_equality operator<=>(std::nullptr_t, weak_equality v) { return v; }

  friend class strong_equality;
  friend class partial_ordering;
  friend class weak_ordering;
  friend class strong_ordering;
};

inline constexpr weak_equality weak_equality::equivalent(cmp_result::equal);
inline constexpr weak_equality weak_equality::nonequivalent(cmp_result::non_equal);

class strong_equality
{
  int value;

  explicit constexpr strong_equality(cmp_result v) noexcept : value(int(v)) {}
  explicit constexpr strong_equality(int v) noexcept : value(v) {}

public:
  static const strong_equality equal;
  static const strong_equality nonequal;
  static const strong_equality equivalent;
  static const strong_equality nonequivalent;

  constexpr operator weak_equality() const noexcept { return weak_equality(value); }

  friend constexpr bool operator==(strong_equality v, std::nullptr_t) { return v.value == 0; }
  friend constexpr bool operator!=(strong_equality v, std::nullptr_t) { return v.value != 0; }

  friend constexpr bool operator==(std::nullptr_t, strong_equality v) { return v.value == 0; }
  friend constexpr bool operator!=(std::nullptr_t, strong_equality v) { return v.value != 0; }

  //operator<=>
  friend constexpr strong_equality operator<=>(strong_equality v, std::nullptr_t) { return v; }
  friend constexpr strong_equality operator<=>(std::nullptr_t, strong_equality v) { return v; }

  friend class strong_ordering;
};

inline constexpr strong_equality strong_equality::equal(cmp_result::equal);
inline constexpr strong_equality strong_equality::nonequal(cmp_result::non_equal);
inline constexpr strong_equality strong_equality::equivalent(cmp_result::equal);
inline constexpr strong_equality strong_equality::nonequivalent(cmp_result::non_equal);

class partial_ordering
{
  int value;

  explicit constexpr partial_ordering(cmp_result v) noexcept : value(int(v)) {}
  explicit constexpr partial_ordering(int v) noexcept : value(v) {}

public:
  static const partial_ordering less;
  static const partial_ordering equivalent;
  static const partial_ordering greater;
  static const partial_ordering unordered;

  constexpr operator weak_equality() const noexcept { return weak_equality(value); }

  friend constexpr bool operator==(partial_ordering v, std::nullptr_t) { return v.value == 0; }
  friend constexpr bool operator!=(partial_ordering v, std::nullptr_t) { return v.value != 0; }
  friend constexpr bool operator< (partial_ordering v, std::nullptr_t) { return v.value < -1; }
  friend constexpr bool operator<=(partial_ordering v, std::nullptr_t) { return v.value < -1 || v.value == 0; }
  friend constexpr bool operator> (partial_ordering v, std::nullptr_t) { return v.value >  1; }
  friend constexpr bool operator>=(partial_ordering v, std::nullptr_t) { return v.value >  1 || v.value == 0; }

  friend constexpr bool operator==(std::nullptr_t, partial_ordering v) { return v.value == 0; }
  friend constexpr bool operator!=(std::nullptr_t, partial_ordering v) { return v.value != 0; }
  friend constexpr bool operator< (std::nullptr_t, partial_ordering v) { return v.value >  1; }
  friend constexpr bool operator<=(std::nullptr_t, partial_ordering v) { return v.value >  1 || v.value == 0; }
  friend constexpr bool operator> (std::nullptr_t, partial_ordering v) { return v.value < -1; }
  friend constexpr bool operator>=(std::nullptr_t, partial_ordering v) { return v.value < -1 || v.value == 0; }

  friend constexpr partial_ordering operator<=>(partial_ordering v, std::nullptr_t) { return v; }
  friend constexpr partial_ordering operator<=>(std::nullptr_t, partial_ordering v) { return partial_ordering(-v.value); }

  friend class weak_ordering;
  friend class strong_ordering;
};

inline constexpr partial_ordering partial_ordering::less(cmp_result::less);
inline constexpr partial_ordering partial_ordering::equivalent(cmp_result::equal);
inline constexpr partial_ordering partial_ordering::greater(cmp_result::greater);
inline constexpr partial_ordering partial_ordering::unordered(cmp_result::pos_unordered);

class weak_ordering
{
  int value;

  explicit constexpr weak_ordering(cmp_result v) noexcept : value(int(v)) {}
  explicit constexpr weak_ordering(int v) noexcept : value(v) {}

public:
  static const weak_ordering less;
  static const weak_ordering equivalent;
  static const weak_ordering greater;

  constexpr operator weak_equality() const noexcept { return weak_equality(value); }
  constexpr operator partial_ordering() const noexcept { return partial_ordering(value); }

  friend constexpr bool operator==(weak_ordering v, std::nullptr_t) { return v.value == 0; }
  friend constexpr bool operator!=(weak_ordering v, std::nullptr_t) { return v.value != 0; }
  friend constexpr bool operator< (weak_ordering v, std::nullptr_t) { return v.value <  0; }
  friend constexpr bool operator<=(weak_ordering v, std::nullptr_t) { return v.value <= 0; }
  friend constexpr bool operator> (weak_ordering v, std::nullptr_t) { return v.value >  0; }
  friend constexpr bool operator>=(weak_ordering v, std::nullptr_t) { return v.value >= 0; }

  friend constexpr bool operator==(std::nullptr_t, weak_ordering v) { return v.value == 0; }
  friend constexpr bool operator!=(std::nullptr_t, weak_ordering v) { return v.value != 0; }
  friend constexpr bool operator< (std::nullptr_t, weak_ordering v) { return v.value >  0; }
  friend constexpr bool operator<=(std::nullptr_t, weak_ordering v) { return v.value >= 0; }
  friend constexpr bool operator> (std::nullptr_t, weak_ordering v) { return v.value <  0; }
  friend constexpr bool operator>=(std::nullptr_t, weak_ordering v) { return v.value <= 0; }

  //operator<=>
  friend constexpr weak_ordering operator<=>(weak_ordering v, std::nullptr_t) { return v; }
  friend constexpr weak_ordering operator<=>(std::nullptr_t, weak_ordering v) { return weak_ordering(-v.value); }

  friend class strong_ordering;
};

inline constexpr weak_ordering weak_ordering::less(cmp_result::less);
inline constexpr weak_ordering weak_ordering::equivalent(cmp_result::equal);
inline constexpr weak_ordering weak_ordering::greater(cmp_result::greater);

class strong_ordering
{
  int value;

  explicit constexpr strong_ordering(cmp_result v) noexcept : value(int(v)) {}
  explicit constexpr strong_ordering(int v) noexcept : value(v) {}

public:
  static const strong_ordering less;
  static const strong_ordering equal;
  static const strong_ordering equivalent;
  static const strong_ordering greater;

  constexpr operator weak_equality() const noexcept { return weak_equality(value); }
  constexpr operator strong_equality() const noexcept { return strong_equality(value); }
  constexpr operator partial_ordering() const noexcept { return partial_ordering(value); }
  constexpr operator weak_ordering() const noexcept { return weak_ordering(value); }

  friend constexpr bool operator==(strong_ordering v, std::nullptr_t) { return v.value == 0; }
  friend constexpr bool operator!=(strong_ordering v, std::nullptr_t) { return v.value != 0; }
  friend constexpr bool operator< (strong_ordering v, std::nullptr_t) { return v.value <  0; }
  friend constexpr bool operator<=(strong_ordering v, std::nullptr_t) { return v.value <= 0; }
  friend constexpr bool operator> (strong_ordering v, std::nullptr_t) { return v.value >  0; }
  friend constexpr bool operator>=(strong_ordering v, std::nullptr_t) { return v.value >= 0; }

  friend constexpr bool operator==(std::nullptr_t, strong_ordering v) { return v.value == 0; }
  friend constexpr bool operator!=(std::nullptr_t, strong_ordering v) { return v.value != 0; }
  friend constexpr bool operator< (std::nullptr_t, strong_ordering v) { return v.value >  0; }
  friend constexpr bool operator<=(std::nullptr_t, strong_ordering v) { return v.value >= 0; }
  friend constexpr bool operator> (std::nullptr_t, strong_ordering v) { return v.value <  0; }
  friend constexpr bool operator>=(std::nullptr_t, strong_ordering v) { return v.value <= 0; }

  friend constexpr strong_ordering operator<=>(strong_ordering v, std::nullptr_t) { return v; }
  friend constexpr strong_ordering operator<=>(std::nullptr_t, strong_ordering v) { return strong_ordering(-v.value); }
};

inline constexpr strong_ordering strong_ordering::less(cmp_result::less);
inline constexpr strong_ordering strong_ordering::equal(cmp_result::equal);
inline constexpr strong_ordering strong_ordering::equivalent(cmp_result::equal);
inline constexpr strong_ordering strong_ordering::greater(cmp_result::greater);

template<typename Type>
void base_eq(Type v)
{
    assert(v == 0);
    assert(!(v != 0));
}

template<typename Type>
void base_ne(Type v)
{
    assert(v != 0);
    assert(!(v == 0));
}

template<typename Type>
void base_ce(Type v) //comparision eq, checks also <, >, <=, >=
{
    base_eq(v);
    assert(v <= 0);
    assert(v >= 0);
    assert(!(v < 0));
    assert(!(v > 0));
}

template<typename Type>
void base_lt(Type v)
{
    base_ne(v);
    assert(v < 0);
    assert(!(v > 0));
    assert(v <= 0);
    assert(!(v >= 0));
}

template<typename Type>
void base_gt(Type v)
{
    base_ne(v);
    assert(v > 0);
    assert(!(v < 0));
    assert(v >= 0);
    assert(!(v <= 0));
}

template<typename Type>
void base_un(Type v)
{
    base_ne<Type>(v);
    assert(!(v < 0));
    assert(!(v > 0));
    assert(!(v <= 0));
    assert(!(v >= 0));
}

//This test also checks result of 0 <=> v and v <=> 0

template<typename Type>
void test_eq(Type v)
{
    base_eq(v);
    base_eq(v <=> 0);
    base_eq(0 <=> v);
}

template<typename Type>
void test_ne(Type v)
{
    base_ne(v);
    base_ne(v <=> 0);
    base_ne(0 <=> v);
}

template<typename Type>
void test_ce(Type v)
{
    base_ce(v);
    base_ce(v <=> 0);
    base_ce(0 <=> v);
}

template<typename Type>
void test_lt(Type v)
{
    base_lt(v);
    base_lt(v <=> 0);
    base_gt(0 <=> v);
}

template<typename Type>
void test_gt(Type v)
{
    base_gt(v);
    base_gt(v <=> 0);
    base_lt(0 <=> v);
}

template<typename Type>
void test_un(Type v)
{
    base_un(v);
    base_un(v <=> 0);
    base_un(0 <=> v);
}


int main()
{
    //weak_equality
    {
      test_eq(weak_equality::equivalent);
      test_ne(weak_equality::nonequivalent);
    }

    //strong_equality
    {
      test_eq(strong_equality::equal);
      test_ne(strong_equality::nonequal);
      test_eq(strong_equality::equivalent);
      test_ne(strong_equality::nonequivalent);

      //weak_equality conversion test
      test_eq<weak_equality>(strong_equality::equal);
      test_ne<weak_equality>(strong_equality::nonequal);
      test_eq<weak_equality>(strong_equality::equivalent);
      test_ne<weak_equality>(strong_equality::nonequivalent);
    }

    //partial_ordering
    {
       test_eq(partial_ordering::equivalent);
       test_lt(partial_ordering::less);
       test_gt(partial_ordering::greater);
       test_un(partial_ordering::unordered);

       //weak_equality conversion test
       test_eq<weak_equality>(partial_ordering::equivalent);
       test_ne<weak_equality>(partial_ordering::less);
       test_ne<weak_equality>(partial_ordering::greater);
       test_ne<weak_equality>(partial_ordering::unordered);
    }

    //weak_ordering
    {
       test_ce(weak_ordering::equivalent);
       test_lt(weak_ordering::less);
       test_gt(weak_ordering::greater);

       //weak_equality conversion test
       test_eq<weak_equality>(weak_ordering::equivalent);
       test_ne<weak_equality>(weak_ordering::less);
       test_ne<weak_equality>(weak_ordering::greater);

       //partial_ordering conversion test
       test_ce<partial_ordering>(weak_ordering::equivalent);
       test_lt<partial_ordering>(weak_ordering::less);
       test_gt<partial_ordering>(weak_ordering::greater);
    }

    //strong_ordering
    {
       test_ce(strong_ordering::equivalent);
       test_ce(strong_ordering::equal);
       test_lt(strong_ordering::less);
       test_gt(strong_ordering::greater);

       //weak_equality conversion test
       test_eq<weak_equality>(strong_ordering::equivalent);
       test_eq<weak_equality>(strong_ordering::equal);
       test_ne<weak_equality>(strong_ordering::less);
       test_ne<weak_equality>(strong_ordering::greater);

       //strong_equality conversion test
       test_eq<strong_equality>(strong_ordering::equivalent);
       test_eq<strong_equality>(strong_ordering::equal);
       test_ne<strong_equality>(strong_ordering::less);
       test_ne<strong_equality>(strong_ordering::greater);

       //partial_ordering conversion test
       test_ce<partial_ordering>(strong_ordering::equivalent);
       test_ce<partial_ordering>(strong_ordering::equal);
       test_lt<partial_ordering>(strong_ordering::less);
       test_gt<partial_ordering>(strong_ordering::greater);

       //weak_ordering conversion test
       test_ce<weak_ordering>(strong_ordering::equivalent);
       test_ce<weak_ordering>(strong_ordering::equal);
       test_lt<weak_ordering>(strong_ordering::less);
       test_gt<weak_ordering>(strong_ordering::greater);
    }
}
