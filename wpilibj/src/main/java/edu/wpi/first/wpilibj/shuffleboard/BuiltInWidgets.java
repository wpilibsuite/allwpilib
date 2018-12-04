/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

/**
 * The types of the widgets bundled with Shuffleboard.
 * <pre>{@code
 * Shuffleboard.getTab("My Tab")
 *   .add("My Number", 0)
 *   .withWidget(BuiltInWidgets.kNumberSlider)
 *   .withProperties(Map.of("min", 0, "max", 1));
 * }</pre>
 */
public enum BuiltInWidgets implements WidgetType {
  kTextView("Text View"),
  kNumberSlider("Number Slider"),
  kNumberBar("Number Bar"),
  kDial("Simple Dial"),
  kGraph("Graph"),
  kBooleanBox("Boolean Box"),
  kToggleButton("Toggle Button"),
  kToggleSwitch("Toggle Switch"),
  kVoltageView("Voltage View"),
  kPowerDistributionPanel("PDP"),
  kComboBoxChooser("ComboBox Chooser"),
  kSplitButtonChooser("Split Button Chooser"),
  kEncoder("Encoder"),
  kRobotPreferences("Robot Preferences"),
  kSpeedController("Speed Controller"),
  kCommand("Command"),
  kPIDCommand("PID Command"),
  kPIDController("PID Controller"),
  kAccelerometer("Accelerometer"),
  k3AxisAccelerometer("3-Axis Accelerometer"),
  kGyro("Gyro"),
  kRelay("Relay"),
  kDifferentialDrive("Differential Drivebase"),
  kMecanumDrive("Mecanum Drivebase"),
  kCameraStream("Camera Stream"),
  ;

  private final String widgetName;

  BuiltInWidgets(String widgetName) {
    this.widgetName = widgetName;
  }

  @Override
  public String getWidgetName() {
    return widgetName;
  }
}
