// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

/**
 * Manage the addressable LEDs as signaling subsystems.
 * 
 * This is the creator and container of the LEDView subsystems.
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

  // layout by LED number of the single physical buffer into three logical views
  // (segments; subsystems) called Main, Top, and EnableDisable.
  // Assume one view starts at 0.
  // Buffer is not cleared.
  // It's just an example.

  private final AddressableLEDBuffer bufferLED;

  // first and last LED ids are inclusive of that LED number
  // Needs a better way to semi-automatically register usage of LEDs.
  // This example is simple and relies on user remembering to do it.
  private final int firstTopLED = 0;
  private final int lastTopLED = 9;
  private final int firstMainLED = 10;
  private final int lastMainLED = 19;
  private final int firstEnableDisableLED = 20;
  private final int lastEnableDisableLED = 29;
  private final int firstHistoryDemoLED = 30;
  private final int lastHistoryDemoLED = 39;
  private final int firstAchieveHueGoal = 40;
  private final int lastAchieveHueGoal = 49;
  
  public LEDView Top;
  public LEDView Main;
  public LEDView EnableDisable;
  public LEDView HistoryDemo;
  public LEDView AchieveHueGoal;

  public RobotSignals(int port) {
  
    // start updating the physical LED strip 

    //  CAUTION
    //  CAUTION
    //  CAUTION
    //  CAUTION
    // Update this length for each view defined.
    // Needs a better way of registering LED usage; this is really (too) simple
    // simplistic view of segments - assume one starts at 0
    int length = Math.max(Math.max(Math.max(Math.max(lastTopLED, lastMainLED), lastEnableDisableLED), lastHistoryDemoLED), lastAchieveHueGoal) + 1;
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
