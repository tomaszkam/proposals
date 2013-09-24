#include <iostream>
#include "bind.hpp"
#include "placeholders.hpp"
#include "additional_placeholders.hpp"

struct printer
{
  void operator()()
  {
    std::cout << std::endl;
  }

  template<typename T, typename... Ts>
  void operator()(const T& t, const Ts&... ts)
  {
    std::cout << t << " " << std::flush;
    operator()(ts...);
  }
};

struct functor
{
  template<typename... Ts>
  int operator()(const Ts&... ts)
  {
    std::cout << "call functor" << std::endl;
    printer{}(ts...);
    return sizeof...(Ts);
  }
};

struct forward_tester
{
  void operator()(int&)
  {
    std::cout << "called operator()(int&)" << std::endl;
  }

  void operator()(const int&)
  {
    std::cout << "called operator()(const int&)" << std::endl;
  }

  void operator()(int&&)
  {
    std::cout << "called operator()(int&&)" << std::endl;
  }

  void operator()(int&) const
  {
    std::cout << "called operator()(int&) const" << std::endl;
  }

  void operator()(const int&) const
  {
    std::cout << "called operator()(const int&) const" << std::endl;
  }
};

void inc(int& i) { ++i; }

int main() 
{
  using namespace functional::placeholders;

  std::cout << std::endl << "Placeholders tests: " << std::endl;
  functional::bind(printer{}, _all)(10);
  functional::bind(printer{}, _all)(10, 20, 30, 40);
  functional::bind(printer{}, _from<1>())(10, 20, 30, 40);
  functional::bind(printer{}, _from<2>())(10, 20, 30, 40);
  functional::bind(printer{}, _from<3>())(10, 20, 30, 40);
  functional::bind(printer{}, _from<4>())(10, 20, 30, 40);
  functional::bind(printer{}, _from<5>())(10, 20, 30, 40);
  functional::bind(printer{}, 1, 2, _all, _4, _3, _from<2>())(10, 20, 30, 40);

  std::cout << std::endl << "Reference test:" << std::endl;
  int a = 0;
  std::cout << "before: " << a << std::endl;
  functional::bind(&inc, std::ref(a))();
  std::cout << "after: " << a << std::endl;

  std::cout << std::endl << "Forwarding test:" << std::endl;
  auto forwardFunctor = functional::bind(forward_tester{}, _1);
  int i = 0;
  forwardFunctor(i);
  const int ci = 0;
  forwardFunctor(ci);
  forwardFunctor(1);

  std::cout << std::endl << "Bind test: " << std::endl;
  functional::bind(printer{}, std::bind(functor{}, _1, _2), functional::bind(functor{}, _from<3>{}))(10, 20, 30, 40);

  std::cout << std::endl << "Test const functor" << std::endl;
  auto normalFunctor = functional::bind(forward_tester{}, 1);
  normalFunctor();
  const auto& constFunctor = normalFunctor;
  constFunctor();


  std::cout << std::endl << "Additional (not proposed) placeholders tests: " << std::endl;
  functional::bind(printer{}, _between<2,3>())(10, 20, 30, 40);
}
