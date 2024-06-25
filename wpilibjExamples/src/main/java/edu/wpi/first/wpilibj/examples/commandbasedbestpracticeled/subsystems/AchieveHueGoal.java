// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/*
 * Example of a subsystem composed only of a goal wrapped in its implementer class to achieve that
 * goal.
 *
 * Note that this is a simple contrived example based on a PID controller. There may be better ways
 * to do PID controller with Commands.
 */

import static edu.wpi.first.units.Units.Seconds;

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
 * trigger and display progress toward goal on the LEDs.
 */
public class AchieveHueGoal {

  private final PIDController m_hueController;
  double m_hueSetpoint;
  double m_currentStateHue;
  private final LEDView m_robotSignals; // where the output is displayed
  public final HueGoal m_hueGoal = new HueGoal(); // subsystem protected goal

  /**
   * Constructor
   *
   * @param robotSignals
   */
  public AchieveHueGoal(LEDView robotSignals) {
    this.m_robotSignals = robotSignals;
    /**
     *  PID initialization.
     * 
     *  The PID controller is ready but not running initially until a setpoint is set. When the
     *  setpoint is set the controller starts and runs forever until the reset function is invoked.
     */
    m_hueSetpoint = Double.NaN; // initialize setpoint (goal) such that the controller doesn't
                                // start until setpoint is set
    m_currentStateHue = 0.0; // also considered the initial and previous state
    final double kP = 0.025;
    final double kI = 0.0;
    final double kD = 0.0;
    m_hueController = new PIDController(kP, kI, kD);
    final double tolerance = 2.0;
    m_hueController.setTolerance(tolerance);
  }

  // Example of methods and triggers that the system will require are put here.

  // Methods that change the subsystem should be private.
  // Methods and triggers that inquire about the system must be public.
  // To get periodic behavior with Command-Based structure the "before" and "after" must be public.
  // There are other methods to get periodic execution. The Robot.addPeriodic could be injected.
  // Subsystem periodic could be used under past threats of deprecation. EventLoop might be of use.

  /**
   * Update Hue Controller Calculation
   * 
   * <p>Must be run periodically based on the Goal that was set.
   */
  private void updateHueController() {
    final double minimumHue = 0.0;
    final double maximumHue = 180.0;
      LEDPattern currentStateSignal;

      if (!Double.isNaN(m_hueSetpoint)) {
        // setpoint has been set so run controller periodically

        // Note that the WPILib PID controller knows if it has a setpoint and measurement
        // but that information is private and not accessible. We need to know that here
      // so the signals stay at their initial state (assumed off) until a setpoint is set.
        m_currentStateHue =
            MathUtil.clamp(
                m_currentStateHue + m_hueController.calculate(m_currentStateHue, m_hueSetpoint),
              minimumHue,
              maximumHue);
          currentStateSignal =
            LEDPattern.solid(Color.fromHSV((int) m_currentStateHue, 200, 200)); // display state;
      } else {
        currentStateSignal =
            LEDPattern.solid(Color.fromHSV(0, 0, 0)); // display state off - black;        
      }

      if (m_hueController.atSetpoint()) {
        currentStateSignal = currentStateSignal.blink(Seconds.of(0.1)); // blink if made it to the Goal
      }
    // changing the state of LED subsystem is by command as it should be for (almost) all subsystems
    m_robotSignals.setSignalOnce(currentStateSignal).schedule();
    }

  /**
   * Run before commands and triggers
   */
  public void runBeforeCommands() {}

  /**
   * Run after commands and triggers
   */
  public void runAfterCommands() {
    updateHueController(); // periodic update of the system (it's not a subsystem - the goal is)
 }

  /**
   * Subsystem to lock the resource if a command is running and provide a default command.
   *
   * <p>A decision must be made on how to set the goal. The controller is always running. Does the
   * goal supplier have to always be running or is it set and forget until a new goal is needed?
   *
   * <p>The default command could be activated to provide the goal if no other goal supplier is
   * running. If set once and forget is used then it is inappropriate to use a default command as
   * that would immediately reset to the default after setting a goal.
   *
   * <p>This subsystem could run "perpetually" with a pre-determined hue goal but it's disabled in
   * this example in favor of no default command to prevent assuming there is one always running.
   *
   * <p>That means the last setpoint is running as no default takes over. For the Xbox trigger, that
   * goes to 0 when released but again we're not supposed to know that from RobotContainer.java.
   * 
   * <p>Note that this implementation does not start the controller until a setpoint as been set.
   * The controller stops when the goal is unset (goal = Double.NaN or reset()).
   *
   * <p>Possible:<pre>
   * Command defaultCommand = runOnce(() -> m_hueSetpoint = defaultHueGoal);
   * setDefaultCommand(defaultCommand);
   * </pre>
   */
  public class HueGoal extends SubsystemBase {
    private HueGoal() {}

    /**
     * Example of how to disallow default command
     *
     * @param def default command
     */
    @Override
    public void setDefaultCommand(Command def) {
      throw new IllegalArgumentException("Default Command not allowed");
    }

    /**
     * Set the goal.
     * 
     * <p>Use this if the goal is known at the time of initialization.
     * 
     * <p>May be called repeatedly for different goals.
     * 
     * <p>Not used in this example program.
     *
     * @param goal hue 0 to 180
     * @return command that can be used to set the goal
     */
    public Command setHueGoal(double goal) {
      return runOnce(() -> m_hueSetpoint = goal);
    }

    /**
     * Set the goal.
     *
     * <p>Use this if the goal is to be supplied dynamically.
     * 
     * <p>Runs forever accepting goals dynamically from the Supplier.
     * 
     * <p>May be interrupted by calling it again to change the Supplier, may cancel by "disable",
     * or may be interrupted by the "reset" command. In those cases this command must be restarted
     * to accept goals.
     * 
     * <p>Used in this example program.
     * 
     * @param goal hue 0 to 180
     * @return command that can be used to set the goal
     */
    public Command setHueGoal(DoubleSupplier goal) {
      return run(() -> m_hueSetpoint = goal.getAsDouble());
    }

    /**
     * reset or stop the controller
     */
    public Command reset() {
      // running any command stops the goal-accepting command
      // setting the hue to NaN flags the controller to stop calculating
      return runOnce(()-> m_hueSetpoint = Double.NaN);
    }
  }
}
