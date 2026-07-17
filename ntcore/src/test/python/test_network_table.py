#
# These tests are leftover from the original pynetworktables tests
#

import pytest


@pytest.fixture(scope="function")
def table1(nt):
    return nt.get_table("/test1")


@pytest.fixture(scope="function")
def table2(nt):
    return nt.get_table("/test2")


def test_put_double(table1):
    table1.put_number("double", 42.42)
    assert table1.get_number("double", None) == 42.42

    assert table1.get_number("Non-Existent", 44.44) == 44.44


def test_put_boolean(table1):
    table1.put_boolean("boolean", True)
    assert table1.get_boolean("boolean", None) == True

    assert table1.get_boolean("Non-Existent", False) == False


def test_put_string(table1):
    table1.put_string("String", "Test 1")
    assert table1.get_string("String", None) == "Test 1"

    assert table1.get_string("Non-Existent", "Test 3") == "Test 3"


def test_getvalue_overloads(table1):
    table1.put_value("float", 35.5)
    assert table1.get_value("float", None) == pytest.approx(35.5)

    table1.put_value("integer", 950)
    assert table1.get_value("integer", None) == pytest.approx(950)

    table1.put_value("boolean", True)
    assert table1.get_value("boolean", None) is True


def test_multi_data_type(table1):
    table1.put_number("double1", 1)
    table1.put_number("double2", 2)
    table1.put_number("double3", 3)
    table1.put_boolean("bool1", False)
    table1.put_boolean("bool2", True)
    table1.put_string("string1", "String 1")
    table1.put_string("string2", "String 2")
    table1.put_string("string3", "String 3")

    assert table1.get_number("double1", None) == 1
    assert table1.get_number("double2", None) == 2
    assert table1.get_number("double3", None) == 3
    assert table1.get_boolean("bool1", None) == False
    assert table1.get_boolean("bool2", None) == True
    assert table1.get_string("string1", None) == "String 1"
    assert table1.get_string("string2", None) == "String 2"
    assert table1.get_string("string3", None) == "String 3"

    table1.put_number("double1", 4)
    table1.put_number("double2", 5)
    table1.put_number("double3", 6)
    table1.put_boolean("bool1", True)
    table1.put_boolean("bool2", False)
    table1.put_string("string1", "String 4")
    table1.put_string("string2", "String 5")
    table1.put_string("string3", "String 6")

    assert table1.get_number("double1", None) == 4
    assert table1.get_number("double2", None) == 5
    assert table1.get_number("double3", None) == 6
    assert table1.get_boolean("bool1", None) == True
    assert table1.get_boolean("bool2", None) == False
    assert table1.get_string("string1", None) == "String 4"
    assert table1.get_string("string2", None) == "String 5"
    assert table1.get_string("string3", None) == "String 6"


def test_multi_table(table1, table2):
    table1.put_number("table1double", 1)
    table1.put_boolean("table1boolean", True)
    table1.put_string("table1string", "Table 1")

    assert table2.get_number("table1double", None) == None
    assert table2.get_boolean("table1boolean", None) == None
    assert table2.get_string("table1string", None) == None

    table2.put_number("table2double", 2)
    table2.put_boolean("table2boolean", False)
    table2.put_string("table2string", "Table 2")

    assert table1.get_number("table2double", None) == None
    assert table1.get_boolean("table2boolean", None) == None
    assert table1.get_string("table2string", None) == None


# def test_get_table(nt, table1, table2):
#     assert nt.get_table("test1") is table1
#     assert nt.get_table("test2") is table2

#     assert nt.get_table("/test1") is table1
#     assert nt.get_table("/test2") is table2

#     assert nt.get_table("/test1/") is table1
#     assert nt.get_table("/test1/").path == "/test1"

#     assert table1 is not table2

#     table3 = nt.get_table("/test3")
#     assert table1 is not table3
#     assert table2 is not table3


# def test_get_subtable(nt, table1):
#     assert not table1.contains_sub_table("test1")

#     st1 = table1.get_sub_table("test1")

#     assert nt.get_table("/test1/test1") is st1
#     assert table1.get_sub_table("test1") is st1

#     # weird, but true -- subtable only exists when key exists
#     assert not table1.contains_sub_table("test1")
#     st1.put_boolean("hi", True)
#     assert table1.contains_sub_table("test1")

#     assert table1.get_sub_tables() == ["test1"]
#     assert st1.get_sub_tables() == []


def test_getkeys(table1):
    assert table1.get_keys() == []
    assert not table1.contains_key("hi")
    assert "hi" not in table1

    table1.put_boolean("hi", True)
    assert table1.get_keys() == ["hi"]

    assert table1.contains_key("hi")
    assert "hi" in table1


def test_flags(table1):
    table1.put_boolean("foo", True)
    assert not table1.is_persistent("foo")

    table1.set_persistent("foo")
    assert table1.is_persistent("foo")

    table1.clear_persistent("foo")
    assert not table1.is_persistent("foo")


# def test_delete(table1):
#     table1.put_boolean("foo", True)
#     assert table1.get_boolean("foo", None) == True

#     table1.delete("foo")
#     assert table1.get_boolean("foo", None) == None


def test_different_type(table1):
    assert table1.put_boolean("foo", True)
    assert table1.get_boolean("foo", None) == True

    assert not table1.put_number("foo", 1)
    assert table1.get_boolean("foo", None) == True
