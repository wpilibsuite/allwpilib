import pytest
import wpimath_test


def test_units_attributes():
    assert wpimath_test.SomeClass.s_constant == 2
    assert wpimath_test.SomeClass.ms_constant1 == 20  # the unit is ms, not seconds
    assert wpimath_test.SomeClass.ms_constant2 == 0.050
    assert wpimath_test.SomeClass.ms_constant3 == 200


def test_units_check_default_by_name1():
    sc = wpimath_test.SomeClass()

    assert sc.checkDefaultByName1(0.020) == True
    assert sc.checkDefaultByName1() == True

    with pytest.raises(RuntimeError):
        sc.checkDefaultByName1(100)


def test_units_check_default_by_name2():
    sc = wpimath_test.SomeClass()

    assert sc.checkDefaultByName2(0.050) == True
    assert sc.checkDefaultByName2() == True

    with pytest.raises(RuntimeError):
        sc.checkDefaultByName2(100)


def test_units_check_default_by_num1():
    sc = wpimath_test.SomeClass()

    assert sc.checkDefaultByNum1(0.050) == True
    assert sc.checkDefaultByNum1() == True

    with pytest.raises(RuntimeError):
        sc.checkDefaultByNum1(100)


def test_units_check_default_by_num2():
    sc = wpimath_test.SomeClass()

    assert sc.checkDefaultByNum2(0.050) == True
    assert sc.checkDefaultByNum2() == True

    with pytest.raises(RuntimeError):
        sc.checkDefaultByNum2(100)


def test_units_ft():
    assert wpimath_test.SomeClass.five_ft == 5.0


def test_units_ft2m():
    sc = wpimath_test.SomeClass()

    assert sc.ft2m(3) == 0.9144


def test_units_ms2s():
    sc = wpimath_test.SomeClass()

    assert sc.ms2s(20) == 0.020


def test_units_s2ms():
    sc = wpimath_test.SomeClass()

    assert sc.s2ms(0.2) == 200.0
