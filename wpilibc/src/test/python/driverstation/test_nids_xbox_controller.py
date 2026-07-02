from wpilib import NiDsXboxController
from wpilib.simulation import NiDsXboxControllerSim
from driverstation.joystick_test_macros import button_test, axis_test


def test_buttons():
    def xbox_button_test(btn_name):
        button_test(NiDsXboxController, NiDsXboxControllerSim, btn_name)

    def xbox_axis_test(axis_name):
        axis_test(NiDsXboxController, NiDsXboxControllerSim, axis_name)

    xbox_button_test("LeftBumperButton")
    xbox_button_test("RightBumperButton")

    xbox_button_test("LeftStickButton")
    xbox_button_test("RightStickButton")

    xbox_button_test("AButton")
    xbox_button_test("BButton")
    xbox_button_test("XButton")
    xbox_button_test("YButton")
    xbox_button_test("BackButton")
    xbox_button_test("StartButton")

    xbox_axis_test("LeftX")
    xbox_axis_test("RightX")
    xbox_axis_test("LeftY")
    xbox_axis_test("RightY")

    xbox_axis_test("LeftTriggerAxis")
    xbox_axis_test("RightTriggerAxis")
