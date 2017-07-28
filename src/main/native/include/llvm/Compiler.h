//===-- llvm/Support/Compiler.h - Compiler abstraction support --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines several macros, based on the current compiler.  This allows
// use of compiler-specific features in a way that remains portable.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_COMPILER_H
#define LLVM_SUPPORT_COMPILER_H

#ifndef __has_feature
# define __has_feature(x) 0
#endif

#ifndef __has_extension
# define __has_extension(x) 0
#endif

#ifndef __has_attribute
# define __has_attribute(x) 0
#endif

#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

/// \macro LLVM_GNUC_PREREQ
/// \brief Extend the default __GNUC_PREREQ even if glibc's features.h isn't
/// available.
#ifndef LLVM_GNUC_PREREQ
# if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#  define LLVM_GNUC_PREREQ(maj, min, patch) \
    ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) + __GNUC_PATCHLEVEL__ >= \
     ((maj) << 20) + ((min) << 10) + (patch))
# elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define LLVM_GNUC_PREREQ(maj, min, patch) \
    ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) >= ((maj) << 20) + ((min) << 10))
# else
#  define LLVM_GNUC_PREREQ(maj, min, patch) 0
# endif
#endif

#ifndef LLVM_CONSTEXPR
# ifdef _MSC_VER
#  if _MSC_VER >= 1900
#   define LLVM_CONSTEXPR constexpr
#  else
#   define LLVM_CONSTEXPR
#  endif
# elif defined(__has_feature)
#  if __has_feature(cxx_constexpr)
#   define LLVM_CONSTEXPR constexpr
#  else
#   define LLVM_CONSTEXPR
#  endif
# elif defined(__GXX_EXPERIMENTAL_CXX0X__)
#  define LLVM_CONSTEXPR constexpr
# elif defined(__has_constexpr)
#  define LLVM_CONSTEXPR constexpr
# else
#  define LLVM_CONSTEXPR
# endif
#endif

#ifndef LLVM_ATTRIBUTE_UNUSED_RESULT
#if __has_attribute(warn_unused_result) || LLVM_GNUC_PREREQ(3, 4, 0)
#define LLVM_ATTRIBUTE_UNUSED_RESULT __attribute__((__warn_unused_result__))
#elif defined(_MSC_VER)
#define LLVM_ATTRIBUTE_UNUSED_RESULT _Check_return_
#else
#define LLVM_ATTRIBUTE_UNUSED_RESULT
#endif
#endif

#ifndef LLVM_UNLIKELY
#if __has_builtin(__builtin_expect) || LLVM_GNUC_PREREQ(4, 0, 0)
#define LLVM_LIKELY(EXPR) __builtin_expect((bool)(EXPR), true)
#define LLVM_UNLIKELY(EXPR) __builtin_expect((bool)(EXPR), false)
#else
#define LLVM_LIKELY(EXPR) (EXPR)
#define LLVM_UNLIKELY(EXPR) (EXPR)
#endif
#endif

#endif
