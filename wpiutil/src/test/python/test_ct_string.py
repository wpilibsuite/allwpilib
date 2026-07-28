from wpiutil_test import module


def test_const_string():
    assert module.const_string() == "#12"
