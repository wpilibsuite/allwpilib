from wpilib import NiDsPS4Controller
from wpilib.simulation import NiDsPS4ControllerSim
from driverstation.joystick_test_macros import button_test, axis_test


def test_buttons():
    def ps4_button_test(btn_name):
        button_test(NiDsPS4Controller, NiDsPS4ControllerSim, btn_name)

    def ps4_axis_test(axis_name):
        axis_test(NiDsPS4Controller, NiDsPS4ControllerSim, axis_name)

    ps4_button_test("square_button")
    ps4_button_test("cross_button")
    ps4_button_test("circle_button")
    ps4_button_test("triangle_button")

    ps4_button_test("l1_button")
    ps4_button_test("r1_button")
    ps4_button_test("l2_button")
    ps4_button_test("r2_button")

    ps4_button_test("share_button")
    ps4_button_test("options_button")

    ps4_button_test("l3_button")
    ps4_button_test("r3_button")

    ps4_button_test("ps_button")
    ps4_button_test("touchpad_button")

    ps4_axis_test("left_x")
    ps4_axis_test("right_x")
    ps4_axis_test("left_y")
    ps4_axis_test("right_y")
    ps4_axis_test("l2_axis")
    ps4_axis_test("r2_axis")
