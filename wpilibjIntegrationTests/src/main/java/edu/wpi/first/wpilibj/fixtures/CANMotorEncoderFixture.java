/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Relay;
import edu.wpi.first.wpilibj.Relay.Direction;
import edu.wpi.first.wpilibj.Relay.Value;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.mockhardware.FakePotentiometerSource;

/**
 * A fixture that wraps a {@link CANJaguar}.
 *
 * @author jonathanleitschuh
 */
public abstract class CANMotorEncoderFixture extends MotorEncoderFixture<CANJaguar> implements
    ITestFixture {
  private static final Logger logger = Logger.getLogger(CANMotorEncoderFixture.class.getName());
  public static final double RELAY_POWER_UP_TIME = .75;
  private FakePotentiometerSource m_potSource;
  private DigitalOutput m_forwardLimit;
  private DigitalOutput m_reverseLimit;
  private Relay m_powerCycler;
  private boolean m_initialized = false;

  protected abstract FakePotentiometerSource giveFakePotentiometerSource();

  protected abstract DigitalOutput giveFakeForwardLimit();

  protected abstract DigitalOutput giveFakeReverseLimit();

  protected abstract Relay givePowerCycleRelay();

  public CANMotorEncoderFixture() {
  }

  private void initialize() {
    synchronized (this) {
      if (!m_initialized) {
        m_initialized = true;// This ensures it is only initialized once

        m_powerCycler = givePowerCycleRelay();
        m_powerCycler.setDirection(Direction.kForward);
        logger.fine("Turning on the power!");
        m_powerCycler.set(Value.kForward);
        m_forwardLimit = giveFakeForwardLimit();
        m_reverseLimit = giveFakeReverseLimit();
        m_forwardLimit.set(false);
        m_reverseLimit.set(false);
        m_potSource = giveFakePotentiometerSource();
        Timer.delay(RELAY_POWER_UP_TIME); // Delay so the relay has time to boot
        // up
      }
    }
  }


  @Override
  public boolean setup() {
    initialize(); // This initializes the Relay first
    return super.setup();
  }

  @Override
  public boolean reset() {
    initialize();
    m_potSource.reset();
    m_forwardLimit.set(false);
    m_reverseLimit.set(false);
    getMotor().setPercentMode(); // Get the Jaguar into a mode where setting the
    // speed means stop
    return super.reset();
  }

  @Override
  public boolean teardown() {
    boolean wasNull = false;
    if (m_potSource != null) {
      m_potSource.free();
      m_potSource = null;
    } else {
      wasNull = true;
    }
    if (m_forwardLimit != null) {
      m_forwardLimit.set(false);
      m_forwardLimit.free();
      m_forwardLimit = null;
    } else {
      wasNull = true;
    }
    if (m_reverseLimit != null) {
      m_reverseLimit.set(false);
      m_reverseLimit.free();
      m_reverseLimit = null;
    } else {
      wasNull = true;
    }
    boolean superTornDown = false;
    try {
      superTornDown = super.teardown();
    } finally {
      try {
        if (getMotor() != null) {
          getMotor().disableControl();
          getMotor().free();
        } else {
          wasNull = true;
        }
      } finally {
        if (m_powerCycler != null) {
          m_powerCycler.free();
          m_powerCycler = null;
        } else {
          wasNull = true;
        }
      }
    }
    if (wasNull) {
      throw new RuntimeException("CANMotorEncoderFixture had a null value at teardown");
    }

    return superTornDown;
  }

  public FakePotentiometerSource getFakePot() {
    initialize();
    return m_potSource;
  }

  public DigitalOutput getForwardLimit() {
    initialize();
    return m_forwardLimit;
  }

  public DigitalOutput getReverseLimit() {
    initialize();
    return m_reverseLimit;
  }

  /**
   * Prints the current status of the fixture.
   */
  public String printStatus() {
    StringBuilder status = new StringBuilder("CAN Motor Encoder Status: ");
    if (getMotor() != null) {
      status.append("\t" + getMotor().getDescription() + "\n");
      status.append("\tFault = " + getMotor().getFaults() + "\n");
      status.append("\tValue = " + getMotor().get() + "\n");
      status.append("\tOutputVoltage = " + getMotor().getOutputVoltage() + "\n");
      status.append("\tPosition = " + getMotor().getPosition() + "\n");
      status.append("\tForward Limit Ok = " + getMotor().getForwardLimitOK() + "\n");
      status.append("\tReverse Limit Ok = " + getMotor().getReverseLimitOK() + "\n");
    } else {
      status.append("\t" + "CANJaguar Motor = null" + "\n");
    }
    if (m_forwardLimit != null) {
      status.append("\tForward Limit Output = " + m_forwardLimit + "\n");
    } else {
      status.append("\tForward Limit Output = null" + "\n");
    }
    if (m_reverseLimit != null) {
      status.append("\tReverse Limit Output = " + m_reverseLimit + "\n");
    } else {
      status.append("\tReverse Limit Output = null" + "\n");
    }

    return status.toString();
  }

  /**
   * Browns out the fixture for a specific ammount of time.
   *
   * @param seconds The number of seconds to brown out for.
   */
  public void brownOut(double seconds) {
    initialize();
    powerOff();
    Timer.delay(seconds);
    powerOn();
  }

  public void powerOff() {
    initialize();
    m_powerCycler.set(Value.kOff);
  }

  public void powerOn() {
    initialize();
    m_powerCycler.set(Value.kForward);
  }

}
