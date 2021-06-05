//===- Format.h - Efficient printf-style formatting for streams -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the format() function, which can be used with other
// LLVM subsystems to provide printf-style formatting.  This gives all the power
// and risk of printf.  This can be used like this (with raw_ostreams as an
// example):
//
//    OS << "mynumber: " << format("%4.5f", 1234.412) << '\n';
//
// Or if you prefer:
//
//  OS << format("mynumber: %4.5f\n", 1234.412);
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_FORMAT_H
#define WPIUTIL_WPI_FORMAT_H

#include "wpi/ArrayRef.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <string_view>
#include <tuple>
#include <utility>

namespace wpi {

/// This is a helper class used for handling formatted output.  It is the
/// abstract base class of a templated derived class.
class format_object_base {
protected:
  const char *Fmt;
  ~format_object_base() = default; // Disallow polymorphic deletion.
  format_object_base(const format_object_base &) = default;
  virtual void home(); // Out of line virtual method.

  /// Call snprintf() for this object, on the given buffer and size.
  virtual int snprint(char *Buffer, unsigned BufferSize) const = 0;

public:
  format_object_base(const char *fmt) : Fmt(fmt) {}

  /// Format the object into the specified buffer.  On success, this returns
  /// the length of the formatted string.  If the buffer is too small, this
  /// returns a length to retry with, which will be larger than BufferSize.
  unsigned print(char *Buffer, unsigned BufferSize) const {
    assert(BufferSize && "Invalid buffer size!");

    // Print the string, leaving room for the terminating null.
    int N = snprint(Buffer, BufferSize);

    // VC++ and old GlibC return negative on overflow, just double the size.
    if (N < 0)
      return BufferSize * 2;

    // Other implementations yield number of bytes needed, not including the
    // final '\0'.
    if (unsigned(N) >= BufferSize)
      return N + 1;

    // Otherwise N is the length of output (not including the final '\0').
    return N;
  }
};

/// These are templated helper classes used by the format function that
/// capture the object to be formatted and the format string. When actually
/// printed, this synthesizes the string into a temporary buffer provided and
/// returns whether or not it is big enough.

// Helper to validate that format() parameters are scalars or pointers.
template <typename... Args> struct validate_format_parameters;
template <typename Arg, typename... Args>
struct validate_format_parameters<Arg, Args...> {
  static_assert(std::is_scalar<Arg>::value,
                "format can't be used with non fundamental / non pointer type");
  validate_format_parameters() { validate_format_parameters<Args...>(); }
};
template <> struct validate_format_parameters<> {};

template <typename... Ts>
class format_object final : public format_object_base {
  std::tuple<Ts...> Vals;

  template <std::size_t... Is>
  int snprint_tuple(char *Buffer, unsigned BufferSize,
                    std::index_sequence<Is...>) const {
#ifdef _MSC_VER
    return _snprintf(Buffer, BufferSize, Fmt, std::get<Is>(Vals)...);
#else
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
    return snprintf(Buffer, BufferSize, Fmt, std::get<Is>(Vals)...);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif
  }

public:
  format_object(const char *fmt, const Ts &... vals)
      : format_object_base(fmt), Vals(vals...) {
    validate_format_parameters<Ts...>();
  }

  int snprint(char *Buffer, unsigned BufferSize) const override {
    return snprint_tuple(Buffer, BufferSize, std::index_sequence_for<Ts...>());
  }
};

/// These are helper functions used to produce formatted output.  They use
/// template type deduction to construct the appropriate instance of the
/// format_object class to simplify their construction.
///
/// This is typically used like:
/// \code
///   OS << format("%0.4f", myfloat) << '\n';
/// \endcode

template <typename... Ts>
inline format_object<Ts...> format(const char *Fmt, const Ts &... Vals) {
  return format_object<Ts...>(Fmt, Vals...);
}

} // end namespace wpi

#endif
