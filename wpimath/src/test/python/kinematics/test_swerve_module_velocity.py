import pytest
import math

from wpimath import Rotation2d, SwerveModuleVelocity


kEpsilon = 1e-9


def test_optimize():
    angle_a = Rotation2d.fromDegrees(45)
    ref_a = SwerveModuleVelocity(-2, Rotation2d.fromDegrees(180))
    ref_a.optimize(angle_a)

    assert ref_a.velocity == pytest.approx(2.0, abs=kEpsilon)
    assert ref_a.angle.degrees() == pytest.approx(0.0, abs=kEpsilon)

    angle_b = Rotation2d.fromDegrees(-50)
    ref_b = SwerveModuleVelocity(4.7, Rotation2d.fromDegrees(41))
    ref_b.optimize(angle_b)

    assert ref_b.velocity == pytest.approx(-4.7, abs=kEpsilon)
    assert ref_b.angle.degrees() == pytest.approx(-139.0, abs=kEpsilon)


def test_no_optimize():
    angle_a = Rotation2d.fromDegrees(0)
    ref_a = SwerveModuleVelocity(2, Rotation2d.fromDegrees(89))
    ref_a.optimize(angle_a)

    assert ref_a.velocity == pytest.approx(2.0, abs=kEpsilon)
    assert ref_a.angle.degrees() == pytest.approx(89.0, abs=kEpsilon)

    angle_b = Rotation2d.fromDegrees(0)
    ref_b = SwerveModuleVelocity(-2, Rotation2d.fromDegrees(-2))
    ref_b.optimize(angle_b)

    assert ref_b.velocity == pytest.approx(-2.0, abs=kEpsilon)
    assert ref_b.angle.degrees() == pytest.approx(-2.0, abs=kEpsilon)


def test_cosine_scaling():
    angle_a = Rotation2d.fromDegrees(0)
    ref_a = SwerveModuleVelocity(2, Rotation2d.fromDegrees(45))
    ref_a.cosineScale(angle_a)

    assert ref_a.velocity == pytest.approx(math.sqrt(2.0), abs=kEpsilon)
    assert ref_a.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_b = Rotation2d.fromDegrees(45)
    ref_b = SwerveModuleVelocity(2, Rotation2d.fromDegrees(45))
    ref_b.cosineScale(angle_b)

    assert ref_b.velocity == pytest.approx(2.0, abs=kEpsilon)
    assert ref_b.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_c = Rotation2d.fromDegrees(-45)
    ref_c = SwerveModuleVelocity(2, Rotation2d.fromDegrees(45))
    ref_c.cosineScale(angle_c)

    assert ref_c.velocity == pytest.approx(0.0, abs=kEpsilon)
    assert ref_c.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_d = Rotation2d.fromDegrees(135)
    ref_d = SwerveModuleVelocity(2, Rotation2d.fromDegrees(45))
    ref_d.cosineScale(angle_d)

    assert ref_d.velocity == pytest.approx(0.0, abs=kEpsilon)
    assert ref_d.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_e = Rotation2d.fromDegrees(-135)
    ref_e = SwerveModuleVelocity(2, Rotation2d.fromDegrees(45))
    ref_e.cosineScale(angle_e)

    assert ref_e.velocity == pytest.approx(-2.0, abs=kEpsilon)
    assert ref_e.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_f = Rotation2d.fromDegrees(180)
    ref_f = SwerveModuleVelocity(2, Rotation2d.fromDegrees(45))
    ref_f.cosineScale(angle_f)

    assert ref_f.velocity == pytest.approx(-math.sqrt(2.0), abs=kEpsilon)
    assert ref_f.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_g = Rotation2d.fromDegrees(0)
    ref_g = SwerveModuleVelocity(-2, Rotation2d.fromDegrees(45))
    ref_g.cosineScale(angle_g)

    assert ref_g.velocity == pytest.approx(-math.sqrt(2.0), abs=kEpsilon)
    assert ref_g.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_h = Rotation2d.fromDegrees(45)
    ref_h = SwerveModuleVelocity(-2, Rotation2d.fromDegrees(45))
    ref_h.cosineScale(angle_h)

    assert ref_h.velocity == pytest.approx(-2.0, abs=kEpsilon)
    assert ref_h.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_i = Rotation2d.fromDegrees(-45)
    ref_i = SwerveModuleVelocity(-2, Rotation2d.fromDegrees(45))
    ref_i.cosineScale(angle_i)

    assert ref_i.velocity == pytest.approx(0.0, abs=kEpsilon)
    assert ref_i.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_j = Rotation2d.fromDegrees(135)
    ref_j = SwerveModuleVelocity(-2, Rotation2d.fromDegrees(45))
    ref_j.cosineScale(angle_j)

    assert ref_j.velocity == pytest.approx(0.0, abs=kEpsilon)
    assert ref_j.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_k = Rotation2d.fromDegrees(-135)
    ref_k = SwerveModuleVelocity(-2, Rotation2d.fromDegrees(45))
    ref_k.cosineScale(angle_k)

    assert ref_k.velocity == pytest.approx(2.0, abs=kEpsilon)
    assert ref_k.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)

    angle_l = Rotation2d.fromDegrees(180)
    ref_l = SwerveModuleVelocity(-2, Rotation2d.fromDegrees(45))
    ref_l.cosineScale(angle_l)

    assert ref_l.velocity == pytest.approx(math.sqrt(2.0), abs=kEpsilon)
    assert ref_l.angle.degrees() == pytest.approx(45.0, abs=kEpsilon)


def test_equality():
    state1 = SwerveModuleVelocity(2, Rotation2d.fromDegrees(90))
    state2 = SwerveModuleVelocity(2, Rotation2d.fromDegrees(90))

    assert state1 == state2


def test_inequality():
    state1 = SwerveModuleVelocity(1, Rotation2d.fromDegrees(90))
    state2 = SwerveModuleVelocity(2, Rotation2d.fromDegrees(90))
    state3 = SwerveModuleVelocity(1, Rotation2d.fromDegrees(89))

    assert state1 != state2
    assert state1 != state3
