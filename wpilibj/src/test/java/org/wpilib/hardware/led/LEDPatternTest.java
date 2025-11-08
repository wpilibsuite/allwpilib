// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.units.Units.Centimeters;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.Microsecond;
import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Percent;
import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.units.Units.Value;
import static edu.wpi.first.wpilibj.LEDPattern.GradientType.kContinuous;
import static edu.wpi.first.wpilibj.LEDPattern.GradientType.kDiscontinuous;
import static edu.wpi.first.wpilibj.util.Color.kBlack;
import static edu.wpi.first.wpilibj.util.Color.kBlue;
import static edu.wpi.first.wpilibj.util.Color.kGreen;
import static edu.wpi.first.wpilibj.util.Color.kLime;
import static edu.wpi.first.wpilibj.util.Color.kMagenta;
import static edu.wpi.first.wpilibj.util.Color.kMidnightBlue;
import static edu.wpi.first.wpilibj.util.Color.kPurple;
import static edu.wpi.first.wpilibj.util.Color.kRed;
import static edu.wpi.first.wpilibj.util.Color.kWhite;
import static edu.wpi.first.wpilibj.util.Color.kYellow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.fail;

import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class LEDPatternTest {
  long m_mockTime;

  // Applies a pattern of White, Yellow, Purple to LED triplets
  LEDPattern m_whiteYellowPurple =
      (reader, writer) -> {
        for (int led = 0; led < reader.getLength(); led++) {
          switch (led % 3) {
            case 0:
              writer.setLED(led, kWhite);
              break;
            case 1:
              writer.setLED(led, kYellow);
              break;
            case 2:
              writer.setLED(led, kPurple);
              break;
            default:
              fail("Bad test setup");
              break;
          }
        }
      };

  @BeforeEach
  void setUp() {
    m_mockTime = 0;
    RobotController.setTimeSource(() -> m_mockTime);
  }

  @AfterEach
  void tearDown() {
    RobotController.setTimeSource(RobotController::getFPGATime);
  }

  @Test
  void solidColor() {
    LEDPattern pattern = LEDPattern.solid(kYellow);
    AddressableLEDBuffer buffer = new AddressableLEDBuffer(99);
    pattern.applyTo(buffer);

    for (int i = 0; i < buffer.getLength(); i++) {
      assertEquals(kYellow, buffer.getLED(i));
    }
  }

  @Test
  void gradient0SetsToBlack() {
    LEDPattern pattern = LEDPattern.gradient(kContinuous);
    AddressableLEDBuffer buffer = new AddressableLEDBuffer(99);
    for (int i = 0; i < buffer.getLength(); i++) {
      buffer.setRGB(i, 127, 128, 129);
    }

    pattern.applyTo(buffer);

    for (int i = 0; i < buffer.getLength(); i++) {
      assertEquals(kBlack, buffer.getLED(i));
    }
  }

  @Test
  void gradient1SetsToSolid() {
    LEDPattern pattern = LEDPattern.gradient(kContinuous, kYellow);

    AddressableLEDBuffer buffer = new AddressableLEDBuffer(99);
    pattern.applyTo(buffer);

    for (int i = 0; i < buffer.getLength(); i++) {
      assertEquals(kYellow, buffer.getLED(i));
    }
  }

  @Test
  void continuousGradient2Colors() {
    LEDPattern pattern = LEDPattern.gradient(kContinuous, kYellow, kPurple);

    AddressableLEDBuffer buffer = new AddressableLEDBuffer(99);
    pattern.applyTo(buffer);

    assertColorEquals(kYellow, buffer.getLED(0));
    assertColorEquals(Color.lerpRGB(kYellow, kPurple, 25 / 49.0), buffer.getLED(25));
    assertColorEquals(kPurple, buffer.getLED(49));
    assertColorEquals(Color.lerpRGB(kYellow, kPurple, 25 / 49.0), buffer.getLED(73));
    assertColorEquals(kYellow, buffer.getLED(98));
  }

  @Test
  void discontinuousGradient2Colors() {
    LEDPattern pattern = LEDPattern.gradient(kDiscontinuous, kYellow, kPurple);

    AddressableLEDBuffer buffer = new AddressableLEDBuffer(99);
    pattern.applyTo(buffer);

    assertColorEquals(kYellow, buffer.getLED(0));
    assertColorEquals(Color.lerpRGB(kYellow, kPurple, 0.5), buffer.getLED(49));
    assertColorEquals(kPurple, buffer.getLED(98));
  }

  @Test
  void gradient3Colors() {
    LEDPattern pattern = LEDPattern.gradient(kContinuous, kYellow, kPurple, kWhite);
    AddressableLEDBuffer buffer = new AddressableLEDBuffer(99);
    pattern.applyTo(buffer);

    assertColorEquals(kYellow, buffer.getLED(0));
    assertColorEquals(Color.lerpRGB(kYellow, kPurple, 25.0 / 33.0), buffer.getLED(25));
    assertColorEquals(kPurple, buffer.getLED(33));
    assertColorEquals(Color.lerpRGB(kPurple, kWhite, 25.0 / 33.0), buffer.getLED(58));
    assertColorEquals(kWhite, buffer.getLED(66));
    assertColorEquals(Color.lerpRGB(kWhite, kYellow, 25.0 / 33.0), buffer.getLED(91));
    assertColorEquals(Color.lerpRGB(kWhite, kYellow, 32.0 / 33.0), buffer.getLED(98));
  }

  @Test
  void discontinuousGradient3Colors() {
    LEDPattern pattern = LEDPattern.gradient(kDiscontinuous, kYellow, kPurple, kWhite);
    AddressableLEDBuffer buffer = new AddressableLEDBuffer(101);
    pattern.applyTo(buffer);

    assertColorEquals(kYellow, buffer.getLED(0));
    assertColorEquals(Color.lerpRGB(kYellow, kPurple, 0.5), buffer.getLED(25));
    assertColorEquals(kPurple, buffer.getLED(50));
    assertColorEquals(Color.lerpRGB(kPurple, kWhite, 0.5), buffer.getLED(75));
    assertColorEquals(kWhite, buffer.getLED(100));
  }

  @Test
  void step0SetsToBlack() {
    LEDPattern pattern = LEDPattern.steps(Map.of());

    AddressableLEDBuffer buffer = new AddressableLEDBuffer(99);
    for (int i = 0; i < buffer.getLength(); i++) {
      buffer.setRGB(i, 127, 128, 129);
    }

    pattern.applyTo(buffer);
    for (int i = 0; i < 99; i++) {
      assertColorEquals(kBlack, buffer.getLED(i));
    }
  }

  @Test
  void step1SetsToSolid() {
    LEDPattern pattern = LEDPattern.steps(Map.of(0.0, kYellow));

    AddressableLEDBuffer buffer = new AddressableLEDBuffer(99);

    pattern.applyTo(buffer);
    for (int i = 0; i < 99; i++) {
      assertColorEquals(kYellow, buffer.getLED(i));
    }
  }

  @Test
  void step1HalfSetsToHalfOffHalfColor() {
    LEDPattern pattern = LEDPattern.steps(Map.of(0.50, kYellow));

    AddressableLEDBuffer buffer = new AddressableLEDBuffer(99);
    pattern.applyTo(buffer);

    // [0, 48] should be black...
    for (int i = 0; i < 49; i++) {
      assertColorEquals(kBlack, buffer.getLED(i));
    }
    // ... and [49, <end>] should be the color that was set
    for (int i = 49; i < buffer.getLength(); i++) {
      assertColorEquals(kYellow, buffer.getLED(i));
    }
  }

  @Test
  void scrollForward() {
    var buffer = new AddressableLEDBuffer(256);

    LEDPattern base =
        (reader, writer) -> {
          for (int led = 0; led < reader.getLength(); led++) {
            writer.setRGB(led, led % 256, led % 256, led % 256);
          }
        };

    // scroll forwards 1/256th (1 LED) per microsecond - this makes mock time easier
    var scroll = base.scrollAtRelativeSpeed(Value.per(Microsecond).of(1 / 256.0));

    for (int time = 0; time < 500; time++) {
      m_mockTime = time;
      scroll.applyTo(buffer);

      for (int led = 0; led < buffer.getLength(); led++) {
        // Base: [(0, 0, 0) (1, 1, 1) (2, 2, 2) (3, 3, 3) (4, 4, 4) ... (255, 255, 255)]
        // Value for every channel should DECREASE by 1 in each timestep, wrapping around 0 and 255

        // t=0,   channel value = (0, 1, 2, ..., 254, 255)
        // t=1,   channel value = (255, 0, 1, ..., 253, 254)
        // t=2,   channel value = (254, 255, 0, ..., 252, 253)
        // t=255, channel value = (1, 2, 3, ..., 255, 0)
        // t=256, channel value = (0, 1, 2, ..., 254, 255)
        int ch = Math.floorMod(led - time, 256);

        assertEquals(new Color8Bit(ch, ch, ch), buffer.getLED8Bit(led));
      }
    }
  }

  @Test
  void scrollBackward() {
    var buffer = new AddressableLEDBuffer(256);

    LEDPattern base =
        (reader, writer) -> {
          for (int led = 0; led < reader.getLength(); led++) {
            writer.setRGB(led, led % 256, led % 256, led % 256);
          }
        };

    // scroll backwards 1/256th (1 LED) per microsecond - this makes mock time easier
    var scroll = base.scrollAtRelativeSpeed(Value.per(Microsecond).of(-1 / 256.0));

    for (int time = 0; time < 500; time++) {
      m_mockTime = time;
      scroll.applyTo(buffer);

      for (int led = 0; led < buffer.getLength(); led++) {
        // Base: [(0, 0, 0) (1, 1, 1) (2, 2, 2) (3, 3, 3) (4, 4, 4) ... (255, 255, 255)]
        // Value for every channel should INCREASE by 1 in each timestep, wrapping around 0 and 255

        // t=0,   channel value = (0, 1, 2, ..., 254, 255)
        // t=1,   channel value = (1, 2, 3, ..., 255, 0)
        // t=2,   channel value = (2, 3, 4, ..., 0, 1)
        // t=255, channel value = (255, 0, 1, ..., 253, 254)
        // t=256, channel value = (0, 1, 2, ..., 254, 255)
        int ch = Math.floorMod(led + time, 256);

        assertEquals(new Color8Bit(ch, ch, ch), buffer.getLED8Bit(led));
      }
    }
  }

  @Test
  void scrollAbsoluteSpeedForward() {
    var buffer = new AddressableLEDBuffer(256);

    LEDPattern base =
        (reader, writer) -> {
          for (int led = 0; led < reader.getLength(); led++) {
            writer.setRGB(led, led % 256, led % 256, led % 256);
          }
        };

    // scroll at 16 m/s, LED spacing = 2cm
    // buffer is 256 LEDs, so total length = 512cm = 5.12m
    // scrolling at 16 m/s yields a period of 0.32 seconds, or 0.00125 seconds per LED (800 LEDs/s)
    var scroll = base.scrollAtAbsoluteSpeed(MetersPerSecond.of(16), Centimeters.of(2));

    for (int time = 0; time < 500; time++) {
      m_mockTime = time * 1_250; // 1.25ms per LED
      scroll.applyTo(buffer);

      for (int led = 0; led < buffer.getLength(); led++) {
        // Base: [(0, 0, 0) (1, 1, 1) (2, 2, 2) (3, 3, 3) (4, 4, 4) ... (255, 255, 255)]
        // Value for every channel should DECREASE by 1 in each timestep, wrapping around 0 and 255

        // t=0,   channel value = (0, 1, 2, ..., 254, 255)
        // t=1,   channel value = (255, 0, 1, ..., 253, 254)
        // t=2,   channel value = (254, 255, 0, ..., 252, 253)
        // t=255, channel value = (1, 2, 3, ..., 255, 0)
        // t=256, channel value = (0, 1, 2, ..., 254, 255)
        int ch = Math.floorMod(led - time, 256);

        assertEquals(new Color8Bit(ch, ch, ch), buffer.getLED8Bit(led));
      }
    }
  }

  @Test
  void scrollAbsoluteSpeedBackward() {
    var buffer = new AddressableLEDBuffer(256);

    LEDPattern base =
        (reader, writer) -> {
          for (int led = 0; led < reader.getLength(); led++) {
            writer.setRGB(led, led % 256, led % 256, led % 256);
          }
        };

    // scroll at 16 m/s, LED spacing = 2cm
    // buffer is 256 LEDs, so total length = 512cm = 5.12m
    // scrolling at 16 m/s yields a period of 0.32 seconds, or 0.00125 seconds per LED (800 LEDs/s)
    var scroll = base.scrollAtAbsoluteSpeed(MetersPerSecond.of(-16), Centimeters.of(2));

    for (int time = 0; time < 500; time++) {
      m_mockTime = time * 1_250; // 1.25ms per LED
      scroll.applyTo(buffer);

      for (int led = 0; led < buffer.getLength(); led++) {
        // Base: [(0, 0, 0) (1, 1, 1) (2, 2, 2) (3, 3, 3) (4, 4, 4) ... (255, 255, 255)]
        // Value for every channel should DECREASE by 1 in each timestep, wrapping around 0 and 255

        // t=0,   channel value = (0, 1, 2, ..., 254, 255)
        // t=1,   channel value = (255, 0, 1, ..., 253, 254)
        // t=2,   channel value = (254, 255, 0, ..., 252, 253)
        // t=255, channel value = (1, 2, 3, ..., 255, 0)
        // t=256, channel value = (0, 1, 2, ..., 254, 255)
        int ch = Math.floorMod(led + time, 256);

        assertEquals(new Color8Bit(ch, ch, ch), buffer.getLED8Bit(led));
      }
    }
  }

  @Test
  void rainbowAtFullSize() {
    var buffer = new AddressableLEDBuffer(180);

    int saturation = 255;
    int value = 255;
    var pattern = LEDPattern.rainbow(saturation, value);

    pattern.applyTo(buffer);

    for (int led = 0; led < buffer.getLength(); led++) {
      assertColorEquals(Color.fromHSV(led, saturation, value), buffer.getLED(led));
    }
  }

  @Test
  void rainbowAtHalfSize() {
    var buffer = new AddressableLEDBuffer(90);

    int saturation = 42;
    int value = 87;
    var pattern = LEDPattern.rainbow(saturation, value);

    pattern.applyTo(buffer);

    for (int led = 0; led < buffer.getLength(); led++) {
      assertColorEquals(Color.fromHSV(led * 2, saturation, value), buffer.getLED(led));
    }
  }

  @Test
  void rainbowAtOneThirdSize() {
    var buffer = new AddressableLEDBuffer(60);

    int saturation = 191;
    int value = 255;
    var pattern = LEDPattern.rainbow(saturation, value);

    pattern.applyTo(buffer);

    for (int led = 0; led < buffer.getLength(); led++) {
      assertColorEquals(Color.fromHSV(led * 3, saturation, value), buffer.getLED(led));
    }
  }

  @Test
  void rainbowAtDoubleSize() {
    var buffer = new AddressableLEDBuffer(360);

    int saturation = 212;
    int value = 93;
    var pattern = LEDPattern.rainbow(saturation, value);

    pattern.applyTo(buffer);

    for (int led = 0; led < buffer.getLength(); led++) {
      assertColorEquals(Color.fromHSV(led / 2, saturation, value), buffer.getLED(led));
    }
  }

  @Test
  void rainbowOddSize() {
    var buffer = new AddressableLEDBuffer(127);
    double scale = 180.0 / buffer.getLength();

    int saturation = 73;
    int value = 128;
    var pattern = LEDPattern.rainbow(saturation, value);

    pattern.applyTo(buffer);

    for (int led = 0; led < buffer.getLength(); led++) {
      assertColorEquals(Color.fromHSV((int) (led * scale), saturation, value), buffer.getLED(led));
    }
  }

  @Test
  void reverseSolid() {
    var buffer = new AddressableLEDBuffer(90);

    var pattern = LEDPattern.solid(Color.kRosyBrown).reversed();
    pattern.applyTo(buffer);

    for (int led = 0; led < buffer.getLength(); led++) {
      assertColorEquals(Color.kRosyBrown, buffer.getLED(led));
    }
  }

  @Test
  void reverseSteps() {
    var buffer = new AddressableLEDBuffer(100);

    var pattern = LEDPattern.steps(Map.of(0, kWhite, 0.5, kYellow)).reversed();
    pattern.applyTo(buffer);

    // colors should be swapped; yellow first, then white
    for (int led = 0; led < buffer.getLength(); led++) {
      if (led < 50) {
        assertColorEquals(kYellow, buffer.getLED(led));
      } else {
        assertColorEquals(kWhite, buffer.getLED(led));
      }
    }
  }

  @Test
  void offsetPositive() {
    var buffer = new AddressableLEDBuffer(21);

    // offset repeats PWY
    var offset = m_whiteYellowPurple.offsetBy(1);
    offset.applyTo(buffer);

    for (int led = 0; led < buffer.getLength(); led++) {
      Color color = buffer.getLED(led);
      switch (led % 3) {
        case 0:
          assertColorEquals(kPurple, color);
          break;
        case 1:
          assertColorEquals(kWhite, color);
          break;
        case 2:
          assertColorEquals(kYellow, color);
          break;
        default:
          fail("Bad test setup");
          break;
      }
    }
  }

  @Test
  void offsetNegative() {
    var buffer = new AddressableLEDBuffer(21);

    // offset repeats YPW
    var offset = m_whiteYellowPurple.offsetBy(-1);
    offset.applyTo(buffer);

    for (int led = 0; led < buffer.getLength(); led++) {
      Color color = buffer.getLED(led);
      switch (led % 3) {
        case 0:
          assertColorEquals(kYellow, color);
          break;
        case 1:
          assertColorEquals(kPurple, color);
          break;
        case 2:
          assertColorEquals(kWhite, color);
          break;
        default:
          fail("Bad test setup");
          break;
      }
    }
  }

  @Test
  void offsetZero() {
    var buffer = new AddressableLEDBuffer(21);

    // offset copies the base pattern, WYP
    var offset = m_whiteYellowPurple.offsetBy(0);
    offset.applyTo(buffer);

    for (int led = 0; led < buffer.getLength(); led++) {
      Color color = buffer.getLED(led);
      switch (led % 3) {
        case 0:
          assertColorEquals(kWhite, color);
          break;
        case 1:
          assertColorEquals(kYellow, color);
          break;
        case 2:
          assertColorEquals(kPurple, color);
          break;
        default:
          fail("Bad test setup");
          break;
      }
    }
  }

  @Test
  void blinkSymmetric() {
    // on for 2 seconds, off for 2 seconds
    var pattern = LEDPattern.solid(kWhite).blink(Seconds.of(2));

    var buffer = new AddressableLEDBuffer(1);

    for (int t = 0; t < 8; t++) {
      m_mockTime = t * 1_000_000L; // time travel 1 second
      pattern.applyTo(buffer);

      Color color = buffer.getLED(0);
      switch (t) {
        case 0:
        case 1:
        case 4:
        case 5:
          assertColorEquals(kWhite, color);
          break;
        case 2:
        case 3:
        case 6:
        case 7:
          assertColorEquals(kBlack, color);
          break;
        default:
          fail("Bad test setup");
          break;
      }
    }
  }

  @Test
  void blinkAsymmetric() {
    // on for 3 seconds, off for 1 second
    var pattern = LEDPattern.solid(kWhite).blink(Seconds.of(3), Seconds.of(1));

    var buffer = new AddressableLEDBuffer(1);

    for (int t = 0; t < 8; t++) {
      m_mockTime = t * 1_000_000L; // time travel 1 second
      pattern.applyTo(buffer);

      Color color = buffer.getLED(0);
      switch (t) {
        case 0:
        case 1:
        case 2: // first period
        case 4:
        case 5:
        case 6: // second period
          assertColorEquals(kWhite, color);
          break;
        case 3:
        case 7:
          assertColorEquals(kBlack, color);
          break;
        default:
          fail("Bad test setup");
          break;
      }
    }
  }

  @Test
  void blinkInSync() {
    AtomicBoolean condition = new AtomicBoolean(false);
    var pattern = LEDPattern.solid(kWhite).synchronizedBlink(condition::get);

    var buffer = new AddressableLEDBuffer(1);

    pattern.applyTo(buffer);
    assertColorEquals(kBlack, buffer.getLED(0));

    condition.set(true);
    pattern.applyTo(buffer);
    assertColorEquals(kWhite, buffer.getLED(0));

    condition.set(false);
    pattern.applyTo(buffer);
    assertColorEquals(kBlack, buffer.getLED(0));
  }

  @Test
  void breathe() {
    final Color midGray = new Color(0.5, 0.5, 0.5);

    var pattern = LEDPattern.solid(kWhite).breathe(Microseconds.of(4));

    var buffer = new AddressableLEDBuffer(1);

    {
      m_mockTime = 0; // start
      pattern.applyTo(buffer);
      assertColorEquals(kWhite, buffer.getLED(0));
    }

    {
      m_mockTime = 1; // midway (down)
      pattern.applyTo(buffer);
      assertColorEquals(midGray, buffer.getLED(0));
    }

    {
      m_mockTime = 2; // bottom
      pattern.applyTo(buffer);
      assertColorEquals(kBlack, buffer.getLED(0));
    }

    {
      m_mockTime = 3; // midway (up)
      pattern.applyTo(buffer);
      assertColorEquals(midGray, buffer.getLED(0));
    }

    {
      m_mockTime = 4; // back to start
      pattern.applyTo(buffer);
      assertColorEquals(kWhite, buffer.getLED(0));
    }
  }

  @Test
  void overlaySolidOnSolid() {
    var overlay = LEDPattern.solid(kYellow).overlayOn(LEDPattern.solid(kWhite));

    var buffer = new AddressableLEDBuffer(1);
    overlay.applyTo(buffer);

    assertColorEquals(kYellow, buffer.getLED(0));
  }

  @Test
  void overlayNearlyBlack() {
    Color overlayColor = new Color(new Color8Bit(1, 0, 0));
    var overlay = LEDPattern.solid(overlayColor).overlayOn(LEDPattern.solid(kWhite));

    var buffer = new AddressableLEDBuffer(1);
    overlay.applyTo(buffer);

    assertColorEquals(overlayColor, buffer.getLED(0));
  }

  @Test
  void overlayMixed() {
    var overlay =
        LEDPattern.steps(Map.of(0, kYellow, 0.5, kBlack)).overlayOn(LEDPattern.solid(kWhite));

    var buffer = new AddressableLEDBuffer(2);
    overlay.applyTo(buffer);

    assertColorEquals(kYellow, buffer.getLED(0));
    assertColorEquals(kWhite, buffer.getLED(1));
  }

  @Test
  void blend() {
    var pattern1 = LEDPattern.solid(kBlue);
    var pattern2 = LEDPattern.solid(kRed);
    var blend = pattern1.blend(pattern2);

    var buffer = new AddressableLEDBuffer(1);
    blend.applyTo(buffer);

    // Individual RGB channels are averaged; #0000FF blended with #FF0000 yields #7F007F
    assertColorEquals(new Color(127, 0, 127), buffer.getLED(0));
  }

  @Test
  void binaryMask() {
    Color color = new Color(123, 123, 123);
    var base = LEDPattern.solid(color);
    // first 50% mask on, last 50% mask off
    var mask = LEDPattern.steps(Map.of(0, kWhite, 0.5, kBlack));
    var masked = base.mask(mask);

    var buffer = new AddressableLEDBuffer(10);
    masked.applyTo(buffer);

    for (int i = 0; i < 5; i++) {
      assertColorEquals(color, buffer.getLED(i));
    }

    for (int i = 5; i < 10; i++) {
      assertColorEquals(kBlack, buffer.getLED(i));
    }
  }

  @Test
  void channelwiseMask() {
    Color baseColor = new Color(123, 123, 123);
    Color halfGray = new Color(0.5, 0.5, 0.5);
    var base = LEDPattern.solid(baseColor);

    var mask =
        LEDPattern.steps(Map.of(0, kRed, 0.2, kLime, 0.4, kBlue, 0.6, halfGray, 0.8, kWhite));

    var masked = base.mask(mask);

    var buffer = new AddressableLEDBuffer(5);
    masked.applyTo(buffer);

    assertColorEquals(new Color(123, 0, 0), buffer.getLED(0)); // red channel only
    assertColorEquals(new Color(0, 123, 0), buffer.getLED(1)); // green channel only
    assertColorEquals(new Color(0, 0, 123), buffer.getLED(2)); // blue channel only

    // mask channels are all 0b00111111, base is 0b00111011,
    // so the AND should give us the unmodified base color
    assertColorEquals(baseColor, buffer.getLED(3));
    assertColorEquals(baseColor, buffer.getLED(4)); // full color allowed
  }

  @Test
  void progressMaskLayer() {
    var progress = new AtomicReference<>(0.0);
    var maskLayer = LEDPattern.progressMaskLayer(progress::get);
    var buffer = new AddressableLEDBuffer(100);

    for (double t = 0; t <= 1.0; t += 0.01) {
      progress.set(t);
      maskLayer.applyTo(buffer);

      int lastMaskedLED = (int) (t * 100);
      for (int i = 0; i < lastMaskedLED; i++) {
        assertColorEquals(
            kWhite,
            buffer.getLED(i),
            "Progress " + lastMaskedLED + "%, LED " + i + " should be WHITE");
      }

      for (int i = lastMaskedLED; i < 100; i++) {
        assertColorEquals(
            kBlack,
            buffer.getLED(i),
            "Progress " + lastMaskedLED + "% , LED " + i + " should be BLACK");
      }
    }
  }

  @Test
  void zeroBrightness() {
    var pattern = LEDPattern.solid(kRed).atBrightness(Percent.of(0));
    var buffer = new AddressableLEDBuffer(1);
    pattern.applyTo(buffer);

    assertColorEquals(kBlack, buffer.getLED(0));
  }

  @Test
  void sameBrightness() {
    var pattern = LEDPattern.solid(kMagenta).atBrightness(Percent.of(100));
    var buffer = new AddressableLEDBuffer(1);
    pattern.applyTo(buffer);

    assertColorEquals(kMagenta, buffer.getLED(0));
  }

  @Test
  void higherBrightness() {
    var pattern = LEDPattern.solid(kMagenta).atBrightness(Value.of(4 / 3.0));
    var buffer = new AddressableLEDBuffer(1);
    pattern.applyTo(buffer);

    assertColorEquals(kMagenta, buffer.getLED(0));
  }

  @Test
  void negativeBrightness() {
    var pattern = LEDPattern.solid(kWhite).atBrightness(Percent.of(-1000));
    var buffer = new AddressableLEDBuffer(1);
    pattern.applyTo(buffer);

    assertColorEquals(kBlack, buffer.getLED(0));
  }

  @Test
  void clippingBrightness() {
    var pattern = LEDPattern.solid(kMidnightBlue).atBrightness(Percent.of(10000));
    var buffer = new AddressableLEDBuffer(1);
    pattern.applyTo(buffer);

    assertColorEquals(kWhite, buffer.getLED(0));
  }

  @Test
  void reverseMask() {
    var pattern =
        LEDPattern.steps(Map.of(0, kRed, 0.25, kBlue, 0.5, kYellow, 0.75, kGreen))
            .mask(LEDPattern.steps(Map.of(0, kWhite, 0.5, kBlack)))
            .reversed();
    var buffer = new AddressableLEDBuffer(8);

    pattern.applyTo(buffer);

    assertColorEquals(kRed, buffer.getLED(7));
    assertColorEquals(kRed, buffer.getLED(6));
    assertColorEquals(kBlue, buffer.getLED(5));
    assertColorEquals(kBlue, buffer.getLED(4));
    assertColorEquals(kBlack, buffer.getLED(3));
    assertColorEquals(kBlack, buffer.getLED(2));
    assertColorEquals(kBlack, buffer.getLED(1));
    assertColorEquals(kBlack, buffer.getLED(0));
  }

  @Test
  void offsetMask() {
    var pattern =
        LEDPattern.steps(Map.of(0, kRed, 0.25, kBlue, 0.5, kYellow, 0.75, kGreen))
            .mask(LEDPattern.steps(Map.of(0, kWhite, 0.5, kBlack)))
            .offsetBy(4);
    var buffer = new AddressableLEDBuffer(8);

    pattern.applyTo(buffer);

    assertColorEquals(kBlack, buffer.getLED(0));
    assertColorEquals(kBlack, buffer.getLED(1));
    assertColorEquals(kBlack, buffer.getLED(2));
    assertColorEquals(kBlack, buffer.getLED(3));
    assertColorEquals(kRed, buffer.getLED(4));
    assertColorEquals(kRed, buffer.getLED(5));
    assertColorEquals(kBlue, buffer.getLED(6));
    assertColorEquals(kBlue, buffer.getLED(7));
  }

  @Test
  void relativeScrollingMask() {
    // [red, red, blue, blue, yellow, yellow, green, green]
    // under a mask of first 50% on, last 50% off
    // [red, red, blue, blue, black, black, black, black]
    // all scrolling at 1 LED per microsecond
    var pattern =
        LEDPattern.steps(Map.of(0, kRed, 0.25, kBlue, 0.5, kYellow, 0.75, kGreen))
            .mask(LEDPattern.steps(Map.of(0, kWhite, 0.5, kBlack)))
            .scrollAtRelativeSpeed(Percent.per(Microsecond).of(12.5));
    var buffer = new AddressableLEDBuffer(8);

    {
      m_mockTime = 0; // start
      pattern.applyTo(buffer);
      assertColorEquals(kRed, buffer.getLED(0));
      assertColorEquals(kRed, buffer.getLED(1));
      assertColorEquals(kBlue, buffer.getLED(2));
      assertColorEquals(kBlue, buffer.getLED(3));
      assertColorEquals(kBlack, buffer.getLED(4));
      assertColorEquals(kBlack, buffer.getLED(5));
      assertColorEquals(kBlack, buffer.getLED(6));
      assertColorEquals(kBlack, buffer.getLED(7));
    }

    {
      m_mockTime = 1;
      pattern.applyTo(buffer);
      assertColorEquals(kBlack, buffer.getLED(0));
      assertColorEquals(kRed, buffer.getLED(1));
      assertColorEquals(kRed, buffer.getLED(2));
      assertColorEquals(kBlue, buffer.getLED(3));
      assertColorEquals(kBlue, buffer.getLED(4));
      assertColorEquals(kBlack, buffer.getLED(5));
      assertColorEquals(kBlack, buffer.getLED(6));
      assertColorEquals(kBlack, buffer.getLED(7));
    }

    {
      m_mockTime = 2;
      pattern.applyTo(buffer);
      assertColorEquals(kBlack, buffer.getLED(0));
      assertColorEquals(kBlack, buffer.getLED(1));
      assertColorEquals(kRed, buffer.getLED(2));
      assertColorEquals(kRed, buffer.getLED(3));
      assertColorEquals(kBlue, buffer.getLED(4));
      assertColorEquals(kBlue, buffer.getLED(5));
      assertColorEquals(kBlack, buffer.getLED(6));
      assertColorEquals(kBlack, buffer.getLED(7));
    }

    {
      m_mockTime = 3;
      pattern.applyTo(buffer);
      assertColorEquals(kBlack, buffer.getLED(0));
      assertColorEquals(kBlack, buffer.getLED(1));
      assertColorEquals(kBlack, buffer.getLED(2));
      assertColorEquals(kRed, buffer.getLED(3));
      assertColorEquals(kRed, buffer.getLED(4));
      assertColorEquals(kBlue, buffer.getLED(5));
      assertColorEquals(kBlue, buffer.getLED(6));
      assertColorEquals(kBlack, buffer.getLED(7));
    }
  }

  @Test
  void absoluteScrollingMask() {
    // [red, red, blue, blue, yellow, yellow, green, green]
    // under a mask of first 50% on, last 50% off
    // [red, red, blue, blue, black, black, black, black]
    // all scrolling at 1 LED per microsecond
    var pattern =
        LEDPattern.steps(Map.of(0, kRed, 0.25, kBlue, 0.5, kYellow, 0.75, kGreen))
            .mask(LEDPattern.steps(Map.of(0, kWhite, 0.5, kBlack)))
            .scrollAtAbsoluteSpeed(Meters.per(Microsecond).of(1), Meters.one());
    var buffer = new AddressableLEDBuffer(8);

    {
      m_mockTime = 0; // start
      pattern.applyTo(buffer);
      assertColorEquals(kRed, buffer.getLED(0));
      assertColorEquals(kRed, buffer.getLED(1));
      assertColorEquals(kBlue, buffer.getLED(2));
      assertColorEquals(kBlue, buffer.getLED(3));
      assertColorEquals(kBlack, buffer.getLED(4));
      assertColorEquals(kBlack, buffer.getLED(5));
      assertColorEquals(kBlack, buffer.getLED(6));
      assertColorEquals(kBlack, buffer.getLED(7));
    }

    {
      m_mockTime = 1;
      pattern.applyTo(buffer);
      assertColorEquals(kBlack, buffer.getLED(0));
      assertColorEquals(kRed, buffer.getLED(1));
      assertColorEquals(kRed, buffer.getLED(2));
      assertColorEquals(kBlue, buffer.getLED(3));
      assertColorEquals(kBlue, buffer.getLED(4));
      assertColorEquals(kBlack, buffer.getLED(5));
      assertColorEquals(kBlack, buffer.getLED(6));
      assertColorEquals(kBlack, buffer.getLED(7));
    }

    {
      m_mockTime = 2;
      pattern.applyTo(buffer);
      assertColorEquals(kBlack, buffer.getLED(0));
      assertColorEquals(kBlack, buffer.getLED(1));
      assertColorEquals(kRed, buffer.getLED(2));
      assertColorEquals(kRed, buffer.getLED(3));
      assertColorEquals(kBlue, buffer.getLED(4));
      assertColorEquals(kBlue, buffer.getLED(5));
      assertColorEquals(kBlack, buffer.getLED(6));
      assertColorEquals(kBlack, buffer.getLED(7));
    }

    {
      m_mockTime = 3;
      pattern.applyTo(buffer);
      assertColorEquals(kBlack, buffer.getLED(0));
      assertColorEquals(kBlack, buffer.getLED(1));
      assertColorEquals(kBlack, buffer.getLED(2));
      assertColorEquals(kRed, buffer.getLED(3));
      assertColorEquals(kRed, buffer.getLED(4));
      assertColorEquals(kBlue, buffer.getLED(5));
      assertColorEquals(kBlue, buffer.getLED(6));
      assertColorEquals(kBlack, buffer.getLED(7));
    }
  }

  void assertColorEquals(Color expected, Color actual) {
    assertEquals(new Color8Bit(expected), new Color8Bit(actual));
  }

  void assertColorEquals(Color expected, Color actual, String message) {
    assertEquals(new Color8Bit(expected), new Color8Bit(actual), message);
  }
}
