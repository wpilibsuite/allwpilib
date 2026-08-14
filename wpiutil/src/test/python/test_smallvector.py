from wpiutil_test import module


def test_smallvec_load():
    assert module.load_smallvec_int([1, 2, 3, 4]) == [1, 2, 3, 4]


def test_smallvecbool_load():
    assert module.load_smallvec_int([True, True, False, True]) == [
        True,
        True,
        False,
        True,
    ]


def test_smallvec_cast():
    assert module.cast_smallvec() == [1, 2, 3, 4]


def test_smallvecimpl_load():
    assert module.load_smallvecimpl_int([1, 2, 3, 4]) == [1, 2, 3, 4]
