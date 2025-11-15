from typing import TYPE_CHECKING

import commands2
from util import *  # type: ignore
from wpilib.simulation import stepTiming

if TYPE_CHECKING:
    from .util import *


def test_onTrue(scheduler: commands2.CommandScheduler):
    finished = OOBoolean(False)
    command1 = commands2.WaitUntilCommand(finished)

    button = InternalButton()
    button.setPressed(False)
    button.onTrue(command1)
    scheduler.run()
    assert not command1.isScheduled()
    button.setPressed(True)
    scheduler.run()
    assert command1.isScheduled()
    finished.set(True)
    scheduler.run()
    assert not command1.isScheduled()


def test_onFalse(scheduler: commands2.CommandScheduler):
    finished = OOBoolean(False)
    command1 = commands2.WaitUntilCommand(finished)

    button = InternalButton()
    button.setPressed(True)
    button.onFalse(command1)
    scheduler.run()
    assert not command1.isScheduled()
    button.setPressed(False)
    scheduler.run()
    assert command1.isScheduled()
    finished.set(True)
    scheduler.run()
    assert not command1.isScheduled()


def test_onChange(scheduler: commands2.CommandScheduler):
    finished = OOBoolean(False)
    command1 = commands2.WaitUntilCommand(finished)

    button = InternalButton()
    button.setPressed(True)
    button.onChange(command1)
    scheduler.run()
    assert not command1.isScheduled()
    button.setPressed(False)
    scheduler.run()
    assert command1.isScheduled()
    finished.set(True)
    scheduler.run()
    assert not command1.isScheduled()


def test_whileTrueRepeatedly(scheduler: commands2.CommandScheduler):
    inits = OOInteger(0)
    counter = OOInteger(0)

    command1 = commands2.FunctionalCommand(
        inits.incrementAndGet,
        lambda: None,
        lambda _: None,
        lambda: counter.incrementAndGet() % 2 == 0,
    ).repeatedly()

    button = InternalButton()
    button.setPressed(False)
    button.whileTrue(command1)
    scheduler.run()
    assert inits == 0
    button.setPressed(True)
    scheduler.run()
    assert inits == 1
    scheduler.run()
    assert inits == 1
    scheduler.run()
    assert inits == 2
    button.setPressed(False)
    scheduler.run()
    assert inits == 2


def test_whileTrueLambdaRunCommand(scheduler: commands2.CommandScheduler):
    counter = OOInteger(0)

    command1 = commands2.RunCommand(counter.incrementAndGet)

    button = InternalButton()
    button.setPressed(False)
    button.whileTrue(command1)
    scheduler.run()
    assert counter == 0
    button.setPressed(True)
    scheduler.run()
    assert counter == 1
    scheduler.run()
    assert counter == 2
    button.setPressed(False)
    scheduler.run()
    assert counter == 2


def test_whileTrueOnce(scheduler: commands2.CommandScheduler):
    startCounter = OOInteger(0)
    endCounter = OOInteger(0)

    command1 = commands2.StartEndCommand(
        startCounter.incrementAndGet, endCounter.incrementAndGet
    )

    button = InternalButton()
    button.setPressed(False)
    button.whileTrue(command1)
    scheduler.run()
    assert startCounter == 0
    assert endCounter == 0
    button.setPressed(True)
    scheduler.run()
    scheduler.run()
    assert startCounter == 1
    assert endCounter == 0
    button.setPressed(False)
    scheduler.run()
    assert startCounter == 1
    assert endCounter == 1


def test_toggleOnTrue(scheduler: commands2.CommandScheduler):
    startCounter = OOInteger(0)
    endCounter = OOInteger(0)

    command1 = commands2.StartEndCommand(
        startCounter.incrementAndGet, endCounter.incrementAndGet
    )

    button = InternalButton()
    button.setPressed(False)
    button.toggleOnTrue(command1)
    scheduler.run()
    assert startCounter == 0
    assert endCounter == 0
    button.setPressed(True)
    scheduler.run()
    scheduler.run()
    assert startCounter == 1
    assert endCounter == 0
    button.setPressed(False)
    scheduler.run()
    assert startCounter == 1
    assert endCounter == 0
    button.setPressed(True)
    scheduler.run()
    assert startCounter == 1
    assert endCounter == 1


def test_cancelWhenActive(scheduler: commands2.CommandScheduler):
    startCounter = OOInteger(0)
    endCounter = OOInteger(0)

    button = InternalButton()
    command1 = commands2.StartEndCommand(
        startCounter.incrementAndGet, endCounter.incrementAndGet
    ).until(button)

    button.setPressed(False)
    command1.schedule()
    scheduler.run()
    assert startCounter == 1
    assert endCounter == 0
    button.setPressed(True)
    scheduler.run()
    assert startCounter == 1
    assert endCounter == 1
    scheduler.run()
    assert startCounter == 1
    assert endCounter == 1


def test_triggerComposition():
    button1 = InternalButton()
    button2 = InternalButton()

    button1.setPressed(True)
    button2.setPressed(False)

    assert button1.and_(button2).getAsBoolean() == False
    assert button1.or_(button2)() == True
    assert bool(button1.negate()) == False
    assert (button1 & ~button2)() == True


def test_triggerCompositionSupplier():
    button1 = InternalButton()
    supplier = lambda: False

    button1.setPressed(True)

    assert button1.and_(supplier)() == False
    assert button1.or_(supplier)() == True


def test_debounce(scheduler: commands2.CommandScheduler):
    command = commands2.Command()
    start_spying_on(command)

    button = InternalButton()
    debounced = button.debounce(0.1)

    debounced.onTrue(command)

    button.setPressed(True)
    scheduler.run()

    verify(command, never()).schedule()

    stepTiming(0.3)

    button.setPressed(True)
    scheduler.run()
    verify(command).schedule()


def test_booleanSupplier():
    button = InternalButton()

    assert button() == False
    button.setPressed(True)
    assert button() == True
