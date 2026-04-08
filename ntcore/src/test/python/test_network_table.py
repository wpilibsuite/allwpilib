#
# These tests are leftover from the original pynetworktables tests
#

import pytest


@pytest.fixture(scope="function")
def table1(nt):
    return nt.getTable("/test1")


@pytest.fixture(scope="function")
def table2(nt):
    return nt.getTable("/test2")


def test_put_double(table1):
    table1.putNumber("double", 42.42)
    assert table1.getNumber("double", None) == 42.42

    assert table1.getNumber("Non-Existant", 44.44) == 44.44


def test_put_boolean(table1):
    table1.putBoolean("boolean", True)
    assert table1.getBoolean("boolean", None) == True

    assert table1.getBoolean("Non-Existant", False) == False


def test_put_string(table1):
    table1.putString("String", "Test 1")
    assert table1.getString("String", None) == "Test 1"

    assert table1.getString("Non-Existant", "Test 3") == "Test 3"


def test_getvalue_overloads(table1):
    table1.putValue("float", 35.5)
    assert table1.getValue("float", None) == pytest.approx(35.5)

    table1.putValue("integer", 950)
    assert table1.getValue("integer", None) == pytest.approx(950)

    table1.putValue("boolean", True)
    assert table1.getValue("boolean", None) is True


def test_multi_data_type(table1):
    table1.putNumber("double1", 1)
    table1.putNumber("double2", 2)
    table1.putNumber("double3", 3)
    table1.putBoolean("bool1", False)
    table1.putBoolean("bool2", True)
    table1.putString("string1", "String 1")
    table1.putString("string2", "String 2")
    table1.putString("string3", "String 3")

    assert table1.getNumber("double1", None) == 1
    assert table1.getNumber("double2", None) == 2
    assert table1.getNumber("double3", None) == 3
    assert table1.getBoolean("bool1", None) == False
    assert table1.getBoolean("bool2", None) == True
    assert table1.getString("string1", None) == "String 1"
    assert table1.getString("string2", None) == "String 2"
    assert table1.getString("string3", None) == "String 3"

    table1.putNumber("double1", 4)
    table1.putNumber("double2", 5)
    table1.putNumber("double3", 6)
    table1.putBoolean("bool1", True)
    table1.putBoolean("bool2", False)
    table1.putString("string1", "String 4")
    table1.putString("string2", "String 5")
    table1.putString("string3", "String 6")

    assert table1.getNumber("double1", None) == 4
    assert table1.getNumber("double2", None) == 5
    assert table1.getNumber("double3", None) == 6
    assert table1.getBoolean("bool1", None) == True
    assert table1.getBoolean("bool2", None) == False
    assert table1.getString("string1", None) == "String 4"
    assert table1.getString("string2", None) == "String 5"
    assert table1.getString("string3", None) == "String 6"


def test_multi_table(table1, table2):
    table1.putNumber("table1double", 1)
    table1.putBoolean("table1boolean", True)
    table1.putString("table1string", "Table 1")

    assert table2.getNumber("table1double", None) == None
    assert table2.getBoolean("table1boolean", None) == None
    assert table2.getString("table1string", None) == None

    table2.putNumber("table2double", 2)
    table2.putBoolean("table2boolean", False)
    table2.putString("table2string", "Table 2")

    assert table1.getNumber("table2double", None) == None
    assert table1.getBoolean("table2boolean", None) == None
    assert table1.getString("table2string", None) == None


# def test_get_table(nt, table1, table2):
#     assert nt.getTable("test1") is table1
#     assert nt.getTable("test2") is table2

#     assert nt.getTable("/test1") is table1
#     assert nt.getTable("/test2") is table2

#     assert nt.getTable("/test1/") is table1
#     assert nt.getTable("/test1/").path == "/test1"

#     assert table1 is not table2

#     table3 = nt.getTable("/test3")
#     assert table1 is not table3
#     assert table2 is not table3


# def test_get_subtable(nt, table1):
#     assert not table1.containsSubTable("test1")

#     st1 = table1.getSubTable("test1")

#     assert nt.getTable("/test1/test1") is st1
#     assert table1.getSubTable("test1") is st1

#     # weird, but true -- subtable only exists when key exists
#     assert not table1.containsSubTable("test1")
#     st1.putBoolean("hi", True)
#     assert table1.containsSubTable("test1")

#     assert table1.getSubTables() == ["test1"]
#     assert st1.getSubTables() == []


def test_getkeys(table1):
    assert table1.getKeys() == []
    assert not table1.containsKey("hi")
    assert "hi" not in table1

    table1.putBoolean("hi", True)
    assert table1.getKeys() == ["hi"]

    assert table1.containsKey("hi")
    assert "hi" in table1


def test_flags(table1):
    table1.putBoolean("foo", True)
    assert not table1.isPersistent("foo")

    table1.setPersistent("foo")
    assert table1.isPersistent("foo")

    table1.clearPersistent("foo")
    assert not table1.isPersistent("foo")


# def test_delete(table1):
#     table1.putBoolean("foo", True)
#     assert table1.getBoolean("foo", None) == True

#     table1.delete("foo")
#     assert table1.getBoolean("foo", None) == None


def test_different_type(table1):
    assert table1.putBoolean("foo", True)
    assert table1.getBoolean("foo", None) == True

    assert not table1.putNumber("foo", 1)
    assert table1.getBoolean("foo", None) == True
