#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
import wpiutil


class MyRobot(wpilib.TimedRobot):
    """
    This sample program shows how to use Mechanism2d - a visual representation of arms, elevators,
    and other mechanisms on dashboards; driven by a node-based API.

    Ligaments are based on other ligaments or roots, and roots are contained in the base
    Mechanism2d object.
    """

    METERS_PER_PULSE = 0.01
    ELEVATOR_MINIMUM_LENGTH = 0.5

    def __init__(self):
        super().__init__()
        self.elevator_motor = wpilib.PWMSparkMax(0)
        self.wrist_motor = wpilib.PWMSparkMax(1)
        self.wrist_pot = wpilib.AnalogPotentiometer(1, 90)
        self.elevator_encoder = wpilib.Encoder(0, 1)
        self.joystick = wpilib.Joystick(0)

        self.elevator_encoder.set_distance_per_pulse(self.METERS_PER_PULSE)

        # the main mechanism object
        self.mech = wpilib.Mechanism2d(3, 3)
        # the mechanism root node
        self.root = self.mech.get_root("climber", 2, 0)

        # MechanismLigament2d objects represent each "section"/"stage" of the mechanism, and are based
        # off the root node or another ligament object
        self.elevator = self.root.append_ligament(
            "elevator", self.ELEVATOR_MINIMUM_LENGTH, 90
        )
        self.wrist = self.elevator.append_ligament(
            "wrist", 0.5, 90, 6, wpiutil.Color8Bit(wpiutil.Color.PURPLE)
        )

        # post the mechanism to the dashboard
        wpilib.SmartDashboard.put_data("Mech2d", self.mech)

    def robot_periodic(self):
        # update the dashboard mechanism's state
        self.elevator.set_length(
            self.ELEVATOR_MINIMUM_LENGTH + self.elevator_encoder.get_distance()
        )
        self.wrist.set_angle(self.wrist_pot.get())

    def teleop_periodic(self):
        self.elevator_motor.set_throttle(self.joystick.get_raw_axis(0))
        self.wrist_motor.set_throttle(self.joystick.get_raw_axis(1))
