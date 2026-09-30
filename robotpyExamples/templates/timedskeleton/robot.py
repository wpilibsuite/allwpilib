#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from wpilib import TimedRobot

'''
The methods in this class are called automatically corresponding to each mode, as descriped in 
the TimedRobot documentation. If you change the name of this class or the package after creating
this project, you must update the Main.java file in the project
'''

class Robot(TimedRobot):
    '''
    This function is run when the robot is first started up and should be used for any
    initialization code.
    '''
    def __init__(self) -> None:
        super().__init__()

    def robot_periodic(self) -> None:

    def autonomous_init(self) -> None:

    def autonomous_periodic(self) -> None:

    def teleop_init(self) -> None:

    def teleop_periodic(self) -> None:

    def disabled_init(self) -> None:

    def disabled_periodic(self) -> None:

    def utility_init(self) -> None:

    def utility_periodic(self) -> None:

    def simulation_init(self) -> None:

    def simulation_periodic(self) -> None: