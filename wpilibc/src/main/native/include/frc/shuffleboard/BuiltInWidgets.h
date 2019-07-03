/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/shuffleboard/WidgetType.h"

namespace frc {

/**
 * The types of the widgets bundled with Shuffleboard.
 *
 * <p>For example, setting a number to be displayed with a slider:
 * <pre>{@code
 * NetworkTableEntry example = Shuffleboard.getTab("My Tab")
 *   .add("My Number", 0)
 *   .withWidget(BuiltInWidgets.kNumberSlider)
 *   .getEntry();
 * }</pre>
 *
 * <p>Each value in this enum goes into detail on what data types that widget
 * can support, as well as the custom properties that widget uses.
 */
enum class BuiltInWidgets {
  /**
   * Displays a value with a simple text field.
   * <br>Supported types:
   * <ul>
   * <li>String</li>
   * <li>Number</li>
   * <li>Boolean</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kTextView,
  /**
   * Displays a number with a controllable slider.
   * <br>Supported types:
   * <ul>
   * <li>Number</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Min</td><td>Number</td><td>-1.0</td><td>The minimum value of the
   * slider</td></tr> <tr><td>Max</td><td>Number</td><td>1.0</td><td>The maximum
   * value of the slider</td></tr> <tr><td>Block
   * increment</td><td>Number</td><td>0.0625</td> <td>How much to move the
   * slider by with the arrow keys</td></tr>
   * </table>
   */
  kNumberSlider,
  /**
   * Displays a number with a view-only bar.
   * <br>Supported types:
   * <ul>
   * <li>Number</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Min</td><td>Number</td><td>-1.0</td><td>The minimum value of the
   * bar</td></tr> <tr><td>Max</td><td>Number</td><td>1.0</td><td>The maximum
   * value of the bar</td></tr>
   * <tr><td>Center</td><td>Number</td><td>0</td><td>The center ("zero") value
   * of the bar</td></tr>
   * </table>
   */
  kNumberBar,
  /**
   * Displays a number with a view-only dial. Displayed values are rounded to
   * the nearest integer. <br>Supported types: <ul> <li>Number</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Min</td><td>Number</td><td>0</td><td>The minimum value of the
   * dial</td></tr> <tr><td>Max</td><td>Number</td><td>100</td><td>The maximum
   * value of the dial</td></tr> <tr><td>Show
   * value</td><td>Boolean</td><td>true</td> <td>Whether or not to show the
   * value as text</td></tr>
   * </table>
   */
  kDial,
  /**
   * Displays a number with a graph. <strong>NOTE:</strong> graphs can be taxing
   * on the computer running the dashboard. Keep the number of visible data
   * points to a minimum. Making the widget smaller also helps with performance,
   * but may cause the graph to become difficult to read. <br>Supported types:
   * <ul>
   * <li>Number</li>
   * <li>Number array</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Visible time</td><td>Number</td><td>30</td>
   * <td>How long, in seconds, should past data be visible for</td></tr>
   * </table>
   */
  kGraph,
  /**
   * Displays a boolean value as a large colored box.
   * <br>Supported types:
   * <ul>
   * <li>Boolean</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Color when true</td><td>Color</td><td>"green"</td>
   * <td>Can be specified as a string ({@code "#00FF00"}) or a rgba integer
   * ({@code 0x00FF0000})
   * </td></tr>
   * <tr><td>Color when false</td><td>Color</td><td>"red"</td>
   * <td>Can be specified as a string or a number</td></tr>
   * </table>
   */
  kBooleanBox,
  /**
   * Displays a boolean with a large interactive toggle button.
   * <br>Supported types:
   * <ul>
   * <li>Boolean</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kToggleButton,
  /**
   * Displays a boolean with a fixed-size toggle switch.
   * <br>Supported types:
   * <ul>
   * <li>Boolean</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kToggleSwitch,
  /**
   * Displays an analog input or a raw number with a number bar.
   * <br>Supported types:
   * <ul>
   * <li>Number</li>
   * <li>{@link edu.wpi.first.wpilibj.AnalogInput}</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Min</td><td>Number</td><td>0</td><td>The minimum value of the
   * bar</td></tr> <tr><td>Max</td><td>Number</td><td>5</td><td>The maximum
   * value of the bar</td></tr>
   * <tr><td>Center</td><td>Number</td><td>0</td><td>The center ("zero") value
   * of the bar</td></tr>
   * <tr><td>Orientation</td><td>String</td><td>"HORIZONTAL"</td>
   * <td>The orientation of the bar. One of {@code ["HORIZONTAL",
   * "VERTICAL"]}</td></tr> <tr><td>Number of tick
   * marks</td><td>Number</td><td>5</td> <td>The number of discrete ticks on the
   * bar</td></tr>
   * </table>
   */
  kVoltageView,
  /**
   * Displays a {@link edu.wpi.first.wpilibj.PowerDistributionPanel
   * PowerDistributionPanel}. <br>Supported types: <ul> <li>{@link
   * edu.wpi.first.wpilibj.PowerDistributionPanel}</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Show voltage and current values</td><td>Boolean</td><td>true</td>
   * <td>Whether or not to display the voltage and current draw</td></tr>
   * </table>
   */
  kPowerDistributionPanel,
  /**
   * Displays a {@link edu.wpi.first.wpilibj.smartdashboard.SendableChooser
   * SendableChooser} with a dropdown combo box with a list of options.
   * <br>Supported types:
   * <ul>
   * <li>{@link edu.wpi.first.wpilibj.smartdashboard.SendableChooser}</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kComboBoxChooser,
  /**
   * Displays a {@link edu.wpi.first.wpilibj.smartdashboard.SendableChooser
   * SendableChooser} with a toggle button for each available option.
   * <br>Supported types:
   * <ul>
   * <li>{@link edu.wpi.first.wpilibj.smartdashboard.SendableChooser}</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kSplitButtonChooser,
  /**
   * Displays an {@link edu.wpi.first.wpilibj.Encoder} displaying its speed,
   * total travelled distance, and its distance per tick. <br>Supported types:
   * <ul>
   * <li>{@link edu.wpi.first.wpilibj.Encoder}</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kEncoder,
  /**
   * Displays a {@link edu.wpi.first.wpilibj.SpeedController SpeedController}.
   * The speed controller will be controllable from the dashboard when test mode
   * is enabled, but will otherwise be view-only. <br>Supported types: <ul>
   * <li>{@link edu.wpi.first.wpilibj.PWMSpeedController}</li>
   * <li>{@link edu.wpi.first.wpilibj.DMC60}</li>
   * <li>{@link edu.wpi.first.wpilibj.Jaguar}</li>
   * <li>{@link edu.wpi.first.wpilibj.PWMTalonSRX}</li>
   * <li>{@link edu.wpi.first.wpilibj.PWMVictorSPX}</li>
   * <li>{@link edu.wpi.first.wpilibj.SD540}</li>
   * <li>{@link edu.wpi.first.wpilibj.Spark}</li>
   * <li>{@link edu.wpi.first.wpilibj.Talon}</li>
   * <li>{@link edu.wpi.first.wpilibj.Victor}</li>
   * <li>{@link edu.wpi.first.wpilibj.VictorSP}</li>
   * <li>{@link edu.wpi.first.wpilibj.SpeedControllerGroup}</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Orientation</td><td>String</td><td>"HORIZONTAL"</td>
   * <td>One of {@code ["HORIZONTAL", "VERTICAL"]}</td></tr>
   * </table>
   */
  kSpeedController,
  /**
   * Displays a command with a toggle button. Pressing the button will start the
   * command, and the button will automatically release when the command
   * completes. <br>Supported types: <ul> <li>{@link
   * edu.wpi.first.wpilibj.command.Command}</li> <li>{@link
   * edu.wpi.first.wpilibj.command.CommandGroup}</li> <li>Any custom subclass of
   * {@code Command} or {@code CommandGroup}</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kCommand,
  /**
   * Displays a PID command with a checkbox and an editor for the PIDF
   * constants. Selecting the checkbox will start the command, and the checkbox
   * will automatically deselect when the command completes. <br>Supported
   * types: <ul> <li>{@link edu.wpi.first.wpilibj.command.PIDCommand}</li>
   * <li>Any custom subclass of {@code PIDCommand}</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kPIDCommand,
  /**
   * Displays a PID controller with an editor for the PIDF constants and a
   * toggle switch for enabling and disabling the controller. <br>Supported
   * types: <ul> <li>{@link edu.wpi.first.wpilibj.PIDController}</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kPIDController,
  /**
   * Displays an accelerometer with a number bar displaying the magnitude of the
   * acceleration and text displaying the exact value. <br>Supported types: <ul>
   * <li>{@link edu.wpi.first.wpilibj.AnalogAccelerometer}</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Min</td><td>Number</td><td>-1</td>
   * <td>The minimum acceleration value to display</td></tr>
   * <tr><td>Max</td><td>Number</td><td>1</td>
   * <td>The maximum acceleration value to display</td></tr>
   * <tr><td>Show text</td><td>Boolean</td><td>true</td>
   * <td>Show or hide the acceleration values</td></tr>
   * <tr><td>Precision</td><td>Number</td><td>2</td>
   * <td>How many numbers to display after the decimal point</td></tr>
   * <tr><td>Show tick marks</td><td>Boolean</td><td>false</td>
   * <td>Show or hide the tick marks on the number bars</td></tr>
   * </table>
   */
  kAccelerometer,
  /**
   * Displays a 3-axis accelerometer with a number bar for each axis'
   * accleration. <br>Supported types: <ul> <li>{@link
   * edu.wpi.first.wpilibj.ADXL345_I2C}</li> <li>{@link
   * edu.wpi.first.wpilibj.ADXL345_SPI}</li> <li>{@link
   * edu.wpi.first.wpilibj.ADXL362}</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Range</td><td>{@link Range}</td><td>k16G</td><td>The accelerometer
   * range</td></tr> <tr><td>Show value</td><td>Boolean</td><td>true</td>
   * <td>Show or hide the acceleration values</td></tr>
   * <tr><td>Precision</td><td>Number</td><td>2</td>
   * <td>How many numbers to display after the decimal point</td></tr>
   * <tr><td>Show tick marks</td><td>Boolean</td><td>false</td>
   * <td>Show or hide the tick marks on the number bars</td></tr>
   * </table>
   */
  k3AxisAccelerometer,
  /**
   * Displays a gyro with a dial from 0 to 360 degrees.
   * <br>Supported types:
   * <ul>
   * <li>{@link edu.wpi.first.wpilibj.ADXRS450_Gyro}</li>
   * <li>{@link edu.wpi.first.wpilibj.AnalogGyro}</li>
   * <li>Any custom subclass of {@code GyroBase} (such as a MXP gyro)</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Major tick
   * spacing</td><td>Number</td><td>45</td><td>Degrees</td></tr>
   * <tr><td>Starting angle</td><td>Number</td><td>180</td>
   * <td>How far to rotate the entire dial, in degrees</td></tr>
   * <tr><td>Show tick mark ring</td><td>Boolean</td><td>true</td></tr>
   * </table>
   */
  kGyro,
  /**
   * Displays a relay with toggle buttons for each supported mode (off, on,
   * forward, reverse). <br>Supported types: <ul> <li>{@link
   * edu.wpi.first.wpilibj.Relay}</li>
   * </ul>
   * <br>This widget has no custom properties.
   */
  kRelay,
  /**
   * Displays a differential drive with a widget that displays the speed of each
   * side of the drivebase and a vector for the direction and rotation of the
   * drivebase. The widget will be controllable if the robot is in test mode.
   * <br>Supported types:
   * <ul>
   * <li>{@link edu.wpi.first.wpilibj.drive.DifferentialDrive}</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Number of wheels</td><td>Number</td><td>4</td><td>Must be a
   * positive even integer
   * </td></tr>
   * <tr><td>Wheel diameter</td><td>Number</td><td>80</td><td>Pixels</td></tr>
   * <tr><td>Show velocity vectors</td><td>Boolean</td><td>true</td></tr>
   * </table>
   */
  kDifferentialDrive,
  /**
   * Displays a mecanum drive with a widget that displays the speed of each
   * wheel, and vectors for the direction and rotation of the drivebase. The
   * widget will be controllable if the robot is in test mode. <br>Supported
   * types: <ul> <li>{@link edu.wpi.first.wpilibj.drive.MecanumDrive}</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Show velocity vectors</td><td>Boolean</td><td>true</td></tr>
   * </table>
   */
  kMecanumDrive,
  /**
   * Displays a camera stream.
   * <br>Supported types:
   * <ul>
   * <li>{@link edu.wpi.cscore.VideoSource} (as long as it is streaming on an
   * MJPEG server)</li>
   * </ul>
   * <br>Custom properties:
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Show crosshair</td><td>Boolean</td><td>true</td>
   * <td>Show or hide a crosshair on the image</td></tr>
   * <tr><td>Crosshair color</td><td>Color</td><td>"white"</td>
   * <td>Can be a string or a rgba integer</td></tr>
   * <tr><td>Show controls</td><td>Boolean</td><td>true</td><td>Show or hide the
   * stream controls
   * </td></tr>
   * <tr><td>Rotation</td><td>String</td><td>"NONE"</td>
   * <td>Rotates the displayed image. One of {@code ["NONE", "QUARTER_CW",
   * "QUARTER_CCW", "HALF"]}
   * </td></tr>
   * </table>
   */
  kCameraStream
};

}  // namespace frc
