// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.Microsecond;
import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Value;

import edu.wpi.first.units.Dimensionless;
import edu.wpi.first.units.Distance;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.units.Velocity;
import edu.wpi.first.units.collections.LongToObjectHashMap;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.wpilibj.DriverStation;

// import edu.wpi.first.wpilibj.util.Color;
import java.util.Map;
import java.util.Objects;
import java.util.function.BooleanSupplier;

/**
 * An LED pattern controls lights on an LED strip to command patterns of color that may change over
 * time. Dynamic patterns should synchronize on an external clock for timed-based animations ({@link
 * WPIUtilJNI#now()} is recommended, since it can be mocked in simulation and unit tests), or on
 * some other dynamic input (see {@link #synchronizedBlink(BooleanSupplier)}, for example).
 *
 * <p>Patterns should be updated periodically in order for animations to play smoothly. For example,
 * a hypothetical LED subsystem could create a {@code Command} that will continuously apply the
 * pattern to its LED data buffer as part of the main periodic loop.
 *
 * <pre><code>
 *   public class LEDs extends SubsystemBase {
 *     private final AddressableLED m_led = new AddressableLED(0);
 *     private final AddressableLEDBuffer m_ledData = new AddressableLEDBuffer(120);
 *
 *     public LEDs() {
 *       m_led.setLength(120);
 *     }
 *
 *    {@literal @}Override
 *     public void periodic() {
 *       m_led.writeData(m_ledData);
 *     }
 *
 *     public Command runPattern(LEDPattern pattern) {
 *       return run(() -> pattern.applyTo(m_ledData));
 *     }
 *   }
 * </code></pre>
 *
 * <p>LED patterns are stateless, and as such can be applied to multiple LED strips (or different
 * sections of the same LED strip, since the roboRIO can only drive a single LED strip). In this
 * example, we split the single buffer into two views - one for the section of the LED strip on the
 * left side of a robot, and another view for the section of LEDs on the right side. The same
 * pattern is able to be applied to both sides
 *
 * <pre><code>
 *   public class LEDs extends SubsystemBase {
 *     private final AddressableLED m_led = new AddressableLED(0);
 *     private final AddressableLEDBuffer m_ledData = new AddressableLEDBuffer(60);
 *     private final AddressableLEDBufferView m_leftData = m_ledData.createView(0, 29);
 *     private final AddressableLEDBufferView m_rightData = m_ledData.createView(30, 59).reversed();
 *
 *     public LEDs() {
 *       m_led.setLength(120);
 *     }
 *
 *    {@literal @}Override
 *     public void periodic() {
 *       m_led.writeData(m_ledData);
 *     }
 *
 *     public Command runPattern(LEDPattern pattern) {
 *       // Use the single input pattern to drive both sides
 *       return runSplitPatterns(pattern, pattern);
 *     }
 *
 *     public Command runSplitPatterns(LEDPattern left, LEDPattern right) {
 *       return run(() -> {
 *         left.applyTo(m_leftData);
 *         right.applyTo(m_rightData);
 *       });
 *     }
 *   }
 * </code></pre>
 */
@FunctionalInterface
public interface LEDPattern {
  /**
   * Writes the pattern to an LED buffer. Dynamic animations should be called periodically (such as
   * with a command or with a periodic method) to refresh the buffer over time.
   *
   * <p>This method is intentionally designed to use separate objects for reading and writing data.
   * By splitting them up, we can easily modify the behavior of some base pattern to make it {@link
   * #scrollAtRelativeSpeed(Measure) scroll}, {@link #blink(Measure, Measure) blink}, or {@link
   * #breathe(Measure) breathe}.
   *
   * @param reader data reader for accessing buffer length and current colors
   * @param writer data writer for setting new LED colors on the buffer
   */
  void applyTo(LEDReader reader, LEDWriter writer);

  /**
   * Convenience for {@link #applyTo(LEDReader, LEDWriter)} when one object provides both a read and
   * a write interface. This is most helpful for playing an animated pattern directly on an {@link
   * AddressableLEDBuffer} for the sake of code clarity.
   *
   * <pre><code>
   *   AddressableLEDBuffer data = new AddressableLEDBuffer(120);
   *   LEDPattern pattern = ...
   *
   *   void periodic() {
   *     pattern.applyTo(data);
   *   }
   * </code></pre>
   *
   * @param readWriter the object to use for both reading and writing to a set of LEDs
   * @param <T> the type of the object that can both read and write LED data
   */
  default <T extends LEDReader & LEDWriter> void applyTo(T readWriter) {
    applyTo(readWriter, readWriter);
  }

  /**
   * Creates a pattern that plays this one in reverse. Has no effect on non-animated patterns.
   *
   * @return the reverse pattern
   */
  default LEDPattern reversed() {
    return (reader, writer) -> {
      int bufLen = reader.getLength();
      applyTo(reader, (i, r, g, b) -> writer.setRGB((bufLen - 1) - i, r, g, b));
    };
  }

  /**
   * Creates a pattern that plays this one, but offset by a certain number of LEDs. The offset
   * pattern will wrap around, if necessary.
   *
   * @param offset how many LEDs to offset by
   * @return the offset pattern
   */
  default LEDPattern offsetBy(int offset) {
    return (reader, writer) -> {
      int bufLen = reader.getLength();
      applyTo(
          reader,
          (i, r, g, b) -> {
            int shiftedIndex = Math.floorMod(i + offset, bufLen);
            writer.setRGB(shiftedIndex, r, g, b);
          });
    };
  }

  /**
   * Creates a pattern that plays this one scrolling up the buffer. The velocity controls how fast
   * the pattern returns back to its original position, and is in terms of the length of the LED
   * strip; scrolling across a segment that is 10 LEDs long will travel twice as fast as on a
   * segment that's only 5 LEDs long (assuming equal LED density on both segments).
   *
   * @param velocity how fast the pattern should move, in terms of how long it takes to do a full
   *     scroll along the length of LEDs and return back to the starting position
   * @return the scrolling pattern
   */
  default LEDPattern scrollAtRelativeSpeed(Measure<Velocity<Dimensionless>> velocity) {
    final double periodMicros = 1 / velocity.in(Value.per(Microsecond));

    return (reader, writer) -> {
      int bufLen = reader.getLength();
      long now = WPIUtilJNI.now();

      // index should move by (buf.length) / (period)
      double t = (now % (long) periodMicros) / periodMicros;
      int offset = (int) (t * bufLen);

      applyTo(
          reader,
          (i, r, g, b) -> {
            // floorMod so if the offset is negative, we still get positive outputs
            int shiftedIndex = Math.floorMod(i + offset, bufLen);

            writer.setRGB(shiftedIndex, r, g, b);
          });
    };
  }

  /**
   * Creates a pattern that plays this one scrolling up an LED strip. A negative velocity makes the
   * pattern play in reverse.
   *
   * @param velocity how fast the pattern should move along a physical LED strip
   * @param ledSpacing the distance between adjacent LEDs on the physical LED strip
   * @return the scrolling pattern
   */
  default LEDPattern scrollAtAbsoluteSpeed(
      Measure<Velocity<Distance>> velocity, Measure<Distance> ledSpacing) {
    // eg velocity = 10 m/s, spacing = 0.01m
    // meters per micro = 1e-5 m/us
    // micros per LED = 1e-2 m / (1e-5 m/us) = 1e-3 us

    var metersPerMicro = velocity.in(Meters.per(Microsecond));
    var microsPerLED = (int) (ledSpacing.in(Meters) / metersPerMicro);

    return (reader, writer) -> {
      int bufLen = reader.getLength();
      long now = WPIUtilJNI.now();

      // every step in time that's a multiple of microsPerLED will increment the offset by 1
      var offset = now / microsPerLED;

      applyTo(
          reader,
          (i, r, g, b) -> {
            // floorMod so if the offset is negative, we still get positive outputs
            int shiftedIndex = Math.floorMod(i + offset, bufLen);

            writer.setRGB(shiftedIndex, r, g, b);
          });
    };
  }

  /**
   * Creates a pattern that switches between playing this pattern and turning the entire LED strip
   * off.
   *
   * @param onTime how long the pattern should play for, per cycle
   * @param offTime how long the pattern should be turned off for, per cycle
   * @return the blinking pattern
   */
  default LEDPattern blink(Measure<Time> onTime, Measure<Time> offTime) {
    final long totalTimeMicros = (long) (onTime.in(Microseconds) + offTime.in(Microseconds));
    final long onTimeMicros = (long) onTime.in(Microseconds);

    return (reader, writer) -> {
      if (WPIUtilJNI.now() % totalTimeMicros < onTimeMicros) {
        applyTo(reader, writer);
      } else {
        kOff.applyTo(reader, writer);
      }
    };
  }

  /**
   * Like {@link #blink(Measure, Measure) blink(onTime, offTime)}, but where the "off" time is
   * exactly equal to the "on" time.
   *
   * @param onTime how long the pattern should play for (and be turned off for), per cycle
   * @return the blinking pattern
   */
  default LEDPattern blink(Measure<Time> onTime) {
    return blink(onTime, onTime);
  }

  /**
   * Creates a pattern that blinks this one on and off in sync with a true/false signal. The pattern
   * will play while the signal outputs {@code true}, and will turn off while the signal outputs
   * {@code false}.
   *
   * @param signal the signal to synchronize with
   * @return the blinking pattern
   */
  default LEDPattern synchronizedBlink(BooleanSupplier signal) {
    return (reader, writer) -> {
      if (signal.getAsBoolean()) {
        applyTo(reader, writer);
      } else {
        kOff.applyTo(reader, writer);
      }
    };
  }

  /**
   * Creates a pattern that brightens and dims this one over time. Brightness follows a sinusoidal
   * pattern.
   *
   * @param period how fast the breathing pattern should complete a single cycle
   * @return the breathing pattern
   */
  default LEDPattern breathe(Measure<Time> period) {
    final long periodMicros = (long) period.in(Microseconds);

    return (reader, writer) -> {
      applyTo(
          reader,
          (i, r, g, b) -> {
            // How far we are in the cycle, in the range [0, 1)
            double t = (WPIUtilJNI.now() % periodMicros) / (double) periodMicros;
            double phase = t * 2 * Math.PI;

            // Apply the cosine function and shift its output from [-1, 1] to [0, 1]
            // Use cosine so the period starts at 100% brightness
            double dim = 1 - (Math.cos(phase) + 1) / 2.0;

            int output = Color.lerpRGB(r, g, b, 0, 0, 0, dim);

            writer.setRGB(
                i,
                Color.unpackRGB(output, Color.RGBChannel.kRed),
                Color.unpackRGB(output, Color.RGBChannel.kGreen),
                Color.unpackRGB(output, Color.RGBChannel.kBlue));
          });
    };
  }

  /**
   * Creates a pattern that plays this pattern overlaid on another. Anywhere this pattern sets an
   * LED to off (or {@link Color#kBlack}), the base pattern will be displayed instead.
   *
   * @param base the base pattern to overlay on top of
   * @return the combined overlay pattern
   */
  default LEDPattern overlayOn(LEDPattern base) {
    return (reader, writer) -> {
      // write the base pattern down first...
      base.applyTo(reader, writer);

      // ... then, overwrite with the illuminated LEDs from the overlay
      applyTo(
          reader,
          (i, r, g, b) -> {
            if (r != 0 || g != 0 || b != 0) {
              writer.setRGB(i, r, g, b);
            }
          });
    };
  }

  /**
   * Creates a pattern that displays outputs as a combination of this pattern and another. Color
   * values are calculated as the average color of both patterns; if both patterns set the same LED
   * to the same color, then it is set to that color, but if one pattern sets to one color and the
   * other pattern sets it to off, then it will show the color of the first pattern but at
   * approximately half brightness. This is different from {@link #overlayOn}, which will show the
   * base pattern at full brightness if the overlay is set to off at that position.
   *
   * @param other the pattern to blend with
   * @return the blended pattern
   */
  default LEDPattern blend(LEDPattern other) {
    return (reader, writer) -> {
      applyTo(reader, writer);

      other.applyTo(
          reader,
          (i, r, g, b) -> {
            int blendedRGB =
                Color.lerpRGB(
                    reader.getRed(i), reader.getGreen(i), reader.getBlue(i), r, g, b, 0.5);

            writer.setRGB(
                i,
                Color.unpackRGB(blendedRGB, Color.RGBChannel.kRed),
                Color.unpackRGB(blendedRGB, Color.RGBChannel.kGreen),
                Color.unpackRGB(blendedRGB, Color.RGBChannel.kBlue));
          });
    };
  }

  /** A pattern that turns off all LEDs. */
  LEDPattern kOff = solid(Color.kBlack);

  /**
   * Creates a pattern that displays a single static color along the entire length of the LED strip.
   *
   * @param color the color to display
   * @return the pattern
   */
  static LEDPattern solid(Color color) {
    return (reader, writer) -> {
      int bufLen = reader.getLength();
      for (int led = 0; led < bufLen; led++) {
        writer.setLED(led, color);
      }
    };
  }

  /**
   * Display a set of colors in steps across the length of the LED strip. No interpolation is done
   * between colors. Colors are specified by the first LED on the strip to show that color. The last
   * color in the map will be displayed all the way to the end of the strip. LEDs positioned before
   * the first specified step will be turned off (you can think of this as if there's a 0 -> black
   * step by default)
   *
   * <pre>
   *   // Display red from 0-33%, white from 33% - 67%, and blue from 67% to 100%
   *   steps(Map.of(0.00, Color.kRed, 0.33, Color.kWhite, 0.67, Color.kBlue))
   *
   *   // Half off, half on
   *   steps(Map.of(0.5, Color.kWhite))
   * </pre>
   *
   * @param steps a map of progress to the color to start displaying at that position along the LED
   *     strip
   * @return a motionless step pattern
   */
  static LEDPattern steps(Map<? extends Number, Color> steps) {
    if (steps.isEmpty()) {
      // no colors specified
      DriverStation.reportWarning("Creating LED steps with no colors!", false);
      return kOff;
    }

    if (steps.size() == 1 && steps.keySet().iterator().next().doubleValue() == 0) {
      // only one color specified, just show a static color
      DriverStation.reportWarning("Creating LED steps with only one color!", false);
      return solid(steps.values().iterator().next());
    }

    return (reader, writer) -> {
      int bufLen = reader.getLength();

      // precompute relevant positions for this buffer so we don't need to do a check
      // on every single LED index
      var stopPositions = new LongToObjectHashMap<Color>();
      steps.forEach(
          (progress, color) -> {
            stopPositions.put((int) Math.floor(progress.doubleValue() * bufLen), color);
          });

      Color currentColor = Color.kBlack;
      for (int led = 0; led < bufLen; led++) {
        currentColor = Objects.requireNonNullElse(stopPositions.get(led), currentColor);

        writer.setLED(led, currentColor);
      }
    };
  }

  /**
   * Creates a pattern that displays a non-animated gradient of colors across the entire length of
   * the LED strip. The gradient wraps around so the start and end of the strip are the same color,
   * which allows the gradient to be modified with a scrolling effect with no discontinuities.
   * Colors are evenly distributed along the full length of the LED strip.
   *
   * @param colors the colors to display in the gradient
   * @return a motionless gradient pattern
   */
  static LEDPattern gradient(Color... colors) {
    if (colors.length == 0) {
      // Nothing to display
      DriverStation.reportWarning("Creating a gradient with no colors!", false);
      return kOff;
    }

    if (colors.length == 1) {
      // No gradients with one color
      DriverStation.reportWarning("Creating a gradient with only one color!", false);
      return solid(colors[0]);
    }

    final int numSegments = colors.length;

    return (reader, writer) -> {
      int bufLen = reader.getLength();
      int ledsPerSegment = bufLen / numSegments;

      for (int led = 0; led < bufLen; led++) {
        int colorIndex = (led / ledsPerSegment) % numSegments;
        int nextColorIndex = (colorIndex + 1) % numSegments;
        double t = (led / (double) ledsPerSegment) % 1;

        Color color = colors[colorIndex];
        Color nextColor = colors[nextColorIndex];
        int gradientColor =
            Color.lerpRGB(
                color.red,
                color.green,
                color.blue,
                nextColor.red,
                nextColor.green,
                nextColor.blue,
                t);

        writer.setRGB(
            led,
            Color.unpackRGB(gradientColor, Color.RGBChannel.kRed),
            Color.unpackRGB(gradientColor, Color.RGBChannel.kGreen),
            Color.unpackRGB(gradientColor, Color.RGBChannel.kBlue));
      }
    };
  }

  /**
   * Creates an LED pattern that displays a rainbow across the color wheel. The rainbow pattern will
   * stretch across the entire length of the LED strip.
   *
   * @param saturation the saturation of the HSV colors, in [0, 255]
   * @param value the value of the HSV colors, in [0, 255]
   * @return the rainbow pattern
   */
  static LEDPattern rainbow(int saturation, int value) {
    return (reader, writer) -> {
      int bufLen = reader.getLength();
      for (int i = 0; i < bufLen; i++) {
        int hue = ((i * 180) / bufLen) % 180;
        writer.setHSV(i, hue, saturation, value);
      }
    };
  }
}
