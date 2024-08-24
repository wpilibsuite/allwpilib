// Copyright (C) 2015-2023 Jonathan Müller and foonathan/memory contributors
// SPDX-License-Identifier: Zlib

/// \file
/// Configuration macros.

#ifndef WPI_MEMORY_CONFIG_HPP_INCLUDED
#define WPI_MEMORY_CONFIG_HPP_INCLUDED

#include <cstddef>

#if !defined(DOXYGEN)
#define WPI_MEMORY_IMPL_IN_CONFIG_HPP
#include "config_impl.hpp"
#undef WPI_MEMORY_IMPL_IN_CONFIG_HPP
#endif

// exception support
#ifndef WPI_HAS_EXCEPTION_SUPPORT
#if defined(__GNUC__) && !defined(__EXCEPTIONS)
#define WPI_HAS_EXCEPTION_SUPPORT 0
#elif defined(_MSC_VER) && !_HAS_EXCEPTIONS
#define WPI_HAS_EXCEPTION_SUPPORT 0
#else
#define WPI_HAS_EXCEPTION_SUPPORT 1
#endif
#endif

#if WPI_HAS_EXCEPTION_SUPPORT
#define WPI_THROW(Ex) throw(Ex)
#else
#include <cstdlib>
#define WPI_THROW(Ex) ((Ex), std::abort())
#endif

// hosted implementation
#ifndef WPI_HOSTED_IMPLEMENTATION
#if !_MSC_VER && !__STDC_HOSTED__
#define WPI_HOSTED_IMPLEMENTATION 0
#else
#define WPI_HOSTED_IMPLEMENTATION 1
#endif
#endif

// log prefix
#define WPI_MEMORY_LOG_PREFIX "wpi::memory"

// version
#define WPI_MEMORY_VERSION                                                                   \
    (WPI_MEMORY_VERSION_MAJOR * 100 + WPI_MEMORY_VERSION_MINOR)

// use this macro to mark implementation-defined types
// gives it more semantics and useful with doxygen
// add PREDEFINED: WPI_IMPL_DEFINED():=implementation_defined
#ifndef WPI_IMPL_DEFINED
#define WPI_IMPL_DEFINED(...) __VA_ARGS__
#endif

// use this macro to mark base class which only purpose is EBO
// gives it more semantics and useful with doxygen
// add PREDEFINED: WPI_EBO():=
#ifndef WPI_EBO
#define WPI_EBO(...) __VA_ARGS__
#endif

#ifndef WPI_ALIAS_TEMPLATE
// defines a template alias
// usage:
// template <typename T>
// WPI_ALIAS_TEMPLATE(bar, foo<T, int>);
// useful for doxygen
#ifdef DOXYGEN
#define WPI_ALIAS_TEMPLATE(Name, ...)                                                        \
    class Name : public __VA_ARGS__                                                                \
    {                                                                                              \
    }
#else
#define WPI_ALIAS_TEMPLATE(Name, ...) using Name = __VA_ARGS__
#endif
#endif

#ifdef DOXYGEN
// dummy definitions of config macros for doxygen

/// The major version number.
/// \ingroup memory_core
#define WPI_MEMORY_VERSION_MAJOR 1

/// The minor version number.
/// \ingroup memory_core
#define WPI_MEMORY_VERSION_MINOR 1

/// The total version number of the form \c Mmm.
/// \ingroup memory_core
#define WPI_MEMORY_VERSION                                                                   \
    (WPI_MEMORY_VERSION_MAJOR * 100 + WPI_MEMORY_VERSION_MINOR)

/// Whether or not the allocation size will be checked,
/// i.e. the \ref wpi::memory::bad_allocation_size thrown.
/// \ingroup memory_core
#define WPI_MEMORY_CHECK_ALLOCATION_SIZE 1

/// Whether or not internal assertions in the library are enabled.
/// \ingroup memory_core
#define WPI_MEMORY_DEBUG_ASSERT 1

/// Whether or not allocated memory will be filled with special values.
/// \ingroup memory_core
#define WPI_MEMORY_DEBUG_FILL 1

/// The size of the fence memory, it has no effect if \ref WPI_MEMORY_DEBUG_FILL is \c false.
/// \note For most allocators, the actual value doesn't matter and they use appropriate defaults to ensure alignment etc.
/// \ingroup memory_core
#define WPI_MEMORY_DEBUG_FENCE 1

/// Whether or not leak checking is enabled.
/// \ingroup memory_core
#define WPI_MEMORY_DEBUG_LEAK_CHECK 1

/// Whether or not the deallocation functions will check for pointers that were never allocated by an allocator.
/// \ingroup memory_core
#define WPI_MEMORY_DEBUG_POINTER_CHECK 1

/// Whether or not the deallocation functions will check for double free errors.
/// This option makes no sense if \ref WPI_MEMORY_DEBUG_POINTER_CHECK is \c false.
/// \ingroup memory_core
#define WPI_MEMORY_DEBUG_DOUBLE_DEALLOC_CHECK 1

/// Whether or not everything is in namespace <tt>wpi::memory</tt>.
/// If \c false, a namespace alias <tt>namespace memory = wpi::memory</tt> is automatically inserted into each header,
/// allowing to qualify everything with <tt>wpi::</tt>.
/// \note This option breaks in combination with using <tt>using namespace wpi;</tt>.
/// \ingroup memory_core
#define WPI_MEMORY_NAMESPACE_PREFIX 1

/// The mode of the automatic \ref wpi::memory::temporary_stack creation.
/// Set to `2` to enable automatic lifetime management of the per-thread stack through nifty counter.
/// Then all memory will be freed upon program termination automatically.
/// Set to `1` to disable automatic lifetime managment of the per-thread stack,
/// requires managing it through the \ref wpi::memory::temporary_stack_initializer.
/// Set to `0` to disable the per-thread stack completely.
/// \ref wpi::memory::get_temporary_stack() will abort the program upon call.
/// \ingroup memory_allocator
#define WPI_MEMORY_TEMPORARY_STACK_MODE 2
#endif

#endif // WPI_MEMORY_CONFIG_HPP_INCLUDED
