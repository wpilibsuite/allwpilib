// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.Relay.Direction;
import edu.wpi.first.wpilibj.Relay.InvalidValueException;
import edu.wpi.first.wpilibj.Relay.Value;
import edu.wpi.first.wpilibj.fixtures.RelayCrossConnectFixture;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilderImpl;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import edu.wpi.first.wpilibj.test.TestBench;
import java.util.logging.Logger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

/** Tests the {@link RelayCrossConnectFixture}. */
public class RelayCrossConnectTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(RelayCrossConnectTest.class.getName());
  private static final NetworkTable table =
      NetworkTableInstance.getDefault().getTable("_RELAY_CROSS_CONNECT_TEST_");
  private RelayCrossConnectFixture m_relayFixture;
  private SendableBuilderImpl m_builder;

  @BeforeEach
  public void setUp() {
    m_relayFixture = TestBench.getRelayCrossConnectFixture();
    m_relayFixture.setup();
    m_builder = new SendableBuilderImpl();
    m_builder.setTable(table);
    m_relayFixture.getRelay().initSendable(m_builder);
  }

  @AfterEach
  public void tearDown() {
    m_relayFixture.reset();
    m_relayFixture.teardown();
  }

  @Test
  public void testBothHigh() {
    m_relayFixture.getRelay().setDirection(Direction.kBoth);
    m_relayFixture.getRelay().set(Value.kOn);
    m_builder.update();
    assertTrue(
        m_relayFixture.getInputOne().get(), "Input one was not high when relay set both high");
    assertTrue(
        m_relayFixture.getInputTwo().get(), "Input two was not high when relay set both high");
    assertEquals(Value.kOn, m_relayFixture.getRelay().get());
    assertEquals("On", table.getEntry("Value").getString(""));
  }

  @Test
  public void testFirstHigh() {
    m_relayFixture.getRelay().setDirection(Direction.kBoth);
    m_relayFixture.getRelay().set(Value.kForward);
    m_builder.update();
    assertFalse(
        m_relayFixture.getInputOne().get(), "Input one was not low when relay set Value.kForward");
    assertTrue(
        m_relayFixture.getInputTwo().get(), "Input two was not high when relay set Value.kForward");
    assertEquals(Value.kForward, m_relayFixture.getRelay().get());
    assertEquals("Forward", table.getEntry("Value").getString(""));
  }

  @Test
  public void testSecondHigh() {
    m_relayFixture.getRelay().setDirection(Direction.kBoth);
    m_relayFixture.getRelay().set(Value.kReverse);
    m_builder.update();
    assertTrue(
        m_relayFixture.getInputOne().get(), "Input one was not high when relay set Value.kReverse");
    assertFalse(
        m_relayFixture.getInputTwo().get(), "Input two was not low when relay set Value.kReverse");
    assertEquals(Value.kReverse, m_relayFixture.getRelay().get());
    assertEquals("Reverse", table.getEntry("Value").getString(""));
  }

  @Test
  public void testForwardDirection() {
    m_relayFixture.getRelay().setDirection(Direction.kForward);
    m_relayFixture.getRelay().set(Value.kOn);
    m_builder.update();
    assertFalse(
        m_relayFixture.getInputOne().get(),
        "Input one was not low when relay set Value.kOn in kForward Direction");
    assertTrue(
        m_relayFixture.getInputTwo().get(),
        "Input two was not high when relay set Value.kOn in kForward Direction");
    assertEquals(Value.kOn, m_relayFixture.getRelay().get());
    assertEquals("On", table.getEntry("Value").getString(""));
  }

  @Test
  public void testReverseDirection() {
    m_relayFixture.getRelay().setDirection(Direction.kReverse);
    m_relayFixture.getRelay().set(Value.kOn);
    m_builder.update();
    assertTrue(
        m_relayFixture.getInputOne().get(),
        "Input one was not high when relay set Value.kOn in kReverse Direction");
    assertFalse(
        m_relayFixture.getInputTwo().get(),
        "Input two was not low when relay set Value.kOn in kReverse Direction");
    assertEquals(Value.kOn, m_relayFixture.getRelay().get());
    assertEquals("On", table.getEntry("Value").getString(""));
  }

  @Test
  public void testSetValueForwardWithDirectionReverseThrowingException() {
    m_relayFixture.getRelay().setDirection(Direction.kForward);
    assertThrows(
        InvalidValueException.class,
        () -> {
          m_relayFixture.getRelay().set(Value.kReverse);
        });
  }

  @Test
  public void testSetValueReverseWithDirectionForwardThrowingException() {
    m_relayFixture.getRelay().setDirection(Direction.kReverse);
    assertThrows(
        InvalidValueException.class,
        () -> {
          m_relayFixture.getRelay().set(Value.kForward);
        });
  }

  @Test
  public void testInitialSettings() {
    m_builder.update();
    assertEquals(Value.kOff, m_relayFixture.getRelay().get());
    // Initially both outputs should be off
    assertFalse(m_relayFixture.getInputOne().get());
    assertFalse(m_relayFixture.getInputTwo().get());
    assertEquals("Off", table.getEntry("Value").getString(""));
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }
}
