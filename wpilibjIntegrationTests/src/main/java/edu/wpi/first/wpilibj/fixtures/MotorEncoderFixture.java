/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.fixtures;

import java.lang.reflect.ParameterizedType;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWM;
import edu.wpi.first.wpilibj.SpeedController;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.test.TestBench;

/**
 * Represents a physically connected Motor and Encoder to allow for unit tests on these different
 * pairs<br> Designed to allow the user to easily setup and tear down the fixture to allow for
 * reuse. This class should be explicitly instantiated in the TestBed class to allow any test to
 * access this fixture. This allows tests to be mailable so that you can easily reconfigure the
 * physical testbed without breaking the tests.
 */
public abstract class MotorEncoderFixture<T extends SpeedController> implements ITestFixture {
  private static final Logger logger = Logger.getLogger(MotorEncoderFixture.class.getName());
  private boolean m_initialized = false;
  private boolean m_tornDown = false;
  protected T m_motor;
  private Encoder m_encoder;
  private final Counter[] m_counters = new Counter[2];
  protected DigitalInput m_alphaSource; // Stored so it can be freed at tear down
  protected DigitalInput m_betaSource;

  /**
   * Default constructor for a MotorEncoderFixture.
   */
  public MotorEncoderFixture() {
  }

  public abstract int getPDPChannel();

  /**
   * Where the implementer of this class should pass the speed controller Constructor should only be
   * called from outside this class if the Speed controller is not also an implementation of PWM
   * interface.
   *
   * @return SpeedController
   */
  protected abstract T giveSpeedController();

  /**
   * Where the implementer of this class should pass one of the digital inputs.
   *
   * <p>CONSTRUCTOR SHOULD NOT BE CALLED FROM OUTSIDE THIS CLASS!
   *
   * @return DigitalInput
   */
  protected abstract DigitalInput giveDigitalInputA();

  /**
   * Where the implementer fo this class should pass the other digital input.
   *
   * <p>CONSTRUCTOR SHOULD NOT BE CALLED FROM OUTSIDE THIS CLASS!
   *
   * @return Input B to be used when this class is instantiated
   */
  protected abstract DigitalInput giveDigitalInputB();

  private final void initialize() {
    synchronized (this) {
      if (!m_initialized) {
        m_initialized = true; // This ensures it is only initialized once

        m_alphaSource = giveDigitalInputA();
        m_betaSource = giveDigitalInputB();


        m_encoder = new Encoder(m_alphaSource, m_betaSource);
        m_counters[0] = new Counter(m_alphaSource);
        m_counters[1] = new Counter(m_betaSource);
        logger.fine("Creating the speed controller!");
        m_motor = giveSpeedController();
      }
    }
  }

  @Override
  public boolean setup() {
    initialize();
    return true;
  }

  /**
   * Gets the motor for this Object.
   *
   * @return the motor this object refers too
   */
  public T getMotor() {
    initialize();
    return m_motor;
  }

  /**
   * Gets the encoder for this object.
   *
   * @return the encoder that this object refers too
   */
  public Encoder getEncoder() {
    initialize();
    return m_encoder;
  }

  public Counter[] getCounters() {
    initialize();
    return m_counters;
  }

  /**
   * Retrieves the name of the motor that this object refers to.
   *
   * @return The simple name of the motor {@link Class#getSimpleName()}
   */
  public String getType() {
    initialize();
    return m_motor.getClass().getSimpleName();
  }

  /**
   * Checks to see if the speed of the motor is within some range of a given value. This is used
   * instead of equals() because doubles can have inaccuracies.
   *
   * @param value    The value to compare against
   * @param accuracy The accuracy range to check against to see if the
   * @return true if the range of values between motors speed accuracy contains the 'value'.
   */
  public boolean isMotorSpeedWithinRange(double value, double accuracy) {
    initialize();
    return Math.abs((Math.abs(m_motor.get()) - Math.abs(value))) < Math.abs(accuracy);
  }

  @Override
  public boolean reset() {
    initialize();
    m_motor.setInverted(false);
    m_motor.set(0);
    Timer.delay(TestBench.MOTOR_STOP_TIME); // DEFINED IN THE TestBench
    m_encoder.reset();
    for (Counter c : m_counters) {
      c.reset();
    }

    boolean wasReset = true;
    wasReset = wasReset && m_motor.get() == 0;
    wasReset = wasReset && m_encoder.get() == 0;
    for (Counter c : m_counters) {
      wasReset = wasReset && c.get() == 0;
    }

    return wasReset;
  }


  /**
   * Safely tears down the MotorEncoder Fixture in a way that makes sure that even if an object
   * fails to initialize the rest of the fixture can still be torn down and the resources
   * deallocated.
   */
  @Override
  public boolean teardown() {
    String type;
    if (m_motor != null) {
      type = getType();
    } else {
      type = "null";
    }
    if (!m_tornDown) {
      boolean wasNull = false;
      if (m_motor instanceof PWM && m_motor != null) {
        ((PWM) m_motor).free();
        m_motor = null;
      } else if (m_motor == null) {
        wasNull = true;
      }
      if (m_encoder != null) {
        m_encoder.free();
        m_encoder = null;
      } else {
        wasNull = true;
      }
      if (m_counters[0] != null) {
        m_counters[0].free();
        m_counters[0] = null;
      } else {
        wasNull = true;
      }
      if (m_counters[1] != null) {
        m_counters[1].free();
        m_counters[1] = null;
      } else {
        wasNull = true;
      }
      if (m_alphaSource != null) {
        m_alphaSource.free();
        m_alphaSource = null;
      } else {
        wasNull = true;
      }
      if (m_betaSource != null) {
        m_betaSource.free();
        m_betaSource = null;
      } else {
        wasNull = true;
      }

      m_tornDown = true;

      if (wasNull) {
        throw new NullPointerException("MotorEncoderFixture had null params at teardown");
      }
    } else {
      throw new RuntimeException(type + " Motor Encoder torn down multiple times");
    }

    return true;
  }

  @Override
  public String toString() {
    StringBuilder string = new StringBuilder("MotorEncoderFixture<");
    // Get the generic type as a class
    @SuppressWarnings("unchecked")
    Class<T> class1 =
        (Class<T>) ((ParameterizedType) getClass().getGenericSuperclass())
            .getActualTypeArguments()[0];
    string.append(class1.getSimpleName());
    string.append(">");
    return string.toString();
  }

}
