// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/StringRef.h>

namespace frc {

/**
 * Represents the type of a widget in Shuffleboard. Using this is preferred over
 * specifying raw strings, to avoid typos and having to know or look up the
 * exact string name for a desired widget.
 *
 * @see BuiltInWidgets the built-in widget types
 */
class WidgetType {
 public:
  explicit constexpr WidgetType(const char* widgetName)
      : m_widgetName(widgetName) {}
  ~WidgetType() = default;

  /**
   * Gets the string type of the widget as defined by that widget in
   * Shuffleboard.
   */
  wpi::StringRef GetWidgetName() const;

 private:
  const char* m_widgetName;
};

}  // namespace frc
