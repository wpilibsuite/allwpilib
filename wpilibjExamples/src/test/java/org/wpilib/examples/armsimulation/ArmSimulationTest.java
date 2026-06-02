// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.armsimulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.math.util.Units;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.JoystickSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.simulation.SimHooks;
import org.wpilib.util.Preferences;

@ResourceLock("timing")
class ArmSimulationTest {
  private Robot robot;
  private Thread thread;

  private PWMMotorControllerSim motorSim;
  private EncoderSim encoderSim;
  private JoystickSim joystickSim;

  @BeforeEach
  void startThread() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    SimHooks.setProgramStarted(false);
    DriverStationSim.resetData();
    robot = new Robot();
    thread = new Thread(robot::startCompetition);
    encoderSim = EncoderSim.createForChannel(Constants.kEncoderAChannel);
    motorSim = new PWMMotorControllerSim(Constants.kMotorPort);
    joystickSim = new JoystickSim(Constants.kJoystickPort);

    thread.start();
    SimHooks.waitForProgramStart();
  }

  @AfterEach
  void stopThread() {
    robot.endCompetition();
    try {
      thread.interrupt();
      thread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    robot.close();
    encoderSim.resetData();
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
      DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
      DriverStationSim.setEnabled(true);
      DriverStationSim.notifyNewData();

      assertTrue(encoderSim.getInitialized());
    }

    {
      // advance 150 timesteps
      SimHooks.stepTiming(3);

      // Ensure arm is still at minimum angle.
      assertEquals(Constants.kMinAngleRads, encoderSim.getDistance(), 2.0);
    }

    {
      // Press button to reach setpoint
      joystickSim.setTrigger(true);
      joystickSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(1.5);

      assertEquals(setpoint, Units.radiansToDegrees(encoderSim.getDistance()), 2.0);

      // advance 25 timesteps to see setpoint is held.
      SimHooks.stepTiming(0.5);

      assertEquals(setpoint, Units.radiansToDegrees(encoderSim.getDistance()), 2.0);
    }

    {
      // Unpress the button to go back down
      joystickSim.setTrigger(false);
      joystickSim.notifyNewData();

      // advance 150 timesteps
      SimHooks.stepTiming(3.0);

      assertEquals(Constants.kMinAngleRads, encoderSim.getDistance(), 2.0);
    }

    {
      // Press button to go back up
      joystickSim.setTrigger(true);
      joystickSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(1.5);

      assertEquals(setpoint, Units.radiansToDegrees(encoderSim.getDistance()), 2.0);

      // advance 25 timesteps to see setpoint is held.
      SimHooks.stepTiming(0.5);

      assertEquals(setpoint, Units.radiansToDegrees(encoderSim.getDistance()), 2.0);
    }

    {
      // Disable
      DriverStationSim.setEnabled(false);
      DriverStationSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(3.5);

      assertEquals(0.0, motorSim.getThrottle(), 0.01);
      assertEquals(Constants.kMinAngleRads, encoderSim.getDistance(), 2.0);
    }
  }
}
