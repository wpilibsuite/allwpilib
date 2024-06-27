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
 * BUT BUT BUT BUT
 * The default command for the LED subsystem is used exactly as described above. The default
 * command is running continuously displaying whatever is currently set as the pattern to display.
 * 
 * A suggestion by CD @Amicus1 for those against using commands except to set the goal:
 * "If this is a verbosity of code issue, I suggest writing the logic as a private subsystem method
 * and exposing it as a command factory."
 */

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.sequence;
import static edu.wpi.first.wpilibj2.command.Commands.waitSeconds;

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
  private double m_currentStateHue; // both the input and output of the controller
  private LEDPattern m_notRunningSignal = LEDPattern.solid(Color.kGray);
  private LEDPattern m_currentStateSignal = m_notRunningSignal; // initially then what to display on LEDs
  private final LEDView m_robotSignals; // where the output is displayed

  /**
   * Constructor
   *
   * @param robotSignals LED subsystem used as output by this subsystem
   */
  public AchieveHueGoal(LEDView robotSignals) {
    m_robotSignals = robotSignals;
    /**
     *  PID initialization.
     * 
     *  The PID controller is ready but not running initially until a command is issued with a
     *  setpoint.
     * 
     *  LED subsystem default command will display continuously in the "background" as patterns
     *  change. Won't need regular display command in command groups but if it was, then asProxy()
     *  needed in group use if default command needs to run in a group, too.
     * 
     *  Considered flipping and using a regular command so there wasn't this perversion of the
     *  default command but I like the default command's ability to restart no matter what happens.
     */
    m_robotSignals.setDefaultCommand(m_robotSignals.setSignal(()-> m_currentStateSignal));
    m_currentStateHue = 0.0; // as input both the initial and previous state
    final double kP = 0.025;
    final double kI = 0.0;
    final double kD = 0.0;
    m_hueController = new PIDController(kP, kI, kD);
    final double tolerance = 2.0; // hue range is 0 to 180
    m_hueController.setTolerance(tolerance);
  }

  /**
   * Set the Goal and Move Toward The Goal.
   * 
   * <p>Runs until the goal has been achieved within the tolerance at which time the end is
   * indicated and the controller/command stops.
   * 
   * @param hueSetpoint the goal is dynamically supplied hue 0 to 180 (computer version of a color wheel)
   * @return command used to set and achieve the goal
   */
  public Command displayHue(DoubleSupplier hueSetpoint) {
    final double minimumHue = 0.0;
    final double maximumHue = 180.0;
    return
      sequence(

        runOnce(this::reset),

        run(() -> { // run to the setpoint displaying state progress as it runs
                m_currentStateHue = // compute the new current state
            MathUtil.clamp(
                        m_currentStateHue
                         + m_hueController.calculate(m_currentStateHue, hueSetpoint.getAsDouble()),
                        minimumHue, maximumHue);
                m_currentStateSignal = // LED color for the current state; default command displays
                    LEDPattern.solid(Color.fromHSV((int) m_currentStateHue, 200, 200));
      }
            )

          .until(m_hueController::atSetpoint), // controller stops; momentum stable or not
          // if the controller needs to keep running, put it in parallel and check atSetpoint to
          // trigger subsequent commands

        // set LED pattern to blink to show controller stopped at setpoint
        runOnce(()-> m_currentStateSignal = m_currentStateSignal.blink(Seconds.of(0.1))),

        waitSeconds(2.0) // let the LEDs blink awhile by the default command in the "background"
      )
      .finallyDo(this::reset); // cleanup the controller it was mostly stopped or interrupted above
  }

  /**
   * Reset or stop the controller
   * 
   * <p>Used initially to assure known stopped state. Used finally to stop anything that needs to.
   */
  public void reset() {
    // also stop other devices as needed but not needed in this example
          m_hueController.reset();
    m_currentStateHue = 0.0; // also considered the initial and previous state
    m_currentStateSignal = m_notRunningSignal;
      }

  /**
   * Immediately interrupt the controller command.
   * 
   * <p>Null command works merely by its existence; assuming the underlying controller command allows interrupts
   * and cleanly handles interrupts with "finallyDo()".
   * 
   * @return Command to interrupt and stop the controller while it's running as a command
   */
  public Command interrupt() {
    return
      runOnce(() -> {});
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
   * Example of how to disallow default command for this subsystem
     *
     * @param def default command
     */
    @Override
    public void setDefaultCommand(Command def) {
      throw new IllegalArgumentException("Default Command not allowed");
  }
}

/** @Oblarg post on Chief Delphi:
 * <pre><code>
 * // Example of two ways to create a PID controller Command as factory in a subsystem
 * // (Don't use the WPILib PIDControllerCommand; create your own with a factory)
 *
 * // in subsystem scope
 * // we need a motor and sensor for feedback
 * private final MotorController motor = new FooMotor();
 * private final Encoder encoder = new Encoder(...);
 *
 * // Example 1 we could have a PIDController as a field in the subsystem itself...
 * private final PIDController controller = new PIDController(...);
 * // this command captures the subsystem's PIDController, like PIDSubsystem
 * public Command moveToPosition(double position) {
 *   return runOnce(controller::reset)
 *         .andThen(run(() -> {
 *                   motor.set(controller.calculate(
 *                   encoder.getPosition(),
 *                   position
 *                   ));
 *         }).finallyDo(motor::stop);
 * }
 *
 * // Example 2 if we don't want to persist the controller in the subsystem after the command ends...
 * // this command captures its *own* controller, like PIDCommand
 * public Command moveToPosition(double position) {
 *   PIDController controller = new PIDController(...);
 *  // we don't have to reset a fresh controller
 *   return run(() -> {
 *                   motor.set(controller.calculate(
 *                   encoder.getPosition(),
 *                   position
 *                   ));
 *         }).finallyDo(motor::stop);
 * }
 * </code></pre>
 */