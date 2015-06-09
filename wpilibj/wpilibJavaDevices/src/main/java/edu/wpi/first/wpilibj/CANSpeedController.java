package edu.wpi.first.wpilibj;

public interface CANSpeedController extends SpeedController {
	/**
	* Mode determines how the motor is controlled, used internally.
  *
  * Note that the Jaguar does not support follower mode.
	*/
	public enum ControlMode {
		PercentVbus((byte)0),
    Current((byte)1),
    Speed((byte)2),
    Position((byte)3),
    Voltage((byte)4),
    Follower((byte)5); // Not supported by Jaguar.

		public byte value;

		public static ControlMode valueOf(byte value) {
			for(ControlMode mode : values()) {
				if(mode.value == value) {
					return mode;
				}
			}

			return null;
		}

		private ControlMode(byte value) {
			this.value = value;
		}
	}

  /**
   * Return the current setpoint.
   *
   * @return the current setpoint, as passed to {@link #set}.
   */
  public double get();

  /**
   * Set the output for whichever mode we are currently in.
   *
   * @param value the setpoint, in some units depending on the mode.
   */
  public void set(double value);

  /**
   * Disables to motor controller output.
   */
  public void disable();

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
   *   sensors.
   *
   * @return the current sensor position.
   */
  public double getPosition();

  /**
   * Return the current velocity of whatever the current selected sensor is.
   *
   * See specific implementations for more information on selecting feedback
   *   sensors.
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
}
