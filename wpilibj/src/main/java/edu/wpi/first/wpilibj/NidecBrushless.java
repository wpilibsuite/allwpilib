/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.networktables.EntryListenerFlags;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;

/**
 * Nidec Brushless Motor.
 */
public class NidecBrushless implements SpeedController, MotorSafety, LiveWindowSendable {
  private final MotorSafetyHelper m_safetyHelper;
  private boolean m_isInverted = false;
  private DigitalOutput m_dio;
  private PWM m_pwm;
  private volatile double m_speed = 0.0;

  /**
   * Constructor.
   *
   * @param pwmChannel The PWM channel that the Nidec Brushless controller is attached to.
   *                   0-9 are on-board, 10-19 are on the MXP port
   * @param dioChannel The DIO channel that the Nidec Brushless controller is attached to.
   *                   0-9 are on-board, 10-25 are on the MXP port
   */
  public NidecBrushless(final int pwmChannel, final int dioChannel) {
    m_safetyHelper = new MotorSafetyHelper(this);
    m_safetyHelper.setExpiration(0.0);
    m_safetyHelper.setSafetyEnabled(false);

    // the dio controls the output (in PWM mode)
    m_dio = new DigitalOutput(dioChannel);
    m_dio.setPWMRate(15625);
    m_dio.enablePWM(0.5);

    // the pwm enables the controller
    m_pwm = new PWM(pwmChannel);
    m_pwm.setRaw(0xffff);

    LiveWindow.addActuator("Nidec Brushless", pwmChannel, this);
    HAL.report(tResourceType.kResourceType_NidecBrushless, pwmChannel);
  }

  /**
   * Set the PWM value.
   *
   * <p>The PWM value is set using a range of -1.0 to 1.0, appropriately scaling the value for the
   * FPGA.
   *
   * @param speed The speed value between -1.0 and 1.0 to set.
   */
  @Override
  public void set(double speed) {
    m_speed = speed;
    m_dio.updateDutyCycle(0.5 + 0.5 * (m_isInverted ? -speed : speed));
    m_safetyHelper.feed();
  }

  /**
   * Get the recently set value of the PWM.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  @Override
  public double get() {
    return m_speed;
  }

  @Override
  public void setInverted(boolean isInverted) {
    m_isInverted = isInverted;
  }

  @Override
  public boolean getInverted() {
    return m_isInverted;
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  @Override
  public void pidWrite(double output) {
    set(output);
  }

  /**
   * Set the safety expiration time.
   *
   * @param timeout The timeout (in seconds) for this motor object
   */
  @Override
  public void setExpiration(double timeout) {
    m_safetyHelper.setExpiration(timeout);
  }

  /**
   * Return the safety expiration time.
   *
   * @return The expiration time value.
   */
  @Override
  public double getExpiration() {
    return m_safetyHelper.getExpiration();
  }

  /**
   * Check if the motor is currently alive or stopped due to a timeout.
   *
   * @return a bool value that is true if the motor has NOT timed out and should still be running.
   */
  @Override
  public boolean isAlive() {
    return m_safetyHelper.isAlive();
  }

  /**
   * Stop the motor. This is called by the MotorSafetyHelper object
   * when it has a timeout for this PWM and needs to stop it from running.
   */
  @Override
  public void stopMotor() {
    disable();
  }

  /**
   * Check if motor safety is enabled.
   *
   * @return True if motor safety is enforced for this object
   */
  @Override
  public boolean isSafetyEnabled() {
    return m_safetyHelper.isSafetyEnabled();
  }

  @Override
  public void setSafetyEnabled(boolean enabled) {
    m_safetyHelper.setSafetyEnabled(enabled);
  }

  @Override
  public String getDescription() {
    return "Nidec " + getChannel();
  }

  @Override
  public void disable() {
    m_dio.updateDutyCycle(0.5);
  }

  /**
   * Gets the channel number associated with the object.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_pwm.getChannel();
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  @Override
  public String getSmartDashboardType() {
    return "Nidec Brushless";
  }

  private NetworkTableEntry m_valueEntry;
  private int m_valueListener;

  @Override
  public void initTable(NetworkTable subtable) {
    if (subtable != null) {
      m_valueEntry = subtable.getEntry("Value");
      updateTable();
    } else {
      m_valueEntry = null;
    }
  }

  @Override
  public void updateTable() {
    if (m_valueEntry != null) {
      m_valueEntry.setDouble(get());
    }
  }

  @Override
  public void startLiveWindowMode() {
    set(0); // Stop for safety
    m_valueListener = m_valueEntry.addListener((event) -> set(event.value.getDouble()),
        EntryListenerFlags.kImmediate | EntryListenerFlags.kNew | EntryListenerFlags.kUpdate);
  }

  @Override
  public void stopLiveWindowMode() {
    set(0); // Stop for safety
    m_valueEntry.removeListener(m_valueListener);
    m_valueListener = 0;
  }
}
