import pytest
import threading
from wpilib import simulation as wsim
from wpimath.units import seconds
from wpilib.opmoderobot import OpModeRobot
from wpilib import OpMode, DriverStation
from hal._wpiHal import RobotMode
from wpiutil import Color


class MockOpMode(OpMode):
    def __init__(self):
        super().__init__()
        self.disabled_periodic_count = 0
        self.op_mode_run_count = 0
        self.op_mode_stop_count = 0

    def disabled_periodic(self):
        self.disabled_periodic_count += 1

    def op_mode_run(self, op_mode_id: int):
        self.op_mode_run_count += 1

    def op_mode_stop(self):
        self.op_mode_stop_count += 1


class OneArgOpMode(OpMode):
    def __init__(self, robot):
        super().__init__()

    def op_mode_run(self, op_mode_id: int):
        pass

    def op_mode_stop(self):
        pass


class MockRobot(OpModeRobot):
    def __init__(self):
        super().__init__()
        self.driver_station_connected_count = 0
        self.none_periodic_count = 0

    def driverStationConnected(self):
        self.driver_station_connected_count += 1

    def nonePeriodic(self):
        self.none_periodic_count += 1


@pytest.fixture(autouse=True)
def sim_timing_setup():
    wsim.pauseTiming()
    wsim.setProgramStarted(False)
    yield
    wsim.resumeTiming()
    DriverStation.clearOpModes()


def test_add_op_mode():
    class MyMockRobot(MockRobot):
        def __init__(self):
            super().__init__()
            self.addOpMode(
                MockOpMode,
                RobotMode.AUTONOMOUS,
                "NoArgOpMode-Auto",
                "Group",
                "Description",
                Color.kWhite,
                Color.kBlack,
            )
            self.addOpMode(
                OneArgOpMode,
                RobotMode.TEST,
                "OneArgOpMode-Test",
                "Group",
                "Description",
                Color.kWhite,
                Color.kBlack,
            )
            self.addOpMode(MockOpMode, RobotMode.TELEOPERATED, "NoArgOpMode")
            self.addOpMode(OneArgOpMode, RobotMode.TELEOPERATED, "OneArgOpMode")
            self.publishOpModes()

    robot = MyMockRobot()
    options = wsim.DriverStationSim.getOpModeOptions()

    assert len(options) == 4

    opt_map = {opt.name: opt for opt in options}

    auto_opt = opt_map["NoArgOpMode-Auto"]
    assert auto_opt.group == "Group"
    assert auto_opt.description == "Description"
    assert auto_opt.textColor == 0xFFFFFF
    assert auto_opt.backgroundColor == 0x000000

    tele_opt = opt_map["NoArgOpMode"]
    assert tele_opt.group == ""
    assert tele_opt.description == ""
    assert tele_opt.textColor == -1
    assert tele_opt.backgroundColor == -1


def test_clear_op_modes():
    class MyMockRobot(MockRobot):
        def __init__(self):
            super().__init__()
            self.addOpMode(MockOpMode, RobotMode.TELEOPERATED, "NoArgOpMode")
            self.publishOpModes()

    robot = MyMockRobot()
    robot.clearOpModes()

    options = wsim.DriverStationSim.getOpModeOptions()
    assert len(options) == 0


def test_remove_op_mode():
    class MyMockRobot(MockRobot):
        def __init__(self):
            super().__init__()
            self.addOpMode(MockOpMode, RobotMode.TELEOPERATED, "NoArgOpMode")
            self.addOpMode(OneArgOpMode, RobotMode.TELEOPERATED, "OneArgOpMode")
            self.publishOpModes()

    robot = MyMockRobot()
    robot.removeOpMode(RobotMode.TELEOPERATED, "NoArgOpMode")
    robot.publishOpModes()

    options = wsim.DriverStationSim.getOpModeOptions()
    assert len(options) == 1
    assert options[0].name == "OneArgOpMode"


def test_none_periodic():
    class MyMockRobot(MockRobot):
        def __init__(self):
            super().__init__()
            self.addOpMode(MockOpMode, RobotMode.TELEOPERATED, "NoArgOpMode")
            self.publishOpModes()

    robot = MyMockRobot()

    robot_thread = threading.Thread(target=robot.startCompetition)
    robot_thread.start()

    wsim.waitForProgramStart()

    wsim.stepTiming(0.110)

    assert robot.none_periodic_count == 2

    robot.endCompetition()
    robot_thread.join()
