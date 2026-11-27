# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import commands2
from robotcontainer import RobotContainer
from commands2 import CommandScheduler, Command
import hal

class MyRobot(commands2.TimedCommandRobot):
    def __init__(self) -> None:
        super().__init__()
        self.robotcontainer = RobotContainer()

    # This method will be called once per scheduler run
    def periodic(self) -> None:
        CommandScheduler.getInstance().run()

    # Run when first disabled
    def disabled_init(self):

    # Run at the beginning of autonomous
    def autonomous_init(self) -> None:
        self.autonomousCommand = self.robotcontainer.get_autonomous_command()
        if self.autonomousCommand is not None:
            self.autonomousCommand.schedule()

    # Runs at the beginning of teleop
    def teleop_init(self) -> None:
        # This makes sure that the autonomous stops running when
        # teleop starts running. If you want the autonomous to
        # continue until interrupted by another command, remove
        # this line or comment it out.
        if self.autonomousCommand is not None:
            self.autonomousCommand.cancel()

    # Runs at the beginning of test
    def test_init(self) -> None:
        # Cancels all running commands at the start of test mode.
        CommandScheduler.getInstance().cancelAll()
