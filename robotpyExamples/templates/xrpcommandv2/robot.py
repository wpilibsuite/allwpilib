#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import Command
from commands2 import CommandScheduler
from wpilib import TimedRobot
from robotcontainer import RobotContainer

# The methods in this class are called automatically corresponding to each mode, as described in
# the TimedRobot documentation. If you change the name of this class or the package after creating
# this project, you must also update the Main.java file in the project.

class Robot(TimedRobot):
    # This function is run when the robot is first started up and should be used for any
    # initialization code.
    
    def __init__(self) -> None:
        super().__init__()
        self.auto_command: Command
        
        # Instantiate our RobotContainer.  This will perform all our button bindings, and put our
        # autonomous chooser on the dashboard.
        self.robot_container = RobotContainer()

    # This function is called every 20 ms, no matter the mode. Use this for items like diagnostics
    # that you want ran during disabled, autonomous, teleoperated and utility.

    # <p>This runs after the mode specific periodic functions, but before LiveWindow and
    # SmartDashboard integrated updating.

    def robot_periodic(self) -> None:
    # Runs the Scheduler.  This is responsible for polling buttons, adding newly-scheduled
    # commands, running already-scheduled commands, removing finished or interrupted commands,
    # and running subsystem periodic() methods.  This must be called from the robot's periodic
    # block in order for anything in the Command-based framework to work.
        CommandScheduler.getInstance().run()

    # This function is called once each time the robot enters Disabled mode
    def disabled_init(self) -> None:

    # This function is called periodically while the robot is disabled
    def disabled_periodic(self) -> None:

    # This autonomous runs the autonomous command selected by your RobotContainer class
    def autonomous_init(self) -> None:
        self.auto_command = self.robot_container.get_autonomous_command()

        # schedule the autonomous command (example)
        if self.auto_command is not None:
            CommandScheduler.getInstance().schedule(self.auto_command)

    # This function is called periodically during autonomous
    def autonomous_periodic(self) -> None:

    def teleop_init(self) -> None:
        # This makes sure that the autonomous stops running when
        # teleop starts running. If you want the autonomous to
        # continue until interrupted by another command, remove
        # this line or comment it out.
        if self.auto_command is not None:
            self.auto_command.cancel()

    # This function is called periodically during operator control.
    def teleop_periodic(self) -> None:

    def utility_init(self) -> None:
        # Cancels all running commands at the start of utility mode.
        CommandScheduler.getInstance().cancelAll()

    # This function is called periodically during utility mode.
    def utility_periodic(self) -> None:
        
        


