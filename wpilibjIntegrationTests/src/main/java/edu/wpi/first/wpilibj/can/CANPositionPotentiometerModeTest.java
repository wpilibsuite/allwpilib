/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import com.googlecode.junittoolbox.PollingWait;
import com.googlecode.junittoolbox.RunnableAssert;

import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

import edu.wpi.first.wpilibj.CANJaguar;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.fixtures.MotorEncoderFixture;

import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;

/**
 * Tests the CAN Motor controller in Potentiometer Mode.
 *
 * @author jonathanleitschuh
 */
public class CANPositionPotentiometerModeTest extends AbstractCANTest {
  private static final Logger logger = Logger.getLogger(CANPositionPotentiometerModeTest.class
      .getName());

  private static final double kStoppedValue = 0;

  private static final int defaultPotAngle = 180;
  private static final double maxPotVoltage = 3.0;

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.can.AbstractCANTest#stopMotor()
   */
  protected void stopMotor() {
    getME().getMotor().set(.5);
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.can.AbstractCANTest#runMotorForward()
   */
  protected void runMotorForward() {
    getME().getMotor().set(1);
  }

  /*
   * (non-Javadoc)
   *
   * @see edu.wpi.first.wpilibj.can.AbstractCANTest#runMotorReverse()
   */
  protected void runMotorReverse() {
    getME().getMotor().set(0);
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Before
  public void setUp() throws Exception {
    getME().getMotor().setPositionMode(CANJaguar.kPotentiometer, 5.0, 0.1, 2.0);
    // getME().getMotor().configPotentiometerTurns(rotationRange);
    getME().getFakePot().setMaxVoltage(maxPotVoltage);
    getME().getFakePot().setVoltage(1.5);
    stopMotor();
    getME().getMotor().enableControl();
    /* The motor might still have momentum from the previous test. */
    Timer.delay(kStartupTime);
  }


  /**
   * NOTICE: This is using the {@link MotorEncoderFixture#getEncoder()} instead of the one built
   * into the CAN Jaguar.
   */
  @Ignore("Encoder is not yet wired to the FPGA")
  @Test
  public void testRotateForward() {
    final int initialPosition = getME().getEncoder().get();
    /* Drive the speed controller briefly to move the encoder */
    getME().getMotor().set(kStoppedValue);
    Timer.delay(kMotorTimeSettling);
    getME().getMotor().set(defaultPotAngle);

    /* The position should have increased */
    assertThat("CAN Jaguar position should have increased after the motor moved", getME()
        .getEncoder().get(), is(greaterThan(initialPosition)));

  }

  /**
   * NOTICE: This is using the {@link MotorEncoderFixture#getEncoder()} instead of the one built
   * into the CAN Jaguar.
   */
  @Ignore("Encoder is not yet wired to the FPGA")
  @Test
  public void testRotateReverse() {
    final int initialPosition = getME().getEncoder().get();
    /* Drive the speed controller briefly to move the encoder */
    getME().getMotor().set(kStoppedValue);
    Timer.delay(kMotorTimeSettling);
    getME().getMotor().set(defaultPotAngle);

    /* The position should have increased */
    assertThat("CAN Jaguar position should have increased after the motor moved", getME()
        .getEncoder().get(), is(greaterThan(initialPosition)));

  }

  /**
   * Test if we can get a position in potentiometer mode, using an analog output as a fake
   * potentiometer.
   */
  @Test
  public void testFakePotentiometerPosition() {
    // TODO When https://github.com/Pragmatists/JUnitParams/issues/5 is resolved
    // make this test parameterized

    // Given
    PollingWait wait =
        new PollingWait().timeoutAfter((long) kPotentiometerSettlingTime, TimeUnit.SECONDS)
            .pollEvery(1, TimeUnit.MILLISECONDS);
    RunnableAssert assertion =
        new RunnableAssert("Waiting for potentiometer position to be correct") {
          @Override
          public void run() throws Exception {
            getME().getMotor().set(0);
            assertEquals(
                "CAN Jaguar should have returned the potentiometer position set by the analog "
                    + "output",
                getME().getFakePot().getVoltage(), getME().getMotor().getPosition() * 3,
                kPotentiometerPositionTolerance * 3);
          }
        };

    // When
    getME().getFakePot().setVoltage(0.0);
    // Then
    wait.until(assertion);

    // When
    getME().getFakePot().setVoltage(1.0);
    // Then
    wait.until(assertion);

    // When
    getME().getFakePot().setVoltage(2.0);
    // Then
    wait.until(assertion);

    // When
    getME().getFakePot().setVoltage(3.0);
    // Then
    wait.until(assertion);
  }

}
