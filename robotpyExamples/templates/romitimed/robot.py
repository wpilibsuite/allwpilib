#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from wpilib import TimedRobot
from wpilib import SendableChooser
from wpilib import SmartDashboard
from romidrivetrain import RomiDrivetrain

K_DEFAULT_AUTO = "Default"
K_CUSTOM_AUTO = "My Auto"

class Robot(TimedRobot):

    
    # The methods in this class are called automatically corresponding to each mode, as described in
    # the TimedRobot documentation. 
    def __init__(self) -> None:
        super().__init__()
        
        self.chooser = SendableChooser()
        self.chooser.setDefaultOption("Default Auto", K_DEFAULT_AUTO)
        self.chooser.addOption("My Auto", K_CUSTOM_AUTO)
        SmartDashboard.putData(self.chooser)

        self.auto_selected = self.chooser._kDefault

        self.drivetrain = RomiDrivetrain()

        # This function is called every 20 ms, no matter the mode. Use this for items like diagnostics
        # that you want ran during disabled, autonomous, teleoperated and utility.
        
    def robot_periodic(self) -> None:

        # This autonomous (along with the chooser code above) shows how to select between different
        # autonomous modes using the dashboard. The sendable chooser code works with
        # SmartDashboard. If you prefer the LabVIEW Dashboard, remove all of the chooser code and
        # uncomment the getString line to get the auto name from the text box below the Gyro
        
        # <p>You can add additional auto modes by adding additional comparisons to the switch structure
        # below with additional strings. If using the SendableChooser make sure to add them to the
        # chooser code above as well.
        
    def autonomous_init(self) -> None:
        self.auto_selected = self.chooser.getSelected()
        print("Auto selected: " + self.auto_selected)
        
        self.drivetrain.resetEncoders()

    # This function is called periodically during autonomous
    def autonomous_periodic(self) -> None:
        match self.auto_selected:
            case K_CUSTOM_AUTO:
                # Put custom auto code here
            case K_DEFAULT_AUTO:
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

