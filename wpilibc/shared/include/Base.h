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

// [[deprecated(msg)]] is a C++14 feature not supported by MSVC or GCC < 4.9.
// We provide an equivalent warning implementation for those compilers here.
#if defined(_MSC_VER)
  #define DEPRECATED(msg) __declspec(deprecated(msg))
#elif defined(__GNUC__)
  #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 8)
    #define DEPRECATED(msg) [[deprecated(msg)]]
  #else
    #define DEPRECATED(msg) __attribute__((deprecated(msg)))
  #endif
#elif __cplusplus > 201103L
  #define DEPRECATED(msg) [[deprecated(msg)]]
#else
  #define DEPRECATED(msg) /*nothing*/
#endif

// Provide std::decay_t when using GCC < 4.9
#if defined(__GNUC__)
  #if __GNUC__ == 4 && __GNUC_MINOR__ < 9
    #include <type_traits>
    namespace std {
    template <class T> using decay_t = typename decay<T>::type;
    }
  #endif
#endif

// A struct to use as a deleter when a std::shared_ptr must wrap a raw pointer
// that is being deleted by someone else.
template<class T>
struct
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

// Define make_unique for C++11-only compilers
#if __cplusplus == 201103L
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
namespace std {
template <class T>
struct _Unique_if {
  typedef unique_ptr<T> _Single_object;
};

template <class T>
struct _Unique_if<T[]> {
  typedef unique_ptr<T[]> _Unknown_bound;
};

template <class T, size_t N>
struct _Unique_if<T[N]> {
  typedef void _Known_bound;
};

template <class T, class... Args>
typename _Unique_if<T>::_Single_object make_unique(Args &&... args) {
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <class T>
typename _Unique_if<T>::_Unknown_bound make_unique(size_t n) {
  typedef typename remove_extent<T>::type U;
  return unique_ptr<T>(new U[n]());
}

template <class T, class... Args>
typename _Unique_if<T>::_Known_bound make_unique(Args &&...) = delete;
}  // namespace std
#endif
