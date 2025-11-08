// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <wpi/framework/TimedRobot.hpp>
#include <wpi/hardware/led/AddressableLED.hpp>
#include <wpi/hardware/led/LEDPattern.hpp>

class Robot : public wpi::TimedRobot {
 public:
  Robot();
  void RobotPeriodic() override;

 private:
  static constexpr int kLength = 60;

  // SmartIO port 1
  wpi::AddressableLED m_led{1};
  std::array<wpi::AddressableLED::LEDData, kLength>
      m_ledBuffer;  // Reuse the buffer

  // Our LED strip has a density of 120 LEDs per meter
  wpi::units::meter_t kLedSpacing{1 / 120.0};

  // Create an LED pattern that will display a rainbow across
  // all hues at maximum saturation and half brightness
  wpi::LEDPattern m_rainbow = wpi::LEDPattern::Rainbow(255, 128);

  // Create a new pattern that scrolls the rainbow pattern across the LED
  // strip, moving at a speed of 1 meter per second.
  wpi::LEDPattern m_scrollingRainbow =
      m_rainbow.ScrollAtAbsoluteSpeed(1_mps, kLedSpacing);
};
