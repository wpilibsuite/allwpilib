// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>
#include <utility>

#include "wpi/util/SymbolExports.hpp"

namespace wpi {

class TelemetryTable;

/**
 * Specifies that a type is capable of telemetry logging to an individual value.
 *
 * Implementations must define a non-member function (can be via ADL lookup) for
 * logging to a single name in a TelemetryTable with the following signature (as
 * enforced by this concept):
 * - void LogValueTo(TelemetryTable& table, std::string_view name, T&& value):
 *   function for logging
 */
template <typename T, typename... I>
concept SupportsTelemetryValue = requires(
    TelemetryTable& table, std::string_view name, T&& value, I... info) {
  { LogValueTo(table, name, std::forward<T>(value), info...) };
};

/**
 * Specifies that a type is capable of telemetry logging to a table.
 *
 * Implementations must define a non-member function (can be via ADL lookup) for
 * logging to a TelemetryTable with the following signature (as enforced by this
 * concept):
 * - void LogTo(TelemetryTable& table, T&& value): function for logging
 */
template <typename T, typename... I>
concept SupportsTelemetry =
    requires(TelemetryTable& table, T&& value, I... info) {
      { LogTo(table, std::forward<T>(value), info...) };
    };

/**
 * Specifies that a type is capable of telemetry logging with a specified type.
 *
 * Implementations must define two non-member functions (can be via ADL lookup)
 * for logging to a TelemetryTable with the following signatures (as enforced by
 * this concept):
 * - std::string_view GetTelemetryTypeName(T&& value): function that returns the
 *   type name
 * - void LogTo(TelemetryTable& table, T&& value): function for logging
 *
 * GetTelemetryTypeName() may return types other than std::string_view, as long
 * as the return value is convertible to std::string_view.
 */
template <typename T, typename... I>
concept SupportsTelemetryWithTypeName =
    SupportsTelemetry<T, I...> && requires(T&& value, I... info) {
      {
        GetTelemetryTypeName(std::forward<T>(value), info...)
      } -> std::convertible_to<std::string_view>;
    };

/**
 * A base interface for complex telemetry objects. Should be used instead of
 * standalone functions when defining telemetry support for non-final classes.
 */
class WPILIB_DLLEXPORT TelemetryLoggable {
  void anchor();  // provide a place for the vtable

 public:
  virtual ~TelemetryLoggable() = default;

  /**
   * Logs the object to a {@link TelemetryTable}.
   *
   * @param table telemetry table
   */
  virtual void LogTo(TelemetryTable& table) const = 0;

  /**
   * Gets the telemetry table type. Default is no specified table type (empty).
   * A mismatch in type during logging results in a reported warning, so this
   * should always return the same value.
   *
   * @return Table type
   */
  virtual std::string_view GetTelemetryType() const { return {}; }
};

template <typename T>
  requires requires(const T& value, TelemetryTable& table) {
    { value.LogTo(table) };
  }
inline void LogTo(wpi::TelemetryTable& table, const T& value) {
  value.LogTo(table);
}

template <typename T>
  requires requires(const T& value) {
    { value.GetTelemetryType() } -> std::convertible_to<std::string_view>;
  }
constexpr std::string_view GetTelemetryTypeName(const T& value) {
  return value.GetTelemetryType();
}

static_assert(SupportsTelemetryWithTypeName<TelemetryLoggable>);

}  // namespace wpi
