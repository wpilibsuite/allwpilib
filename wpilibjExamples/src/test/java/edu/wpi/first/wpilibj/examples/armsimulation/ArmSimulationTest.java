// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.armsimulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.Preferences;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj.simulation.EncoderSim;
import edu.wpi.first.wpilibj.simulation.JoystickSim;
import edu.wpi.first.wpilibj.simulation.PWMSim;
import edu.wpi.first.wpilibj.simulation.RoboRioSim;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

@ResourceLock("timing")
class ArmSimulationTest {
  private Robot m_robot;
  private Thread m_thread;

  private PWMSim m_motorSim;
  private EncoderSim m_encoderSim;
  private JoystickSim m_joystickSim;

  @BeforeEach
  void startThread() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    DriverStationSim.resetData();
    m_robot = new Robot();
    m_thread = new Thread(m_robot::startCompetition);
    m_encoderSim = EncoderSim.createForChannel(Constants.kEncoderAChannel);
    m_motorSim = new PWMSim(Constants.kMotorPort);
    m_joystickSim = new JoystickSim(Constants.kJoystickPort);

    m_thread.start();
    SimHooks.stepTiming(0.0); // Wait for Notifiers
  }

  @AfterEach
  void stopThread() {
    m_robot.endCompetition();
    try {
      m_thread.interrupt();
      m_thread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    m_robot.close();
    m_encoderSim.resetData();
    m_motorSim.resetData();
    Preferences.remove(Constants.kArmPKey);
    Preferences.remove(Constants.kArmPositionKey);
    Preferences.removeAll();
    RoboRioSim.resetData();
    DriverStationSim.resetData();
    DriverStationSim.notifyNewData();
  }

  @ValueSource(doubles = {Constants.kDefaultArmSetpointDegrees, 25.0, 50.0})
  @ParameterizedTest
  void teleopTest(double setpoint) {
    assertTrue(Preferences.containsKey(Constants.kArmPositionKey));
    assertTrue(Preferences.containsKey(Constants.kArmPKey));
    Preferences.setDouble(Constants.kArmPositionKey, setpoint);
    assertEquals(setpoint, Preferences.getDouble(Constants.kArmPositionKey, Double.NaN));
    // teleop init
    {
      DriverStationSim.setAutonomous(false);
      DriverStationSim.setEnabled(true);
      DriverStationSim.notifyNewData();

      assertTrue(m_motorSim.getInitialized());
      assertTrue(m_encoderSim.getInitialized());
    }

    {
      // advance 150 timesteps
      SimHooks.stepTiming(3);

      // Ensure arm is still at minimum angle.
      assertEquals(Constants.kMinAngleRads, m_encoderSim.getDistance(), 2.0);
    }

    {
      // Press button to reach setpoint
      m_joystickSim.setTrigger(true);
      m_joystickSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(1.5);

      assertEquals(setpoint, Units.radiansToDegrees(m_encoderSim.getDistance()), 2.0);

      // advance 25 timesteps to see setpoint is held.
      SimHooks.stepTiming(0.5);

      assertEquals(setpoint, Units.radiansToDegrees(m_encoderSim.getDistance()), 2.0);
    }

    {
      // Unpress the button to go back down
      m_joystickSim.setTrigger(false);
      m_joystickSim.notifyNewData();

      // advance 150 timesteps
      SimHooks.stepTiming(3.0);

      assertEquals(Constants.kMinAngleRads, m_encoderSim.getDistance(), 2.0);
    }

    {
      // Press button to go back up
      m_joystickSim.setTrigger(true);
      m_joystickSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(1.5);

      assertEquals(setpoint, Units.radiansToDegrees(m_encoderSim.getDistance()), 2.0);

      // advance 25 timesteps to see setpoint is held.
      SimHooks.stepTiming(0.5);

      assertEquals(setpoint, Units.radiansToDegrees(m_encoderSim.getDistance()), 2.0);
    }

    {
      // Disable
      DriverStationSim.setAutonomous(false);
      DriverStationSim.setEnabled(false);
      DriverStationSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(3.5);

      assertEquals(0.0, m_motorSim.getSpeed(), 0.01);
      assertEquals(Constants.kMinAngleRads, m_encoderSim.getDistance(), 2.0);
    }
  }
}
