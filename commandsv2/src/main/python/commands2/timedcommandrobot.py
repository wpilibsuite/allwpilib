# notrack
from wpilib import TimedRobot

from .commandscheduler import CommandScheduler

seconds = float


class TimedCommandRobot(TimedRobot):
    kSchedulerOffset = 0.005

    def __init__(self, period: seconds = TimedRobot.kDefaultPeriod / 1000) -> None:
        super().__init__(period)
        self.addPeriodic(
            CommandScheduler.getInstance().run, period, self.kSchedulerOffset
        )
