# notrack
from wpilib import TimedRobot

from .commandscheduler import CommandScheduler

seconds = float


class TimedCommandRobot(TimedRobot):
    SCHEDULER_OFFSET = 0.005

    def __init__(self, period: seconds = TimedRobot.DEFAULT_PERIOD / 1000) -> None:
        super().__init__(period)
        self.add_periodic(
            CommandScheduler.get_instance().run, period, self.SCHEDULER_OFFSET
        )
