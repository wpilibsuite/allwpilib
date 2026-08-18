import commands2

from util import *  # type: ignore


def test_deferred_functions(scheduler: commands2.CommandScheduler):
    inner_command = commands2.Command()
    command = commands2.DeferredCommand(lambda: inner_command)

    start_spying_on(inner_command)
    start_spying_on(command)

    command.initialize()
    verify(inner_command).initialize()

    command.execute()
    verify(inner_command).execute()

    assert not command.is_finished()
    verify(inner_command).is_finished()

    inner_command.is_finished = lambda: True
    assert command.is_finished()
    verify(inner_command, times=times(2)).is_finished()

    command.end(False)
    verify(inner_command).end(False)


def test_deferred_supplier_only_called_during_init(
    scheduler: commands2.CommandScheduler,
):
    supplier_called = 0

    def supplier() -> commands2.Command:
        nonlocal supplier_called
        supplier_called += 1
        return commands2.Command()

    command = commands2.DeferredCommand(supplier)
    assert supplier_called == 0

    scheduler.schedule(command)
    assert supplier_called == 1
    scheduler.run()

    scheduler.schedule(command)
    assert supplier_called == 1


def test_deferred_requirements(scheduler: commands2.CommandScheduler):
    subsystem = commands2.Subsystem()
    command = commands2.DeferredCommand(lambda: commands2.Command(), subsystem)

    assert subsystem in command.get_requirements()


def test_deferred_null_command(scheduler: commands2.CommandScheduler):
    command = commands2.DeferredCommand(lambda: None)  # type: ignore
    command.initialize()
    command.execute()
    command.is_finished()
    command.end(False)
