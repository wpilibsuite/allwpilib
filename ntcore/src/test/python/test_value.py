import ntcore
import pytest


def test_value_bool():
    v = ntcore.Value.make_boolean(True)
    assert v.get_boolean() == True
    assert v.value() == True


def test_mkvalue_bool():
    v = ntcore.Value.make_value(True)
    assert v.get_boolean() == True
    assert v.value() == True


def test_bool_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.BOOLEAN)
    v = f(True)
    assert v.get_boolean() == True


def test_value_int():
    v = ntcore.Value.make_integer(2)
    assert v.get_integer() == 2
    assert v.value() == 2


def test_mkvalue_int():
    v = ntcore.Value.make_value(2)
    assert v.get_integer() == 2
    assert v.value() == 2


def test_int_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.INTEGER)
    v = f(2)
    assert v.get_integer() == 2


def test_value_double():
    v = ntcore.Value.make_double(2.5)
    assert v.get_double() == pytest.approx(2.5)
    assert v.value() == pytest.approx(2.5)


def test_mkvalue_double():
    v = ntcore.Value.make_value(2.5)
    assert v.get_double() == pytest.approx(2.5)
    assert v.value() == pytest.approx(2.5)


def test_double_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.DOUBLE)
    v = f(2.2)
    assert v.get_double() == pytest.approx(2.2)


def test_value_float():
    v = ntcore.Value.make_float(2.5)
    assert v.get_float() == pytest.approx(2.5)
    assert v.value() == pytest.approx(2.5)


def test_mkvalue_float():
    pass  # make_value cannot create a float value


def test_float_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.FLOAT)
    v = f(2.2)
    assert v.get_float() == pytest.approx(2.2)


def test_value_str():
    v = ntcore.Value.make_string("s")
    assert v.get_string() == "s"
    assert v.value() == "s"


def test_mkvalue_str():
    v = ntcore.Value.make_value("s")
    assert v.get_string() == "s"
    assert v.value() == "s"


def test_str_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.STRING)
    v = f("abc")
    assert v.get_string() == "abc"


def test_value_raw():
    v = ntcore.Value.make_raw(b"raw")
    assert v.get_raw() == b"raw"
    assert v.value() == b"raw"


def test_mkvalue_raw():
    v = ntcore.Value.make_value(b"raw")
    assert v.get_raw() == b"raw"
    assert v.value() == b"raw"


def test_raw_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.RAW)
    v = f(b"raw")
    assert v.get_raw() == b"raw"


def test_value_bool_list():
    v = ntcore.Value.make_boolean_array([True, False])
    assert v.get_boolean_array() == [True, False]
    assert v.value() == [True, False]


def test_mkvalue_bool_list():
    v = ntcore.Value.make_value([True, False])
    assert v.get_boolean_array() == [True, False]
    assert v.value() == [True, False]


def test_bool_list_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.BOOLEAN_ARRAY)
    v = f([True, False])
    assert v.get_boolean_array() == [True, False]


def test_value_int_list():
    v = ntcore.Value.make_integer_array([1, 2])
    assert v.get_integer_array() == [1, 2]
    assert v.value() == [1, 2]


def test_mkvalue_int_list():
    v = ntcore.Value.make_value([1, 2])
    assert v.get_integer_array() == [1, 2]
    assert v.value() == [1, 2]


def test_int_list_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.INTEGER_ARRAY)
    v = f([1, 2])
    assert v.get_integer_array() == [1, 2]


def test_value_double_list():
    v = ntcore.Value.make_double_array([1.1, 2.2])
    assert v.get_double_array() == [pytest.approx(1.1), pytest.approx(2.2)]
    assert v.value() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_mkvalue_double_list():
    v = ntcore.Value.make_value([1.1, 2.2])
    assert v.get_double_array() == [pytest.approx(1.1), pytest.approx(2.2)]
    assert v.value() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_double_list_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.DOUBLE_ARRAY)
    v = f([1.1, 2.2])
    assert v.get_double_array() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_value_float_list():
    v = ntcore.Value.make_float_array([1.1, 2.2])
    assert v.get_float_array() == [pytest.approx(1.1), pytest.approx(2.2)]
    assert v.value() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_mkvalue_float_list():
    pass  # not possible to use make_value to make a float list


def test_float_list_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.FLOAT_ARRAY)
    v = f([1.1, 2.2])
    assert v.get_float_array() == [pytest.approx(1.1), pytest.approx(2.2)]


def test_value_str_list():
    v = ntcore.Value.make_string_array(["a", "b"])
    assert v.get_string_array() == ["a", "b"]
    assert v.value() == ["a", "b"]


def test_mkvalue_str_list():
    v = ntcore.Value.make_value(["a", "b"])
    assert v.get_string_array() == ["a", "b"]
    assert v.value() == ["a", "b"]


def test_str_list_factory():
    f = ntcore.Value.get_factory_by_type(ntcore.NetworkTableType.STRING_ARRAY)
    v = f(["a", "b"])
    assert v.get_string_array() == ["a", "b"]
