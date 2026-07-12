import pytest

from ntcore import NetworkTableInstance, NetworkTableType
from ntcore.util import ntproperty, ChooserControl

# def test_autoupdatevalue(nt):

#     # tricksy: make sure that this works *before* initialization
#     # of network tables happens!
#     nt.shutdown()

#     foo = nt.get_global_auto_update_value("/SmartDashboard/foo", True, True)
#     assert foo.value == True
#     assert foo.get() == True

#     nt.start_test_mode()

#     assert foo.value == True
#     assert foo.get() == True

#     t = nt.get_table("/SmartDashboard")
#     assert t.get_boolean("foo", None) == True
#     t.put_boolean("foo", False)

#     assert foo.value == False


def test_ntproperty(nt: NetworkTableInstance):
    class Foo:
        robot_time = ntproperty(
            "/SmartDashboard/robotTime", 0.0, write_default=False, inst=nt
        )
        ds_time = ntproperty("/SmartDashboard/dsTime", 0.0, write_default=True, inst=nt)
        test_int_array = ntproperty(
            "/SmartDashboard/testIntArray", [1, 2, 3], write_default=True, inst=nt
        )

        test_bool_array = ntproperty(
            "/SmartDashboard/testBoolArray", [True, False], write_default=True, inst=nt
        )

        test_float_array = ntproperty(
            "/SmartDashboard/testFloatArray",
            [1.1, 1.2, 1.3],
            write_default=True,
            type=NetworkTableType.FLOAT_ARRAY,
            inst=nt,
        )

    f = Foo()

    t = nt.get_table("/SmartDashboard")

    assert f.robot_time == 0
    assert t.get_number("robotTime", None) == 0

    f.robot_time = 2
    assert t.get_number("robotTime", None) == 2

    t.put_number("robotTime", 4)
    assert f.robot_time == 4

    assert f.test_int_array == [1, 2, 3]
    f.test_int_array = [4, 5, 6]
    assert f.test_int_array == [4, 5, 6]

    assert f.test_bool_array == [True, False]
    f.test_bool_array = [False, True]
    assert f.test_bool_array == [False, True]

    assert f.test_float_array == [
        pytest.approx(1.1),
        pytest.approx(1.2),
        pytest.approx(1.3),
    ]
    f.test_float_array = [4.1, 5.1, 6.1]
    assert f.test_float_array == [
        pytest.approx(4.1),
        pytest.approx(5.1),
        pytest.approx(6.1),
    ]


def test_ntproperty_emptyarray(nt: NetworkTableInstance):
    with pytest.raises(TypeError):

        class Foo1:
            test_array = ntproperty(
                "/SmartDashboard/testArray", [], write_default=True, inst=nt
            )

    with pytest.raises(TypeError):

        class Foo2:
            test_array = ntproperty(
                "/SmartDashboard/testArray", [], write_default=False, inst=nt
            )


def test_ntproperty_multitest(nt: NetworkTableInstance):
    """
    Checks to see that ntproperties still work between NT restarts

    This is needed to ensure that ntproperty gets reset between
    pyfrc tests
    """

    class Foo:
        robot_time = ntproperty(
            "/SmartDashboard/robotTime", 0.0, write_default=False, inst=nt
        )
        ds_time = ntproperty("/SmartDashboard/dsTime", 0.0, write_default=True, inst=nt)

    for i in range(3):
        print("Iteration", i)

        f = Foo()

        t = nt.get_table("/SmartDashboard")

        assert f.robot_time == 0
        assert f.ds_time == 0

        assert t.get_number("robotTime", None) == 0
        assert t.get_number("dsTime", None) == 0

        f.robot_time = 2
        assert t.get_number("robotTime", None) == 2
        assert t.get_number("dsTime", None) == 0

        t.put_number("robotTime", 4)
        assert f.robot_time == 4
        assert f.ds_time == 0
        nt.stop_local()
        nt._reset()
        nt.start_local()


def test_chooser_control(nt: NetworkTableInstance):
    c = ChooserControl("Autonomous Mode", inst=nt)

    assert c.get_choices() == []
    assert c.get_selected() is None

    c.set_selected("foo")
    assert c.get_selected() == "foo"

    t = nt.get_table("/SmartDashboard/Autonomous Mode")
    assert t.get_string("selected", None) == "foo"

    t.put_string_array("options", ("option1", "option2"))
    assert c.get_choices() == ["option1", "option2"]
