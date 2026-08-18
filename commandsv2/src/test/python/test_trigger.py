from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore
from wpilib.simulation import step_timing

if TYPE_CHECKING:
    from .util import *


def test_on_true(scheduler: commands2.CommandScheduler):
    finished = OOBoolean(False)
    command1 = commands2.WaitUntilCommand(finished)

    button = InternalButton()
    button.set_pressed(False)
    button.on_true(command1)
    scheduler.run()
    assert not command1.is_scheduled()
    button.set_pressed(True)
    scheduler.run()
    assert command1.is_scheduled()
    finished.set(True)
    scheduler.run()
    assert not command1.is_scheduled()


def test_on_false(scheduler: commands2.CommandScheduler):
    finished = OOBoolean(False)
    command1 = commands2.WaitUntilCommand(finished)

    button = InternalButton()
    button.set_pressed(True)
    button.on_false(command1)
    scheduler.run()
    assert not command1.is_scheduled()
    button.set_pressed(False)
    scheduler.run()
    assert command1.is_scheduled()
    finished.set(True)
    scheduler.run()
    assert not command1.is_scheduled()


def test_on_change(scheduler: commands2.CommandScheduler):
    finished = OOBoolean(False)
    command1 = commands2.WaitUntilCommand(finished)

    button = InternalButton()
    button.set_pressed(True)
    button.on_change(command1)
    scheduler.run()
    assert not command1.is_scheduled()
    button.set_pressed(False)
    scheduler.run()
    assert command1.is_scheduled()
    finished.set(True)
    scheduler.run()
    assert not command1.is_scheduled()


def test_while_true_repeatedly(scheduler: commands2.CommandScheduler):
    inits = OOInteger(0)
    counter = OOInteger(0)

    command1 = commands2.FunctionalCommand(
        inits.increment_and_get,
        lambda: None,
        lambda _: None,
        lambda: counter.increment_and_get() % 2 == 0,
    ).repeatedly()

    button = InternalButton()
    button.set_pressed(False)
    button.while_true(command1)
    scheduler.run()
    assert inits == 0
    button.set_pressed(True)
    scheduler.run()
    assert inits == 1
    scheduler.run()
    assert inits == 1
    scheduler.run()
    assert inits == 2
    button.set_pressed(False)
    scheduler.run()
    assert inits == 2


def test_while_true_lambda_run_command(scheduler: commands2.CommandScheduler):
    counter = OOInteger(0)

    command1 = commands2.RunCommand(counter.increment_and_get)

    button = InternalButton()
    button.set_pressed(False)
    button.while_true(command1)
    scheduler.run()
    assert counter == 0
    button.set_pressed(True)
    scheduler.run()
    assert counter == 1
    scheduler.run()
    assert counter == 2
    button.set_pressed(False)
    scheduler.run()
    assert counter == 2


def test_while_true_once(scheduler: commands2.CommandScheduler):
    start_counter = OOInteger(0)
    end_counter = OOInteger(0)

    command1 = commands2.StartEndCommand(
        start_counter.increment_and_get, end_counter.increment_and_get
    )

    button = InternalButton()
    button.set_pressed(False)
    button.while_true(command1)
    scheduler.run()
    assert start_counter == 0
    assert end_counter == 0
    button.set_pressed(True)
    scheduler.run()
    scheduler.run()
    assert start_counter == 1
    assert end_counter == 0
    button.set_pressed(False)
    scheduler.run()
    assert start_counter == 1
    assert end_counter == 1


def test_toggle_on_true(scheduler: commands2.CommandScheduler):
    start_counter = OOInteger(0)
    end_counter = OOInteger(0)

    command1 = commands2.StartEndCommand(
        start_counter.increment_and_get, end_counter.increment_and_get
    )

    button = InternalButton()
    button.set_pressed(False)
    button.toggle_on_true(command1)
    scheduler.run()
    assert start_counter == 0
    assert end_counter == 0
    button.set_pressed(True)
    scheduler.run()
    scheduler.run()
    assert start_counter == 1
    assert end_counter == 0
    button.set_pressed(False)
    scheduler.run()
    assert start_counter == 1
    assert end_counter == 0
    button.set_pressed(True)
    scheduler.run()
    assert start_counter == 1
    assert end_counter == 1


def test_cancel_when_active(scheduler: commands2.CommandScheduler):
    start_counter = OOInteger(0)
    end_counter = OOInteger(0)

    button = InternalButton()
    command1 = commands2.StartEndCommand(
        start_counter.increment_and_get, end_counter.increment_and_get
    ).until(button)

    button.set_pressed(False)
    command1.schedule()
    scheduler.run()
    assert start_counter == 1
    assert end_counter == 0
    button.set_pressed(True)
    scheduler.run()
    assert start_counter == 1
    assert end_counter == 1
    scheduler.run()
    assert start_counter == 1
    assert end_counter == 1


def test_trigger_composition():
    button1 = InternalButton()
    button2 = InternalButton()

    button1.set_pressed(True)
    button2.set_pressed(False)

    assert button1.and_(button2).get_as_boolean() == False
    assert button1.or_(button2)() == True
    assert bool(button1.negate()) == False
    assert (button1 & ~button2)() == True


def test_trigger_composition_supplier():
    button1 = InternalButton()
    supplier = lambda: False

    button1.set_pressed(True)

    assert button1.and_(supplier)() == False
    assert button1.or_(supplier)() == True


def test_debounce(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    start_spying_on(command)

    button = InternalButton()
    debounced = button.debounce(0.1)

    debounced.on_true(command)

    button.set_pressed(True)
    scheduler.run()

    verify(command, never()).schedule()

    step_timing(0.3)

    button.set_pressed(True)
    scheduler.run()
    verify(command).schedule()


def test_boolean_supplier():
    button = InternalButton()

    assert button() == False
    button.set_pressed(True)
    assert button() == True
