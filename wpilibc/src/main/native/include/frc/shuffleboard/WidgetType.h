/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/Twine.h>

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
  explicit WidgetType(const char* widgetName) : m_widgetName(widgetName) {}
  ~WidgetType() = default;

  /**
   * Gets the string type of the widget as defined by that widget in
   * Shuffleboard.
   */
  wpi::StringRef GetWidgetName() const;

 private:
  wpi::StringRef m_widgetName;
};

}  // namespace frc
