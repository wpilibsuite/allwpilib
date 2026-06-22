import pytest

from ntcore import NetworkTableInstance
from wpilib import Mechanism2d, MechanismLigament2d, MechanismRoot2d, SmartDashboard
from wpiutil import Color8Bit


def test_create_mechanism():
    m = Mechanism2d(100, 100)
    r1 = m.getRoot("r1", 10, 10)
    l1 = r1.appendLigament("l1", 4, 3)
    l2 = l1.appendLigament("l2", 4, 3)
    assert l2 is not None


def test_canvas(nt: NetworkTableInstance):
    mechanism = Mechanism2d(5, 10)
    dims_entry = nt.getEntry("/SmartDashboard/mechanism/dims")
    color_entry = nt.getEntry("/SmartDashboard/mechanism/backgroundColor")

    SmartDashboard.putData("mechanism", mechanism)
    SmartDashboard.updateValues()

    dims = dims_entry.getDoubleArray([])
    assert dims[0] == pytest.approx(5.0)
    assert dims[1] == pytest.approx(10.0)
    assert color_entry.getString("") == "#000020"

    mechanism.setBackgroundColor(Color8Bit(255, 255, 255))
    SmartDashboard.updateValues()
    assert color_entry.getString("") == "#FFFFFF"


def test_root(nt: NetworkTableInstance):
    mechanism = Mechanism2d(5, 10)
    x_entry = nt.getEntry("/SmartDashboard/mechanism/root/x")
    y_entry = nt.getEntry("/SmartDashboard/mechanism/root/y")

    root = mechanism.getRoot("root", 1, 2)
    SmartDashboard.putData("mechanism", mechanism)
    SmartDashboard.updateValues()

    assert x_entry.getDouble(0.0) == pytest.approx(1.0)
    assert y_entry.getDouble(0.0) == pytest.approx(2.0)

    root.setPosition(2, 4)
    SmartDashboard.updateValues()

    assert x_entry.getDouble(0.0) == pytest.approx(2.0)
    assert y_entry.getDouble(0.0) == pytest.approx(4.0)


def test_ligament(nt: NetworkTableInstance):
    mechanism = Mechanism2d(5, 10)
    angle_entry = nt.getEntry("/SmartDashboard/mechanism/root/ligament/angle")
    color_entry = nt.getEntry("/SmartDashboard/mechanism/root/ligament/color")
    length_entry = nt.getEntry("/SmartDashboard/mechanism/root/ligament/length")
    weight_entry = nt.getEntry("/SmartDashboard/mechanism/root/ligament/weight")

    root = mechanism.getRoot("root", 1, 2)
    ligament = root.appendLigament("ligament", 3, 90, 1, Color8Bit(255, 255, 255))
    SmartDashboard.putData("mechanism", mechanism)

    assert ligament.getAngle() == pytest.approx(angle_entry.getDouble(0.0))
    assert ligament.getColor().hexString() == color_entry.getString("")
    assert ligament.getLength() == pytest.approx(length_entry.getDouble(0.0))
    assert ligament.getLineWeight() == pytest.approx(weight_entry.getDouble(0.0))

    ligament.setAngle(45)
    ligament.setColor(Color8Bit(0, 0, 0))
    ligament.setLength(2)
    ligament.setLineWeight(4)
    SmartDashboard.updateValues()

    assert ligament.getAngle() == pytest.approx(angle_entry.getDouble(0.0))
    assert ligament.getColor().hexString() == color_entry.getString("")
    assert ligament.getLength() == pytest.approx(length_entry.getDouble(0.0))
    assert ligament.getLineWeight() == pytest.approx(weight_entry.getDouble(0.0))

    angle_entry.setDouble(22.5)
    color_entry.setString("#FF00FF")
    length_entry.setDouble(4.0)
    weight_entry.setDouble(6.0)
    SmartDashboard.updateValues()

    assert ligament.getAngle() == pytest.approx(angle_entry.getDouble(0.0))
    assert ligament.getColor().hexString() == color_entry.getString("")
    assert ligament.getLength() == pytest.approx(length_entry.getDouble(0.0))
    assert ligament.getLineWeight() == pytest.approx(weight_entry.getDouble(0.0))
