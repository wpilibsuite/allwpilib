import wpiutil


def test_python_stack_trace():
    st = wpiutil._wpiutil.getStackTrace(0)
    assert __file__ in st

    st = wpiutil._wpiutil.getStackTraceDefault(0)
    assert __file__ not in st
