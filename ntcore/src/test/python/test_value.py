import ntcore
import pytest


def test_value_bool():
    v = ntcore.Value.makeBoolean(True)
    assert v.getBoolean() == True
    assert v.value() == True


def test_mkvalue_bool():
    v = ntcore.Value.makeValue(True)
    assert v.getBoolean() == True
    assert v.value() == True


def test_bool_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kBoolean)
    v = f(True)
    assert v.getBoolean() == True


def test_value_int():
    v = ntcore.Value.makeInteger(2)
    assert v.getInteger() == 2
    assert v.value() == 2


def test_mkvalue_int():
    v = ntcore.Value.makeValue(2)
    assert v.getInteger() == 2
    assert v.value() == 2


def test_int_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kInteger)
    v = f(2)
    assert v.getInteger() == 2


def test_value_double():
    v = ntcore.Value.makeDouble(2.5)
    assert v.getDouble() == pytest.approx(2.5)
    assert v.value() == pytest.approx(2.5)


def test_mkvalue_double():
    v = ntcore.Value.makeValue(2.5)
    assert v.getDouble() == pytest.approx(2.5)
    assert v.value() == pytest.approx(2.5)


def test_double_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kDouble)
    v = f(2.2)
    assert v.getDouble() == pytest.approx(2.2)


def test_value_float():
    v = ntcore.Value.makeFloat(2.5)
    assert v.getFloat() == pytest.approx(2.5)
    assert v.value() == pytest.approx(2.5)


def test_mkvalue_float():
    pass  # makeValue cannot create a float value


def test_float_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kFloat)
    v = f(2.2)
    assert v.getFloat() == pytest.approx(2.2)


def test_value_str():
    v = ntcore.Value.makeString("s")
    assert v.getString() == "s"
    assert v.value() == "s"


def test_mkvalue_str():
    v = ntcore.Value.makeValue("s")
    assert v.getString() == "s"
    assert v.value() == "s"


def test_str_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kString)
    v = f("abc")
    assert v.getString() == "abc"


def test_value_raw():
    v = ntcore.Value.makeRaw(b"raw")
    assert v.getRaw() == b"raw"
    assert v.value() == b"raw"


def test_mkvalue_raw():
    v = ntcore.Value.makeValue(b"raw")
    assert v.getRaw() == b"raw"
    assert v.value() == b"raw"


def test_raw_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kRaw)
    v = f(b"raw")
    assert v.getRaw() == b"raw"


def test_value_bool_list():
    v = ntcore.Value.makeBooleanArray([True, False])
    assert v.getBooleanArray() == [True, False]
    assert v.value() == [True, False]


def test_mkvalue_bool_list():
    v = ntcore.Value.makeValue([True, False])
    assert v.getBooleanArray() == [True, False]
    assert v.value() == [True, False]


def test_bool_list_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kBooleanArray)
    v = f([True, False])
    assert v.getBooleanArray() == [True, False]


def test_value_int_list():
    v = ntcore.Value.makeIntegerArray([1, 2])
    assert v.getIntegerArray() == [1, 2]
    assert v.value() == [1, 2]


def test_mkvalue_int_list():
    v = ntcore.Value.makeValue([1, 2])
    assert v.getIntegerArray() == [1, 2]
    assert v.value() == [1, 2]


def test_int_list_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kIntegerArray)
    v = f([1, 2])
    assert v.getIntegerArray() == [1, 2]


def test_value_double_list():
    v = ntcore.Value.makeDoubleArray([1.1, 2.2])
    assert v.getDoubleArray() == [pytest.approx(1.1), pytest.approx(2.2)]
    assert v.value() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_mkvalue_double_list():
    v = ntcore.Value.makeValue([1.1, 2.2])
    assert v.getDoubleArray() == [pytest.approx(1.1), pytest.approx(2.2)]
    assert v.value() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_double_list_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kDoubleArray)
    v = f([1.1, 2.2])
    assert v.getDoubleArray() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_value_float_list():
    v = ntcore.Value.makeFloatArray([1.1, 2.2])
    assert v.getFloatArray() == [pytest.approx(1.1), pytest.approx(2.2)]
    assert v.value() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_mkvalue_float_list():
    pass  # not possible to use makeValue to make a float list


def test_float_list_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kFloatArray)
    v = f([1.1, 2.2])
    assert v.getFloatArray() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_value_str_list():
    v = ntcore.Value.makeStringArray(["a", "b"])
    assert v.getStringArray() == ["a", "b"]
    assert v.value() == ["a", "b"]


def test_mkvalue_str_list():
    v = ntcore.Value.makeValue(["a", "b"])
    assert v.getStringArray() == ["a", "b"]
    assert v.value() == ["a", "b"]


def test_str_list_factory():
    f = ntcore.Value.getFactoryByType(ntcore.NetworkTableType.kStringArray)
    v = f(["a", "b"])
    assert v.getStringArray() == ["a", "b"]
