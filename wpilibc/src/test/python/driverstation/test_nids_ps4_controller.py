from wpilib import NiDsPS4Controller
from wpilib.simulation import NiDsPS4ControllerSim
from driverstation.joystick_test_macros import button_test, axis_test


def test_buttons():
    def ps4_button_test(btn_name):
        button_test(NiDsPS4Controller, NiDsPS4ControllerSim, btn_name)

    def ps4_axis_test(axis_name):
        axis_test(NiDsPS4Controller, NiDsPS4ControllerSim, axis_name)

    ps4_button_test("SquareButton")
    ps4_button_test("CrossButton")
    ps4_button_test("CircleButton")
    ps4_button_test("TriangleButton")

    ps4_button_test("L1Button")
    ps4_button_test("R1Button")
    ps4_button_test("L2Button")
    ps4_button_test("R2Button")

    ps4_button_test("ShareButton")
    ps4_button_test("OptionsButton")

    ps4_button_test("L3Button")
    ps4_button_test("R3Button")

    ps4_button_test("PSButton")
    ps4_button_test("TouchpadButton")

    ps4_axis_test("LeftX")
    ps4_axis_test("RightX")
    ps4_axis_test("LeftY")
    ps4_axis_test("RightY")
    ps4_axis_test("L2Axis")
    ps4_axis_test("R2Axis")
