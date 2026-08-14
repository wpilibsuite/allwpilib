#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

import wpilib
from wpilib import OpModeRobot


# The methods in this class are called automatically as described in the OpModeRobot documentation.
# OpMode classes anywhere in the package (or sub-packages) where this class is located are
# automatically registered to display in the Driver Station. If you change the name of this class
# or the package after creating this project, you must also update the Main.java file in the
# project.

class Robot(OpModeRobot):
    # This function is run when the robot is first started up and should be used for any
    # initialization code.
    def __init__(self):
        super().__init__()

    # This function is called exactly once when the DS first connects
    def driver_station_connected(self) -> None:

    # This function is called periodically anytime when no opmode is selected, including when the
    # Driver Station is disconnected.
    
    def none_periodic(self) -> None:
