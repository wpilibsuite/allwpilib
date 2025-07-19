from wpiutil_test import module


def test_stringmap_load():
    assert module.load_stringmap_int({"one": 11, "two": 22, "three": 33}) == {
        "one": 11,
        "two": 22,
        "three": 33,
    }


def test_stringmap_cast():
    assert module.cast_stringmap() == {"one": 1, "two": 2}
