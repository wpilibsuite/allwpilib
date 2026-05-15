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
        wpilib.SmartDashboard.putData("PDP", self.pdp)

    def robotPeriodic(self):
        # Get the current going through channel 7, in Amperes.
        # The PDP returns the current in increments of 0.125A.
        # At low currents the current readings tend to be less accurate.
        current7 = self.pdp.getCurrent(7)
        wpilib.SmartDashboard.putNumber("Current Channel 7", current7)

        # Get the voltage going into the PDP, in Volts.
        # The PDP returns the voltage in increments of 0.05 Volts.
        voltage = self.pdp.getVoltage()
        wpilib.SmartDashboard.putNumber("Voltage", voltage)

        # Retrieves the temperature of the PDP, in degrees Celsius.
        temperatureCelsius = self.pdp.getTemperature()
        wpilib.SmartDashboard.putNumber("Temperature", temperatureCelsius)

        # Get the total current of all channels.
        totalCurrent = self.pdp.getTotalCurrent()
        wpilib.SmartDashboard.putNumber("Total Current", totalCurrent)

        # Get the total power of all channels.
        # Power is the bus voltage multiplied by the current with the units Watts.
        totalPower = self.pdp.getTotalPower()
        wpilib.SmartDashboard.putNumber("Total Power", totalPower)

        # Get the total energy of all channels.
        # Energy is the power summed over time with units Joules.
        totalEnergy = self.pdp.getTotalEnergy()
        wpilib.SmartDashboard.putNumber("Total Energy", totalEnergy)
