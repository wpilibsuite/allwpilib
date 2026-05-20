
import wpiutil
import time
import pytest

def test_default():
    wpi_now = wpiutil.now() * 1e-6
    py_now = int(time.time())

    # Allow a one second delta. We don't care about it being all that accurate in the
    # test, just that we are in the same galaxy
    assert py_now == pytest.approx(wpi_now, abs=1)


NOW_TIMESTAMP_S = 0

def custom_now_getter():
    global NOW_TIMESTAMP_S
    return int(NOW_TIMESTAMP_S * 1e6)


@pytest.fixture
def custom_fixture():
    wpiutil.SetNowImpl(custom_now_getter)
    yield
    wpiutil.SetNowImpl(None)


def test_custom_timestamp(custom_fixture):
    global NOW_TIMESTAMP_S

    assert 0 == wpiutil.now()

    NOW_TIMESTAMP_S = 1.5
    assert 1_500_000 == wpiutil.now()
    
    NOW_TIMESTAMP_S = 100
    assert 100_000_000 == wpiutil.now()

    # Set it back to the standard implementation and expect its roughly milliseconds since 1970
    wpiutil.SetNowImpl(None)
    wpi_now = wpiutil.now() * 1e-6
    py_now = int(time.time())
    assert py_now == pytest.approx(wpi_now, abs=1)


