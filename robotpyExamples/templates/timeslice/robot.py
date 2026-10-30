#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from commands2 import InstantCommand
from wpilib import TimesliceRobot
from wpilib import SendableChooser
from wpilib import SmartDashboard

K_DEFAULT_AUTO = "Default"
K_CUSTOM_AUTO = "My Auto"

class Robot(TimesliceRobot):
    def __init__(self) -> None:
        # Run robot periodic() functions for 5 ms and run controllers every 10 ms
        super().__init__(0.005, 0.01)

        # Runs for 2 ms after robot periodic functions
        super().schedule(lambda: , 0.002) 

        # Runs for 2 ms after first controller function
        super().schedule(lambda: , 0.002)

        # Total usage: 5 ms (robot) + 2 ms (controller 1) + 2 ms (controller 2)
        # = 9 ms -> 90% allocated

        self.chooser = SendableChooser()
        self.chooser.setDefaultOption("Default Auto", K_DEFAULT_AUTO)
        self.chooser.addOption("My Auto", K_CUSTOM_AUTO)
        SmartDashboard.putData("Auto choices", self.chooser)

        self.auto_selected = self.chooser.getSelected()
    
    # This function is called every robot packet, no matter the mode. Use this for items like
    # diagnostics that you want ran during disabled, autonomous, teleoperated and utility.
    
    # <p>This runs after the mode specific periodic functions, but before LiveWindow and
    # SmartDashboard integrated updating.
    def robot_periodic(self) -> None:
        # This autonomous (along with the chooser code above) shows how to select between different
        # autonomous modes using the dashboard. The sendable chooser code works with the Java
        # SmartDashboard. If you prefer the LabVIEW Dashboard, remove all of the chooser code and
        # uncomment the getString line to get the auto name from the text box below the Gyro
        
        # <p>You can add additional auto modes by adding additional comparisons to the switch structure
        # below with additional strings. If using the SendableChooser make sure to add them to the
        # chooser code above as well.
        
    def autonomous_init(self) -> None:
        self.auto_selected = self.chooser.getSelected()
        print("Auto selected: " + self.auto_selected)

    # This function is called periodically during autonomous
    def autonomous_periodic(self) -> None:
        match self.auto_selected:
            case K_CUSTOM_AUTO:
                # Put custom auto code here
            case K_DEFAULT_AUTO:
                # Put default auto code here
            default: 


    # This function is called once when teleop is enabled
    def teleop_init(self) -> None:

    # This function is called periodically during operator control
    def teleop_periodic(self) -> None:

    # This function is called once when the robot is disabled
    def disabled_init(self) -> None:

    # This function is called periodically when disabled
    def disabled_periodic(self) -> None:

    # This function is called once when utility mode is enabled
    def utility_init(self) -> None:

    # This function is called periodically during utility mode
    def utility_periodic(self) -> None:
