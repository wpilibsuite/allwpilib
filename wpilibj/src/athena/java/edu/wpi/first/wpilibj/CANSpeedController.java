/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
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
   * Mode determines how the motor is controlled, used internally. This is meant
   * to be subclassed by enums
   * (see {@link CANTalon.TalonControlMode CANTalon.TalonControlMode}).
   *
   *
   * Note that the Jaguar does not support follower mode.
   */
  public interface ControlMode {
      /**
       * Gets the name of this control mode. Since this interface should only be
       * subclassed by enumerations, this will be overridden by the builtin
       * name() method.
       */
      public String name();
      /**
       * Checks if this control mode is PID-compatible.
       */
      public boolean isPID();
      /**
       * Gets the integer value of this control mode.
       */
      public int getValue();
  }

  /**
   * Gets the current control mode.
   *
   * @return the current control mode
   */
  public ControlMode getControlMode();

  /**
   * Sets the control mode of this speed controller.
   *
   * @param mode the the new mode
   */
  public void setControlMode(int mode);

  /**
   * Set the proportional PID constant.
   */
  public void setP(double p);

  /**
   * Set the integral PID constant.
   */
  public void setI(double i);

  /**
   * Set the derivative PID constant.
   */
  public void setD(double d);

  /**
   * Set the feed-forward PID constant. This method is optional to implement.
   */
  public default void setF(double f) {
  }

  /**
   * Gets the feed-forward PID constant. This method is optional to implement.
   * If a subclass does not implement this, it will always return zero.
   */
  public default double getF() {
      return 0.0;
  }

  /**
   * Get the current input (battery) voltage.
   *
   * @return the input voltage to the controller (in Volts).
   */
  public double getBusVoltage();

  /**
   * Get the current output voltage.
   *
   * @return the output voltage to the motor in volts.
   */
  public double getOutputVoltage();

  /**
   * Get the current being applied to the motor.
   *
   * @return the current motor current (in Amperes).
   */
  public double getOutputCurrent();

  /**
   * Get the current temperature of the controller.
   *
   * @return the current temperature of the controller, in degrees Celsius.
   */
  public double getTemperature();

  /**
   * Return the current position of whatever the current selected sensor is.
   *
   * See specific implementations for more information on selecting feedback
   * sensors.
   *
   * @return the current sensor position.
   */
  public double getPosition();

  /**
   * Return the current velocity of whatever the current selected sensor is.
   *
   * See specific implementations for more information on selecting feedback
   * sensors.
   *
   * @return the current sensor velocity.
   */
  public double getSpeed();

  /**
   * Set the maximum rate of change of the output voltage.
   *
   * @param rampRate the maximum rate of change of the votlage, in Volts / sec.
   */
  public void setVoltageRampRate(double rampRate);

  /**
   * All CAN Speed Controllers have the same SmartDashboard type: "CANSpeedController".
   */
  String SMART_DASHBOARD_TYPE = "CANSpeedController";

  @Override
  public default void updateTable() {
      ITable table = getTable();
      if(table != null) {
          table.putString("~TYPE~", SMART_DASHBOARD_TYPE);
          table.putString("Type", getClass().getSimpleName()); // "CANTalon", "CANJaguar"
          table.putNumber("Mode", getControlMode().getValue());
          if (getControlMode().isPID()) {
              // CANJaguar throws an exception if you try to get its PID constants
              // when it's not in a PID-compatible mode
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
  public default String getSmartDashboardType() {
      return SMART_DASHBOARD_TYPE;
  }

  /**
   * Creates an ITableListener for the LiveWindow table for this CAN speed
   * controller.
   */
  public default ITableListener createTableListener() {
      return (table, key, value, isNew) -> {
          switch(key) {
              case "Enabled":
                  if ((Boolean) value) {
                      enable();
                  } else {
                      disable();
                  }
                  break;
              case "Value": set((Double) value); break;
              case "Mode": setControlMode(((Double) value).intValue()); break;
          }
          if(getControlMode().isPID()) {
            switch(key) {
                case "p": setP((Double) value); break;
                case "i": setI((Double) value); break;
                case "d": setD((Double) value); break;
                case "f": setF((Double) value); break;
            }
          }
      };
  }


}
