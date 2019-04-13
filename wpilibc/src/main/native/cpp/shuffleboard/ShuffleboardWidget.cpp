/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/ShuffleboardWidget.h"

using namespace frc;

static constexpr const char* widgetStrings[] = {
    "Text View",
    "Number Slider",
    "Number Bar",
    "Simple Dial",
    "Graph",
    "Boolean Box",
    "Toggle Button",
    "Toggle Switch",
    "Voltage View",
    "PDP",
    "ComboBox Chooser",
    "Split Button Chooser",
    "Encoder",
    "Speed Controller",
    "Command",
    "PID Command",
    "PID Controller",
    "Accelerometer",
    "3-Axis Accelerometer",
    "Gyro",
    "Relay",
    "Differential Drivebase",
    "Mecanum Drivebase",
    "Camera Stream",
};

const char* detail::GetStringForWidgetType(BuiltInWidgets type) {
  return widgetStrings[static_cast<int>(type)];
}
