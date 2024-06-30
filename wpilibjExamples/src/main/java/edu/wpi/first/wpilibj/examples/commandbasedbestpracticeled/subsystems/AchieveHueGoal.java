// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/*
 * Example of a subsystem that uses a (PID) controller to achieve a goal (setpoint).
 *
 * There are several possible ways to implement this function.
 * 
 * Some teams use the structure of a perpetually running subsystem or other class that is the
 * controller that periodically updates. No commands are used to calculate or schedule
 * calculations. It responds to commands only to accept the goal (setpoint). There are a few ways
 * the periodic update can be scheduled:
 * 
 *  use the Subsystem.periodic() method (not recommended and may be deprecated sometime),
 * 
 *  inject the Robot.addPeriodic() method (allows update faster or slower than the normal loop but
 *  is hard to specify exactly where to sync with other commands - run first is nearly precise but
 *  run last is only approximated),
 * 
 *  use team supplied runBeforeCommands() and runAfterCommands() methods surrounding the
 *  Robot.CommandScheduler.getInstance() method.
 * 
 * Those goal-setting commands or other schemes needed in the above controllers may or may not be
 * protected by use of their own subsystem.
 * 
 * The above schemes might not be fully in the spirit of command-based so this example is not
 * encouraging that usage.
 * 
 * There are few ways to run a controller scheduled from the Command Loop.
 * 
 * The default command will run perpetually if there is no other overriding command issued. That
 * default command can encompass the controller and manage the goal setting (setpoint). The default
 * command has an advantage of automatically restarting if interrupted for any reason.
 * 
 * A normal command can run perpetually and encompass the controller and manage the goal (setpoint).
 * Normal commands do not restart if interrupted (unless some periodic monitoring process is added
 * such as a default command or Robot.periodic() method to restart the interrupted command).
 * 
 * The best alternative to all the above schemes usually is a controller is run by command when it
 * is needed.
 * 
 * A suggestion by CD @Amicus1 for those against using commands except to set the goal:
 * "If this is a verbosity of code issue, I suggest writing the logic as a private subsystem method
 * and exposing it as a command factory."
 * 
 * That is the bulk of the controller does not have to be coded "inline" making a very long command.
 * The logic of the controller doesn't even have to be within a command or subsystem but can reside
 * in its own class. The example of using a PID controller in this example subsystem works like
 * that. The WPILib PIDController is an essentially independent class with all the PID logic. It is
 * used by a command that accepts a setpoint and calls the appropriate calculation and setpoint
 * methods. The command and thus the calculations run continuously to the goal as they are needed.
 * 
 * In this example another command runs perpetually to display on LEDs the continuous output LED
 * pattern from the PID controller.
 * 
 * Thus, this example of PID controller to reach a goal of a selected color is actually a
 * combination of two of the above command-based periodic schemes.
 * 
 * First, a WPILib PID controller class is used on demand of a command to converge to the selected
 * color. The iterative command loop paces the calculations.
 * 
 * Second, the display of the progress of the PID controller calculations to achieve
 * the selected color is by a continuously running command that was scheduled when the robot code
 * started. That command displays the color to be displayed at each iteration of the command loop.
 * In this particular complex example the code was a bit cleaner with the LED color display running
 * essentially hidden in the background without the additional parallel command surrounding the
 * controller command with to simultaneously display on the LEDs.
 * 
 * The advantage of a "hidden" command is it is more modular with simpler modules. The disadvantage
 * is you don't see the whole picture as readily and forget the proper use of a hidden function.
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
 * Use a PID controller to achieve (slowly by over-damped kP gain) a color hue goal set by joystick
 * right trigger axis and display progress toward goal on the LEDs.
 */
public class AchieveHueGoal extends SubsystemBase {

  private final PIDController m_hueController;
  private double m_currentStateHue; // both the input and output of the controller
                                  // (simple example without the need of separate input and output)
  private LEDPattern m_notSeekingGoalSignal = LEDPattern.solid(Color.kGray); // controller off signal
   // initial state signal then continuously controller output signal to display on LEDs
  private LEDPattern m_currentStateSignal = m_notSeekingGoalSignal;
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
     *  LED command will display continuously in the "background" as patterns change.
     */
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
  public Command achieveHue(DoubleSupplier hueSetpoint) {
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

        waitSeconds(2.0) // let the LEDs blink awhile by the LED display command in the background
      )
      .finallyDo(this::reset); // the controller was mostly stopped or interrupted above
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
    m_currentStateSignal = m_notSeekingGoalSignal;
      }

  /**
   * Immediately interrupt the controller command.
   * 
   * <p>Null command works merely by its existence; assuming the underlying controller command allows interrupts
   * and cleanly handles interrupts with "finallyDo()".
   * 
   * @return Command to interrupt and stop the controller while it's running as a command
   */
  public final Command interrupt() {
    return
      runOnce(() -> {});
    }

  /**
   * Display current controller status signal
   * 
   * @return Command to set color display
   */
  public final Command achieveHueDisplay() {
    return
      m_robotSignals.setSignal(()-> m_currentStateSignal);
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

/** The above example code of a PID controller usage follows #1 of @Oblarg post on Chief Delphi:
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