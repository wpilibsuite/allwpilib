from wpiutil_test import module


def test_smallset_load():
    assert module.load_smallset_int({1, 2, 3, 4}) == {1, 2, 3, 4}


def test_smallsetbool_load():
    assert module.load_smallset_int({True, True, False, True}) == {
        True,
        True,
        False,
        True,
    }


def test_smallset_cast():
    assert module.cast_smallset() == {1, 2, 3, 4}
