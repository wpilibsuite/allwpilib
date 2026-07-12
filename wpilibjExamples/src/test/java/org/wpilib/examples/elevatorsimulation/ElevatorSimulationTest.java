// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.elevatorsimulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.EncoderSim;
import org.wpilib.simulation.JoystickSim;
import org.wpilib.simulation.PWMMotorControllerSim;
import org.wpilib.simulation.RoboRioSim;
import org.wpilib.simulation.SimHooks;

@ResourceLock("timing")
class ElevatorSimulationTest {
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
    RoboRioSim.resetData();
    DriverStationSim.resetData();
    DriverStationSim.notifyNewData();
  }

  @Test
  void teleopTest() {
    // teleop init
    {
      DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
      DriverStationSim.setEnabled(true);
      DriverStationSim.notifyNewData();

      assertTrue(encoderSim.getInitialized());
    }

    {
      // advance 50 timesteps
      SimHooks.stepTiming(1);

      // Ensure elevator is still at 0.
      assertEquals(0.0, encoderSim.getDistance(), 0.05);
    }

    {
      // Press button to reach setpoint
      joystickSim.setTrigger(true);
      joystickSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(1.5);

      assertEquals(Constants.kSetpoint, encoderSim.getDistance(), 0.05);

      // advance 25 timesteps to see setpoint is held.
      SimHooks.stepTiming(0.5);

      assertEquals(Constants.kSetpoint, encoderSim.getDistance(), 0.05);
    }

    {
      // Unpress the button to go back down
      joystickSim.setTrigger(false);
      joystickSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(1.5);

      assertEquals(0.0, encoderSim.getDistance(), 0.05);
    }

    {
      // Press button to go back up
      joystickSim.setTrigger(true);
      joystickSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(1.5);

      assertEquals(Constants.kSetpoint, encoderSim.getDistance(), 0.05);

      // advance 25 timesteps to see setpoint is held.
      SimHooks.stepTiming(0.5);

      assertEquals(Constants.kSetpoint, encoderSim.getDistance(), 0.05);
    }

    {
      // Disable
      DriverStationSim.setEnabled(false);
      DriverStationSim.notifyNewData();

      // advance 75 timesteps
      SimHooks.stepTiming(1.5);

      assertEquals(0.0, motorSim.getThrottle(), 0.05);
      assertEquals(0.0, encoderSim.getDistance(), 0.05);
    }
  }
}
