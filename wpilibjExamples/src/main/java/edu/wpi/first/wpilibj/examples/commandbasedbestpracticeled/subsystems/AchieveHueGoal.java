// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/*
 * Example of a subsystem that is a (PID) controller to achieve a goal (setpoint).
 *
 * WHAT THIS IS NOT:
 * Some teams use the structure of a (perpetually, possibly) running system (class) that is
 * self-contained and periodically updated within itself. No commands are used to calculate or
 * schedule calculations. It responds to commands only to accept the goal (setpoint).
 * 
 * That scheme might not be fully in the spirit of command-based so this example is not encouraging
 * that usage.
 * 
 * A suggestion by CD @Amicus1 for those against using commands except to set the goal:
 * If this is a verbosity of code issue, I suggest writing the logic as a private subsystem method
 * and exposing it as a command factory.
 */

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.sequence;

import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDView;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

import java.util.function.DoubleSupplier;

/**
 * PID controller to achieve (slowly by over-damped kP gain) a color hue goal set by joystick right
 * trigger axis and display progress toward goal on the LEDs.
 */
public class AchieveHueGoal extends SubsystemBase {

  private final PIDController m_hueController;
  double m_hueSetpoint;
  double m_currentStateHue;
  LEDPattern m_currentStateSignal; // what to display
  private final LEDView m_robotSignals; // where the output is displayed

  /**
   * Constructor
   *
   * @param robotSignals
   */
  public AchieveHueGoal(LEDView robotSignals) {
    this.m_robotSignals = robotSignals;
    /**
     *  PID initialization.
     *  The PID controller is ready but not running initially until a command is issued
     *  with a setpoint.
     */
    m_currentStateHue = 0.0; // also considered the initial and previous state
    final double kP = 0.025;
    final double kI = 0.0;
    final double kD = 0.0;
    m_hueController = new PIDController(kP, kI, kD);
    final double tolerance = 2.0; // hue range is 0 to 180
    m_hueController.setTolerance(tolerance);
  }

  // Methods that change the subsystem should be private.
  // Methods that inquire about the system must be public.
  // Triggers bound within should be private.

  /**
   * Set the Goal and Move Toward The Goal.
   * 
   * <p>Runs until the goal has been achieved within the tolerance at which time the end is
   * indicated and the controller/command stops.
   * 
   * @param goal dynamically supplied hue 0 to 180 (computer version of a color wheel)
   * @return command used to set and achieve the goal
   */
  public Command displayHue(DoubleSupplier hueSetpoint) {
    final double minimumHue = 0.0;
    final double maximumHue = 180.0;
    return
      sequence(

        runOnce(()->{ // be sure of a fresh start
          m_hueController.reset();
          m_currentStateHue = 0; // also considered the initial and previous state
          m_currentStateSignal = LEDPattern.kOff; // initialize pattern since the deadline below
                                                  // has a race to use it
          }
          ),

        run(() -> { // run to the setpoint displaying state progress as it runs
                m_currentStateHue = // compute the current state
            MathUtil.clamp(
                        m_currentStateHue
                         + m_hueController.calculate(m_currentStateHue, hueSetpoint.getAsDouble()),
              minimumHue,
              maximumHue);
                m_currentStateSignal = // color for the current state
                    LEDPattern.solid(Color.fromHSV((int) m_currentStateHue, 200, 200));
      }
            ).until(m_hueController::atSetpoint)
        .deadlineFor( // display the color of the current state as it's continually recalculated above
        m_robotSignals.setSignal(()->m_currentStateSignal)),

        m_robotSignals.setSignal(()->m_currentStateSignal.blink(Seconds.of(0.1)))
          .withTimeout(3.),

        runOnce(()->{ // be neat; stop other devices as needed
          m_hueController.reset();
          m_currentStateHue = 0; // also considered the initial and previous state
      }
          ),

        m_robotSignals.setSignalOnce(LEDPattern.solid(Color.fromHSV(100, 100, 100))) // off signal
      );
    }

  /**
   * Run before commands and triggers
   */
  public void runBeforeCommands() {}

  /**
   * Run after commands and triggers
   */
  public void runAfterCommands() {}

    /**
     * Example of how to disallow default command
     *
     * @param def default command
     */
    @Override
    public void setDefaultCommand(Command def) {
      throw new IllegalArgumentException("Default Command not allowed");
  }
}
