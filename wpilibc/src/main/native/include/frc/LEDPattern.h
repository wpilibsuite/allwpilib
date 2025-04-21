// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <span>
#include <utility>

#include <units/frequency.h>
#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>

#include "frc/AddressableLED.h"
#include "util/Color.h"

namespace frc {

class LEDPattern {
 public:
  /**
   * A wrapper around a length and an arbitrary reader function that accepts an
   * LED index and returns data for the LED at that index. This configuration
   * allows us to abstract over different container types without templating.
   */
  class LEDReader {
   public:
    LEDReader(std::function<frc::AddressableLED::LEDData(int)> impl,
              size_t size)
        : m_impl{std::move(impl)}, m_size{size} {}

    frc::AddressableLED::LEDData operator[](size_t index) const {
      return m_impl(index);
    }

    size_t size() const { return m_size; }

   private:
    std::function<frc::AddressableLED::LEDData(int)> m_impl;
    size_t m_size;
  };

  explicit LEDPattern(std::function<void(frc::LEDPattern::LEDReader,
                                         std::function<void(int, frc::Color)>)>
                          impl);

  void ApplyTo(LEDReader reader,
               std::function<void(int, frc::Color)> writer) const;

  /**
   * Writes the pattern to an LED buffer. Dynamic animations should be called
   * periodically (such as with a command or with a periodic method) to refresh
   * the buffer over time.
   *
   * This method is intentionally designed to use separate objects for reading
   * and writing data. By splitting them up, we can easily modify the behavior
   * of some base pattern to make it scroll, blink, or breathe by intercepting
   * the data writes to transform their behavior to whatever we like.
   *
   * @param data the current data of the LED strip
   * @param writer data writer for setting new LED colors on the LED strip
   */
  void ApplyTo(std::span<frc::AddressableLED::LEDData> data,
               std::function<void(int, frc::Color)> writer) const;

  /**
   * Writes the pattern to an LED buffer. Dynamic animations should be called
   * periodically (such as with a command or with a periodic method) to refresh
   * the buffer over time.
   *
   * This method is intentionally designed to use separate objects for reading
   * and writing data. By splitting them up, we can easily modify the behavior
   * of some base pattern to make it scroll, blink, or breathe by intercepting
   * the data writes to transform their behavior to whatever we like.
   *
   * @param data the current data of the LED strip
   */
  void ApplyTo(std::span<frc::AddressableLED::LEDData> data) const;

  /**
   * Creates a pattern with remapped indices.
   *
   * @param indexMapper the index mapper
   * @return the mapped pattern
   */
  [[nodiscard]]
  LEDPattern MapIndex(std::function<size_t(size_t, size_t)> indexMapper);

  /**
   * Creates a pattern that displays this one in reverse. Scrolling patterns
   * will scroll in the opposite direction (but at the same speed). It will
   * treat the end of an LED strip as the start, and the start of the strip as
   * the end. This can be useful for making ping-pong patterns that travel from
   * one end of an LED strip to the other, then reverse direction and move back
   * to the start. This can also be useful when working with LED strips
   * connected in a serpentine pattern (where the start of one strip is
   * connected to the end of the previous one).
   *
   * @return the reverse pattern
   */
  [[nodiscard]]
  LEDPattern Reversed();

  /**
   * Creates a pattern that displays this one, but offset by a certain number of
   * LEDs. The offset pattern will wrap around, if necessary.
   *
   * @param offset how many LEDs to offset by
   * @return the offset pattern
   */
  [[nodiscard]]
  LEDPattern OffsetBy(int offset);

  /**
   * Creates a pattern that plays this one scrolling up the buffer. The velocity
   * controls how fast the pattern returns back to its original position, and is
   * in terms of the length of the LED strip; scrolling across a segment that is
   * 10 LEDs long will travel twice as fast as on a segment that's only 5 LEDs
   * long (assuming equal LED density on both segments).
   */
  [[nodiscard]]
  LEDPattern ScrollAtRelativeSpeed(units::hertz_t velocity);

  /**
   * Creates a pattern that plays this one scrolling up an LED strip. A negative
   * velocity makes the pattern play in reverse.
   *
   * <p>For example, scrolling a pattern at 4 inches per second along an LED
   * strip with 60 LEDs per meter:
   *
   * <pre>
   *   // LEDs per meter, a known value taken from the spec sheet of our
   * particular LED strip units::meter_t LED_SPACING = units::meter_t{1 /60.0};
   *
   *   frc::LEDPattern rainbow = frc::LEDPattern::Rainbow();
   *   frc::LEDPattern scrollingRainbow =
   *     rainbow.ScrollAtAbsoluteSpeed(units::feet_per_second_t{1 / 3.0},
   * LED_SPACING);
   * </pre>
   *
   * <p>Note that this pattern will scroll <i>faster</i> if applied to a less
   * dense LED strip (such as 30 LEDs per meter), or <i>slower</i> if applied to
   * a denser LED strip (such as 120 or 144 LEDs per meter).
   *
   * @param velocity how fast the pattern should move along a physical LED strip
   * @param ledSpacing the distance between adjacent LEDs on the physical LED
   * strip
   * @return the scrolling pattern
   */
  [[nodiscard]]
  LEDPattern ScrollAtAbsoluteSpeed(units::meters_per_second_t velocity,
                                   units::meter_t ledSpacing);

  /**
   * Creates a pattern that switches between playing this pattern and turning
   * the entire LED strip off.
   *
   * @param onTime how long the pattern should play for, per cycle
   * @param offTime how long the pattern should be turned off for, per cycle
   * @return the blinking pattern
   */
  [[nodiscard]]
  LEDPattern Blink(units::second_t onTime, units::second_t offTime);

  /**
   * Like {@link LEDPattern::Blink(units::second_t)}, but where the
   * "off" time is exactly equal to the "on" time.
   *
   * @param onTime how long the pattern should play for (and be turned off for),
   * per cycle
   * @return the blinking pattern
   */
  [[nodiscard]]
  LEDPattern Blink(units::second_t onTime);

  /**
   * Creates a pattern that blinks this one on and off in sync with a true/false
   * signal. The pattern will play while the signal outputs {@code true}, and
   * will turn off while the signal outputs
   * {@code false}.
   *
   * @param signal the signal to synchronize with
   * @return the blinking pattern
   */
  [[nodiscard]]
  LEDPattern SynchronizedBlink(std::function<bool()> signal);

  /**
   * Creates a pattern that brightens and dims this one over time. Brightness
   * follows a sinusoidal pattern.
   *
   * @param period how fast the breathing pattern should complete a single cycle
   * @return the breathing pattern
   */
  [[nodiscard]]
  LEDPattern Breathe(units::second_t period);

  /**
   * Creates a pattern that plays this pattern overlaid on another. Anywhere
   * this pattern sets an LED to off (or {@link frc::Color::kBlack}), the base
   * pattern will be displayed instead.
   *
   * @param base the base pattern to overlay on top of
   * @return the combined overlay pattern
   */
  [[nodiscard]]
  LEDPattern OverlayOn(const LEDPattern& base);

  /**
   * Creates a pattern that displays outputs as a combination of this pattern
   * and another. Color values are calculated as the average color of both
   * patterns; if both patterns set the same LED to the same color, then it is
   * set to that color, but if one pattern sets to one color and the other
   * pattern sets it to off, then it will show the color of the first pattern
   * but at approximately half brightness. This is different from {@link
   * LEDPattern::OverlayOn(const LEDPattern&)}, which will show the base pattern
   * at full brightness if the overlay is set to off at that position.
   *
   * @param other the pattern to blend with
   * @return the blended pattern
   */
  [[nodiscard]]
  LEDPattern Blend(const LEDPattern& other);

  /**
   * Similar to {@link LEDPattern::Blend(const LEDPattern&)}, but performs a
   * bitwise mask on each color channel rather than averaging the colors for
   * each LED. This can be helpful for displaying only a portion of the base
   * pattern by applying a mask that sets the desired area to white, and all
   * other areas to black. However, it can also be used to display only certain
   * color channels or hues; for example, masking with {@code
   * LEDPattern.color(Color.kRed)} will turn off the green and blue channels on
   * the output pattern, leaving only the red LEDs to be illuminated.
   *
   * @param mask the mask to apply
   * @return the masked pattern
   */
  [[nodiscard]]
  LEDPattern Mask(const LEDPattern& mask);

  /**
   * Creates a pattern that plays this one, but at a different brightness.
   * Brightness multipliers are applied per-channel in the RGB space; no HSL or
   * HSV conversions are applied. Multipliers are also uncapped, which may
   * result in the original colors washing out and appearing less saturated or
   * even just a bright white.
   *
   * <p>This method is predominantly intended for dimming LEDs to avoid
   * painfully bright or distracting patterns from playing (apologies to the
   * 2024 NE Greater Boston field staff).
   *
   * <p>For example, dimming can be done simply by adding a call to
   * `atBrightness` at the end of a pattern:
   *
   * <pre>
   *   // Solid red, but at 50% brightness
   *   frc::LEDPattern::Solid(frc::Color::kRed).AtBrightness(0.5);
   *
   *   // Solid white, but at only 10% (i.e. ~0.5V)
   *   frc::LEDPattern::Solid(frc:Color::kWhite).AtBrightness(0.1);
   * </pre>
   *
   * @param relativeBrightness the multiplier to apply to all channels to modify
   * brightness
   * @return the input pattern, displayed at
   */
  [[nodiscard]]
  LEDPattern AtBrightness(double relativeBrightness);

  /** A pattern that turns off all LEDs. */
  static LEDPattern Off();

  /**
   * Creates a pattern that displays a single static color along the entire
   * length of the LED strip.
   *
   * @param color the color to display
   * @return the pattern
   */
  static LEDPattern Solid(const Color color);

  /**
   * Creates a pattern that works as a mask layer for {@link
   * LEDPattern::Mask(const LEDPattern&)} that illuminates only the portion of
   * the LED strip corresponding with some progress. The mask pattern will start
   * from the base and set LEDs to white at a proportion equal to the progress
   * returned by the function. Some usages for this could be for displaying
   * progress of a flywheel to its target velocity, progress of a complex
   * autonomous sequence, or the height of an elevator.
   *
   * <p>For example, creating a mask for displaying a red-to-blue gradient,
   * starting from the red end, based on where an elevator is in its range of
   * travel.
   *
   * <pre>
   * frc::LEDPattern basePattern =
   *   frc::LEDPattern::Gradient(frc::Color::kRed, frc::Color::kBlue);
   * frc::LEDPattern progressPattern =
   *   basePattern.Mask(frc::LEDPattern::ProgressMaskLayer([&]() {
   *     return elevator.GetHeight() / elevator.MaxHeight();
   *   });
   * </pre>
   *
   * @param progressFunction the function to call to determine the progress.
   * This should return values in the range [0, 1]; any values outside that
   * range will be clamped.
   * @return the mask pattern
   */
  static LEDPattern ProgressMaskLayer(std::function<double()> progressFunction);

  /**
   * Display a set of colors in steps across the length of the LED strip. No
   * interpolation is done between colors. Colors are specified by the first LED
   * on the strip to show that color. The last color in the map will be
   * displayed all the way to the end of the strip. LEDs positioned before the
   * first specified step will be turned off (you can think of this as if
   * there's a 0 -> black step by default).
   *
   * @param steps a map of progress to the color to start displaying at that
   * position along the LED strip
   * @return a motionless step pattern
   */
  static LEDPattern Steps(std::span<const std::pair<double, Color>> steps);

  /**
   * Display a set of colors in steps across the length of the LED strip. No
   * interpolation is done between colors. Colors are specified by the first LED
   * on the strip to show that color. The last color in the map will be
   * displayed all the way to the end of the strip. LEDs positioned before the
   * first specified step will be turned off (you can think of this as if
   * there's a 0 -> black step by default).
   *
   * @param steps a map of progress to the color to start displaying at that
   * position along the LED strip
   * @return a motionless step pattern
   */
  static LEDPattern Steps(
      std::initializer_list<std::pair<double, Color>> steps);

  /** Types of gradients. */
  enum GradientType {
    /**
     * A continuous gradient, where the gradient wraps around to allow for
     * seamless scrolling effects.
     */
    kContinuous,
    /**
     * A discontinuous gradient, where the first pixel is set to the first color
     * of the gradient and the final pixel is set to the last color of the
     * gradient. There is no wrapping effect, so scrolling effects will display
     * an obvious seam.
     */
    kDiscontinuous
  };

  /**
   * Creates a pattern that displays a non-animated gradient of colors across
   * the entire length of the LED strip. Colors are evenly distributed along the
   * full length of the LED strip. The gradient type is configured with the
   * {@code type} parameter, allowing the gradient to be either continuous (no
   * seams, good for scrolling effects) or discontinuous (a clear seam is
   * visible, but the gradient applies to the full length of the LED strip
   * without needing to use some space for wrapping).
   *
   * @param type the type of gradient (continuous or discontinuous)
   * @param colors the colors to display in the gradient
   * @return a motionless gradient pattern
   */
  static LEDPattern Gradient(GradientType type, std::span<const Color> colors);

  /**
   * Creates a pattern that displays a non-animated gradient of colors across
   * the entire length of the LED strip. Colors are evenly distributed along the
   * full length of the LED strip. The gradient type is configured with the
   * {@code type} parameter, allowing the gradient to be either continuous (no
   * seams, good for scrolling effects) or discontinuous (a clear seam is
   * visible, but the gradient applies to the full length of the LED strip
   * without needing to use some space for wrapping).
   *
   * @param type the type of gradient (continuous or discontinuous)
   * @param colors the colors to display in the gradient
   * @return a motionless gradient pattern
   */
  static LEDPattern Gradient(GradientType type,
                             std::initializer_list<Color> colors);

  /**
   * Creates an LED pattern that displays a rainbow across the color wheel. The
   * rainbow pattern will stretch across the entire length of the LED strip.
   *
   * @param saturation the saturation of the HSV colors, in [0, 255]
   * @param value the value of the HSV colors, in [0, 255]
   * @return the rainbow pattern
   */
  static LEDPattern Rainbow(int saturation, int value);

 private:
  std::function<void(frc::LEDPattern::LEDReader,
                     std::function<void(int, frc::Color)>)>
      m_impl;
};
}  // namespace frc
