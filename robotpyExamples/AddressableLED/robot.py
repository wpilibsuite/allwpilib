#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpimath.units


class MyRobot(wpilib.TimedRobot):
    def __init__(self) -> None:
        super().__init__()

        # SmartIO port 1
        self.led = wpilib.AddressableLED(1)

        # Reuse buffer
        # Default to a length of 60
        self.ledData = [wpilib.AddressableLED.LEDData() for _ in range(60)]
        self.led.setLength(len(self.ledData))

        # Set the data
        self.led.setData(self.ledData)

        # Create an LED pattern that will display a rainbow across
        # all hues at maximum saturation and half brightness
        self.rainbow = wpilib.LEDPattern.rainbow(255, 128)

        # Our LED strip has a density of 120 LEDs per meter
        self.kLedSpacing = 1 / 120.0

        # Create a new pattern that scrolls the rainbow pattern across the LED strip, moving at a
        # speed of 1 meter per second.
        self.scrollingRainbow = self.rainbow.scrollAtAbsoluteSpeed(
            1,
            self.kLedSpacing,
        )

    def robotPeriodic(self) -> None:
        # Update the buffer with the rainbow animation
        self.scrollingRainbow.applyTo(self.ledData)
        # Set the LEDs
        self.led.setData(self.ledData)
