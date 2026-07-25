# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import math

import pytest
import wpimath.units as units

from wpilib import AddressableLEDBuffer, LEDPattern, RobotController
from wpiutil import Color, Color8Bit


def lerp_rgb(a: Color, b: Color, t: float) -> Color8Bit:
    a8 = Color8Bit(a)
    b8 = Color8Bit(b)
    return Color8Bit(
        int(a8.red + (b8.red - a8.red) * t),
        int(a8.green + (b8.green - a8.green) * t),
        int(a8.blue + (b8.blue - a8.blue) * t),
    )


@pytest.fixture(autouse=True)
def restore_time_source():
    RobotController.set_time_source(lambda: 0)
    yield
    RobotController.set_time_source(RobotController.get_time)


def test_apply_to_buffer_direct():
    buffer = AddressableLEDBuffer(4)
    LEDPattern.solid(Color.YELLOW).apply_to(buffer[:])

    for i in range(len(buffer)):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.YELLOW)


def test_apply_to_view_direct():
    buffer = AddressableLEDBuffer(6)
    view = buffer[2:5]
    LEDPattern.solid(Color.AQUA).apply_to(view)

    assert buffer.get_led_8bit(1) == Color8Bit(Color.BLACK)
    assert buffer.get_led_8bit(2) == Color8Bit(Color.AQUA)
    assert buffer.get_led_8bit(3) == Color8Bit(Color.AQUA)
    assert buffer.get_led_8bit(4) == Color8Bit(Color.AQUA)
    assert buffer.get_led_8bit(5) == Color8Bit(Color.BLACK)


def test_solid_color():
    buffer = AddressableLEDBuffer(99)
    LEDPattern.solid(Color.YELLOW).apply_to(buffer[:])

    for i in range(len(buffer)):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.YELLOW)


def test_gradient_0_sets_to_black():
    pattern = LEDPattern.gradient(LEDPattern.GradientType.CONTINUOUS, [])
    buffer = AddressableLEDBuffer(99)

    for i in range(len(buffer)):
        buffer.set_rgb(i, 127, 128, 129)

    pattern.apply_to(buffer[:])

    for i in range(len(buffer)):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.BLACK)


def test_gradient_1_sets_to_solid():
    pattern = LEDPattern.gradient(LEDPattern.GradientType.CONTINUOUS, [Color.YELLOW])
    buffer = AddressableLEDBuffer(99)
    pattern.apply_to(buffer[:])

    for i in range(len(buffer)):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.YELLOW)


def test_continuous_gradient_2_colors():
    pattern = LEDPattern.gradient(
        LEDPattern.GradientType.CONTINUOUS, [Color.YELLOW, Color.PURPLE]
    )
    buffer = AddressableLEDBuffer(99)
    pattern.apply_to(buffer[:])

    assert buffer.get_led_8bit(0) == Color8Bit(Color.YELLOW)
    assert buffer.get_led_8bit(25) == lerp_rgb(Color.YELLOW, Color.PURPLE, 25 / 49.0)
    assert buffer.get_led_8bit(49) == Color8Bit(Color.PURPLE)
    assert buffer.get_led_8bit(73) == lerp_rgb(Color.YELLOW, Color.PURPLE, 25 / 49.0)
    assert buffer.get_led_8bit(98) == Color8Bit(Color.YELLOW)


def test_discontinuous_gradient_2_colors():
    pattern = LEDPattern.gradient(
        LEDPattern.GradientType.DISCONTINUOUS, [Color.YELLOW, Color.PURPLE]
    )
    buffer = AddressableLEDBuffer(99)
    pattern.apply_to(buffer[:])

    assert buffer.get_led_8bit(0) == Color8Bit(Color.YELLOW)
    assert buffer.get_led_8bit(49) == lerp_rgb(Color.YELLOW, Color.PURPLE, 0.5)
    assert buffer.get_led_8bit(98) == Color8Bit(Color.PURPLE)


def test_step_0_sets_to_black():
    pattern = LEDPattern.steps([])
    buffer = AddressableLEDBuffer(99)
    for i in range(len(buffer)):
        buffer.set_rgb(i, 127, 128, 129)

    pattern.apply_to(buffer[:])

    for i in range(len(buffer)):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.BLACK)


def test_step_1_sets_to_solid():
    pattern = LEDPattern.steps([(0.0, Color.YELLOW)])
    buffer = AddressableLEDBuffer(99)
    pattern.apply_to(buffer[:])

    for i in range(len(buffer)):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.YELLOW)


def test_step_half_sets_to_half_off_half_color():
    pattern = LEDPattern.steps([(0.5, Color.YELLOW)])
    buffer = AddressableLEDBuffer(99)
    pattern.apply_to(buffer[:])

    for i in range(49):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.BLACK)
    for i in range(49, len(buffer)):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.YELLOW)


def make_grayscale_pattern():
    return LEDPattern(
        lambda reader, writer: [
            writer(led, Color(led % 256, led % 256, led % 256))
            for led in range(reader.size())
        ]
    )


@pytest.mark.skip(reason="Python bindings do not expose a way to mock wpi::util::Now()")
def test_scroll_forward():
    buffer = AddressableLEDBuffer(256)
    pattern = make_grayscale_pattern().scroll_at_relative_speed(units.hertz(1 / 256.0))

    for time in range(10):
        RobotController.set_time_source(lambda t=time: t)
        pattern.apply_to(buffer[:])

        for led in range(len(buffer)):
            ch = (led - time) % 256
            assert buffer.get_led_8bit(led) == Color8Bit(ch, ch, ch)


@pytest.mark.skip(reason="Python bindings do not expose a way to mock wpi::util::Now()")
def test_scroll_backward():
    buffer = AddressableLEDBuffer(256)
    pattern = make_grayscale_pattern().scroll_at_relative_speed(units.hertz(-1 / 256.0))

    for time in range(10):
        RobotController.set_time_source(lambda t=time: t)
        pattern.apply_to(buffer[:])

        for led in range(len(buffer)):
            ch = (led + time) % 256
            assert buffer.get_led_8bit(led) == Color8Bit(ch, ch, ch)


def test_rainbow_at_full_size():
    buffer = AddressableLEDBuffer(180)
    saturation = 255
    value = 255
    pattern = LEDPattern.rainbow(saturation, value)
    pattern.apply_to(buffer[:])

    for led in range(len(buffer)):
        assert buffer.get_led_8bit(led) == Color8Bit(
            Color.from_hsv(led, saturation, value)
        )


def test_rainbow_odd_size():
    buffer = AddressableLEDBuffer(127)
    scale = 180.0 / len(buffer)
    saturation = 73
    value = 128
    pattern = LEDPattern.rainbow(saturation, value)
    pattern.apply_to(buffer[:])

    for led in range(len(buffer)):
        expected = Color8Bit(Color.from_hsv(int(led * scale), saturation, value))
        assert buffer.get_led_8bit(led) == expected


def test_reverse_solid():
    buffer = AddressableLEDBuffer(90)
    pattern = LEDPattern.solid(Color.ROSY_BROWN).reversed()
    pattern.apply_to(buffer[:])

    for led in range(len(buffer)):
        assert buffer.get_led_8bit(led) == Color8Bit(Color.ROSY_BROWN)


def test_reverse_steps():
    buffer = AddressableLEDBuffer(100)
    pattern = LEDPattern.steps([(0.0, Color.WHITE), (0.5, Color.YELLOW)]).reversed()
    pattern.apply_to(buffer[:])

    for led in range(len(buffer)):
        expected = Color8Bit(Color.YELLOW if led < 50 else Color.WHITE)
        assert buffer.get_led_8bit(led) == expected


def white_yellow_purple(reader, writer):
    colors = [Color.WHITE, Color.YELLOW, Color.PURPLE]
    for led in range(reader.size()):
        writer(led, colors[led % 3])


@pytest.mark.parametrize(
    "offset,expected",
    [
        (1, [Color.PURPLE, Color.WHITE, Color.YELLOW]),
        (-1, [Color.YELLOW, Color.PURPLE, Color.WHITE]),
        (0, [Color.WHITE, Color.YELLOW, Color.PURPLE]),
    ],
)
def test_offset_pattern(offset, expected):
    buffer = AddressableLEDBuffer(21)
    pattern = LEDPattern(white_yellow_purple).offset_by(offset)
    pattern.apply_to(buffer[:])

    for led in range(len(buffer)):
        assert buffer.get_led_8bit(led) == Color8Bit(expected[led % 3])


@pytest.mark.skip(reason="Python bindings do not expose a way to mock wpi::util::Now()")
def test_blink_symmetric():
    pattern = LEDPattern.solid(Color.WHITE).blink(units.seconds(2))
    buffer = AddressableLEDBuffer(1)

    for t in range(8):
        RobotController.set_time_source(lambda tick=t: tick * 1_000_000)
        pattern.apply_to(buffer[:])
        expected = Color8Bit(Color.WHITE if t % 4 < 2 else Color.BLACK)
        assert buffer.get_led_8bit(0) == expected


def test_blink_in_sync():
    state = {"on": False}
    pattern = LEDPattern.solid(Color.WHITE).synchronized_blink(lambda: state["on"])
    buffer = AddressableLEDBuffer(1)

    pattern.apply_to(buffer[:])
    assert buffer.get_led_8bit(0) == Color8Bit(Color.BLACK)

    state["on"] = True
    pattern.apply_to(buffer[:])
    assert buffer.get_led_8bit(0) == Color8Bit(Color.WHITE)

    state["on"] = False
    pattern.apply_to(buffer[:])
    assert buffer.get_led_8bit(0) == Color8Bit(Color.BLACK)


@pytest.mark.skip(reason="Python bindings do not expose a way to mock wpi::util::Now()")
def test_breathe():
    pattern = LEDPattern.solid(Color.WHITE).breathe(units.microseconds(4))
    buffer = AddressableLEDBuffer(1)

    RobotController.set_time_source(lambda: 0)
    pattern.apply_to(buffer[:])
    assert buffer.get_led_8bit(0) == Color8Bit(Color.WHITE)

    RobotController.set_time_source(lambda: 1)
    pattern.apply_to(buffer[:])
    assert buffer.get_led_8bit(0) == Color8Bit(Color(0.5, 0.5, 0.5))

    RobotController.set_time_source(lambda: 2)
    pattern.apply_to(buffer[:])
    assert buffer.get_led_8bit(0) == Color8Bit(Color.BLACK)


def test_overlay_solid_on_solid():
    overlay = LEDPattern.solid(Color.YELLOW).overlay_on(LEDPattern.solid(Color.WHITE))
    buffer = AddressableLEDBuffer(1)
    overlay.apply_to(buffer[:])
    assert buffer.get_led_8bit(0) == Color8Bit(Color.YELLOW)


def test_progress_mask_layer():
    progress = {"value": 0.0}
    pattern = LEDPattern.progress_mask_layer(lambda: progress["value"])
    buffer = AddressableLEDBuffer(10)

    progress["value"] = 0.3
    pattern.apply_to(buffer[:])

    for i in range(3):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.WHITE)
    for i in range(3, 10):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.BLACK)


def test_blend():
    pattern = LEDPattern.solid(Color.BLUE).blend(LEDPattern.solid(Color.RED))
    buffer = AddressableLEDBuffer(1)
    pattern.apply_to(buffer[:])
    assert buffer.get_led_8bit(0) == Color8Bit(Color(127, 0, 127))


def test_binary_mask():
    base = LEDPattern.solid(Color(123, 123, 123))
    mask = LEDPattern.steps([(0.0, Color.WHITE), (0.5, Color.BLACK)])
    pattern = base.mask(mask)
    buffer = AddressableLEDBuffer(4)
    pattern.apply_to(buffer[:])

    for i in range(2):
        assert buffer.get_led_8bit(i) == Color8Bit(Color(123, 123, 123))
    for i in range(2, 4):
        assert buffer.get_led_8bit(i) == Color8Bit(Color.BLACK)
