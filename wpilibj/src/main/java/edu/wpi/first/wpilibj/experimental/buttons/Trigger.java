/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.buttons;

import java.util.function.BooleanSupplier;

import edu.wpi.first.wpilibj.SendableBase;
import edu.wpi.first.wpilibj.experimental.command.Command;
import edu.wpi.first.wpilibj.experimental.command.CommandScheduler;
import edu.wpi.first.wpilibj.experimental.command.InstantCommand;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * This class provides an easy way to link commands to inputs.
 *
 * <p>It is very easy to link a button to a command. For instance, you could link the trigger
 * button of a joystick to a "score" command.
 *
 * <p>It is encouraged that teams write a subclass of Trigger if they want to have something unusual
 * (for instance, if they want to react to the user holding a button while the robot is reading a
 * certain sensor input). For this, they only have to write the {@link Trigger#get()} method to get
 * the full functionality of the Trigger class.
 */
@SuppressWarnings("PMD.TooManyMethods")
public class Trigger extends SendableBase {
  private volatile boolean m_sendablePressed;
  private final BooleanSupplier m_isActive;

  /**
   * Creates a new trigger with the given condition determining whether it is active.
   *
   * @param isActive returns whether or not the trigger should be active
   */
  public Trigger(BooleanSupplier isActive) {
    m_isActive = isActive;
  }

  /**
   * Creates a new trigger that is always inactive.  Useful only as a no-arg constructor
   * for subclasses that will be overriding {@link Trigger#get()} anyway.
   */
  public Trigger() {
    m_isActive = () -> false;
  }

  /**
   * Returns whether or not the trigger is active.
   *
   * <p>This method will be called repeatedly a command is linked to the Trigger.
   *
   * @return whether or not the trigger condition is active.
   */
  public boolean get() {
    return m_isActive.getAsBoolean();
  }

  /**
   * Returns whether get() return true or the internal table for SmartDashboard use is pressed.
   *
   * @return whether get() return true or the internal table for SmartDashboard use is pressed.
   */
  @SuppressWarnings("PMD.UselessParentheses")
  private boolean grab() {
    return get() || m_sendablePressed;
  }

  /**
   * Starts the given command whenever the trigger just becomes active.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public void whenActive(final Command command, boolean interruptible) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (!m_pressedLast && pressed) {
          command.schedule(interruptible);
        }

        m_pressedLast = pressed;
      }
    }.start();
  }

  /**
   * Starts the given command whenever the trigger just becomes active.  The command is set to be
   * interruptible.
   *
   * @param command the command to start
   */
  public void whenActive(final Command command) {
    whenActive(command, true);
  }

  /**
   * Runs the given runnable whenever the trigger just becomes active.
   *
   * @param toRun the runnable to run
   */
  public void whenActive(final Runnable toRun) {
    whenActive(new InstantCommand(toRun));
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * {@link Command#schedule(boolean)} will be called repeatedly while the trigger is active, and
   * will be canceled when the trigger becomes inactive.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public void whileActiveContinuous(final Command command, boolean interruptible) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (pressed) {
          command.schedule(interruptible);
        } else if (m_pressedLast && !pressed) {
          command.cancel();
        }

        m_pressedLast = pressed;
      }
    }.start();
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * {@link Command#schedule(boolean)} will be called repeatedly while the trigger is active, and
   * will be canceled when the trigger becomes inactive.  The command is set to be interruptible.
   *
   * @param command the command to start
   */
  public void whileActiveContinuous(final Command command) {
    whileActiveContinuous(command, true);
  }

  /**
   * Constantly runs the given runnable while the button is held.
   *
   * @param toRun the runnable to run
   */
  public void whileActiveContinuous(final Runnable toRun) {
    whileActiveContinuous(new InstantCommand(toRun));
  }

  /**
   * Starts the given command when the trigger initially becomes active, and ends it when it
   * becomes inactive, but does not re-start it in-between.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public void whileActiveOnce(final Command command, boolean interruptible) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (!m_pressedLast && pressed) {
          command.schedule(interruptible);
        } else if (m_pressedLast && !pressed) {
          command.cancel();
        }

        m_pressedLast = pressed;
      }
    }.start();
  }

  /**
   * Starts the given command when the trigger initially becomes active, and ends it when it
   * becomes inactive, but does not re-start it in-between.  The command is set to be interruptible.
   *
   * @param command the command to start
   */
  public void whileActiveOnce(final Command command) {
    whileActiveOnce(command, true);
  }

  /**
   * Starts the command when the trigger becomes inactive.
   *
   * @param command       the command to start
   * @param interruptible whether the command is interruptible
   */
  public void whenInactive(final Command command, boolean interruptible) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (m_pressedLast && !pressed) {
          command.schedule(interruptible);
        }

        m_pressedLast = pressed;
      }
    }.start();
  }

  /**
   * Starts the command when the trigger becomes inactive.  The command is set to be interruptible.
   *
   * @param command the command to start
   */
  public void whenInactive(final Command command) {
    whenInactive(command, true);
  }

  /**
   * Runs the given runnable when the trigger becomes inactive.
   *
   * @param toRun the runnable to run
   */
  public void whenInactive(final Runnable toRun) {
    whenInactive(new InstantCommand(toRun));
  }

  /**
   * Toggles a command when the trigger becomes active.
   *
   * @param command       the command to toggle
   * @param interruptible whether the command is interruptible
   */
  public void toggleWhenActive(final Command command, boolean interruptible) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (!m_pressedLast && pressed) {
          if (command.isScheduled()) {
            command.cancel();
          } else {
            command.schedule(interruptible);
          }
        }

        m_pressedLast = pressed;
      }
    }.start();
  }

  /**
   * Toggles a command when the trigger becomes active.  The command is set to be interruptible.
   *
   * @param command the command to toggle
   */
  public void toggleWhenActive(final Command command) {
    toggleWhenActive(command, true);
  }

  /**
   * Cancels a command when the trigger becomes active.
   *
   * @param command the command to cancel
   */
  public void cancelWhenActive(final Command command) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (!m_pressedLast && pressed) {
          command.cancel();
        }

        m_pressedLast = pressed;
      }
    }.start();
  }

  /**
   * An internal class of {@link Trigger}. The user should ignore this, it is only public to
   * interface between packages.
   */
  public abstract static class ButtonScheduler {
    public abstract void execute();

    public void start() {
      CommandScheduler.getInstance().addButton(this);
    }
  }

  /**
   * Composes this trigger with another trigger, returning a new trigger that is active when both
   * triggers are active.
   *
   * @param trigger the trigger to compose with
   * @return the trigger that is active when both triggers are active
   */
  public Trigger and(Trigger trigger) {
    return new Trigger(() -> grab() && trigger.grab());
  }

  /**
   * Composes this trigger with another trigger, returning a new trigger that is active when either
   * trigger is active.
   *
   * @param trigger the trigger to compose with
   * @return the trigger that is active when either trigger is active
   */
  public Trigger or(Trigger trigger) {
    return new Trigger(() -> grab() || trigger.grab());
  }

  /**
   * Creates a new trigger that is active when this trigger is inactive, i.e. that acts as the
   * negation of this trigger.
   *
   * @return the negated trigger
   */
  public Trigger negate() {
    return new Trigger(() -> !grab());
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Button");
    builder.setSafeState(() -> m_sendablePressed = false);
    builder.addBooleanProperty("pressed", this::grab, value -> m_sendablePressed = value);
  }
}
