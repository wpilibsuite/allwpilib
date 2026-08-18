import wpiutil
import pytest


def test_default():
    start_time = wpiutil.get_program_start_time()
    now = wpiutil.now()

    assert start_time == wpiutil.get_program_start_time()
    assert start_time <= now
    assert now <= wpiutil.now()


NOW_TIMESTAMP_S = 0


@pytest.fixture(autouse=True)
def reset_timestamp():
    wpiutil.set_now_impl(None)
    yield
    wpiutil.set_now_impl(None)


def custom_now_getter():
    global NOW_TIMESTAMP_S
    return int(NOW_TIMESTAMP_S * 1e6)


@pytest.fixture
def custom_fixture():
    global NOW_TIMESTAMP_S
    NOW_TIMESTAMP_S = 0
    wpiutil.set_now_impl(custom_now_getter)
    yield


def test_custom_timestamp(custom_fixture):
    global NOW_TIMESTAMP_S

    assert 0 == wpiutil.now()
    assert 0 == wpiutil.get_program_start_time()

    NOW_TIMESTAMP_S = 1.5
    assert 1_500_000 == wpiutil.now()
    assert 0 == wpiutil.get_program_start_time()

    NOW_TIMESTAMP_S = 100
    assert 100_000_000 == wpiutil.now()
    assert 0 == wpiutil.get_program_start_time()

    # Set it back to the standard implementation and expect monotonic time.
    wpiutil.set_now_impl(None)
    now = wpiutil.now()
    assert wpiutil.get_program_start_time() <= now
    assert now <= wpiutil.now()


def test_set_now_impl_updates_program_start_time():
    global NOW_TIMESTAMP_S
    original_start_time = wpiutil.get_program_start_time()

    NOW_TIMESTAMP_S = 1.5
    wpiutil.set_now_impl(custom_now_getter)

    assert 1_500_000 == wpiutil.now()
    assert 1_500_000 == wpiutil.get_program_start_time()

    NOW_TIMESTAMP_S = 100
    assert 100_000_000 == wpiutil.now()
    assert 1_500_000 == wpiutil.get_program_start_time()

    wpiutil.set_now_impl(None)

    assert original_start_time == wpiutil.get_program_start_time()
