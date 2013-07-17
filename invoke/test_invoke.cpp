#include "invoke_option2.hpp"
#include "invoke_option1.hpp"
#include "test_classes.hpp"

using namespace test_classes;

Base base;
Class clazz;
Derived derived;
Class* rawPointer;
Pointer smartPointer;
ValueConversion valueConversion;
ReferenceConversion referenceConversion;
Mixed mixed;

int main()
{
  using functional::option2::invoke;
 
  invoke(&Class::normal, Class{});
  //invoke(&Class::normal, base);
  invoke(&Class::normal, clazz);
  invoke(&Class::normal, derived);
  invoke(&Class::normal, rawPointer);
  invoke(&Class::normal, smartPointer);
  invoke(&Class::normal, valueConversion);
  invoke(&Class::normal, referenceConversion);
  invoke(&Class::normal, mixed);
 
  /* Wont work on gcc 4.8.1 and clang 3.2 because of bug with in implementation
     of is_member_function_pointer for ref qualified methods.
     See: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57388
          http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57825
  //invoke(&Class::reference, Class{});
  //invoke(&Class::reference, base);
  invoke(&Class::reference, clazz);
  invoke(&Class::reference, derived);
  invoke(&Class::reference, rawPointer);
  invoke(&Class::reference, smartPointer);
  invoke(&Class::reference, valueConversion);
  invoke(&Class::reference, referenceConversion);
  invoke(&Class::reference, mixed);*/

  invoke(&Class::member, Class{});
  //invoke(&Class::member, base);
  invoke(&Class::member, clazz);
  invoke(&Class::member, derived);
  invoke(&Class::member, rawPointer);
  invoke(&Class::member, smartPointer);
  invoke(&Class::member, valueConversion);
  invoke(&Class::member, referenceConversion);
  invoke(&Class::member, mixed);

  invoke([](int) {}, 1);
}
