/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

// MSVC 2013 doesn't allow "= default" on move constructors, but since we are
// (currently) only actually using the move constructors in non-MSVC situations
// (ie, wpilibC++Devices), we can just ignore it in MSVC.
#if !defined(_MSC_VER)
#define DEFAULT_MOVE_CONSTRUCTOR(ClassName)     \
ClassName(ClassName &&) = default
#else
#define DEFAULT_MOVE_CONSTRUCTOR(ClassName)
#endif

#if (__cplusplus < 201103L)
	#if !defined(_MSC_VER)
		#define nullptr NULL
	#endif
	#define constexpr const
#endif

#if defined(_MSC_VER)
  #define noexcept throw()
#endif

// A struct to use as a deleter when a std::shared_ptr must wrap a raw pointer
// that is being deleted by someone else.
// This should only be called in deprecated functions; using it anywhere else
// will throw warnings.
template<class T>
struct
#if !defined(_MSC_VER)
  [[deprecated]]
#else
  __declspec(deprecated)
#endif
NullDeleter {
  void operator()(T *) const noexcept {};
};

#include <atomic>
// Use this for determining whether the default move constructor has been
// called on a containing object. This serves the purpose of allowing us to
// use the default move constructor of an object for moving all the data around
// while being able to use this to, for instance, chose not to de-allocate
// a PWM port in a destructor.
struct HasBeenMoved {
  HasBeenMoved(HasBeenMoved&& other) {
    other.moved = true;
    moved = false;
  }
  HasBeenMoved() = default;
  std::atomic<bool> moved{false};
  operator bool() const { return moved; }
};
