import pytest

from ntcore import NetworkTableInstance
from wpilib import Mechanism2d, MechanismLigament2d, MechanismRoot2d, SmartDashboard
from wpiutil import Color8Bit


def test_create_mechanism():
    m = Mechanism2d(100, 100)
    r1 = m.get_root("r1", 10, 10)
    l1 = r1.append_ligament("l1", 4, 3)
    l2 = l1.append_ligament("l2", 4, 3)
    assert l2 is not None


def test_canvas(nt: NetworkTableInstance):
    mechanism = Mechanism2d(5, 10)
    dims_entry = nt.get_entry("/SmartDashboard/mechanism/dims")
    color_entry = nt.get_entry("/SmartDashboard/mechanism/backgroundColor")

    SmartDashboard.put_data("mechanism", mechanism)
    SmartDashboard.update_values()

    dims = dims_entry.get_double_array([])
    assert dims[0] == pytest.approx(5.0)
    assert dims[1] == pytest.approx(10.0)
    assert color_entry.get_string("") == "#000020"

    mechanism.set_background_color(Color8Bit(255, 255, 255))
    SmartDashboard.update_values()
    assert color_entry.get_string("") == "#FFFFFF"


def test_root(nt: NetworkTableInstance):
    mechanism = Mechanism2d(5, 10)
    x_entry = nt.get_entry("/SmartDashboard/mechanism/root/x")
    y_entry = nt.get_entry("/SmartDashboard/mechanism/root/y")

    root = mechanism.get_root("root", 1, 2)
    SmartDashboard.put_data("mechanism", mechanism)
    SmartDashboard.update_values()

    assert x_entry.get_double(0.0) == pytest.approx(1.0)
    assert y_entry.get_double(0.0) == pytest.approx(2.0)

    root.set_position(2, 4)
    SmartDashboard.update_values()

    assert x_entry.get_double(0.0) == pytest.approx(2.0)
    assert y_entry.get_double(0.0) == pytest.approx(4.0)


def test_ligament(nt: NetworkTableInstance):
    mechanism = Mechanism2d(5, 10)
    angle_entry = nt.get_entry("/SmartDashboard/mechanism/root/ligament/angle")
    color_entry = nt.get_entry("/SmartDashboard/mechanism/root/ligament/color")
    length_entry = nt.get_entry("/SmartDashboard/mechanism/root/ligament/length")
    weight_entry = nt.get_entry("/SmartDashboard/mechanism/root/ligament/weight")

    root = mechanism.get_root("root", 1, 2)
    ligament = root.append_ligament("ligament", 3, 90, 1, Color8Bit(255, 255, 255))
    SmartDashboard.put_data("mechanism", mechanism)

    assert ligament.get_angle() == pytest.approx(angle_entry.get_double(0.0))
    assert ligament.get_color().hex_string() == color_entry.get_string("")
    assert ligament.get_length() == pytest.approx(length_entry.get_double(0.0))
    assert ligament.get_line_weight() == pytest.approx(weight_entry.get_double(0.0))

    ligament.set_angle(45)
    ligament.set_color(Color8Bit(0, 0, 0))
    ligament.set_length(2)
    ligament.set_line_weight(4)
    SmartDashboard.update_values()

    assert ligament.get_angle() == pytest.approx(angle_entry.get_double(0.0))
    assert ligament.get_color().hex_string() == color_entry.get_string("")
    assert ligament.get_length() == pytest.approx(length_entry.get_double(0.0))
    assert ligament.get_line_weight() == pytest.approx(weight_entry.get_double(0.0))

    angle_entry.set_double(22.5)
    color_entry.set_string("#FF00FF")
    length_entry.set_double(4.0)
    weight_entry.set_double(6.0)
    SmartDashboard.update_values()

    assert ligament.get_angle() == pytest.approx(angle_entry.get_double(0.0))
    assert ligament.get_color().hex_string() == color_entry.get_string("")
    assert ligament.get_length() == pytest.approx(length_entry.get_double(0.0))
    assert ligament.get_line_weight() == pytest.approx(weight_entry.get_double(0.0))
