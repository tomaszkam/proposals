//        Copyright Tomasz Kamiński 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "type_traits.hpp"
#include "test_classes.hpp"

using namespace test_classes;

static_assert(!type_traits::is_target_reference<Base&, int Class::*>::value, "checking Base");
static_assert(!type_traits::is_target_reference<Pointer&, int Class::*>::value, "checking Pointer");
static_assert(!type_traits::is_target_reference<ValueConversion&, int Class::*>::value, "checking Pointer");
static_assert(!type_traits::is_target_reference<ReferenceConversion&, int Class::*>::value, "checking Pointer");
static_assert(!type_traits::is_target_reference<Mixed&, int Class::*>::value, "checking Mixed");

static_assert(type_traits::is_target_reference<Class&, int Class::*>::value, "checking Class&");
static_assert(type_traits::is_target_reference<Class const&, int Class::*>::value, "checking Class const&");
static_assert(type_traits::is_target_reference<Class&&, int Class::*>::value, "checking Class&");
static_assert(type_traits::is_target_reference<Class const&&, int Class::*>::value, "checking Class const&&");

static_assert(type_traits::is_target_reference<Derived&, int Derived::*>::value, "checking Derived&");
static_assert(type_traits::is_target_reference<Derived const&, int Derived::*>::value, "checking Derived const&");
static_assert(type_traits::is_target_reference<Derived&&, int Derived::*>::value, "checking Derived&");
static_assert(type_traits::is_target_reference<Derived const&&, int Derived::*>::value, "checking Derived const&&");

Base base;
Class clazz;
Derived derived;
Class* rawPointer;
Pointer smartPointer;
ValueConversion valueConversion;
ReferenceConversion referenceConversion;
Mixed mixed;
ConstMixed constMixed;

static_assert(!type_traits::is_pointer_compatible_with_member_pointer<decltype(base), int Class::*>::value, "checking Base");
static_assert(!type_traits::is_pointer_compatible_with_member_pointer<decltype(clazz), int Class::*>::value, "checking Class");
static_assert(!type_traits::is_pointer_compatible_with_member_pointer<decltype(derived), int Class::*>::value, "checking Derived");
static_assert(type_traits::is_pointer_compatible_with_member_pointer<decltype(rawPointer), int Class::*>::value, "checking RawPointer");
static_assert(type_traits::is_pointer_compatible_with_member_pointer<decltype(smartPointer), int Class::*>::value, "checking SmartPointer");
static_assert(!type_traits::is_pointer_compatible_with_member_pointer<decltype(valueConversion), int Class::*>::value, "checking ValueConversion");
static_assert(!type_traits::is_pointer_compatible_with_member_pointer<decltype(referenceConversion), int Class::*>::value, "checking ReferenceConversion");
static_assert(type_traits::is_pointer_compatible_with_member_pointer<decltype(mixed), int Class::*>::value, "checking Mixed");
static_assert(type_traits::is_pointer_compatible_with_member_pointer<decltype(constMixed), int Class::*>::value, "checking ConstMixed");

static_assert(!type_traits::is_wrapper_compatible_with_member_pointer<decltype(base), int Class::*>::value, "checking Base");
static_assert(type_traits::is_wrapper_compatible_with_member_pointer<decltype(clazz), int Class::*>::value, "checking Class");
static_assert(type_traits::is_wrapper_compatible_with_member_pointer<decltype(derived), int Class::*>::value, "checking Derived");
static_assert(!type_traits::is_wrapper_compatible_with_member_pointer<decltype(rawPointer), int Class::*>::value, "checking RawPointer");
static_assert(!type_traits::is_wrapper_compatible_with_member_pointer<decltype(smartPointer), int Class::*>::value, "checking SmartPointer");
static_assert(type_traits::is_wrapper_compatible_with_member_pointer<decltype(valueConversion), int Class::*>::value, "checking ValueConversion");
static_assert(type_traits::is_wrapper_compatible_with_member_pointer<decltype(referenceConversion), int Class::*>::value, "checking ReferenceConversion");
static_assert(type_traits::is_wrapper_compatible_with_member_pointer<decltype(mixed), int Class::*>::value, "checking Mixed");
static_assert(type_traits::is_wrapper_compatible_with_member_pointer<decltype(constMixed), int Class::*>::value, "checking ConstMixed");

int main()
{}
