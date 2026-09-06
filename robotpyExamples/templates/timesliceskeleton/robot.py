#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from wpilib import TimesliceRobot

# The methods in this class are called automatically correspnding to each mode, as described in
# the TimeslaceRobot documentation. If you change the name of this class or the package after
# creating this project, you must also update the Main.java file in the project

class Robot(TimesliceRobot):
    def __init__(self) -> None:
        # Runs robot periodic() functions for 5 ms, and run controllers for every 10 ms
        super().__init__(0.005, 0.01)

        # Runs for 2 ms after robot periodic functions
        super().schedule(lambda: , 0.002)

        # Runs for 2 ms after first controlled function
        super().schedule(lambda: , 0.002)
        
        # Total usage:
        # 5 ms (robot) + 2 ms (controller 1) + 2 ms (controller 2) = 9 ms
        # 9 ms / 10 ms -> 90% allocated
    
    def robot_periodic(self) -> None:

    def autonomous_init(self) -> None:

    def autonomous_periodic(self) -> None:

    def teleop_init(self) -> None:

    def teleop_periodic(self) -> None:

    def disabled_init(self) -> None:

    def disabled_periodic(self) -> None:

    def utility_init(self) -> None:

    def utility_periodic(self) -> None:
