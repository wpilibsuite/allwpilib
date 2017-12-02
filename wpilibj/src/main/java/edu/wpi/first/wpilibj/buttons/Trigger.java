/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.buttons;

import edu.wpi.first.wpilibj.SendableBase;
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
public abstract class Trigger extends SendableBase {
  private volatile boolean m_sendableState;

  /**
   * Returns whether or not the trigger is active.
   *
   * <p>This method will be called repeatedly a command is linked to the Trigger.
   *
   * @return whether or not the trigger condition is active.
   */
  public abstract boolean get();

  public abstract boolean getPressed();

  public abstract boolean getReleased();

  /**
   * Returns whether get() return true or the internal table for SmartDashboard use is pressed.
   *
   * @return whether get() return true or the internal table for SmartDashboard use is pressed.
   */
  @SuppressWarnings("PMD.UselessParentheses")
  private boolean getSendableState() {
    return m_sendableState;
  }

  /**
   * Starts the given command whenever the trigger just becomes active.
   *
   * @param command the command to start
   */
  public void whenActive(final Command command) {
    new ButtonScheduler() {
      private boolean m_lastState = getSendableState();

      @Override
      public void execute() {
        boolean state = getSendableState();

        if (!m_lastState && state) {
          // If sendable button wasn't pressed and is now, start command
          command.start();
        } else if (getPressed()) {
          // Otherwise, check whether the normal button has been pressed
          command.start();
        }

        m_lastState = state;
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
      private boolean m_lastState = getSendableState();

      @Override
      public void execute() {
        boolean state = getSendableState();

        if (!m_lastState && state) {
          // If sendable button wasn't pressed and is now, start command
          command.start();
        } else if (getPressed()) {
          // Otherwise, check whether the normal button was pressed
          command.start();
        } else if (m_lastState && !state) {
          // If sendable button was pressed and isn't now, stop command
          command.cancel();
        } else if (getReleased()) {
          // Otherwise, check whether the normal button was released
          command.cancel();
        }

        m_lastState = state;
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
      private boolean m_lastState = getSendableState();

      @Override
      public void execute() {
        boolean state = getSendableState();

        if (m_lastState && !state) {
          // If sendable button was pressed and isn't now, start command
          command.start();
        } else if (getReleased()) {
          // Otherwise, check whether the normal button has been released
          command.start();
        }

        m_lastState = state;
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
      private boolean m_lastState = getSendableState();

      @Override
      public void execute() {
        boolean state = getSendableState();

        if (!m_lastState && state) {
          // If sendable button wasn't pressed and is now, start command
          if (command.isRunning()) {
            command.cancel();
          } else {
            command.start();
          }
        } else if (getPressed()) {
          // Otherwise, check whether the normal button has been pressed
          if (command.isRunning()) {
            command.cancel();
          } else {
            command.start();
          }
        }

        m_lastState = state;
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
      private boolean m_lastState = getSendableState();

      @Override
      public void execute() {
        boolean state = getSendableState();

        if (!m_lastState && state) {
          // If sendable button wasn't pressed and is now, cancel command
          command.cancel();
        } else if (getPressed()) {
          // Otherwise, check whether the normal button has been pressed
          command.cancel();
        }

        m_lastState = state;
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
    builder.setSafeState(() -> m_sendableState = false);
    builder.addBooleanProperty("pressed", this::getSendableState, value -> m_sendableState = value);
  }
}
