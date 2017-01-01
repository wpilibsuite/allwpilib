/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

public interface CANSpeedController extends SpeedController, PIDInterface, LiveWindowSendable {
  /**
   * Mode determines how the motor is controlled, used internally. This is meant to be subclassed by
   * enums
   *
   * <p>Note that the Jaguar does not support follower mode.
   */
  interface ControlMode {
    /**
     * Gets the name of this control mode. Since this interface should only be subclassed by
     * enumerations, this will be overridden by the builtin name() method.
     */
    String name();

    /**
     * Checks if this control mode is PID-compatible.
     */
    boolean isPID();

    /**
     * Gets the integer value of this control mode.
     */
    int getValue();
  }

  /**
   * Gets the current control mode.
   *
   * @return the current control mode
   */
  ControlMode getControlMode();

  /**
   * Sets the control mode of this speed controller.
   *
   * @param mode the the new mode
   */
  void setControlMode(int mode);

  /**
   * Set the proportional PID constant.
   */
  @SuppressWarnings("ParameterName")
  void setP(double p);

  /**
   * Set the integral PID constant.
   */
  @SuppressWarnings("ParameterName")
  void setI(double i);

  /**
   * Set the derivative PID constant.
   */
  @SuppressWarnings("ParameterName")
  void setD(double d);

  /**
   * Set the feed-forward PID constant. This method is optional to implement.
   */
  @SuppressWarnings("ParameterName")
  default void setF(double f) {
  }

  /**
   * Gets the feed-forward PID constant. This method is optional to implement. If a subclass does
   * not implement this, it will always return zero.
   */
  default double getF() {
    return 0.0;
  }

  /**
   * Get the current input (battery) voltage.
   *
   * @return the input voltage to the controller (in Volts).
   */
  double getBusVoltage();

  /**
   * Get the current output voltage.
   *
   * @return the output voltage to the motor in volts.
   */
  double getOutputVoltage();

  /**
   * Get the current being applied to the motor.
   *
   * @return the current motor current (in Amperes).
   */
  double getOutputCurrent();

  /**
   * Get the current temperature of the controller.
   *
   * @return the current temperature of the controller, in degrees Celsius.
   */
  double getTemperature();

  /**
   * Return the current position of whatever the current selected sensor is.
   *
   * <p>See specific implementations for more information on selecting feedback sensors.
   *
   * @return the current sensor position.
   */
  double getPosition();

  /**
   * Return the current velocity of whatever the current selected sensor is.
   *
   * <p>See specific implementations for more information on selecting feedback sensors.
   *
   * @return the current sensor velocity.
   */
  double getSpeed();

  /**
   * Set the maximum rate of change of the output voltage.
   *
   * @param rampRate the maximum rate of change of the votlage, in Volts / sec.
   */
  void setVoltageRampRate(double rampRate);

  /**
   * All CAN Speed Controllers have the same SmartDashboard type: "CANSpeedController".
   */
  String SMART_DASHBOARD_TYPE = "CANSpeedController";

  @Override
  default void updateTable() {
    ITable table = getTable();
    if (table != null) {
      table.putString("~TYPE~", SMART_DASHBOARD_TYPE);
      table.putString("Type", getClass().getSimpleName());
      table.putNumber("Mode", getControlMode().getValue());
      if (getControlMode().isPID()) {
        table.putNumber("p", getP());
        table.putNumber("i", getI());
        table.putNumber("d", getD());
        table.putNumber("f", getF());
      }
      table.putBoolean("Enabled", isEnabled());
      table.putNumber("Value", get());
    }
  }

  @Override
  default String getSmartDashboardType() {
    return SMART_DASHBOARD_TYPE;
  }

  /**
   * Creates an ITableListener for the LiveWindow table for this CAN speed controller.
   */
  default ITableListener createTableListener() {
    return (table, key, value, isNew) -> {
      switch (key) {
        case "Enabled":
          if ((Boolean) value) {
            enable();
          } else {
            disable();
          }
          break;
        case "Value":
          set((Double) value);
          break;
        case "Mode":
          setControlMode(((Double) value).intValue());
          break;
        default:
          break;
      }
      if (getControlMode().isPID()) {
        switch (key) {
          case "p":
            setP((Double) value);
            break;
          case "i":
            setI((Double) value);
            break;
          case "d":
            setD((Double) value);
            break;
          case "f":
            setF((Double) value);
            break;
          default:
            break;
        }
      }
    };
  }


}
