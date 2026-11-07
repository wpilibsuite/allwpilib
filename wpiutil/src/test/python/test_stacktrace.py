import wpiutil


def test_python_stack_trace():
    st = wpiutil._wpiutil.get_stack_trace(0)
    assert __file__ in st

    st = wpiutil._wpiutil.get_stack_trace_default(0)
    assert __file__ not in st
