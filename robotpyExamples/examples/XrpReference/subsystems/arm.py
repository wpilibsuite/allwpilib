#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
import xrp


class Arm(commands2.Subsystem):

    def __init__(self):
        """Creates a new Arm."""

        # Device number 4 maps to the physical Servo 1 port on the XRP
        self.arm_servo = xrp.XRPServo(4)

    def periodic(self):
        """This method will be called once per scheduler run"""

    def set_angle(self, angle_deg: float):
        """
        Set the current angle of the arm (0 - 180 degrees).

        :param angleDeg: Desired arm angle in degrees
        """
        self.arm_servo.set_angle(angle_deg)
