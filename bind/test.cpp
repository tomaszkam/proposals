#include <iostream>
#include "bind.hpp"
#include "placeholders.hpp"

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

int main() 
{
  using namespace functional::placeholders;
  functional::bind(printer{}, _all)(10);
  functional::bind(printer{}, _all)(10, 20, 30, 40);
  functional::bind(printer{}, _from<1>())(10, 20, 30, 40);
  functional::bind(printer{}, _from<2>())(10, 20, 30, 40);
  functional::bind(printer{}, _from<3>())(10, 20, 30, 40);
  functional::bind(printer{}, _from<4>())(10, 20, 30, 40);
  functional::bind(printer{}, _from<5>())(10, 20, 30, 40);
};
