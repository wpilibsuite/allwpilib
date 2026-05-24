#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib

kSolenoidButton = 1
kDoubleSolenoidForwardButton = 2
kDoubleSolenoidReverseButton = 3
kCompressorButton = 4


class MyRobot(wpilib.TimedRobot):
    def __init__(self) -> None:
        super().__init__()
        self.joystick = wpilib.Joystick(0)

        # Solenoid corresponds to a single solenoid.
        # In this case, it's connected to channel 0 of a PH with the default CAN ID.
        self.solenoid = wpilib.Solenoid(
            busId=0, moduleType=wpilib.PneumaticsModuleType.REV_PH, channel=0
        )

        # DoubleSolenoid corresponds to a double solenoid.
        # In this case, it's connected to channels 1 and 2 of a PH with the default CAN ID.
        self.doubleSolenoid = wpilib.DoubleSolenoid(
            busId=0,
            moduleType=wpilib.PneumaticsModuleType.REV_PH,
            forwardChannel=1,
            reverseChannel=2,
        )

        # Compressor connected to a PH with a default CAN ID (1)
        self.compressor = wpilib.Compressor(
            busId=0, moduleType=wpilib.PneumaticsModuleType.REV_PH
        )

        # Publish elements to dashboard.
        wpilib.SmartDashboard.putData("Single Solenoid", self.solenoid)
        wpilib.SmartDashboard.putData("Double Solenoid", self.doubleSolenoid)
        wpilib.SmartDashboard.putData("Compressor", self.compressor)

    def teleopPeriodic(self) -> None:
        # Get the pressure (in PSI) from the analog sensor connected to the PH.
        # This function is supported only on the PH! On a PCM, this returns 0.
        wpilib.SmartDashboard.putNumber(
            "PH Pressure [PSI]", self.compressor.getPressure()
        )
        # Get compressor current draw.
        wpilib.SmartDashboard.putNumber(
            "Compressor Current", self.compressor.getCurrent()
        )
        # Get whether the compressor is active.
        wpilib.SmartDashboard.putBoolean(
            "Compressor Active", self.compressor.isEnabled()
        )
        # Get the digital pressure switch connected to the PCM/PH.
        # The switch is open when the pressure is over ~120 PSI.
        wpilib.SmartDashboard.putBoolean(
            "Pressure Switch", self.compressor.getPressureSwitchValue()
        )

        # The output of getRawButton is true/false depending on whether
        # the button is pressed; set takes a boolean for whether
        # to retract the solenoid (false) or extend it (true).
        self.solenoid.set(self.joystick.getRawButton(kSolenoidButton))

        # getRawButtonPressed will only return true once per press.
        # If a button is pressed, set the solenoid to the respective channel.
        if self.joystick.getRawButtonPressed(kDoubleSolenoidForwardButton):
            self.doubleSolenoid.set(wpilib.DoubleSolenoid.Value.FORWARD)
        elif self.joystick.getRawButtonPressed(kDoubleSolenoidReverseButton):
            self.doubleSolenoid.set(wpilib.DoubleSolenoid.Value.REVERSE)

        # On button press, toggle the compressor.
        if self.joystick.getRawButtonPressed(kCompressorButton):
            isCompressorEnabled = self.compressor.isEnabled()
            if isCompressorEnabled:
                # Disable closed-loop mode on the compressor.
                self.compressor.disable()
            else:
                # Change the if statements to select the closed-loop you want to use:

                if False:
                    # Enable closed-loop mode based on the digital pressure switch.
                    # The switch is open when the pressure is over ~120 PSI.
                    self.compressor.enableDigital()

                if True:
                    # Enable closed-loop mode based on the analog pressure sensor.
                    # The compressor will run while the pressure is in the specified
                    # range ([70 PSI, 120 PSI] in this example).
                    # Analog mode exists only on the PH! On the PCM, enables digital.
                    self.compressor.enableAnalog(70, 120)

                if False:
                    # Enable closed-loop mode based on both the digital pressure switch
                    # AND the analog pressure sensor connected to the PH.
                    # Hybrid mode exists only on the PH! On the PCM, enables digital.
                    self.compressor.enableHybrid(70, 120)
