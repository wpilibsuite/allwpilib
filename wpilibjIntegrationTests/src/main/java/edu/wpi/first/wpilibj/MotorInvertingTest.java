// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.Arrays;
import java.util.Collection;
import java.util.logging.Logger;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

/** Tests Inversion of motors using the MotorController setInverted. */
public class MotorInvertingTest extends AbstractComsSetup {
  static MotorEncoderFixture<?> fixture = null;
  private static final double motorspeed = 0.2;
  private static final double delaytime = 0.3;

  /**
   * Constructs the test.
   *
   * @param afixture The fixture under test.
   */
  public void init(MotorEncoderFixture<?> afixture) {
    logger.fine("Constructor with: " + afixture.getType());
    if (fixture != null && !fixture.equals(afixture)) {
      fixture.teardown();
    }
    fixture = afixture;
    fixture.setup();
  }

  public static Collection<MotorEncoderFixture<?>[]> generateData() {
    // logger.fine("Loading the MotorList");
    return Arrays.asList(
        new MotorEncoderFixture<?>[][] {
          {TestBench.getTalonPair()}, {TestBench.getVictorPair()}, {TestBench.getJaguarPair()}
        });
  }

  private static final Logger logger = Logger.getLogger(MotorInvertingTest.class.getName());

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @BeforeEach
  public void setUp() {
    // Reset the fixture elements before every test
    fixture.reset();
  }

  @AfterAll
  public static void tearDown() {
    fixture.getMotor().setInverted(false);
    // Clean up the fixture after the test
    fixture.teardown();
  }

  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testInvertingPositive(MotorEncoderFixture<?> mef) {
    init(mef);

    fixture.getMotor().setInverted(false);
    fixture.getMotor().set(motorspeed);
    Timer.delay(delaytime);
    final boolean initDirection = fixture.getEncoder().getDirection();
    fixture.getMotor().setInverted(true);
    fixture.getMotor().set(motorspeed);
    Timer.delay(delaytime);
    assertFalse(
        fixture.getEncoder().getDirection() == initDirection,
        "Inverting with Positive value does not change direction");
    fixture.getMotor().set(0);
  }

  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testInvertingNegative(MotorEncoderFixture<?> mef) {
    init(mef);

    fixture.getMotor().setInverted(false);
    fixture.getMotor().set(-motorspeed);
    Timer.delay(delaytime);
    final boolean initDirection = fixture.getEncoder().getDirection();
    fixture.getMotor().setInverted(true);
    fixture.getMotor().set(-motorspeed);
    Timer.delay(delaytime);
    assertFalse(
        fixture.getEncoder().getDirection() == initDirection,
        "Inverting with Negative value does not change direction");
    fixture.getMotor().set(0);
  }

  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testInvertingSwitchingPosToNeg(MotorEncoderFixture<?> mef) {
    init(mef);

    fixture.getMotor().setInverted(false);
    fixture.getMotor().set(motorspeed);
    Timer.delay(delaytime);
    final boolean initDirection = fixture.getEncoder().getDirection();
    fixture.getMotor().setInverted(true);
    fixture.getMotor().set(-motorspeed);
    Timer.delay(delaytime);
    assertTrue(
        fixture.getEncoder().getDirection() == initDirection,
        "Inverting with Switching value does change direction");
    fixture.getMotor().set(0);
  }

  @ParameterizedTest(name = "{index}: {0}")
  @MethodSource("generateData")
  public void testInvertingSwitchingNegToPos(MotorEncoderFixture<?> mef) {
    init(mef);

    fixture.getMotor().setInverted(false);
    fixture.getMotor().set(-motorspeed);
    Timer.delay(delaytime);
    final boolean initDirection = fixture.getEncoder().getDirection();
    fixture.getMotor().setInverted(true);
    fixture.getMotor().set(motorspeed);
    Timer.delay(delaytime);
    assertTrue(
        fixture.getEncoder().getDirection() == initDirection,
        "Inverting with Switching value does change direction");
    fixture.getMotor().set(0);
  }
}
