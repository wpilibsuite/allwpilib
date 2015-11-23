/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2014. All Rights Reserved. */
/* Open Source Software - may be modified and shared by FRC teams. The code */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project. */
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
 * @author jonathanleitschuh
 *
 */
public abstract class CANMotorEncoderFixture extends MotorEncoderFixture<CANJaguar> implements
    ITestFixture {
  private static final Logger logger = Logger.getLogger(CANMotorEncoderFixture.class.getName());
  public static final double RELAY_POWER_UP_TIME = .75;
  private FakePotentiometerSource potSource;
  private DigitalOutput forwardLimit;
  private DigitalOutput reverseLimit;
  private Relay powerCycler;
  private boolean initialized = false;

  protected abstract FakePotentiometerSource giveFakePotentiometerSource();

  protected abstract DigitalOutput giveFakeForwardLimit();

  protected abstract DigitalOutput giveFakeReverseLimit();

  protected abstract Relay givePowerCycleRelay();

  public CANMotorEncoderFixture() {}

  private void initialize() {
    synchronized (this) {
      if (!initialized) {
        initialized = true;// This ensures it is only initialized once

        powerCycler = givePowerCycleRelay();
        powerCycler.setDirection(Direction.kForward);
        logger.fine("Turning on the power!");
        powerCycler.set(Value.kForward);
        forwardLimit = giveFakeForwardLimit();
        reverseLimit = giveFakeReverseLimit();
        forwardLimit.set(false);
        reverseLimit.set(false);
        potSource = giveFakePotentiometerSource();
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
    potSource.reset();
    forwardLimit.set(false);
    reverseLimit.set(false);
    getMotor().setPercentMode(); // Get the Jaguar into a mode where setting the
                                 // speed means stop
    return super.reset();
  }

  @Override
  public boolean teardown() {
    boolean wasNull = false;
    if (potSource != null) {
      potSource.free();
      potSource = null;
    } else
      wasNull = true;
    if (forwardLimit != null) {
      forwardLimit.set(false);
      forwardLimit.free();
      forwardLimit = null;
    } else
      wasNull = true;
    if (reverseLimit != null) {
      reverseLimit.set(false);
      reverseLimit.free();
      reverseLimit = null;
    } else
      wasNull = true;
    boolean superTornDown = false;
    try {
      superTornDown = super.teardown();
    } finally {
      try {
        if (getMotor() != null) {
          getMotor().disableControl();
          getMotor().free();
        } else
          wasNull = true;
      } finally {
        if (powerCycler != null) {
          powerCycler.free();
          powerCycler = null;
        } else
          wasNull = true;
      }
    }
    if (wasNull) {
      throw new RuntimeException("CANMotorEncoderFixture had a null value at teardown");
    }

    return superTornDown;
  }

  public FakePotentiometerSource getFakePot() {
    initialize();
    return potSource;
  }

  public DigitalOutput getForwardLimit() {
    initialize();
    return forwardLimit;
  }

  public DigitalOutput getReverseLimit() {
    initialize();
    return reverseLimit;
  }

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
    if (forwardLimit != null) {
      status.append("\tForward Limit Output = " + forwardLimit + "\n");
    } else {
      status.append("\tForward Limit Output = null" + "\n");
    }
    if (reverseLimit != null) {
      status.append("\tReverse Limit Output = " + reverseLimit + "\n");
    } else {
      status.append("\tReverse Limit Output = null" + "\n");
    }

    return status.toString();
  }

  public void brownOut(double seconds) {
    initialize();
    powerOff();
    Timer.delay(seconds);
    powerOn();
  }

  public void powerOff() {
    initialize();
    powerCycler.set(Value.kOff);
  }

  public void powerOn() {
    initialize();
    powerCycler.set(Value.kForward);
  }

}
