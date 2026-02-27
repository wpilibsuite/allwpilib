#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command, Subsystem
import wpilib


class Pneumatics(Subsystem):
    """Subsystem for managing the compressor, pressure sensor, etc."""

    # External analog pressure sensor
    # product-specific voltage->pressure conversion, see product manual
    # in this case, 250(V/5)-25
    # the scale parameter in the AnalogPotentiometer constructor is scaled from 1 instead of 5,
    # so if r is the raw AnalogPotentiometer output, the pressure is 250r-25
    kScale = 250.0
    kOffset = -25.0

    def __init__(self) -> None:
        super().__init__()
        self.pressureTransducer = wpilib.AnalogPotentiometer(
            # the AnalogIn port
            2,
            self.kScale,
            self.kOffset,
        )

        # Compressor connected to a PCM with a default CAN ID (0)
        self.compressor = wpilib.Compressor(0, wpilib.PneumaticsModuleType.CTREPCM)

    def getPressure(self) -> float:
        """Query the analog pressure sensor.

        :returns: the measured pressure, in PSI
        """
        # Get the pressure (in PSI) from an analog pressure sensor connected to the RIO.
        return self.pressureTransducer.get()

    def disableCompressorCommand(self) -> Command:
        """Disable the compressor closed-loop for as long as the command runs.

        Structured this way as the compressor is enabled by default.

        :returns: command
        """
        return self.startEnd(
            # Disable closed-loop mode on the compressor.
            self.compressor.disable,
            # Enable closed-loop mode based on the digital pressure switch connected to the
            # PCM/PH.
            # The switch is open when the pressure is over ~120 PSI.
            self.compressor.enableDigital,
        ).withName("Compressor Disabled")
