from typing import TYPE_CHECKING

import commands2
from ntcore import NetworkTableInstance
from util import *  # type: ignore

if TYPE_CHECKING:
    from .util import *


def test_networkbutton(
    scheduler: commands2.CommandScheduler, nt_instance: NetworkTableInstance
):
    # command = commands2.Command()
    command = commands2.Command()
    start_spying_on(command)

    pub = nt_instance.get_table("TestTable").get_boolean_topic("Test").publish()

    button = commands2.button.NetworkButton(nt_instance, "TestTable", "Test")

    pub.set(False)
    button.on_true(command)
    scheduler.run()
    assert command.schedule.times_called == 0
    pub.set(True)
    scheduler.run()
    scheduler.run()
    verify(command).schedule()
