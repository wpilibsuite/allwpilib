#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from typing import override

import telemetry
import wpilib


class MyRobot(wpilib.TimedRobot):
    """
    Game Data from 2026 snippets for wpilib-docs.
    https://docs.wpilib.org/en/latest/docs/yearly-overview/2026-game-data.html
    """

    def is_hub_active(self) -> bool:
        alliance = wpilib.MatchState.get_alliance()
        # If we have no alliance, we cannot be enabled, therefore no hub.
        if alliance is None:
            return False

        # Hub is always enabled in autonomous.
        if wpilib.RobotState.is_autonomous_enabled():
            return True

        # At this point if we're not teleop enabled, there is no hub.
        if not wpilib.RobotState.is_teleop_enabled():
            return False

        # We're teleop enabled, compute.
        match_time = wpilib.MatchState.get_match_time()
        game_data = wpilib.MatchState.get_game_data()

        match game_data:
            case "R":
                red_inactive_first = True
            case "B":
                red_inactive_first = False
            case _:
                # No or invalid game data, assume hub is active.
                return True

        # Shift 1 is active for blue if red won auto, or red if blue won auto.
        shift1_active = (
            not red_inactive_first
            if alliance == wpilib.Alliance.RED
            else red_inactive_first
        )

        if match_time > 130:
            return True  # Transition shift, hub is active
        elif match_time > 105:
            # Shift 1
            return shift1_active
        elif match_time > 80:
            # Shift 2
            return not shift1_active
        elif match_time > 55:
            # Shift 3
            return shift1_active
        elif match_time > 30:
            # Shift 4
            return not shift1_active
        else:
            return True  # End game, hub always active

    @override
    def teleop_periodic(self):
        telemetry.log("Hub active", self.is_hub_active())
        telemetry.log("Match time", wpilib.MatchState.get_match_time())

        game_data = wpilib.MatchState.get_game_data() or "No game data"
        telemetry.log("Game data", game_data)

        alliance = wpilib.MatchState.get_alliance()
        telemetry.log(
            "Alliance", alliance.name if alliance is not None else "No alliance"
        )

    def handle_game_data(self):
        data = wpilib.MatchState.get_game_data()
        if data:
            match data:
                case "B":
                    # Blue case code
                    pass
                case "R":
                    # Red case code
                    pass
                case _:
                    # This is corrupt data
                    pass
        else:
            # Code for no data received yet
            pass
