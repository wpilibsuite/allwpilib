from wpiutil_test.module import (
    cast_json_arg,
    cast_json_val,
    max_int64,
    min_int64,
    max_uint64,
)
import pytest
import math


def test_json_invalid():
    with pytest.raises(TypeError):
        cast_json_val(lambda: object())


def test_json_none():
    assert cast_json_arg(None) == None


def test_json_bool():
    assert cast_json_arg(True) == True
    assert cast_json_arg(False) == False


def test_json_int():
    assert cast_json_arg(36) == 36

    assert cast_json_arg(min_int64) == min_int64
    with pytest.raises(ValueError):
        cast_json_arg(min_int64 - 1)

    assert cast_json_arg(max_int64) == max_int64
    assert cast_json_arg(max_uint64) == max_uint64
    with pytest.raises(ValueError):
        cast_json_arg(max_uint64 + 1)


def test_json_float():
    assert cast_json_arg(36.37) == 36.37
    assert cast_json_arg(math.inf) == math.inf
    assert math.isnan(cast_json_arg(math.nan))


def test_json_string():
    assert cast_json_arg("hi") == "hi"


def test_json_list():
    v = [36, "hello", False]
    assert cast_json_arg(v) == v

    assert cast_json_arg([]) == []

    tv = (36, "hello", False)
    assert cast_json_arg(tv) == v


def test_json_dict():
    d = {"number": 1234, "hello": "world"}
    assert cast_json_arg(d) == d

    assert cast_json_arg({}) == {}

    assert cast_json_arg({1: 2}) == {"1": 2}
    assert cast_json_arg({None: 2}) == {"None": 2}
    assert cast_json_arg({1.2: 2}) == {"1.2": 2}
    assert cast_json_arg({False: 2}) == {"False": 2}

    with pytest.raises(TypeError):
        cast_json_arg({object(): 2})
