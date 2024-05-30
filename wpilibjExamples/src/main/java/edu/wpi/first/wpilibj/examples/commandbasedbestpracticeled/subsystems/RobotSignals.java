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
 * Needs a better way of registering LED usage; this is really (too) simple
 * and relies on the user finding it here and remembering to do it.
 */

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import frc.robot.AddressableLED;
import frc.robot.AddressableLEDBuffer;
import frc.robot.AddressableLEDBufferView;
import frc.robot.LEDPattern;

/*
 * All Commands factories are "public."
 * 
 * All other methods are "private" to prevent other classes from forgetting to
 * add requirements of these resources if creating commands from these methods.
 */

public class RobotSignals {

  /**
   * Represents a supplier of LEDPattern for creating dynamic LEDPatterns.
   * 
   * Can't overload methods using generic interfaces parameters like Supplier
   * so make our own interface to use in overloads
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

  private final AddressableLED strip;

  // Layout by LED number of the single physical buffer into multiple logical
  // views or resources/subsystems.

  // simplistic view of segments - assume one starts at 0.
  // first and last LED ids are inclusive of that LED number
 
  private final int firstTopLED = 0;
  private final int lastTopLED = 7;
  private final int firstMainLED = 8;
  private final int lastMainLED = 15;
  private final int firstEnableDisableLED = 16;
  private final int lastEnableDisableLED = 23;
  private final int firstHistoryDemoLED = 24;
  private final int lastHistoryDemoLED = 31;
  private final int firstAchieveHueGoal = 32;
  private final int lastAchieveHueGoal = 39;
  // CAUTION CAUTION CAUTION -- Update this length for each view defined.
  private final int length = Math.max(Math.max(Math.max(Math.max(
      lastTopLED, lastMainLED), lastEnableDisableLED), lastHistoryDemoLED), lastAchieveHueGoal) + 1;
 
  public LEDView Top;
  public LEDView Main;
  public LEDView EnableDisable;
  public LEDView HistoryDemo;
  public LEDView AchieveHueGoal;

  private final AddressableLEDBuffer bufferLED;

  public RobotSignals(int port) {
  
    // start updating the physical LED strip 

    strip = new AddressableLED(port);
    strip.setLength(length);
    strip.start();

    bufferLED = new AddressableLEDBuffer(length); // buffer for all of the LEDs

    // create the resources (subsystems) as views of the LED buffer
    Top = new LEDView(bufferLED.createView(firstTopLED, lastTopLED));
    Main = new LEDView(bufferLED.createView(firstMainLED, lastMainLED));
    EnableDisable = new LEDView(bufferLED.createView(firstEnableDisableLED, lastEnableDisableLED));
    HistoryDemo = new LEDView(bufferLED.createView(firstHistoryDemoLED, lastHistoryDemoLED));
    AchieveHueGoal = new LEDView(bufferLED.createView(firstAchieveHueGoal, lastAchieveHueGoal));
  }

  public void beforeCommands() {}

  public void afterCommands() {

    strip.setData(bufferLED); // run periodically to send the buffer to the LEDs
  }

  /**
   * LED view resource (subsystem)
   */
  public class LEDView extends SubsystemBase {

    private final AddressableLEDBufferView view;

    private LEDView(AddressableLEDBufferView view) {
      this.view = view;
    }

    /*
     * Public Commands
     */

    /**
     * Example of how to allow one (or none) default command to be set.
     */
    @Override
    public void setDefaultCommand(Command def) {

      if (getDefaultCommand() != null) {
        throw new IllegalArgumentException("Default Command already set");
      }

      if(def != null) {
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

      return
        run(()->pattern.applyTo(view))
          .ignoringDisable(true)
          .withName("LedSet");
    }

    /**
     * Put a dynamic LED Pattern into the view
     * 
     * @param pattern
     * @return
     */
    public Command setSignal(LEDPatternSupplier pattern) {
      
      return
        run(()->pattern.get().applyTo(view))
          .ignoringDisable(true)
          .withName("LedSetS");
    }
  } // End LEDView
}
