// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

import java.util.function.Supplier;

import edu.wpi.first.wpilibj.AddressableLED;
import edu.wpi.first.wpilibj.AddressableLEDBuffer;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import frc.robot.PeriodicTask;

/*
 * All Commands factories are "public."
 * 
 * All other methods are "private" to prevent other classes from forgetting to
 * add requirements of these resources if creating commands from these methods.
 */

public class RobotSignals  {
  private final AddressableLED strip;

  // layout by LED number of the single physical buffer
  // into two logical segments (subsystems) called Main and Top
  public final AddressableLEDBuffer bufferLED;
  private final int firstMainLED = 11; // inclusive
  private final int lastMainLED = 32; // inclusive
  private final int firstTopLED = 0; // inclusive
  private final int lastTopLED = 10; // inclusive

  public LEDTopSubsystem Top;
  public LEDMainSubsystem Main;

  public RobotSignals(int port, PeriodicTask periodicTask) {
    int length = Math.max(lastTopLED, lastMainLED) - Math.min(firstTopLED, firstMainLED) + 1;
    // start updating the physical LED strip
    strip = new AddressableLED(port);
    strip.setLength(length);
    strip.start();

    bufferLED = new AddressableLEDBuffer(length);
    Top = new LEDTopSubsystem();
    Main = new LEDMainSubsystem();

    // start the periodic process to send the buffered data to the LED
    periodicTask.register(()->strip.setData(bufferLED), 0.24, 0.019);
  }

  /**
   * Sets all LEDs in the segment to the same color
   * @param color
   */
  private void setTopSignal(Color color)
  {
      for(int index=firstTopLED; index<=lastTopLED; index++ ) {
        bufferLED.setLED(index, color);      
      }
  }

  /**
   * Sets the LEDS in a segment to the param value
   * @param buffer
   */
  private void setTopSignal(AddressableLEDBuffer buffer)
  {//FIXME check for buffer short and clear remainder of Top buffer
      int i = 0;
      for(int index=firstTopLED; index<=lastTopLED; index++ ) {
        bufferLED.setLED(index, buffer.getLED(i++));      
      }
  }

  /**
   * Sets all LEDs in the segment to the same color
   * @param color
   */
  private void setMainSignal(Color color)
  {
    for(int index=firstMainLED; index<=lastMainLED; index++) {
      bufferLED.setLED(index, color);    
    }
  }

  /**
   * Sets the LEDS in a segment to the param value
   * @param buffer
   */
  private void setMainSignal(AddressableLEDBuffer buffer)
  {//FIXME check for buffer short and clear remainder of Main buffer
      int i = 0;
      for(int index=firstMainLED; index<=lastMainLED; index++ ) {
        bufferLED.setLED(index, buffer.getLED(i++));      
      }
  }

  /**
   * Main LEDs subsystem
   */
  public class LEDMainSubsystem extends SubsystemBase {

    public LEDMainSubsystem() {
      setDefaultCommand(
        Commands.run(()->setMainSignal(new Color(0., 1., 1.)), this)
          .ignoringDisable(true)
          .withName("LedDefaultMain"));
    }

    public Command setSignal(Color color) {
      return
        run(()->setMainSignal(color))
          .ignoringDisable(true)
          .withName("LedSetMainC");
    }

    public Command setSignal(AddressableLEDBuffer buffer) {
      return
        run(()->setMainSignal(buffer))
          .ignoringDisable(true)
          .withName("LedSetMainB");
    }

    public Command setSignal(Supplier<Color> color) {
      return
        run(()->setTopSignal(color.get()))
          .ignoringDisable(true)
          .withName("LedSetMainS");
    }

    public Command runPattern(LEDPattern pattern) {
      return run(() -> pattern.applyTo());
    }
  } // End Main LEDs subsystem

  /**
   * Top LEDs subsystem
   */
  public class LEDTopSubsystem extends SubsystemBase {

    public LEDTopSubsystem() {
      Color defaultColor = new Color(1., 0., 1.);
      setDefaultCommand(
        Commands.run(()->setTopSignal(defaultColor), this)
          .ignoringDisable(true)
          .withName("LedDefaultTop"));
    }

    public Command setSignal(Color color) {
      return
        run(()->setTopSignal(color))
          .ignoringDisable(true)
          .withName("LedSetTopC");
    }

    public Command setSignal(AddressableLEDBuffer buffer) {
      return
        run(()->setTopSignal(buffer))
          .ignoringDisable(true)
          .withName("LedSetTopB");
    }

    public Command setSignal(Supplier<Color> color) {
      return
        run(()->setTopSignal(color.get()))
          .ignoringDisable(true)
          .withName("LedSetTopS");
    }

    public Command runPattern(LEDPattern pattern) {
      return run(() -> pattern.applyTo());
    }
  } // End Top LEDs subsystem
}



/* parking lot

  import static edu.wpi.first.units.Units.Seconds;
  private final AddressableLEDBufferView view;
  view = led.createView(...);
  private final AddressableLEDBufferView view;
  view = led.createView(...);
  private final LEDPattern disabled = LEDPattern.solid(Color.kRed).breathe(Seconds.of(2));
  private final LEDPattern enabled = LEDPattern.solid(Color.kGreen).breathe(Seconds.of(2));
  private final LEDPattern defaultPattern = () -> (DriverStation.isDisabled() ? disabled : enabled).applyTo();
  private final LEDPattern blink = LEDPattern.solid(Color.kMagenta).blink(Seconds.of(0.2));
  private final LEDPattern orange = LEDPattern.solid(Color.kOrange);

  ledMain.setDefaultCommand(ledMain.runPattern(defaultPattern).ignoringDisable(true).withName("LedDefaultMain"));
  ledTop.setDefaultCommand(ledTop.runPattern(defaultPattern).ignoringDisable(true).withName("LedDefaultTop"));


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

 */