from wpiutil_test import module


def test_load_array_int():
    assert module.load_array_int((1, 2, 3, 4)) == (1, 2, 3, 4)
    assert module.load_array_int([1, 2, 3, 4]) == (1, 2, 3, 4)


def test_load_array_annotation():
    assert (
        module.load_array_int.__doc__
        == "load_array_int(arg0: Tuple[typing.SupportsInt, typing.SupportsInt, typing.SupportsInt, typing.SupportsInt]) -> Tuple[int, int, int, int]\n"
    )
    assert (
        module.load_array_int1.__doc__
        == "load_array_int1(arg0: Tuple[typing.SupportsInt]) -> Tuple[int]\n"
    )
