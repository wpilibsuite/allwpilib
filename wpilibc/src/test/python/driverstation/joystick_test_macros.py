import pytest


def axis_test(controller_class, sim_class, axis_name):
    joy = controller_class(2)
    joysim = sim_class(joy)

    sim_setter_func = getattr(joysim, "set_" + axis_name)
    joy_get_func = getattr(joy, "get_" + axis_name)

    sim_setter_func(0.35)
    joysim.notify_new_data()
    assert joy_get_func() == pytest.approx(0.35, abs=0.001)


def button_test(controller_class, sim_class, btn_name):
    joy = controller_class(1)
    joysim = sim_class(joy)

    joy_get_func = getattr(joy, "get_" + btn_name)
    joy_get_pressed_func = getattr(joy, "get_" + btn_name + "_pressed")
    joy_get_released_func = getattr(joy, "get_" + btn_name + "_released")

    sim_setter_func = getattr(joysim, "set_" + btn_name)

    sim_setter_func(False)
    joysim.notify_new_data()
    assert False == joy_get_func()

    # need to call pressed and released to clear flags
    joy_get_pressed_func()
    joy_get_released_func()

    sim_setter_func(True)
    joysim.notify_new_data()
    assert True == joy_get_func()
    assert True == joy_get_pressed_func()
    assert False == joy_get_released_func()

    sim_setter_func(False)
    joysim.notify_new_data()
    assert False == joy_get_func()
    assert False == joy_get_pressed_func()
    assert True == joy_get_released_func()
