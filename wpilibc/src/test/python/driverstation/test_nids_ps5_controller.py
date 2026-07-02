from wpilib import NiDsPS5Controller
from wpilib.simulation import NiDsPS5ControllerSim
from driverstation.joystick_test_macros import button_test, axis_test


def test_buttons():
    def ps5_button_test(btn_name):
        button_test(NiDsPS5Controller, NiDsPS5ControllerSim, btn_name)

    def ps5_axis_test(axis_name):
        axis_test(NiDsPS5Controller, NiDsPS5ControllerSim, axis_name)

    ps5_button_test("SquareButton")
    ps5_button_test("CrossButton")
    ps5_button_test("CircleButton")
    ps5_button_test("TriangleButton")

    ps5_button_test("L1Button")
    ps5_button_test("R1Button")
    ps5_button_test("L2Button")
    ps5_button_test("R2Button")

    ps5_button_test("CreateButton")
    ps5_button_test("OptionsButton")

    ps5_button_test("L3Button")
    ps5_button_test("R3Button")

    ps5_button_test("PSButton")
    ps5_button_test("TouchpadButton")

    ps5_axis_test("LeftX")
    ps5_axis_test("RightX")
    ps5_axis_test("LeftY")
    ps5_axis_test("RightY")
    ps5_axis_test("L2Axis")
    ps5_axis_test("R2Axis")
