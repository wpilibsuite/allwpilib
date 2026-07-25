#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib


class MyRobot(wpilib.TimedRobot):
    """
    This is a sample program showing how to retrieve information from the Power
    Distribution Panel via CAN. The information will be displayed under variables
    through the SmartDashboard.
    """

    def __init__(self):
        """Robot initialization function"""
        super().__init__()

        # Object for dealing with the Power Distribution Panel (PDP).
        self.pdp = wpilib.PowerDistribution(0)

        # Put the PDP itself to the dashboard
        wpilib.SmartDashboard.put_data("PDP", self.pdp)

    def robot_periodic(self):
        # Get the current going through channel 7, in Amperes.
        # The PDP returns the current in increments of 0.125A.
        # At low currents the current readings tend to be less accurate.
        current7 = self.pdp.get_current(7)
        wpilib.SmartDashboard.put_number("Current Channel 7", current7)

        # Get the voltage going into the PDP, in Volts.
        # The PDP returns the voltage in increments of 0.05 Volts.
        voltage = self.pdp.get_voltage()
        wpilib.SmartDashboard.put_number("Voltage", voltage)

        # Retrieves the temperature of the PDP, in degrees Celsius.
        temperature_celsius = self.pdp.get_temperature()
        wpilib.SmartDashboard.put_number("Temperature", temperature_celsius)

        # Get the total current of all channels.
        total_current = self.pdp.get_total_current()
        wpilib.SmartDashboard.put_number("Total Current", total_current)

        # Get the total power of all channels.
        # Power is the bus voltage multiplied by the current with the units Watts.
        total_power = self.pdp.get_total_power()
        wpilib.SmartDashboard.put_number("Total Power", total_power)

        # Get the total energy of all channels.
        # Energy is the power summed over time with units Joules.
        total_energy = self.pdp.get_total_energy()
        wpilib.SmartDashboard.put_number("Total Energy", total_energy)
