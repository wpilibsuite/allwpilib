// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "wpi/cs/cscore_cpp.hpp"

namespace wpi::cs {

/**
 * @defgroup cscore_oo cscore C++ object-oriented API
 *
 * Recommended interface for C++, identical to Java API.
 *
 * <p>The classes are RAII and handle reference counting internally.
 *
 * @{
 */

// Forward declarations so friend declarations work correctly
class ImageSource;
class VideoEvent;
class VideoSink;
class VideoSource;

/**
 * A source or sink property.
 */
class VideoProperty {
  friend class ImageSource;
  friend class VideoEvent;
  friend class VideoSink;
  friend class VideoSource;

 public:
  enum Kind {
    /// No specific property.
    kNone = CS_PROP_NONE,
    /// Boolean property.
    kBoolean = CS_PROP_BOOLEAN,
    /// Integer property.
    kInteger = CS_PROP_INTEGER,
    /// String property.
    kString = CS_PROP_STRING,
    /// Enum property.
    kEnum = CS_PROP_ENUM
  };

  VideoProperty() = default;

  /**
   * Returns property name.
   *
   * @return Property name.
   */
  std::string GetName() const {
    m_status = 0;
    return GetPropertyName(m_handle, &m_status);
  }

  /**
   * Returns property kind.
   *
   * @return Property kind.
   */
  Kind GetKind() const { return m_kind; }

  /**
   * Returns true if property is valid.
   *
   * @return True if property is valid.
   */
  explicit operator bool() const { return m_kind != kNone; }

  /**
   * Returns true if property is a boolean.
   *
   * @return True if property is a boolean.
   */
  bool IsBoolean() const { return m_kind == kBoolean; }

  /**
   * Returns true if property is an integer.
   *
   * @return True if property is an integer.
   */
  bool IsInteger() const { return m_kind == kInteger; }

  /**
   * Returns true if property is a string.
   *
   * @return True if property is a string.
   */
  bool IsString() const { return m_kind == kString; }

  /**
   * Returns true if property is an enum.
   *
   * @return True if property is an enum.
   */
  bool IsEnum() const { return m_kind == kEnum; }

  /**
   * Returns property value.
   *
   * @return Property value.
   */
  int Get() const {
    m_status = 0;
    return GetProperty(m_handle, &m_status);
  }

  /**
   * Sets property value.
   *
   * @param value Property value.
   */
  void Set(int value) {
    m_status = 0;
    SetProperty(m_handle, value, &m_status);
  }

  /**
   * Returns property minimum value.
   *
   * @return Property minimum value.
   */
  int GetMin() const {
    m_status = 0;
    return GetPropertyMin(m_handle, &m_status);
  }

  /**
   * Returns property maximum value.
   *
   * @return Property maximum value.
   */
  int GetMax() const {
    m_status = 0;
    return GetPropertyMax(m_handle, &m_status);
  }

  /**
   * Returns property step size.
   *
   * @return Property step size.
   */
  int GetStep() const {
    m_status = 0;
    return GetPropertyStep(m_handle, &m_status);
  }

  /**
   * Returns property default value.
   *
   * @return Property default value.
   */
  int GetDefault() const {
    m_status = 0;
    return GetPropertyDefault(m_handle, &m_status);
  }

  /**
   * Returns the string property value.
   *
   * <p>This function is string-specific.
   *
   * @return The string property value.
   */
  std::string GetString() const {
    m_status = 0;
    return GetStringProperty(m_handle, &m_status);
  }

  /**
   * Returns the string property value as a reference to the given buffer.
   *
   * This function is string-specific.
   *
   * @param buf The backing storage to which to write the property value.
   * @return The string property value as a reference to the given buffer.
   */
  std::string_view GetString(wpi::util::SmallVectorImpl<char>& buf) const {
    m_status = 0;
    return GetStringProperty(m_handle, buf, &m_status);
  }

  /**
   * Sets the string property value.
   *
   * This function is string-specific.
   *
   * @param value String property value.
   */
  void SetString(std::string_view value) {
    m_status = 0;
    SetStringProperty(m_handle, value, &m_status);
  }

  /**
   * Returns the possible values for the enum property value.
   *
   * This function is enum-specific.
   *
   * @return The possible values for the enum property value.
   */
  std::vector<std::string> GetChoices() const {
    m_status = 0;
    return GetEnumPropertyChoices(m_handle, &m_status);
  }

  /**
   * Returns the last status.
   *
   * @return The last status.
   */
  CS_Status GetLastStatus() const { return m_status; }

 private:
  explicit VideoProperty(CS_Property handle) : m_handle(handle) {
    m_status = 0;
    if (handle == 0) {
      m_kind = kNone;
    } else {
      m_kind = static_cast<Kind>(
          static_cast<int>(GetPropertyKind(handle, &m_status)));
    }
  }

  VideoProperty(CS_Property handle, Kind kind)
      : m_handle(handle), m_kind(kind) {}

  mutable CS_Status m_status{0};
  CS_Property m_handle{0};
  Kind m_kind{kNone};
};

}  // namespace wpi::cs
