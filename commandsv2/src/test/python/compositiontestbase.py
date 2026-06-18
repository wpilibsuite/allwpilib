from typing import Generic, TypeVar

import commands2
import pytest

# T = TypeVar("T", bound=commands2.Command)
# T = commands2.Command

from util import *


class SingleCompositionTestBase:
    def composeSingle(self, member: commands2.Command):
        raise NotImplementedError

    @pytest.mark.parametrize(
        "interruptionBehavior",
        [
            commands2.InterruptionBehavior.kCancelSelf,
            commands2.InterruptionBehavior.kCancelIncoming,
        ],
    )
    def test_interruptible(self, interruptionBehavior: commands2.InterruptionBehavior):
        command = self.composeSingle(
            commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                interruptionBehavior
            )
        )
        assert command.getInterruptionBehavior() == interruptionBehavior

    @pytest.mark.parametrize("runsWhenDisabled", [True, False])
    def test_runWhenDisabled(self, runsWhenDisabled: bool):
        command = self.composeSingle(
            commands2.WaitUntilCommand(lambda: False).ignoringDisable(runsWhenDisabled)
        )
        assert command.runsWhenDisabled() == runsWhenDisabled

    def test_command_in_other_composition(self):
        command = commands2.InstantCommand()
        wrapped = commands2.WrapperCommand(command)
        with pytest.raises(commands2.IllegalCommandUse):
            self.composeSingle(command)

    def test_command_in_multiple_compositions(self):
        command = commands2.InstantCommand()
        self.composeSingle(command)
        with pytest.raises(commands2.IllegalCommandUse):
            self.composeSingle(command)

    def test_compose_then_schedule(self, scheduler: commands2.CommandScheduler):
        command = commands2.InstantCommand()
        self.composeSingle(command)
        with pytest.raises(commands2.IllegalCommandUse):
            scheduler.schedule(command)

    def test_schedule_then_compose(self, scheduler: commands2.CommandScheduler):
        command = commands2.RunCommand(lambda: None)
        scheduler.schedule(command)
        with pytest.raises(commands2.IllegalCommandUse):
            self.composeSingle(command)


class MultiCompositionTestBase(SingleCompositionTestBase):
    def compose(self, *members: commands2.Command):
        raise NotImplementedError

    def composeSingle(self, member: commands2.Command):
        return self.compose(member)

    @pytest.mark.parametrize(
        "expected,command1,command2,command3",
        [
            pytest.param(
                commands2.InterruptionBehavior.kCancelSelf,
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelSelf
                ),
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelSelf
                ),
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelSelf
                ),
                id="AllCancelSelf",
            ),
            pytest.param(
                commands2.InterruptionBehavior.kCancelIncoming,
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelIncoming
                ),
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelIncoming
                ),
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelIncoming
                ),
                id="AllCancelIncoming",
            ),
            pytest.param(
                commands2.InterruptionBehavior.kCancelSelf,
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelSelf
                ),
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelSelf
                ),
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelIncoming
                ),
                id="TwoCancelSelfOneIncoming",
            ),
            pytest.param(
                commands2.InterruptionBehavior.kCancelSelf,
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelIncoming
                ),
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelIncoming
                ),
                commands2.WaitUntilCommand(lambda: False).withInterruptBehavior(
                    commands2.InterruptionBehavior.kCancelSelf
                ),
                id="TwoCancelIncomingOneSelf",
            ),
        ],
    )
    def test_interruptible(self, expected, command1, command2, command3):
        command = self.compose(command1, command2, command3)
        assert command.getInterruptionBehavior() == expected

    @pytest.mark.parametrize(
        "expected,command1,command2,command3",
        [
            pytest.param(
                False,
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(False),
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(False),
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(False),
                id="AllFalse",
            ),
            pytest.param(
                True,
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(True),
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(True),
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(True),
                id="AllTrue",
            ),
            pytest.param(
                False,
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(True),
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(True),
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(False),
                id="TwoTrueOneFalse",
            ),
            pytest.param(
                False,
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(False),
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(False),
                commands2.WaitUntilCommand(lambda: False).ignoringDisable(True),
                id="TwoFalseOneTrue",
            ),
        ],
    )
    def test_runWhenDisabled(self, expected, command1, command2, command3):
        command = self.compose(command1, command2, command3)
        assert command.runsWhenDisabled() == expected
