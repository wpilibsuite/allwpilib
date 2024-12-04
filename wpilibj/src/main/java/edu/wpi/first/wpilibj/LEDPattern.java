// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.Microsecond;
import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Value;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.units.collections.LongToObjectHashMap;
import edu.wpi.first.units.measure.Dimensionless;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.Frequency;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.wpilibj.util.Color;
import java.util.Map;
import java.util.Objects;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;

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
 *       m_led.start();
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
 * pattern is able to be applied to both sides.
 *
 * <pre><code>
 *   public class LEDs extends SubsystemBase {
 *     private final AddressableLED m_led = new AddressableLED(0);
 *     private final AddressableLEDBuffer m_ledData = new AddressableLEDBuffer(60);
 *     private final AddressableLEDBufferView m_leftData = m_ledData.createView(0, 29);
 *     private final AddressableLEDBufferView m_rightData = m_ledData.createView(30, 59).reversed();
 *
 *     public LEDs() {
 *       m_led.setLength(60);
 *       m_led.start();
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
  /** A functional interface for index mapping functions. */
  @FunctionalInterface
  interface IndexMapper {
    /**
     * Maps the index.
     *
     * @param bufLen Length of the buffer
     * @param index The index to map
     * @return The mapped index
     */
    int apply(int bufLen, int index);
  }

  /**
   * Writes the pattern to an LED buffer. Dynamic animations should be called periodically (such as
   * with a command or with a periodic method) to refresh the buffer over time.
   *
   * <p>This method is intentionally designed to use separate objects for reading and writing data.
   * By splitting them up, we can easily modify the behavior of some base pattern to make it {@link
   * #scrollAtRelativeSpeed(Frequency) scroll}, {@link #blink(Time, Time) blink}, or {@link
   * #breathe(Time) breathe} by intercepting the data writes to transform their behavior to whatever
   * we like.
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
   * Creates a pattern with remapped indices.
   *
   * @param indexMapper the index mapper
   * @return the mapped pattern
   */
  default LEDPattern mapIndex(IndexMapper indexMapper) {
    return (reader, writer) -> {
      int bufLen = reader.getLength();
      applyTo(
          new LEDReader() {
            @Override
            public int getLength() {
              return reader.getLength();
            }

            @Override
            public int getRed(int index) {
              return reader.getRed(indexMapper.apply(bufLen, index));
            }

            @Override
            public int getGreen(int index) {
              return reader.getGreen(indexMapper.apply(bufLen, index));
            }

            @Override
            public int getBlue(int index) {
              return reader.getBlue(indexMapper.apply(bufLen, index));
            }
          },
          (i, r, g, b) -> writer.setRGB(indexMapper.apply(bufLen, i), r, g, b));
    };
  }

  /**
   * Creates a pattern that displays this one in reverse. Scrolling patterns will scroll in the
   * opposite direction (but at the same speed). It will treat the end of an LED strip as the start,
   * and the start of the strip as the end. This can be useful for making ping-pong patterns that
   * travel from one end of an LED strip to the other, then reverse direction and move back to the
   * start. This can also be useful when working with LED strips connected in a serpentine pattern
   * (where the start of one strip is connected to the end of the previous one); however, consider
   * using a {@link AddressableLEDBufferView#reversed() reversed view} of the overall buffer for
   * that segment rather than reversing patterns.
   *
   * @return the reverse pattern
   * @see AddressableLEDBufferView#reversed()
   */
  default LEDPattern reversed() {
    return mapIndex((length, index) -> length - 1 - index);
  }

  /**
   * Creates a pattern that plays this one, but offset by a certain number of LEDs. The offset
   * pattern will wrap around, if necessary.
   *
   * @param offset how many LEDs to offset by
   * @return the offset pattern
   */
  default LEDPattern offsetBy(int offset) {
    return mapIndex((length, index) -> Math.floorMod(index + offset, length));
  }

  /**
   * Creates a pattern that plays this one scrolling up the buffer. The velocity controls how fast
   * the pattern returns back to its original position, and is in terms of the length of the LED
   * strip; scrolling across a segment that is 10 LEDs long will travel twice as fast as on a
   * segment that's only 5 LEDs long (assuming equal LED density on both segments).
   *
   * <p>For example, scrolling a pattern by one quarter of any LED strip's length per second,
   * regardless of the total number of LEDs on that strip:
   *
   * <pre>
   *   LEDPattern rainbow = LEDPattern.rainbow(255, 255);
   *   LEDPattern scrollingRainbow = rainbow.scrollAtRelativeSpeed(Percent.per(Second).of(25));
   * </pre>
   *
   * @param velocity how fast the pattern should move, in terms of how long it takes to do a full
   *     scroll along the length of LEDs and return back to the starting position
   * @return the scrolling pattern
   */
  default LEDPattern scrollAtRelativeSpeed(Frequency velocity) {
    final double periodMicros = velocity.asPeriod().in(Microseconds);

    return mapIndex(
        (bufLen, index) -> {
          long now = RobotController.getTime();

          // index should move by (buf.length) / (period)
          double t = (now % (long) periodMicros) / periodMicros;
          int offset = (int) (t * bufLen);

          return Math.floorMod(index + offset, bufLen);
        });
  }

  /**
   * Creates a pattern that plays this one scrolling up an LED strip. A negative velocity makes the
   * pattern play in reverse.
   *
   * <p>For example, scrolling a pattern at 4 inches per second along an LED strip with 60 LEDs per
   * meter:
   *
   * <pre>
   *   // LEDs per meter, a known value taken from the spec sheet of our particular LED strip
   *   Distance LED_SPACING = Meters.of(1.0 / 60);
   *
   *   LEDPattern rainbow = LEDPattern.rainbow();
   *   LEDPattern scrollingRainbow =
   *     rainbow.scrollAtAbsoluteSpeed(InchesPerSecond.of(4), LED_SPACING);
   * </pre>
   *
   * <p>Note that this pattern will scroll <i>faster</i> if applied to a less dense LED strip (such
   * as 30 LEDs per meter), or <i>slower</i> if applied to a denser LED strip (such as 120 or 144
   * LEDs per meter).
   *
   * @param velocity how fast the pattern should move along a physical LED strip
   * @param ledSpacing the distance between adjacent LEDs on the physical LED strip
   * @return the scrolling pattern
   */
  default LEDPattern scrollAtAbsoluteSpeed(LinearVelocity velocity, Distance ledSpacing) {
    // eg velocity = 10 m/s, spacing = 0.01m
    // meters per micro = 1e-5 m/us
    // micros per LED = 1e-2 m / (1e-5 m/us) = 1e-3 us

    var metersPerMicro = velocity.in(Meters.per(Microsecond));
    var microsPerLED = (int) (ledSpacing.in(Meters) / metersPerMicro);

    return mapIndex(
        (bufLen, index) -> {
          long now = RobotController.getTime();

          // every step in time that's a multiple of microsPerLED will increment the offset by 1
          var offset = (int) (now / microsPerLED);

          // floorMod so if the offset is negative, we still get positive outputs
          return Math.floorMod(index + offset, bufLen);
        });
  }

  /**
   * Creates a pattern that switches between playing this pattern and turning the entire LED strip
   * off.
   *
   * @param onTime how long the pattern should play for, per cycle
   * @param offTime how long the pattern should be turned off for, per cycle
   * @return the blinking pattern
   */
  default LEDPattern blink(Time onTime, Time offTime) {
    final long totalTimeMicros = (long) (onTime.in(Microseconds) + offTime.in(Microseconds));
    final long onTimeMicros = (long) onTime.in(Microseconds);

    return (reader, writer) -> {
      if (RobotController.getTime() % totalTimeMicros < onTimeMicros) {
        applyTo(reader, writer);
      } else {
        kOff.applyTo(reader, writer);
      }
    };
  }

  /**
   * Like {@link #blink(Time, Time) blink(onTime, offTime)}, but where the "off" time is exactly
   * equal to the "on" time.
   *
   * @param onTime how long the pattern should play for (and be turned off for), per cycle
   * @return the blinking pattern
   */
  default LEDPattern blink(Time onTime) {
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
  default LEDPattern breathe(Time period) {
    final long periodMicros = (long) period.in(Microseconds);

    return (reader, writer) -> {
      applyTo(
          reader,
          (i, r, g, b) -> {
            // How far we are in the cycle, in the range [0, 1)
            double t = (RobotController.getTime() % periodMicros) / (double) periodMicros;
            double phase = t * 2 * Math.PI;

            // Apply the cosine function and shift its output from [-1, 1] to [0, 1]
            // Use cosine so the period starts at 100% brightness
            double dim = (Math.cos(phase) + 1) / 2.0;

            int output = Color.lerpRGB(0, 0, 0, r, g, b, dim);

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

  /**
   * Similar to {@link #blend(LEDPattern)}, but performs a bitwise mask on each color channel rather
   * than averaging the colors for each LED. This can be helpful for displaying only a portion of
   * the base pattern by applying a mask that sets the desired area to white, and all other areas to
   * black. However, it can also be used to display only certain color channels or hues; for
   * example, masking with {@code LEDPattern.color(Color.kRed)} will turn off the green and blue
   * channels on the output pattern, leaving only the red LEDs to be illuminated.
   *
   * @param mask the mask to apply
   * @return the masked pattern
   */
  default LEDPattern mask(LEDPattern mask) {
    return (reader, writer) -> {
      // Apply the current pattern down as normal...
      applyTo(reader, writer);

      mask.applyTo(
          reader,
          (i, r, g, b) -> {
            // ... then perform a bitwise AND operation on each channel to apply the mask
            writer.setRGB(i, r & reader.getRed(i), g & reader.getGreen(i), b & reader.getBlue(i));
          });
    };
  }

  /**
   * Creates a pattern that plays this one, but at a different brightness. Brightness multipliers
   * are applied per-channel in the RGB space; no HSL or HSV conversions are applied. Multipliers
   * are also uncapped, which may result in the original colors washing out and appearing less
   * saturated or even just a bright white.
   *
   * <p>This method is predominantly intended for dimming LEDs to avoid painfully bright or
   * distracting patterns from playing (apologies to the 2024 NE Greater Boston field staff).
   *
   * <p>For example, dimming can be done simply by adding a call to `atBrightness` at the end of a
   * pattern:
   *
   * <pre>
   *   // Solid red, but at 50% brightness
   *   LEDPattern.solid(Color.kRed).atBrightness(Percent.of(50));
   *
   *   // Solid white, but at only 10% (i.e. ~0.5V)
   *   LEDPattern.solid(Color.kWhite).atBrightness(Percent.of(10));
   * </pre>
   *
   * @param relativeBrightness the multiplier to apply to all channels to modify brightness
   * @return the input pattern, displayed at
   */
  default LEDPattern atBrightness(Dimensionless relativeBrightness) {
    double multiplier = relativeBrightness.in(Value);

    return (reader, writer) -> {
      applyTo(
          reader,
          (i, r, g, b) -> {
            // Clamp RGB values to keep them in the range [0, 255].
            // Otherwise, the casts to byte would result in values like 256 wrapping to 0

            writer.setRGB(
                i,
                (int) MathUtil.clamp(r * multiplier, 0, 255),
                (int) MathUtil.clamp(g * multiplier, 0, 255),
                (int) MathUtil.clamp(b * multiplier, 0, 255));
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
   * Creates a pattern that works as a mask layer for {@link #mask(LEDPattern)} that illuminates
   * only the portion of the LED strip corresponding with some progress. The mask pattern will start
   * from the base and set LEDs to white at a proportion equal to the progress returned by the
   * function. Some usages for this could be for displaying progress of a flywheel to its target
   * velocity, progress of a complex autonomous sequence, or the height of an elevator.
   *
   * <p>For example, creating a mask for displaying a red-to-blue gradient, starting from the red
   * end, based on where an elevator is in its range of travel.
   *
   * <pre>
   *   LEDPattern basePattern = gradient(Color.kRed, Color.kBlue);
   *   LEDPattern progressPattern =
   *     basePattern.mask(progressMaskLayer(() -> elevator.getHeight() / elevator.maxHeight());
   * </pre>
   *
   * @param progressSupplier the function to call to determine the progress. This should return
   *     values in the range [0, 1]; any values outside that range will be clamped.
   * @return the mask pattern
   */
  static LEDPattern progressMaskLayer(DoubleSupplier progressSupplier) {
    return (reader, writer) -> {
      double progress = MathUtil.clamp(progressSupplier.getAsDouble(), 0, 1);

      int bufLen = reader.getLength();
      int max = (int) (bufLen * progress);

      for (int led = 0; led < max; led++) {
        writer.setLED(led, Color.kWhite);
      }

      for (int led = max; led < bufLen; led++) {
        writer.setLED(led, Color.kBlack);
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

  /** Types of gradients. */
  enum GradientType {
    /**
     * A continuous gradient, where the gradient wraps around to allow for seamless scrolling
     * effects.
     */
    kContinuous,

    /**
     * A discontinuous gradient, where the first pixel is set to the first color of the gradient and
     * the final pixel is set to the last color of the gradient. There is no wrapping effect, so
     * scrolling effects will display an obvious seam.
     */
    kDiscontinuous
  }

  /**
   * Creates a pattern that displays a non-animated gradient of colors across the entire length of
   * the LED strip. Colors are evenly distributed along the full length of the LED strip. The
   * gradient type is configured with the {@code type} parameter, allowing the gradient to be either
   * continuous (no seams, good for scrolling effects) or discontinuous (a clear seam is visible,
   * but the gradient applies to the full length of the LED strip without needing to use some space
   * for wrapping).
   *
   * @param type the type of gradient (continuous or discontinuous)
   * @param colors the colors to display in the gradient
   * @return a motionless gradient pattern
   */
  static LEDPattern gradient(GradientType type, Color... colors) {
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
      int ledsPerSegment =
          switch (type) {
            case kContinuous -> bufLen / numSegments;
            case kDiscontinuous -> (bufLen - 1) / (numSegments - 1);
          };

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
