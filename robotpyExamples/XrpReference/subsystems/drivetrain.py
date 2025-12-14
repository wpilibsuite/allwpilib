#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import math

import commands2
import wpilib
import wpiutil
import xrp


class Drivetrain(commands2.Subsystem):
    kGearRatio = (30.0 / 14.0) * (28.0 / 16.0) * (36.0 / 9.0) * (26.0 / 8.0)  #  48.75:1
    kCountsPerMotorShaftRev = 12.0
    kCountsPerRevolution = kCountsPerMotorShaftRev * kGearRatio  # 585.0
    kWheelDiameterInch = 2.3622  # 60 mm

    def __init__(self) -> None:

        # The XRP has the left and right motors set to
        # PWM channels 0 and 1 respectively
        self.leftMotor = xrp.XRPMotor(0)
        self.rightMotor = xrp.XRPMotor(1)

        # The XRP has onboard encoders that are hardcoded
        # to use DIO pins 4/5 and 6/7 for the left and right
        self.leftEncoder = wpilib.Encoder(4, 5)
        self.rightEncoder = wpilib.Encoder(6, 7)

        # Set up the differential drive controller
        self.drive = wpilib.DifferentialDrive(self.leftMotor.set, self.rightMotor.set)

        # TODO: these don't work
        # wpiutil.SendableRegistry.addChild(self.drive, self.leftMotor)
        # wpiutil.SendableRegistry.addChild(self.drive, self.rightMotor)

        # Set up the XRPGyro
        self.gyro = xrp.XRPGyro()

        # We need to invert one side of the drivetrain so that positive voltages
        # result in both sides moving forward. Depending on how your robot's
        # gearbox is constructed, you might have to invert the left side instead.
        self.rightMotor.setInverted(True)

        # Use inches as unit for encoder distances
        self.leftEncoder.setDistancePerPulse(
            (math.pi * self.kWheelDiameterInch) / self.kCountsPerRevolution
        )
        self.rightEncoder.setDistancePerPulse(
            (math.pi * self.kWheelDiameterInch) / self.kCountsPerRevolution
        )
        self.resetEncoders()

    def arcadeDrive(self, xaxisVelocity: float, zaxisRotate: float) -> None:
        """
        Drives the robot using arcade controls.

        :param xaxisVelocity: the commanded forward movement
        :param zaxisRotate: the commanded rotation
        """
        self.drive.arcadeDrive(xaxisVelocity, zaxisRotate)

    def resetEncoders(self) -> None:
        """Resets the drive encoders to currently read a position of 0."""
        self.leftEncoder.reset()
        self.rightEncoder.reset()

    def getLeftEncoderCount(self) -> int:
        return self.leftEncoder.get()

    def getRightEncoderCount(self) -> int:
        return self.rightEncoder.get()

    def getLeftDistanceInch(self) -> float:
        return self.leftEncoder.getDistance()

    def getRightDistanceInch(self) -> float:
        return self.rightEncoder.getDistance()

    def getAverageDistanceInch(self) -> float:
        """Gets the average distance of the TWO encoders."""
        return (self.getLeftDistanceInch() + self.getRightDistanceInch()) / 2.0

    def getGyroAngleX(self) -> float:
        """Current angle of the Romi around the X-axis.

        :returns: The current angle of the Romi in degrees
        """
        return self.gyro.getAngleX()

    def getGyroAngleY(self) -> float:
        """Current angle of the Romi around the Y-axis.

        :returns: The current angle of the Romi in degrees
        """
        return self.gyro.getAngleY()

    def getGyroAngleZ(self) -> float:
        """Current angle of the Romi around the Z-axis.

        :returns: The current angle of the Romi in degrees
        """
        return self.gyro.getAngleZ()

    def resetGyro(self) -> None:
        """Reset the gyro"""
        self.gyro.reset()

    def periodic(self) -> None:
        """This method will be called once per scheduler run"""
