import argparse
import importlib.metadata
import os
import sys
import typing

import wpilib


if sys.version_info < (3, 10):

    def entry_points(group):
        eps = importlib.metadata.entry_points()
        return eps.get(group, [])

else:
    entry_points = importlib.metadata.entry_points


def _int_env_default(name: str, fallback: int) -> int:
    value = os.environ.get(name)
    if value is None:
        return fallback
    try:
        return int(value)
    except ValueError:
        return fallback


class RunRomi:
    """
    Runs the robot using the HAL simulator connected to a ROMI
    """

    def __init__(self, parser: argparse.ArgumentParser):
        parser.add_argument(
            "--nogui",
            default=False,
            action="store_true",
            help="Don't use the WPILib simulation gui",
        )

        sim_group = parser.add_argument_group("Additional simulation extensions")
        self.simexts = {}

        for entry_point in entry_points(group="robotpy_sim.2027"):
            try:
                sim_ext_module = entry_point.load()
            except ImportError:
                print(f"WARNING: Error detected in {entry_point}")
                continue

            self.simexts[entry_point.name] = sim_ext_module

            try:
                if entry_point.name == "ws-client":
                    cmd_help = argparse.SUPPRESS
                else:
                    cmd_help = importlib.metadata.metadata(entry_point.dist.name)[
                        "summary"
                    ]
            except AttributeError:
                cmd_help = "Load specified simulation extension"
            sim_group.add_argument(
                f"--{entry_point.name}",
                default=False,
                action="store_true",
                help=cmd_help,
            )

        parser.add_argument(
            "--host",
            default=os.environ.get("HALSIMWS_HOST", "10.0.0.2"),
            help="ROMI websocket host (default: HALSIMWS_HOST or 10.0.0.2)",
        )
        parser.add_argument(
            "--port",
            type=int,
            default=_int_env_default("HALSIMWS_PORT", 3300),
            help="ROMI websocket port (default: HALSIMWS_PORT or 3300)",
        )

    def run(
        self,
        options: argparse.Namespace,
        project_path: "os.PathLike[str]",
        robot_class: typing.Type[wpilib.RobotBase],
    ):
        if "ws-client" not in self.simexts:
            print(
                "ws-client HALSim extension is missing. Reinstall robotpy-halsim-ws to use run-romi",
                file=sys.stderr,
            )
            return False

        os.environ["HALSIMWS_HOST"] = options.host
        os.environ["HALSIMWS_PORT"] = str(options.port)

        options.ws_client = True

        if not options.nogui:
            try:
                import halsim_gui
            except ImportError:
                print("robotpy-halsim-gui is not installed!", file=sys.stderr)
                return False
            else:
                halsim_gui.loadExtension()

        cwd = os.getcwd()

        for name, module in self.simexts.items():
            if getattr(options, name.replace("-", "_"), False):
                try:
                    module.loadExtension()
                except Exception:
                    print(f"Error loading {name}!", file=sys.stderr)
                    raise

        os.chdir(cwd)
        return robot_class.main(robot_class)
