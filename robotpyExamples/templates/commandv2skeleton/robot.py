#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import commands2
from robotcontainer import RobotContainer
from commands2 import CommandScheduler

class MyRobot(commands2.TimedCommandRobot):
    def __init__(self) -> None:
        self.robotContainer = RobotContainer()

    def robot_periodic(self) -> None:
        # Runs the Scheduler.  This is responsible for polling buttons, adding newly-scheduled
        # commands, running already-scheduled commands, removing finished or interrupted commands,
        # and running subsystem periodic() methods.  This must be called from the robot's periodic
        # block in order for anything in the Command-based framework to work.
        CommandScheduler.getInstance().run()

    def autonomous_init(self) -> None:
        self.autonomousCommand = self.robotContainer.get_autonomous_command()

        # schedule the autonomous command (example)
        if self.autonomousCommand is not None:
            CommandScheduler.getInstance().schedule(self.autonomousCommand)

    def teleop_init(self) -> None:
        # This makes sure that the autonomous stops running when
        # teleop starts running. If you want the autonomous to
        # continue until interrupted by another command, remove
        # this line or comment it out.
        if self.autonomousCommand is not None:
            self.autonomousCommand.cancel()
    
    def test_init(self) -> None:
        # Cancels all running commands at the start of test mode.
        commands2.CommandScheduler.getInstance().cancelAll()