from wpilib import NiDsPS5Controller
from wpilib.simulation import NiDsPS5ControllerSim
from driverstation.joystick_test_macros import button_test, axis_test


def test_buttons():
    def ps5_button_test(btn_name):
        button_test(NiDsPS5Controller, NiDsPS5ControllerSim, btn_name)

    def ps5_axis_test(axis_name):
        axis_test(NiDsPS5Controller, NiDsPS5ControllerSim, axis_name)

    ps5_button_test("square_button")
    ps5_button_test("cross_button")
    ps5_button_test("circle_button")
    ps5_button_test("triangle_button")

    ps5_button_test("l1_button")
    ps5_button_test("r1_button")
    ps5_button_test("l2_button")
    ps5_button_test("r2_button")

    ps5_button_test("create_button")
    ps5_button_test("options_button")

    ps5_button_test("l3_button")
    ps5_button_test("r3_button")

    ps5_button_test("ps_button")
    ps5_button_test("touchpad_button")

    ps5_axis_test("left_x")
    ps5_axis_test("right_x")
    ps5_axis_test("left_y")
    ps5_axis_test("right_y")
    ps5_axis_test("l2_axis")
    ps5_axis_test("r2_axis")
