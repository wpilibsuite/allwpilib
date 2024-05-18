// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

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

public class RobotSignals  {

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
    // update this length for each view defined
    // 
    int length = Math.max(Math.max(Math.max(Math.max(lastTopLED, lastMainLED), lastEnableDisableLED), lastHistoryDemoLED), lastAchieveHueGoal) + 1; // simplistic view of segments - one starts at 0
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
    private boolean isDefaultSet = false; // locks out multiple set default command

    private LEDView(AddressableLEDBufferView view) {
      this.view = view;
    }

    /*
     * Public Commands
     */

    /**
     * Allow no more than one call to this set of the view (resource, subsystem) default command
     */
    @Override
    public void setDefaultCommand(Command def) {

      StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
      StackTraceElement element = stackTrace[2];
      System.out.println("I was called by a method named: " + element.getMethodName());
      System.out.println("That method is in class: " + element.getClassName());

      if (isDefaultSet) {
        throw new IllegalArgumentException("Default Command already set");
      }
      isDefaultSet = true;
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


/* parking lot

C:\Users\RKT\frc\FRC2024\allwpilib-main\wpilibjExamples\src\main\java\edu\wpi\first\wpilibj\examples\addressableled\Main.java
    private void rainbow() {
    // For every pixel
    for (var i = 0; i < m_ledBuffer.getLength(); i++) {
      // Calculate the hue - hue is easier for rainbows because the color
      // shape is a circle so only one value needs to precess
      final var hue = (m_rainbowFirstPixelHue + (i * 180 / m_ledBuffer.getLength())) % 180;
      // Set the value
      m_ledBuffer.setHSV(i, hue, 255, 128);
    }
    // Increase by to make the rainbow "move"
    m_rainbowFirstPixelHue += 3;
    // Check bounds
    m_rainbowFirstPixelHue %= 180;
  }


///// Sam's Robot.java
// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.addressableled;

import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;

import edu.wpi.first.units.Distance;
import edu.wpi.first.units.Measure;
import edu.wpi.first.wpilibj.AddressableLED;
import edu.wpi.first.wpilibj.AddressableLEDBuffer;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.TimedRobot;

public class Robot extends TimedRobot {
  private AddressableLED m_led;
  private AddressableLEDBuffer m_ledBuffer;

  // Create an LED pattern that will display a rainbow across
  // all hues at maximum saturation and half brightness
  private final LEDPattern m_rainbow = LEDPattern.rainbow(255, 128);

  // Our LED strip has a density of 120 LEDs per meter
  private static final Measure<Distance> kLedSpacing = Meters.of(1 / 120.0);

  // Create a new pattern that scrolls the rainbow pattern across the LED strip, moving at a speed
  // of 1 meter per second.
  private final LEDPattern m_scrollingRainbow =
      m_rainbow.scrollAtAbsoluteSpeed(MetersPerSecond.of(1), kLedSpacing);

  @Override
  public void robotInit() {
    // PWM port 9
    // Must be a PWM header, not MXP or DIO
    m_led = new AddressableLED(9);

    // Reuse buffer
    // Default to a length of 60, start empty output
    // Length is expensive to set, so only set it once, then just update data
    m_ledBuffer = new AddressableLEDBuffer(60);
    m_led.setLength(m_ledBuffer.getLength());

    // Set the data
    m_led.setData(m_ledBuffer);
    m_led.start();
  }

  @Override
  public void robotPeriodic() {
    // Update the buffer with the rainbow animation
    m_scrollingRainbow.applyTo(m_ledBuffer);
    // Set the LEDs
    m_led.setData(m_ledBuffer);
  }
}
 */