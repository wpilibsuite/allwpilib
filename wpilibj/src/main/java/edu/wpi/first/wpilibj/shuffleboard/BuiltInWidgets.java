// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

/**
 * The types of the widgets bundled with Shuffleboard.
 *
 * <p>For example, setting a number to be displayed with a slider:
 *
 * <pre>{@code
 * GenericEntry example = Shuffleboard.getTab("My Tab")
 *   .add("My Number", 0)
 *   .withWidget(BuiltInWidgets.kNumberSlider)
 *   .withProperties(Map.of("min", 0, "max", 1))
 *   .getEntry();
 * }</pre>
 *
 * <p>Each value in this enum goes into detail on what data types that widget can support, as well
 * as the custom properties that widget uses.
 */
public enum BuiltInWidgets implements WidgetType {
  /**
   * Displays a value with a simple text field. <br>
   * Supported types:
   *
   * <ul>
   *   <li>String
   *   <li>Number
   *   <li>Boolean
   * </ul>
   *
   * <br>
   * This widget has no custom properties.
   */
  kTextView("Text View"),
  /**
   * Displays a number with a controllable slider. <br>
   * Supported types:
   *
   * <ul>
   *   <li>Number
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Min</td><td>Number</td><td>-1.0</td><td>The minimum value of the slider</td></tr>
   * <tr><td>Max</td><td>Number</td><td>1.0</td><td>The maximum value of the slider</td></tr>
   * <tr><td>Block increment</td><td>Number</td><td>0.0625</td>
   * <td>How much to move the slider by with the arrow keys</td></tr>
   * </table>
   */
  kNumberSlider("Number Slider"),
  /**
   * Displays a number with a view-only bar. <br>
   * Supported types:
   *
   * <ul>
   *   <li>Number
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Min</td><td>Number</td><td>-1.0</td><td>The minimum value of the bar</td></tr>
   * <tr><td>Max</td><td>Number</td><td>1.0</td><td>The maximum value of the bar</td></tr>
   * <tr><td>Center</td><td>Number</td><td>0</td><td>The center ("zero") value of the bar</td></tr>
   * </table>
   */
  kNumberBar("Number Bar"),
  /**
   * Displays a number with a view-only dial. Displayed values are rounded to the nearest integer.
   * <br>
   * Supported types:
   *
   * <ul>
   *   <li>Number
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Min</td><td>Number</td><td>0</td><td>The minimum value of the dial</td></tr>
   * <tr><td>Max</td><td>Number</td><td>100</td><td>The maximum value of the dial</td></tr>
   * <tr><td>Show value</td><td>Boolean</td><td>true</td>
   * <td>Whether or not to show the value as text</td></tr>
   * </table>
   */
  kDial("Simple Dial"),
  /**
   * Displays a number with a graph. <strong>NOTE:</strong> graphs can be taxing on the computer
   * running the dashboard. Keep the number of visible data points to a minimum. Making the widget
   * smaller also helps with performance, but may cause the graph to become difficult to read. <br>
   * Supported types:
   *
   * <ul>
   *   <li>Number
   *   <li>Number array
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Visible time</td><td>Number</td><td>30</td>
   * <td>How long, in seconds, should past data be visible for</td></tr>
   * </table>
   */
  kGraph("Graph"),
  /**
   * Displays a boolean value as a large colored box. <br>
   * Supported types:
   *
   * <ul>
   *   <li>Boolean
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Color when true</td><td>Color</td><td>"green"</td>
   * <td>Can be specified as a string ({@code "#00FF00"}) or a rgba integer ({@code 0x00FF0000})
   * </td></tr>
   * <tr><td>Color when false</td><td>Color</td><td>"red"</td>
   * <td>Can be specified as a string or a number</td></tr>
   * </table>
   */
  kBooleanBox("Boolean Box"),
  /**
   * Displays a boolean with a large interactive toggle button. <br>
   * Supported types:
   *
   * <ul>
   *   <li>Boolean
   * </ul>
   *
   * <br>
   * This widget has no custom properties.
   */
  kToggleButton("Toggle Button"),
  /**
   * Displays a boolean with a fixed-size toggle switch. <br>
   * Supported types:
   *
   * <ul>
   *   <li>Boolean
   * </ul>
   *
   * <br>
   * This widget has no custom properties.
   */
  kToggleSwitch("Toggle Switch"),
  /**
   * Displays an analog input or a raw number with a number bar. <br>
   * Supported types:
   *
   * <ul>
   *   <li>Number
   *   <li>{@link edu.wpi.first.wpilibj.AnalogInput}
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Min</td><td>Number</td><td>0</td><td>The minimum value of the bar</td></tr>
   * <tr><td>Max</td><td>Number</td><td>5</td><td>The maximum value of the bar</td></tr>
   * <tr><td>Center</td><td>Number</td><td>0</td><td>The center ("zero") value of the bar</td></tr>
   * <tr><td>Orientation</td><td>String</td><td>"HORIZONTAL"</td>
   * <td>The orientation of the bar. One of {@code ["HORIZONTAL", "VERTICAL"]}</td></tr>
   * <tr><td>Number of tick marks</td><td>Number</td><td>5</td>
   * <td>The number of discrete ticks on the bar</td></tr>
   * </table>
   */
  kVoltageView("Voltage View"),
  /**
   * Displays a {@link edu.wpi.first.wpilibj.PowerDistribution PowerDistribution}. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.PowerDistribution}
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Show voltage and current values</td><td>Boolean</td><td>true</td>
   * <td>Whether or not to display the voltage and current draw</td></tr>
   * </table>
   */
  kPowerDistribution("PDP"),
  /**
   * Displays a {@link edu.wpi.first.wpilibj.smartdashboard.SendableChooser SendableChooser} with a
   * dropdown combo box with a list of options. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.smartdashboard.SendableChooser}
   * </ul>
   *
   * <br>
   * This widget has no custom properties.
   */
  kComboBoxChooser("ComboBox Chooser"),
  /**
   * Displays a {@link edu.wpi.first.wpilibj.smartdashboard.SendableChooser SendableChooser} with a
   * toggle button for each available option. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.smartdashboard.SendableChooser}
   * </ul>
   *
   * <br>
   * This widget has no custom properties.
   */
  kSplitButtonChooser("Split Button Chooser"),
  /**
   * Displays an {@link edu.wpi.first.wpilibj.Encoder} displaying its speed, total traveled
   * distance, and its distance per tick. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.Encoder}
   * </ul>
   *
   * <br>
   * This widget has no custom properties.
   */
  kEncoder("Encoder"),
  /**
   * Displays a {@link edu.wpi.first.wpilibj.motorcontrol.MotorController MotorController}. The
   * motor controller will be controllable from the dashboard when test mode is enabled, but will
   * otherwise be view-only. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.PWMMotorController}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.DMC60}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.Jaguar}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.PWMTalonFX}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.PWMTalonSRX}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.PWMVenom}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.PWMVictorSPX}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.SD540}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.Spark}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.Talon}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.Victor}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.VictorSP}
   *   <li>{@link edu.wpi.first.wpilibj.motorcontrol.MotorControllerGroup}
   *   <li>Any custom subclass of {@code MotorController}
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Orientation</td><td>String</td><td>"HORIZONTAL"</td>
   * <td>One of {@code ["HORIZONTAL", "VERTICAL"]}</td></tr>
   * </table>
   */
  kMotorController("Motor Controller"),
  /**
   * Displays a command with a toggle button. Pressing the button will start the command, and the
   * button will automatically release when the command completes. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj2.command.Command}
   *   <li>Any custom subclass of {@code Command}
   * </ul>
   *
   * <br>
   * This widget has no custom properties.
   */
  kCommand("Command"),
  /**
   * Displays a PID command with a checkbox and an editor for the PIDF constants. Selecting the
   * checkbox will start the command, and the checkbox will automatically deselect when the command
   * completes. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj2.command.PIDCommand}
   *   <li>Any custom subclass of {@code PIDCommand}
   * </ul>
   *
   * <br>
   * This widget has no custom properties.
   */
  kPIDCommand("PID Command"),
  /**
   * Displays a PID controller with an editor for the PIDF constants and a toggle switch for
   * enabling and disabling the controller. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.math.controller.PIDController}
   * </ul>
   *
   * <br>
   * This widget has no custom properties.
   */
  kPIDController("PID Controller"),
  /**
   * Displays an accelerometer with a number bar displaying the magnitude of the acceleration and
   * text displaying the exact value. <br>
   * Custom properties:
   *
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
  kAccelerometer("Accelerometer"),
  /**
   * Displays a 3-axis accelerometer with a number bar for each axis' acceleration. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.ADXL345_I2C}
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Show value</td><td>Boolean</td><td>true</td>
   * <td>Show or hide the acceleration values</td></tr>
   * <tr><td>Precision</td><td>Number</td><td>2</td>
   * <td>How many numbers to display after the decimal point</td></tr>
   * <tr><td>Show tick marks</td><td>Boolean</td><td>false</td>
   * <td>Show or hide the tick marks on the number bars</td></tr>
   * </table>
   */
  k3AxisAccelerometer("3-Axis Accelerometer"),
  /**
   * Displays a gyro with a dial from 0 to 360 degrees. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.AnalogGyro}
   *   <li>Any custom subclass of {@code GyroBase} (such as a MXP gyro)
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Major tick spacing</td><td>Number</td><td>45</td><td>Degrees</td></tr>
   * <tr><td>Starting angle</td><td>Number</td><td>180</td>
   * <td>How far to rotate the entire dial, in degrees</td></tr>
   * <tr><td>Show tick mark ring</td><td>Boolean</td><td>true</td></tr>
   * </table>
   */
  kGyro("Gyro"),
  /**
   * Displays a relay with toggle buttons for each supported mode (off, on, forward, reverse). <br>
   * This widget has no custom properties.
   */
  kRelay("Relay"),
  /**
   * Displays a differential drive with a widget that displays the speed of each side of the
   * drivebase and a vector for the direction and rotation of the drivebase. The widget will be
   * controllable if the robot is in test mode. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.drive.DifferentialDrive}
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Number of wheels</td><td>Number</td><td>4</td><td>Must be a positive even integer
   * </td></tr>
   * <tr><td>Wheel diameter</td><td>Number</td><td>80</td><td>Pixels</td></tr>
   * <tr><td>Show velocity vectors</td><td>Boolean</td><td>true</td></tr>
   * </table>
   */
  kDifferentialDrive("Differential Drivebase"),
  /**
   * Displays a mecanum drive with a widget that displays the speed of each wheel, and vectors for
   * the direction and rotation of the drivebase. The widget will be controllable if the robot is in
   * test mode. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.drive.MecanumDrive}
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Show velocity vectors</td><td>Boolean</td><td>true</td></tr>
   * </table>
   */
  kMecanumDrive("Mecanum Drivebase"),
  /**
   * Displays a camera stream. <br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.cscore.VideoSource} (as long as it is streaming on an MJPEG server)
   * </ul>
   *
   * <br>
   * Custom properties:
   *
   * <table>
   * <tr><th>Name</th><th>Type</th><th>Default Value</th><th>Notes</th></tr>
   * <tr><td>Show crosshair</td><td>Boolean</td><td>true</td>
   * <td>Show or hide a crosshair on the image</td></tr>
   * <tr><td>Crosshair color</td><td>Color</td><td>"white"</td>
   * <td>Can be a string or a rgba integer</td></tr>
   * <tr><td>Show controls</td><td>Boolean</td><td>true</td><td>Show or hide the stream controls
   * </td></tr>
   * <tr><td>Rotation</td><td>String</td><td>"NONE"</td>
   * <td>Rotates the displayed image. One of {@code ["NONE", "QUARTER_CW", "QUARTER_CCW", "HALF"]}
   * </td></tr>
   * </table>
   */
  kCameraStream("Camera Stream"),
  /**
   * Displays a Field2d object.<br>
   * Supported types:
   *
   * <ul>
   *   <li>{@link edu.wpi.first.wpilibj.smartdashboard.Field2d}
   * </ul>
   */
  kField("Field"),
  ;

  private final String m_widgetName;

  BuiltInWidgets(String widgetName) {
    this.m_widgetName = widgetName;
  }

  @Override
  public String getWidgetName() {
    return m_widgetName;
  }
}
