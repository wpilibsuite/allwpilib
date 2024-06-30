// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/**
 * Display a random color signal periodically and don't reuse the same color too soon (if reasonably
 * possible).
 *
 * <p>This is a FSM that depends on the current state, the transition event trigger, and the
 * historical previous states.
 *
 * <p>This demonstrates a command that calls another command upon completion by setting up a
 * trigger. This demonstrates accessing historical data to determine the next state. This
 * demonstrates using persistent data to periodically refresh outputs past the completion of this
 * command. This demonstrates running a command periodically based on the past time history.
 *
 * <p>Caution - this is a simple, contrived example. This command essentially runs periodically the
 * hard way using memory of a time and a trigger on the time. There may be better ways to do that
 * simple scheduling structure, for example, the use of the "runBeforeCommands()" or
 * "runAfterCommands()" periodic methods as used for a sub-purpose here is a good way.
 */

import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDView;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class HistoryFSM extends SubsystemBase {
  private final LEDView m_robotSignals;
  private Random m_rand = new Random();

  // Periodic output variable used for each run of "afterCommands()"
  // this is used before command to set it is run so start with LEDs off
  private LEDPattern m_persistentPatternDemo = LEDPattern.solid(Color.kBlack);

  // Add a color [hue number as subscript] and last time used to the history
  // so that color isn't used again during a lockout period.

  // Make the history in as narrow of scope as possible. For this simple example the scope is
  // perfectly narrow (this instance scope) since the history doesn't depend on any values
  // from other subsystems.

  // Also saved from historical values are the "current" color so it persists through multiple
  // iterations.

  // Time data is saved for how long a color is to persist in the display.

  private static final int m_computerColorWheel = 180; // max count of hues numbered 0 to 179
  // list of the last times of all the colors so try not to repeat for a long time so repeats are
  // rare
  private List<Measure<Time>> m_lastTimeHistoryOfColors = new ArrayList<>(m_computerColorWheel);

  private static final double m_beginningOfTime = 0.0;
  private static final double m_endOfTime = Double.MAX_VALUE;

  // time the current color display should end and a new color selected
  // initialize so the time doesn't trigger anything until the "Y" button is pressed
  private Measure<Time> m_nextTime = Milliseconds.of(m_endOfTime);
  Measure<Time> m_changeColorPeriod = Seconds.of(2.0); // display color for this long
  Measure<Time> m_colorLockoutPeriod = Seconds.of(20.0); // try not to reuse a color for this long

  public HistoryFSM(LEDView robotSignals) {
    this.m_robotSignals = robotSignals;

    fillInitialTimes(); // initialize last time used for all the hues of the color wheel
  }

  /** Create an initialized list of hues */
  private void fillInitialTimes() {
    // initially indicate hue hasn't been used in a long time ago so available immediately
    for (int i = 0; i < m_computerColorWheel; i++) {
      m_lastTimeHistoryOfColors.add(Seconds.of(m_beginningOfTime));
    }
  }

  /** Set the time for the trigger of its next periodic run */
  private void setNextTime() {
    Measure<Time> currentTime = Milliseconds.of(System.currentTimeMillis());
    m_nextTime = currentTime.plus(m_changeColorPeriod);
  }

  /**
   * Elapsed Timer determines if in the color change lockout period or not. Resets automatically.
   *
   * @return has time elapsed
   */
  public boolean timesUp() {
    if (m_nextTime.lt(Milliseconds.of(System.currentTimeMillis()))) {
      // reset; a command may run that will set the correct periodic "nextTime".
      // Otherwise wait for other triggering to restart.
      // This locks-out automatic restarting on disable-to-enable change; other trigger
      // required to get it started again.
      m_nextTime = Milliseconds.of(m_endOfTime);
      return true;
    }
    return false; // not time to trigger yet
  }

  /**
   * Get the next color and set the timer for the next color change
   * @return Command to do it
   */
  public Command newColor() {
    return
      runOnce(this::getHSV) // new color
      .andThen(runOnce(this::setNextTime) // next time for new color
      ).withName("History FSM Sequence");
  }

  /**
   * Sets a color and quits immediately assuming the color persists somehow (in
   * "m_persistentPatternDemo") until the next color is later requested.
   *
   * <p>Set a random color that hasn't been used in the last "m_colorLockoutPeriod"
   */
  private void getHSV() {
    Measure<Time> currentTime = Milliseconds.of(System.currentTimeMillis());
    int randomHue; // to be the next color
    int loopCounter = 0; // count attempts to find a different hue
    int loopCounterLimit = 20; // limit attempts to find a different hue
    // reasonable limit related to:
    // number of colors, how often colors change, how long to lockout a color.

    do {
      // Generate random numbers for hues in range of the computer color wheel
      randomHue = m_rand.nextInt(m_computerColorWheel);
      // if hue hasn't been used recently, then use it now and update its history
      var colorTime = m_lastTimeHistoryOfColors.get(randomHue); // get the associated time
      if (colorTime.lt(currentTime.minus(m_colorLockoutPeriod))) {
        m_lastTimeHistoryOfColors.set(randomHue, currentTime);
        break;
      }
      // hue used recently so loop to get another hue
      // limit attempts - no infinite loops allowed
    } while (++loopCounter < loopCounterLimit);

    m_persistentPatternDemo = LEDPattern.solid(Color.fromHSV(randomHue, 200, 200));

    // Set and refresh the color could be done many ways.
    // Here it's set once and assumed to persist by some other means than this method.
    // Access to the LEDs is only by command.
    m_robotSignals.setSignalOnce(m_persistentPatternDemo).withName("History FSM LED").schedule();
    // This subsystem wasn't designed to run in disabled mode. Because of the way the LED subsystem
    // works the last color selected persists in Disabled mode but new colors are not selected.
    // Could check here for that and black out if necessary. Or do something in disabledInit().
    // This usage of scheduling a command within a subsystem is not the same as a command directly
    // scheduling another command; that technique should be avoided.
  }

  /** Example of how to disallow default command */
  @Override
  public void setDefaultCommand(Command def) {
    throw new IllegalArgumentException("Default Command not allowed");
  }

  /**
   * Run before commands and triggers
   */
  public void runBeforeCommands() {}

  int m_verificationPrintCounter; // 0; limit verification prints counter

  /**
   * Run after commands and triggers
   */
  public void runAfterCommands() {

    boolean verificationPrint = false; // verification - not for production
    if (verificationPrint) {
      m_verificationPrintCounter++;
      int verificationPrintLimit = 50 /*hz*/ * 12 /*seconds*/;
      if (m_verificationPrintCounter % verificationPrintLimit == 0) { // dump every 12 seconds (50hz * 12 = 600)
        System.out.println("current time " + System.currentTimeMillis());
        for (int i = 0; i < m_lastTimeHistoryOfColors.size(); i++) {
          System.out.println(i + " " + m_lastTimeHistoryOfColors.get(i).toLongString());
        }
      }
    }
  }
}
