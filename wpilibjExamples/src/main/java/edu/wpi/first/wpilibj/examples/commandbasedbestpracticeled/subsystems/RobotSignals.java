// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

/**
 * Manage the addressable LEDs as signaling subsystems.
 * 
 * This is the creator and container of the LEDView subsystems.
 * 
 * Buffer is not cleared.
 * 
 * It's just a simple example.
 * 
 * Needs a better way of registering LED usage; this is really (too) simple and relies on the user
 * finding it here and remembering to do it.
 */

import frc.robot.AddressableLED;
import frc.robot.AddressableLEDBuffer;
import frc.robot.AddressableLEDBufferView;
import frc.robot.LEDPattern;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

/*
 * All Commands factories are "public."
 * 
 * All other methods are "private" to prevent other classes from forgetting to add requirements of
 * these resources if creating commands from these methods.
 */

public class RobotSignals {

  /**
   * Represents a supplier of LEDPattern for creating dynamic LEDPatterns.
   * 
   * Can't overload methods using generic interfaces parameters like Supplier so make our own
   * interface to use in overloads
   * 
   */
  @FunctionalInterface
  public interface LEDPatternSupplier {
    /**
     * Gets a result.
     *
     * @return a result
     */
    LEDPattern get();
  }

  private final AddressableLED m_Strip;

  // Layout by LED number of the single physical buffer into multiple logical
  // views or resources/subsystems.

  // simplistic view of segments - assume one starts at 0.
  // first and last LED ids are inclusive of that LED number

  private static final int m_FirstTopLED = 0;
  private static final int m_LastTopLED = 7;
  private static final int m_FirstMainLED = 8;
  private static final int m_LastMainLED = 15;
  private static final int m_FirstEnableDisableLED = 16;
  private static final int m_LastEnableDisableLED = 23;
  private static final int m_FirstHistoryDemoLED = 24;
  private static final int m_LastHistoryDemoLED = 31;
  private static final int m_FirstAchieveHueGoal = 32;
  private static final int m_LastAchieveHueGoal = 39;
  // CAUTION CAUTION CAUTION -- Update this length for each view defined.
  private static final int length = Math.max(Math.max(
      Math.max(Math.max(m_LastTopLED, m_LastMainLED), m_LastEnableDisableLED),
      m_LastHistoryDemoLED), m_LastAchieveHueGoal) + 1;

  public LEDView m_Top;
  public LEDView m_Main;
  public LEDView m_EnableDisable;
  public LEDView m_HistoryDemo;
  public LEDView m_AchieveHueGoal;

  private final AddressableLEDBuffer bufferLED;

  public RobotSignals(int port) {

    // start updating the physical LED strip

    m_Strip = new AddressableLED(port);
    m_Strip.setLength(length);
    m_Strip.start();

    bufferLED = new AddressableLEDBuffer(length); // buffer for all of the LEDs

    // create the resources (subsystems) as views of the LED buffer
    m_Top = new LEDView(bufferLED.createView(m_FirstTopLED, m_LastTopLED));
    m_Main = new LEDView(bufferLED.createView(m_FirstMainLED, m_LastMainLED));
    m_EnableDisable = new LEDView(
        bufferLED.createView(m_FirstEnableDisableLED, m_LastEnableDisableLED));
    m_HistoryDemo = new LEDView(
        bufferLED.createView(m_FirstHistoryDemoLED, m_LastHistoryDemoLED));
    m_AchieveHueGoal = new LEDView(
        bufferLED.createView(m_FirstAchieveHueGoal, m_LastAchieveHueGoal));
  }

  /**
   * Run before commands and triggers
   */
  public void beforeCommands() {}

  /**
   * Run after commands and triggers
   */
  public void afterCommands() {

    m_Strip.setData(bufferLED); // run periodically to send the buffer to the LEDs
  }

  /**
   * LED view resource (subsystem)
   */
  public class LEDView extends SubsystemBase {

    private final AddressableLEDBufferView m_View;

    private LEDView(AddressableLEDBufferView view) {
      this.m_View = view;
    }

    /*
     * Public Commands
     */

    /**
     * Example of how to allow one (or none) default command to be set.
     * 
     * @param def default command
     */
    @Override
    public void setDefaultCommand(Command def) {

      if (getDefaultCommand() != null) {
        throw new IllegalArgumentException("Default Command already set");
      }

      if (def != null) {
        super.setDefaultCommand(def);
      }
    }

    /**
     * Put an LED Pattern into the view
     * 
     * @param pattern
     * @return
     */
    public Command setSignal(LEDPattern pattern) {

      return run(() -> pattern.applyTo(m_View)).ignoringDisable(true)
          .withName("LedSet");
    }

    /**
     * Put a dynamic LED Pattern into the view
     * 
     * @param pattern
     * @return
     */
    public Command setSignal(LEDPatternSupplier pattern) {

      return run(() -> pattern.get().applyTo(m_View)).ignoringDisable(true)
          .withName("LedSetS");
    }
  } // End LEDView
}
