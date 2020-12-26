// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.buttons;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.Scheduler;
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
public abstract class Trigger implements Sendable {
  private volatile boolean m_sendablePressed;

  /**
   * Returns whether or not the trigger is active.
   *
   * <p>This method will be called repeatedly a command is linked to the Trigger.
   *
   * @return whether or not the trigger condition is active.
   */
  public abstract boolean get();

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
   * @param command the command to start
   */
  public void whenActive(final Command command) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (!m_pressedLast && pressed) {
          command.start();
        }

        m_pressedLast = pressed;
      }
    }.start();
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * {@link Command#start()} will be called repeatedly while the trigger is active, and will be
   * canceled when the trigger becomes inactive.
   *
   * @param command the command to start
   */
  public void whileActive(final Command command) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (pressed) {
          command.start();
        } else if (m_pressedLast && !pressed) {
          command.cancel();
        }

        m_pressedLast = pressed;
      }
    }.start();
  }

  /**
   * Starts the command when the trigger becomes inactive.
   *
   * @param command the command to start
   */
  public void whenInactive(final Command command) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (m_pressedLast && !pressed) {
          command.start();
        }

        m_pressedLast = pressed;
      }
    }.start();
  }

  /**
   * Toggles a command when the trigger becomes active.
   *
   * @param command the command to toggle
   */
  public void toggleWhenActive(final Command command) {
    new ButtonScheduler() {
      private boolean m_pressedLast = grab();

      @Override
      public void execute() {
        boolean pressed = grab();

        if (!m_pressedLast && pressed) {
          if (command.isRunning()) {
            command.cancel();
          } else {
            command.start();
          }
        }

        m_pressedLast = pressed;
      }
    }.start();
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
      Scheduler.getInstance().addButton(this);
    }
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Button");
    builder.setSafeState(() -> m_sendablePressed = false);
    builder.addBooleanProperty("pressed", this::grab, value -> m_sendablePressed = value);
  }
}
