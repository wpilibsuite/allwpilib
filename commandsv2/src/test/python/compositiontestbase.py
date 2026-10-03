from typing import Generic, TypeVar

import commands2
import pytest

# T = TypeVar("T", bound=commands2.Command)
# T = commands2.Command

from util import *


class SingleCompositionTestBase:
    def compose_single(self, member: commands2.Command):
        raise NotImplementedError

    @pytest.mark.parametrize(
        "interruption_behavior",
        [
            commands2.InterruptionBehavior.CANCEL_SELF,
            commands2.InterruptionBehavior.CANCEL_INCOMING,
        ],
    )
    def test_interruptible(self, interruption_behavior: commands2.InterruptionBehavior):
        command = self.compose_single(
            commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                interruption_behavior
            )
        )
        assert command.get_interruption_behavior() == interruption_behavior

    @pytest.mark.parametrize("runs_when_disabled", [True, False])
    def test_run_when_disabled(self, runs_when_disabled: bool):
        command = self.compose_single(
            commands2.WaitUntilCommand(lambda: False).ignoring_disable(
                runs_when_disabled
            )
        )
        assert command.runs_when_disabled() == runs_when_disabled

    def test_command_in_other_composition(self):
        command = commands2.InstantCommand()
        wrapped = commands2.WrapperCommand(command)
        with pytest.raises(commands2.IllegalCommandUse):
            self.compose_single(command)

    def test_command_in_multiple_compositions(self):
        command = commands2.InstantCommand()
        self.compose_single(command)
        with pytest.raises(commands2.IllegalCommandUse):
            self.compose_single(command)

    def test_compose_then_schedule(self, scheduler: commands2.CommandScheduler):
        command = commands2.InstantCommand()
        self.compose_single(command)
        with pytest.raises(commands2.IllegalCommandUse):
            scheduler.schedule(command)

    def test_schedule_then_compose(self, scheduler: commands2.CommandScheduler):
        command = commands2.RunCommand(lambda: None)
        scheduler.schedule(command)
        with pytest.raises(commands2.IllegalCommandUse):
            self.compose_single(command)


class MultiCompositionTestBase(SingleCompositionTestBase):
    def compose(self, *members: commands2.Command):
        raise NotImplementedError

    def compose_single(self, member: commands2.Command):
        return self.compose(member)

    @pytest.mark.parametrize(
        "expected,command1,command2,command3",
        [
            pytest.param(
                commands2.InterruptionBehavior.CANCEL_SELF,
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_SELF
                ),
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_SELF
                ),
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_SELF
                ),
                id="AllCancelSelf",
            ),
            pytest.param(
                commands2.InterruptionBehavior.CANCEL_INCOMING,
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_INCOMING
                ),
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_INCOMING
                ),
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_INCOMING
                ),
                id="AllCancelIncoming",
            ),
            pytest.param(
                commands2.InterruptionBehavior.CANCEL_SELF,
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_SELF
                ),
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_SELF
                ),
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_INCOMING
                ),
                id="TwoCancelSelfOneIncoming",
            ),
            pytest.param(
                commands2.InterruptionBehavior.CANCEL_SELF,
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_INCOMING
                ),
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_INCOMING
                ),
                commands2.WaitUntilCommand(lambda: False).with_interrupt_behavior(
                    commands2.InterruptionBehavior.CANCEL_SELF
                ),
                id="TwoCancelIncomingOneSelf",
            ),
        ],
    )
    def test_interruptible(self, expected, command1, command2, command3):
        command = self.compose(command1, command2, command3)
        assert command.get_interruption_behavior() == expected

    @pytest.mark.parametrize(
        "expected,command1,command2,command3",
        [
            pytest.param(
                False,
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(False),
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(False),
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(False),
                id="AllFalse",
            ),
            pytest.param(
                True,
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(True),
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(True),
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(True),
                id="AllTrue",
            ),
            pytest.param(
                False,
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(True),
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(True),
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(False),
                id="TwoTrueOneFalse",
            ),
            pytest.param(
                False,
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(False),
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(False),
                commands2.WaitUntilCommand(lambda: False).ignoring_disable(True),
                id="TwoFalseOneTrue",
            ),
        ],
    )
    def test_run_when_disabled(self, expected, command1, command2, command3):
        command = self.compose(command1, command2, command3)
        assert command.runs_when_disabled() == expected
