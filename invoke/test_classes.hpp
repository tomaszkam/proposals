#ifndef TEST_CLASSES_HPP
#define TEST_CLASSES_HPP

#include <stdexcept>

namespace test_classes
{
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
}

#endif //TEST_CLASSES_HPP
