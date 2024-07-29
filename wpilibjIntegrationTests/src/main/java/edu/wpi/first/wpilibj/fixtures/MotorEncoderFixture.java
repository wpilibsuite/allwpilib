// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.fixtures;

import edu.wpi.first.wpilibj.Counter;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWM;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.motorcontrol.MotorController;
import edu.wpi.first.wpilibj.test.TestBench;
import java.lang.reflect.ParameterizedType;
import java.util.logging.Logger;

/**
 * Represents a physically connected Motor and Encoder to allow for unit tests on these different
 * pairs<br>
 * Designed to allow the user to easily setup and tear down the fixture to allow for reuse. This
 * class should be explicitly instantiated in the TestBed class to allow any test to access this
 * fixture. This allows tests to be mailable so that you can easily reconfigure the physical testbed
 * without breaking the tests.
 */
@SuppressWarnings("removal")
public abstract class MotorEncoderFixture<T extends MotorController> implements ITestFixture {
  private static final Logger logger = Logger.getLogger(MotorEncoderFixture.class.getName());
  private boolean m_initialized = false;
  private boolean m_tornDown = false;
  protected T m_motor;
  private Encoder m_encoder;
  private final Counter[] m_counters = new Counter[2];
  protected DigitalInput m_alphaSource; // Stored so it can be freed at tear down
  protected DigitalInput m_betaSource;

  /** Default constructor for a MotorEncoderFixture. */
  public MotorEncoderFixture() {}

  public abstract int getPDPChannel();

  /**
   * Where the implementer of this class should pass the motor controller Constructor should only be
   * called from outside this class if the Motor controller is not also an implementation of PWM
   * interface.
   *
   * @return MotorController
   */
  protected abstract T giveMotorController();

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

  private void initialize() {
    synchronized (this) {
      if (!m_initialized) {
        m_initialized = true; // This ensures it is only initialized once

        m_alphaSource = giveDigitalInputA();
        m_betaSource = giveDigitalInputB();

        m_encoder = new Encoder(m_alphaSource, m_betaSource);
        m_counters[0] = new Counter(m_alphaSource);
        m_counters[1] = new Counter(m_betaSource);
        logger.fine("Creating the motor controller!");
        m_motor = giveMotorController();
      }
    }
  }

  @Override
  public void setup() {
    initialize();
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
   * @param value The value to compare against
   * @param accuracy The accuracy range to check against to see if the
   * @return true if the range of values between motors speed accuracy contains the 'value'.
   */
  public boolean isMotorSpeedWithinRange(double value, double accuracy) {
    initialize();
    return Math.abs(Math.abs(m_motor.get()) - Math.abs(value)) < Math.abs(accuracy);
  }

  @Override
  public void reset() {
    initialize();
    m_motor.setInverted(false);
    m_motor.set(0);
    Timer.delay(TestBench.MOTOR_STOP_TIME); // DEFINED IN THE TestBench
    m_encoder.reset();
    for (Counter c : m_counters) {
      c.reset();
    }
  }

  /**
   * Safely tears down the MotorEncoder Fixture in a way that makes sure that even if an object
   * fails to initialize the rest of the fixture can still be torn down and the resources
   * deallocated.
   */
  @Override
  public void teardown() {
    if (!m_tornDown) {
      if (m_motor != null) {
        if (m_motor instanceof PWM motor) {
          motor.close();
        }
        m_motor = null;
      }
      if (m_encoder != null) {
        m_encoder.close();
        m_encoder = null;
      }
      if (m_counters[0] != null) {
        m_counters[0].close();
        m_counters[0] = null;
      }
      if (m_counters[1] != null) {
        m_counters[1].close();
        m_counters[1] = null;
      }
      if (m_alphaSource != null) {
        m_alphaSource.close();
        m_alphaSource = null;
      }
      if (m_betaSource != null) {
        m_betaSource.close();
        m_betaSource = null;
      }

      m_tornDown = true;
    }
  }

  @Override
  public String toString() {
    StringBuilder string = new StringBuilder("MotorEncoderFixture<");
    // Get the generic type as a class
    @SuppressWarnings("unchecked")
    Class<T> class1 =
        (Class<T>)
            ((ParameterizedType) getClass().getGenericSuperclass()).getActualTypeArguments()[0];
    string.append(class1.getSimpleName());
    string.append(">");
    return string.toString();
  }
}
