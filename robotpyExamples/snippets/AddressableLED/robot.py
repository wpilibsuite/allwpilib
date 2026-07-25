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
        self.led_data = wpilib.AddressableLEDBuffer(60)
        self.led.set_length(len(self.led_data))

        # Set the data
        self.led.set_data(self.led_data)

        # Create an LED pattern that will display a rainbow across
        # all hues at maximum saturation and half brightness
        self.rainbow = wpilib.LEDPattern.rainbow(255, 128)

        # Our LED strip has a density of 120 LEDs per meter
        self.LED_SPACING = 1 / 120.0

        # Create a new pattern that scrolls the rainbow pattern across the LED strip, moving at a
        # velocity of 1 meter per second.
        self.scrolling_rainbow = self.rainbow.scroll_at_absolute_velocity(
            1,
            self.LED_SPACING,
        )

    def robot_periodic(self) -> None:
        # Update the buffer with the rainbow animation
        self.scrolling_rainbow.apply_to(self.led_data)
        # Set the LEDs
        self.led.set_data(self.led_data)
