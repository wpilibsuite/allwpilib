# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import math

import pytest

from wpimath import AntiTipping, Rotation3d


def test_below_threshold_generates_no_correction():
    anti_tipping = AntiTipping(0.1, math.radians(3.0), 2.0)
    flat = Rotation3d(math.radians(1.0), math.radians(1.0), 0.0)
    correction = anti_tipping.calculate(flat)

    assert correction.vx == pytest.approx(0.0, abs=1e-6)
    assert correction.vy == pytest.approx(0.0, abs=1e-6)


def test_forward_tip_drives_forward():
    anti_tipping = AntiTipping(0.1, math.radians(3.0), 2.0)
    correction = anti_tipping.calculate(Rotation3d(0.0, math.radians(10.0), 0.0))

    assert correction.vx > 0.0
    assert correction.vy == pytest.approx(0.0, abs=1e-6)


def test_backward_tip_drives_backward():
    anti_tipping = AntiTipping(0.1, math.radians(3.0), 2.0)
    correction = anti_tipping.calculate(Rotation3d(0.0, math.radians(-10.0), 0.0))

    assert correction.vx < 0.0
    assert correction.vy == pytest.approx(0.0, abs=1e-6)


def test_right_roll_drives_right():
    anti_tipping = AntiTipping(0.1, math.radians(3.0), 2.0)
    correction = anti_tipping.calculate(Rotation3d(math.radians(15.0), 0.0, 0.0))

    assert correction.vx == pytest.approx(0.0, abs=1e-6)
    assert correction.vy < 0.0


def test_left_roll_drives_left():
    anti_tipping = AntiTipping(0.1, math.radians(3.0), 2.0)
    correction = anti_tipping.calculate(Rotation3d(math.radians(-15.0), 0.0, 0.0))

    assert correction.vx == pytest.approx(0.0, abs=1e-6)
    assert correction.vy > 0.0
    