import hal


def test_hal_simdevice():
    device = hal.SimDevice("deviceName")
    v = device.createInt("i", hal.SimDevice.Direction.INPUT, 1)
    assert v.get() == 1
    assert v.type == hal.Type.INT

    v.set(4)
    assert v.get() == 4


def test_hal_simdevice_enum():
    device = hal.SimDevice("enumDevice")
    names = ["one", "two", "three"]
    v = device.createEnum("e", hal.SimDevice.Direction.INPUT, names, 0)

    assert v.get() == 0
    assert v.type == hal.Type.ENUM

    for value, name in enumerate(names):
        v.set(value)
        assert repr(v) == f"<SimEnum name={name} value={value}>"


def test_hal_simdevice_enum_double():
    device = hal.SimDevice("enumDevice")
    names = ["one", "two", "three"]
    values = [0.0, 1.0, 2.0]
    v = device.createEnumDouble("e", hal.SimDevice.Direction.INPUT, names, values, 0)

    assert v.get() == 0
    assert v.type == hal.Type.ENUM

    # TODO: update this test to not use repr once https://github.com/wpilibsuite/allwpilib/issues/7800
    #       is resolved

    for value, (name, dvalue) in enumerate(zip(names, values)):
        v.set(value)
        assert repr(v) == f"<SimEnum name={name} value={value} dvalue={dvalue:.6f}>"


def test_hal_send_error(capsys):
    hal._wpiHal.__test_senderr()
    cap = capsys.readouterr()
    assert cap.err == "Error at location: �badmessage\ncallstack\n\n"
