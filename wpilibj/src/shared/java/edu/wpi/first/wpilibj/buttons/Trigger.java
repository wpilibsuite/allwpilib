/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.buttons;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * This class provides an easy way to link commands to inputs.
 *
 * It is very easy to link a button to a command. For instance, you could link
 * the trigger button of a joystick to a "score" command.
 *
 * It is encouraged that teams write a subclass of Trigger if they want to have
 * something unusual (for instance, if they want to react to the user holding a
 * button while the robot is reading a certain sensor input). For this, they
 * only have to write the {@link Trigger#get()} method to get the full
 * functionality of the Trigger class.
 *
 * @author Joe Grinstead
 */
public abstract class Trigger implements Sendable {

  /**
   * Returns whether or not the trigger is active
   *
   * This method will be called repeatedly a command is linked to the Trigger.
   *
   * @return whether or not the trigger condition is active.
   */
  public abstract boolean get();

  /**
   * Returns whether get() return true or the internal table for SmartDashboard
   * use is pressed.
   *$
   * @return whether get() return true or the internal table for SmartDashboard
   *         use is pressed
   */
  private boolean grab() {
    return get()
        || (table != null /* && table.isConnected() */&& table.getBoolean("pressed", false));// FIXME
                                                                                             // make
                                                                                             // is
                                                                                             // connected
                                                                                             // work?
  }

  /**
   * Starts the given command whenever the trigger just becomes active.
   *$
   * @param command the command to start
   */
  public void whenActive(final Command command) {
    new ButtonScheduler() {

      boolean pressedLast = grab();

      public void execute() {
        if (grab()) {
          if (!pressedLast) {
            pressedLast = true;
            command.start();
          }
        } else {
          pressedLast = false;
        }
      }
    }.start();
  }

  /**
   * Constantly starts the given command while the button is held.
   *
   * {@link Command#start()} will be called repeatedly while the trigger is
   * active, and will be canceled when the trigger becomes inactive.
   *
   * @param command the command to start
   */
  public void whileActive(final Command command) {
    new ButtonScheduler() {

      boolean pressedLast = grab();

      public void execute() {
        if (grab()) {
          pressedLast = true;
          command.start();
        } else {
          if (pressedLast) {
            pressedLast = false;
            command.cancel();
          }
        }
      }
    }.start();
  }

  /**
   * Starts the command when the trigger becomes inactive
   *$
   * @param command the command to start
   */
  public void whenInactive(final Command command) {
    new ButtonScheduler() {

      boolean pressedLast = grab();

      public void execute() {
        if (grab()) {
          pressedLast = true;
        } else {
          if (pressedLast) {
            pressedLast = false;
            command.start();
          }
        }
      }
    }.start();
  }

  /**
   * Toggles a command when the trigger becomes active
   *$
   * @param command the command to toggle
   */
  public void toggleWhenActive(final Command command) {
    new ButtonScheduler() {

      boolean pressedLast = grab();

      public void execute() {
        if (grab()) {
          if (!pressedLast) {
            pressedLast = true;
            if (command.isRunning()) {
              command.cancel();
            } else {
              command.start();
            }
          }
        } else {
          pressedLast = false;
        }
      }
    }.start();
  }

  /**
   * Cancels a command when the trigger becomes active
   *$
   * @param command the command to cancel
   */
  public void cancelWhenActive(final Command command) {
    new ButtonScheduler() {

      boolean pressedLast = grab();

      public void execute() {
        if (grab()) {
          if (!pressedLast) {
            pressedLast = true;
            command.cancel();
          }
        } else {
          pressedLast = false;
        }
      }
    }.start();
  }

  /**
   * An internal class of {@link Trigger}. The user should ignore this, it is
   * only public to interface between packages.
   */
  public abstract class ButtonScheduler {
    public abstract void execute();

    protected void start() {
      Scheduler.getInstance().addButton(this);
    }
  }

  /**
   * These methods continue to return the "Button" SmartDashboard type until we
   * decided to create a Trigger widget type for the dashboard.
   */
  public String getSmartDashboardType() {
    return "Button";
  }

  private ITable table;

  public void initTable(ITable table) {
    this.table = table;
    if (table != null) {
      table.putBoolean("pressed", get());
    }
  }

  /**
   * {@inheritDoc}
   */
  public ITable getTable() {
    return table;
  }
}
