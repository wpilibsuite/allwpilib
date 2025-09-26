import pytest

from ntcore import NetworkTableInstance, NetworkTableType
from ntcore.util import ntproperty, ChooserControl


# def test_autoupdatevalue(nt):

#     # tricksy: make sure that this works *before* initialization
#     # of network tables happens!
#     nt.shutdown()

#     foo = nt.getGlobalAutoUpdateValue("/SmartDashboard/foo", True, True)
#     assert foo.value == True
#     assert foo.get() == True

#     nt.startTestMode()

#     assert foo.value == True
#     assert foo.get() == True

#     t = nt.getTable("/SmartDashboard")
#     assert t.getBoolean("foo", None) == True
#     t.putBoolean("foo", False)

#     assert foo.value == False


def test_ntproperty(nt: NetworkTableInstance):
    class Foo(object):
        robotTime = ntproperty(
            "/SmartDashboard/robotTime", 0.0, writeDefault=False, inst=nt
        )
        dsTime = ntproperty("/SmartDashboard/dsTime", 0.0, writeDefault=True, inst=nt)
        testIntArray = ntproperty(
            "/SmartDashboard/testIntArray", [1, 2, 3], writeDefault=True, inst=nt
        )

        testBoolArray = ntproperty(
            "/SmartDashboard/testBoolArray", [True, False], writeDefault=True, inst=nt
        )

        testFloatArray = ntproperty(
            "/SmartDashboard/testFloatArray",
            [1.1, 1.2, 1.3],
            writeDefault=True,
            type=NetworkTableType.kFloatArray,
            inst=nt,
        )

    f = Foo()

    t = nt.getTable("/SmartDashboard")

    assert f.robotTime == 0
    assert t.getNumber("robotTime", None) == 0

    f.robotTime = 2
    assert t.getNumber("robotTime", None) == 2

    t.putNumber("robotTime", 4)
    assert f.robotTime == 4

    assert f.testIntArray == [1, 2, 3]
    f.testIntArray = [4, 5, 6]
    assert f.testIntArray == [4, 5, 6]

    assert f.testBoolArray == [True, False]
    f.testBoolArray = [False, True]
    assert f.testBoolArray == [False, True]

    assert f.testFloatArray == [
        pytest.approx(1.1),
        pytest.approx(1.2),
        pytest.approx(1.3),
    ]
    f.testFloatArray = [4.1, 5.1, 6.1]
    assert f.testFloatArray == [
        pytest.approx(4.1),
        pytest.approx(5.1),
        pytest.approx(6.1),
    ]


def test_ntproperty_emptyarray(nt: NetworkTableInstance):
    with pytest.raises(TypeError):

        class Foo1(object):
            testArray = ntproperty(
                "/SmartDashboard/testArray", [], writeDefault=True, inst=nt
            )

    with pytest.raises(TypeError):

        class Foo2(object):
            testArray = ntproperty(
                "/SmartDashboard/testArray", [], writeDefault=False, inst=nt
            )


def test_ntproperty_multitest(nt: NetworkTableInstance):
    """
    Checks to see that ntproperties still work between NT restarts

    This is needed to ensure that ntproperty gets reset between
    pyfrc tests
    """

    class Foo(object):
        robotTime = ntproperty(
            "/SmartDashboard/robotTime", 0.0, writeDefault=False, inst=nt
        )
        dsTime = ntproperty("/SmartDashboard/dsTime", 0.0, writeDefault=True, inst=nt)

    for i in range(3):
        print("Iteration", i)

        f = Foo()

        t = nt.getTable("/SmartDashboard")

        assert f.robotTime == 0
        assert f.dsTime == 0

        assert t.getNumber("robotTime", None) == 0
        assert t.getNumber("dsTime", None) == 0

        f.robotTime = 2
        assert t.getNumber("robotTime", None) == 2
        assert t.getNumber("dsTime", None) == 0

        t.putNumber("robotTime", 4)
        assert f.robotTime == 4
        assert f.dsTime == 0
        nt.stopLocal()
        nt._reset()
        nt.startLocal()


def test_chooser_control(nt: NetworkTableInstance):
    c = ChooserControl("Autonomous Mode", inst=nt)

    assert c.getChoices() == []
    assert c.getSelected() is None

    c.setSelected("foo")
    assert c.getSelected() == "foo"

    t = nt.getTable("/SmartDashboard/Autonomous Mode")
    assert t.getString("selected", None) == "foo"

    t.putStringArray("options", ("option1", "option2"))
    assert c.getChoices() == ["option1", "option2"]
