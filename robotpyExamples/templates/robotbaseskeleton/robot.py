#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from enum import auto
import threading
from threading import Thread
import wpilib
from wpilib import RobotState
from wpilib import DriverStationBackend
from wpilib import DriverStation
from wpilib import RobotBase
from wpilib._wpilib import _DriverStationModeThread
from hal import ControlWord
from hal import RobotMode
import wpiutil.sync

class Robot(RobotBase):
    def __init__(self) -> None:
        super().__init__()
        self.exit = False

    def disabled(self) -> None:

    def autonomous(self) -> None:


    def teleop(self) -> None:

    def utility(self) -> None:

    def startCompetition(self):
        RobotState.addOpMode(RobotMode.AUTONOMOUS, "Auto")
        RobotState.addOpMode(RobotMode.TELEOPERATED, "Teleop")
        RobotState.addOpMode(RobotMode.UTILITY, "Utility")
        RobotState.publishOpModes()

        word = ControlWord()
        modeThread = _DriverStationModeThread(word)

        event = wpiutil.sync.makeEvent(False, False)

        DriverStationBackend.observeUserProgramStarting()
        self.exit = False
        while getattr(threading.current_thread(), "keep_running", True) & self.exit != False:
            word = DriverStationBackend.getControlWord() #TODO check this
            modeThread.inControl(word)
            if DriverStationBackend.isDisabled():
                self.disabled()
                while DriverStationBackend.isDisabled():
                    try:
                        wpiutil.sync.waitForObject(event)
                    except:
                        threading.current_thread() #TODO interrupt thread

            elif DriverStationBackend.isAutonomous():
                self.autonomous()
                while DriverStationBackend.isAutonomousEnabled():
                    try:
                        wpiutil.sync.waitForObject(event)
                    except InterruptedError as e:
                        # TODO interrupt thread
            elif DriverStationBackend.isUtility():
                self.utility
                while DriverStationBackend.isUtilityEnabled():
                    try:
                        wpiutil.sync.waitForObject(event)
                    except InterruptedError as e:
                        # TODO interrupt thread
            else:
                self.teleop
                while DriverStationBackend.isTeleopEnabled():
                    try:
                        wpiutil.sync.waitForObject(event)
                    except InterruptedError as e:
                        # TODO interrupt thread
        DriverStationBackend.removeRefreshedDataEventHandle(event)
        modeThread.close()

    def endCompetition(self):
        exit = True


