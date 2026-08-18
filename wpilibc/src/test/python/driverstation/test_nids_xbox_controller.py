from wpilib import NiDsXboxController
from wpilib.simulation import NiDsXboxControllerSim
from driverstation.joystick_test_macros import button_test, axis_test


def test_buttons():
    def xbox_button_test(btn_name):
        button_test(NiDsXboxController, NiDsXboxControllerSim, btn_name)

    def xbox_axis_test(axis_name):
        axis_test(NiDsXboxController, NiDsXboxControllerSim, axis_name)

    xbox_button_test("left_bumper_button")
    xbox_button_test("right_bumper_button")

    xbox_button_test("left_stick_button")
    xbox_button_test("right_stick_button")

    xbox_button_test("a_button")
    xbox_button_test("b_button")
    xbox_button_test("x_button")
    xbox_button_test("y_button")
    xbox_button_test("back_button")
    xbox_button_test("start_button")

    xbox_axis_test("left_x")
    xbox_axis_test("right_x")
    xbox_axis_test("left_y")
    xbox_axis_test("right_y")

    xbox_axis_test("left_trigger_axis")
    xbox_axis_test("right_trigger_axis")
