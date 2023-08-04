// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
    "Motor Controller",
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
    "Field",
};

const char* detail::GetStringForWidgetType(BuiltInWidgets type) {
  return widgetStrings[static_cast<int>(type)];
}
