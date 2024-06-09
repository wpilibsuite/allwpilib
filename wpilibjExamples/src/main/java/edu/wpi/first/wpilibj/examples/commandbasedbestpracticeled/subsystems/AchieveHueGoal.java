// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/*
 * Example of a subsystem composed only of a goal wrapped in its implementer class to achieve that
 * goal.
 *
 * Note that this is a simple contrived example based on a PID controller. There may be better ways
 * to do PID controller entirely within a subsystem.
 */

import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDView;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.Trigger;

import java.util.function.DoubleSupplier;

/**
 * PID controller to achieve (slowly by over-damped kP gain) a color hue goal set by joystick right
 * trigger and display on the LEDs.
 */
public class AchieveHueGoal {
  // PID initialization.

  // The PID controller is not running initially until a setpoint is set.

  // There is no running command to make new setpoints until the Xbox right trigger axis is
  // pressed at which time a command runs forever to accept new setpoints.
  
  // This is handled in RobotContainer.java and this class shouldn't even know that to be
  // able to comment here on how the setpoint is determined.

  private static final double m_kP = 0.025;
  private static final double m_kI = 0.0;
  private static final double m_kD = 0.0;
  private final PIDController m_hueController = new PIDController(m_kP, m_kI, m_kD);
  private static final double m_minimumHue = 0.0;
  private static final double m_maximumHue = 180.0;
  // initialize setpoint (goal) such that the controller doesn't start until setpoint is set
  private double m_hueSetpoint = Double.NaN;
  private double m_currentStateHue = 0.0; // also considered the initial and previous state

  private final LEDView m_robotSignals; // where the output is displayed

  public final HueGoal m_hueGoal = new HueGoal(); // subsystem protected goal

  /**
   * Constructor
   *
   * @param robotSignals
   */
  public AchieveHueGoal(LEDView robotSignals) {
    this.m_robotSignals = robotSignals;
  }

  // Example of methods and triggers that the system will require are put here.

  // Methods that change the system should be "private".
  // Methods and triggers that inquire about the system should be "public".

  // This particular state inquiry is an example only and isn't used for the demonstration.
  public final Trigger atHueGoal = new Trigger(this::isAtHueGoal);

  private boolean isAtHueGoal() {
    return m_hueController.atSetpoint();
  }

  /**
   * Run before commands and triggers
   */
  public void beforeCommands() {}

  /**
   * Run after commands and triggers
   */
  public void afterCommands() {
    if (!Double.isNaN(m_hueSetpoint)) {
      // setpoint has been set so run controller periodically

      // Note that the WPILib PID controller knows if it has a setpoint and measurement
      // but that information is private and not accessible. We need to know that here
      // so the signals stay at their initial state (assumed off) until a setpoint set.
      m_currentStateHue =
          MathUtil.clamp(
              m_currentStateHue + m_hueController.calculate(m_currentStateHue, m_hueSetpoint),
              m_minimumHue,
              m_maximumHue);
      LEDPattern persistentPatternDemo =
          LEDPattern.solid(Color.fromHSV((int) m_currentStateHue, 200, 200)); // display state;
      m_robotSignals.setSignal(persistentPatternDemo).schedule(); // access to the LEDs is only by
                                                    // command in this example so do it that way. 
    }
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
   *
   * <p>Command defaultCommand = Commands.run( () -> m_hueSetpoint = defaultHueGoal , this);
   * setDefaultCommand(defaultCommand);
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
     * <p>Generally not needed but may be interrupted by calling it again to change the Supplier
     * or may need to be "refreshed" if stopped by "disable".
     * 
     * <p>Used in this example program.
     * 
     * @param goal hue 0 to 180
     * @return command that can be used to set the goal
     */
    public Command setHueGoal(DoubleSupplier goal) {
      return run(() -> m_hueSetpoint = goal.getAsDouble());
    }
  }
}
