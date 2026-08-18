import pytest
import wpimath_test


def test_units_attributes():
    assert wpimath_test.SomeClass.S_CONSTANT == 2
    assert wpimath_test.SomeClass.MS_CONSTANT1 == 20  # the unit is ms, not seconds
    assert wpimath_test.SomeClass.MS_CONSTANT2 == 0.050
    assert wpimath_test.SomeClass.MS_CONSTANT3 == 200


def test_units_check_default_by_name1():
    sc = wpimath_test.SomeClass()

    assert sc.check_default_by_name1(0.020) == True
    assert sc.check_default_by_name1() == True

    with pytest.raises(RuntimeError):
        sc.check_default_by_name1(100)


def test_units_check_default_by_name2():
    sc = wpimath_test.SomeClass()

    assert sc.check_default_by_name2(0.050) == True
    assert sc.check_default_by_name2() == True

    with pytest.raises(RuntimeError):
        sc.check_default_by_name2(100)


def test_units_check_default_by_num1():
    sc = wpimath_test.SomeClass()

    assert sc.check_default_by_num1(0.050) == True
    assert sc.check_default_by_num1() == True

    with pytest.raises(RuntimeError):
        sc.check_default_by_num1(100)


def test_units_check_default_by_num2():
    sc = wpimath_test.SomeClass()

    assert sc.check_default_by_num2(0.050) == True
    assert sc.check_default_by_num2() == True

    with pytest.raises(RuntimeError):
        sc.check_default_by_num2(100)


def test_units_ft():
    assert wpimath_test.SomeClass.FIVE_FT == 5.0


def test_units_ft2m():
    sc = wpimath_test.SomeClass()

    assert sc.ft2m(3) == 0.9144


def test_units_ms2s():
    sc = wpimath_test.SomeClass()

    assert sc.ms2s(20) == 0.020


def test_units_s2ms():
    sc = wpimath_test.SomeClass()

    assert sc.s2ms(0.2) == 200.0
