import importlib
from pathlib import Path
import sys
import textwrap
import threading
import tunables
from typing import get_args, get_origin, get_overloads, get_type_hints

import pytest
import wpilib
import wpilib._impl.opmode as opmode_impl
from wpilib import OpMode
from wpilib import simulation as wsim
from wpilib._wpilib import OpModeRobotBase, RobotState
from wpilib.opmodes import OpModeRobot, autonomous, teleop, utility
from hal import RobotMode
from wpiutil import Color


@pytest.fixture(autouse=True)
def reset_decorated_opmodes(monkeypatch):
    monkeypatch.setattr(opmode_impl, "_decorated_opmodes", [])
    RobotState.clear_opmodes()
    yield
    for module_name in tuple(sys.modules):
        if (
            module_name == "samplebot"
            or module_name.startswith("samplebot.")
            or module_name == "robot"
            or module_name == "opmodes"
            or module_name.startswith("opmodes.")
        ):
            sys.modules.pop(module_name)


def import_robot_package(monkeypatch, tmp_path, robot_source, module_contents):
    package_name = "samplebot"
    package_dir = tmp_path / package_name
    opmodes_dir = package_dir / "opmodes"
    opmodes_dir.mkdir(parents=True)
    (package_dir / "__init__.py").write_text("")
    (package_dir / "robot.py").write_text(textwrap.dedent(robot_source))
    (opmodes_dir / "__init__.py").write_text("")

    for relative_path, source in module_contents.items():
        module_path = package_dir / relative_path
        module_path.parent.mkdir(parents=True, exist_ok=True)
        module_path.write_text(textwrap.dedent(source))

    for module_name in tuple(sys.modules):
        if module_name == package_name or module_name.startswith(f"{package_name}."):
            sys.modules.pop(module_name)

    monkeypatch.syspath_prepend(str(tmp_path))
    importlib.invalidate_caches()
    return package_dir, importlib.import_module(f"{package_name}.robot")


def test_opmode_decorators_attach_metadata():
    @autonomous(group="Drive", description="Auto desc")
    class AutoMode(OpMode):
        pass

    @teleop
    class TeleMode(OpMode):
        pass

    @utility(
        name="Arm Test",
        text_color=Color.WHITE,
        background_color=Color.BLACK,
    )
    class UtilityMode(OpMode):
        pass

    assert AutoMode._wpilib_opmode_metadata.mode == RobotMode.AUTONOMOUS
    assert AutoMode._wpilib_opmode_metadata.name == "AutoMode"
    assert AutoMode._wpilib_opmode_metadata.group == "Drive"
    assert AutoMode._wpilib_opmode_metadata.description == "Auto desc"
    assert TeleMode._wpilib_opmode_metadata.mode == RobotMode.TELEOPERATED
    assert TeleMode._wpilib_opmode_metadata.name == "TeleMode"
    assert UtilityMode._wpilib_opmode_metadata.mode == RobotMode.UTILITY
    assert UtilityMode._wpilib_opmode_metadata.name == "Arm Test"
    assert UtilityMode._wpilib_opmode_metadata.text_color == Color.WHITE
    assert UtilityMode._wpilib_opmode_metadata.background_color == Color.BLACK
    assert wpilib.autonomous is autonomous
    assert wpilib.teleop is teleop
    assert wpilib.utility is utility


def test_opmode_decorators_expose_typed_overloads_and_docstrings():
    for decorator in (autonomous, teleop, utility):
        overloads = get_overloads(decorator)
        assert len(overloads) == 2
        for overload in overloads:
            hints = get_type_hints(overload)
            assert hints["name"] is str
            assert hints["group"] is str
            assert hints["description"] is str
            assert hints["text_color"] == Color | None
            assert hints["background_color"] == Color | None

        bare_return = get_type_hints(overloads[0])["return"]
        configured_return = get_type_hints(overloads[1])["return"]
        subtype = get_args(bare_return)[0]
        configured_parameters, configured_result = get_args(configured_return)
        assert get_origin(bare_return) is type
        assert subtype.__bound__ is OpMode
        assert get_args(configured_parameters[0])[0] is subtype
        assert get_args(configured_result)[0] is subtype
        assert decorator.__doc__
        assert "bare" in decorator.__doc__
        assert "configured" in decorator.__doc__


def test_opmode_decorators_preserve_distinct_function_local_classes():
    def make_mode(name):
        @utility(name=name)
        class LocalMode(OpMode):
            pass

        return LocalMode

    first_mode = make_mode("First Local")
    second_mode = make_mode("Second Local")

    class MinimalRobot(OpModeRobot):
        pass

    MinimalRobot()

    assert opmode_impl.decorated_opmodes() == (first_mode, second_mode)
    assert sorted(
        option.name for option in wsim.DriverStationSim.get_opmode_options()
    ) == ["First Local", "Second Local"]


def test_opmode_decorator_rejects_invalid_class_and_duplicate_mode():
    with pytest.raises(TypeError, match="OpMode subclass"):
        autonomous(type("NotAnOpMode", (), {}))

    @teleop
    class DriveMode(OpMode):
        pass

    with pytest.raises(ValueError, match="multiple opmode decorators"):
        autonomous(DriveMode)


def test_opmode_detector_recognizes_imported_module_alias():
    source = """
        from wpilib import PeriodicOpMode
        from wpilib import opmodes as modes

        @modes.utility
        class UtilityMode(PeriodicOpMode):
            pass
    """

    assert opmode_impl._has_opmode_decorator(textwrap.dedent(source), "utility.py")


@pytest.mark.parametrize("decorator", ["autonomous", "teleop", "utility"])
@pytest.mark.parametrize("configured", [False, True])
@pytest.mark.parametrize(
    "import_statement, decorator_prefix",
    [
        ("import wpilib.opmodes", "wpilib"),
        ("import wpilib.opmodes", "wpilib.opmodes"),
        ("import wpilib.opmodes as modes", "modes"),
    ],
)
def test_opmode_robot_discovers_decorators_after_dotted_import(
    monkeypatch, tmp_path, decorator, configured, import_statement, decorator_prefix
):
    decorator_args = '(group="Drive")' if configured else ""
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "opmodes/dotted_import.py": f"""
                from wpilib import OpMode
                {import_statement}

                @{decorator_prefix}.{decorator}{decorator_args}
                class DiscoveredMode(OpMode):
                    pass
            """,
        },
    )
    assert "samplebot.opmodes.dotted_import" not in sys.modules

    robot_module.Robot()

    options = wsim.DriverStationSim.get_opmode_options()
    assert [option.name for option in options] == ["DiscoveredMode"]
    assert options[0].group == ("Drive" if configured else "")
    assert "samplebot.opmodes.dotted_import" in sys.modules


@pytest.mark.parametrize("constructor_args", ["", "auto_discover=True"])
def test_opmode_robot_auto_discovers_bounded_opmodes(
    monkeypatch, tmp_path, constructor_args
):
    pkg, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        f"""
        import wpilib as wpi

        class Robot(wpi.OpModeRobot):
            def __init__(self):
                super().__init__({constructor_args})
        """,
        {
            "opmodes/auto_mode.py": """
                import wpilib as wpi

                @wpi.autonomous(name="Auto")
                class AutoMode(wpi.PeriodicOpMode):
                    pass
            """,
            "opmodes/nested/__init__.py": "",
            "opmodes/nested/tele_mode.py": """
                from wpilib import PeriodicOpMode
                from wpilib.opmodes import teleop as tele

                @tele(group="Drive")
                class TeleMode(PeriodicOpMode):
                    pass
            """,
            "opmodes/ignored.py": """
                from pathlib import Path
                Path(__file__).with_name("ignored-imported").touch()
            """,
        },
    )

    robot_module.Robot()

    options = {
        option.name: option for option in wsim.DriverStationSim.get_opmode_options()
    }
    assert set(options) == {"Auto", "TeleMode"}
    assert options["TeleMode"].group == "Drive"
    assert not (pkg / "opmodes" / "ignored-imported").exists()


def test_opmode_robot_can_disable_auto_discovery(monkeypatch, tmp_path):
    package_dir, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        @wpilib.autonomous
        class ImportedMode(wpilib.OpMode):
            pass

        class Robot(wpilib.OpModeRobot):
            def __init__(self):
                super().__init__(auto_discover=False)
        """,
        {
            "opmodes/discovered_mode.py": """
                from pathlib import Path
                import wpilib

                Path(__file__).with_name("mode-imported").touch()

                @wpilib.teleop
                class DiscoveredMode(wpilib.OpMode):
                    pass
            """,
        },
    )

    robot = robot_module.Robot()

    assert "samplebot.opmodes.discovered_mode" not in sys.modules
    assert not (package_dir / "opmodes" / "mode-imported").exists()
    assert not wsim.DriverStationSim.get_opmode_options()

    # Explicit publication must not reveal any automatically registered modes.
    robot.publish_opmodes()
    assert not wsim.DriverStationSim.get_opmode_options()

    robot.add_opmode(robot_module.ImportedMode, RobotMode.AUTONOMOUS, "Manual")
    assert not wsim.DriverStationSim.get_opmode_options()
    robot.publish_opmodes()
    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "Manual"
    ]


def test_disabled_auto_discovery_does_not_publish_pending_opmodes():
    class ManualMode(OpMode):
        pass

    publisher = OpModeRobot()
    publisher.add_opmode(ManualMode, RobotMode.AUTONOMOUS, "Manual")
    assert not wsim.DriverStationSim.get_opmode_options()

    robot = OpModeRobot(auto_discover=False)

    assert not wsim.DriverStationSim.get_opmode_options()
    publisher.publish_opmodes()
    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "Manual"
    ]


@pytest.mark.parametrize("has_init", [True, False])
@pytest.mark.parametrize("preload", [True, False])
def test_opmode_robot_discovers_multiple_implicit_namespace_modules(
    monkeypatch, tmp_path, has_init, preload
):
    package_dir, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "opmodes/namespace/first.py": """
                import wpilib

                @wpilib.autonomous
                class FirstMode(wpilib.OpMode):
                    pass
            """,
            "opmodes/namespace/second.py": """
                import wpilib

                @wpilib.teleop
                class SecondMode(wpilib.OpMode):
                    pass
            """,
        },
    )

    if not has_init:
        (package_dir / "opmodes" / "__init__.py").unlink()
    if preload:
        importlib.import_module("samplebot.opmodes")

    robot_module.Robot()

    assert {option.name for option in wsim.DriverStationSim.get_opmode_options()} == {
        "FirstMode",
        "SecondMode",
    }


@pytest.mark.parametrize("has_init", [True, False])
@pytest.mark.parametrize(
    ("robot_path", "robot_module_name", "opmodes_path", "expected_name"),
    [
        ("samplebot/__init__.py", "samplebot", "samplebot/opmodes", "PackageMode"),
        (
            "samplebot/robot/__init__.py",
            "samplebot.robot",
            "samplebot/robot/opmodes",
            "NestedPackageMode",
        ),
        ("robot.py", "robot", "opmodes", "TopLevelMode"),
    ],
)
def test_opmode_robot_derives_opmodes_package_from_robot_source(
    monkeypatch,
    tmp_path,
    robot_path,
    robot_module_name,
    opmodes_path,
    expected_name,
    has_init,
):
    robot_file = tmp_path / robot_path
    robot_file.parent.mkdir(parents=True, exist_ok=True)
    if robot_path == "samplebot/robot/__init__.py":
        (tmp_path / "samplebot" / "__init__.py").write_text("")
    robot_file.write_text(
        "import wpilib\n\nclass Robot(wpilib.OpModeRobot):\n    pass\n"
    )
    opmodes_dir = tmp_path / opmodes_path
    opmodes_dir.mkdir(parents=True)
    if has_init:
        (opmodes_dir / "__init__.py").write_text("")
    (opmodes_dir / "mode.py").write_text(
        "import wpilib\n\n"
        f"@wpilib.autonomous(name={expected_name!r})\n"
        "class Mode(wpilib.OpMode):\n    pass\n"
    )

    monkeypatch.syspath_prepend(str(tmp_path))
    importlib.invalidate_caches()
    robot_module = importlib.import_module(robot_module_name)
    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        expected_name
    ]


@pytest.mark.parametrize("has_init", [True, False])
def test_opmode_robot_rejects_preloaded_package_origin_collision(
    monkeypatch, tmp_path, caplog, has_init
):
    conflict_dir = tmp_path / "conflict"
    conflict_opmodes = conflict_dir / "opmodes"
    conflict_opmodes.mkdir(parents=True)
    (conflict_opmodes / "__init__.py").write_text("")
    (conflict_opmodes / "mode.py").write_text(
        "from pathlib import Path\n"
        'Path(__file__).with_name("conflicting-mode-imported").touch()\n'
    )
    monkeypatch.syspath_prepend(str(conflict_dir))
    importlib.import_module("opmodes")

    robot_dir = tmp_path / "robot_project"
    robot_dir.mkdir()
    (robot_dir / "robot.py").write_text(
        "import wpilib\n\nclass Robot(wpilib.OpModeRobot):\n    pass\n"
    )
    expected_opmodes = robot_dir / "opmodes"
    expected_opmodes.mkdir()
    if has_init:
        (expected_opmodes / "__init__.py").write_text("")
    (expected_opmodes / "mode.py").write_text(
        "import wpilib\n\n"
        "@wpilib.autonomous\n"
        "class ExpectedMode(wpilib.OpMode):\n    pass\n"
    )
    monkeypatch.syspath_prepend(str(robot_dir))
    importlib.invalidate_caches()

    importlib.import_module("robot").Robot()

    assert not wsim.DriverStationSim.get_opmode_options()
    assert not (conflict_opmodes / "conflicting-mode-imported").exists()
    assert "opmodes.mode" in caplog.text
    assert str(expected_opmodes / "mode.py") in caplog.text


def test_post_resolution_origin_mismatch_does_not_register_foreign_class(
    monkeypatch, tmp_path, caplog
):
    package_dir, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "opmodes/raced_mode.py": """
                import wpilib

                @wpilib.autonomous(name="Expected")
                class ExpectedMode(wpilib.OpMode):
                    pass
            """,
        },
    )
    foreign_path = tmp_path / "foreign_raced_mode.py"
    foreign_path.write_text(
        "import wpilib\n\n"
        "@wpilib.teleop(name='Foreign')\n"
        "class ForeignMode(wpilib.OpMode):\n    pass\n"
    )
    real_import_module = opmode_impl.importlib.import_module

    def raced_import(module_name):
        if module_name != "samplebot.opmodes.raced_mode":
            return real_import_module(module_name)
        spec = importlib.util.spec_from_file_location(module_name, foreign_path)
        module = importlib.util.module_from_spec(spec)
        sys.modules[module_name] = module
        spec.loader.exec_module(module)
        return module

    monkeypatch.setattr(opmode_impl, "_has_origin_collision", lambda *args: False)
    monkeypatch.setattr(opmode_impl.importlib, "import_module", raced_import)

    robot_module.Robot()

    assert not wsim.DriverStationSim.get_opmode_options()
    assert "samplebot.opmodes.raced_mode" in caplog.text
    assert str(package_dir / "opmodes" / "raced_mode.py") in caplog.text


def test_rejected_scanned_origin_does_not_invalidate_explicit_class(
    monkeypatch, tmp_path
):
    conflict_dir = tmp_path / "conflict"
    conflict_opmodes = conflict_dir / "opmodes"
    conflict_opmodes.mkdir(parents=True)
    (conflict_opmodes / "__init__.py").write_text("")
    (conflict_opmodes / "base.py").write_text(
        "import wpilib\n\n"
        "@wpilib.autonomous(name='External Base')\n"
        "class BaseMode(wpilib.OpMode):\n    pass\n"
    )
    monkeypatch.syspath_prepend(str(conflict_dir))
    importlib.import_module("opmodes.base")

    robot_dir = tmp_path / "robot_project"
    robot_dir.mkdir()
    (robot_dir / "robot.py").write_text(
        "import wpilib\n\nclass Robot(wpilib.OpModeRobot):\n    pass\n"
    )
    expected_opmodes = robot_dir / "opmodes"
    expected_opmodes.mkdir()
    (expected_opmodes / "__init__.py").write_text("")
    (expected_opmodes / "base.py").write_text(
        "import wpilib\n\n"
        "@wpilib.autonomous(name='Scanned Base')\n"
        "class BaseMode(wpilib.OpMode):\n    pass\n"
    )
    (expected_opmodes / "child.py").write_text(
        "from opmodes.base import BaseMode\n\n" "class ChildMode(BaseMode):\n    pass\n"
    )
    monkeypatch.syspath_prepend(str(robot_dir))
    importlib.invalidate_caches()

    importlib.import_module("robot").Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "External Base"
    ]


def test_rejected_scanned_child_origin_does_not_invalidate_explicit_class(
    monkeypatch, tmp_path
):
    package_dir, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib
        import samplebot.external_base

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "external_base.py": """
                import wpilib

                @wpilib.autonomous(name="External Base")
                class ExternalBaseMode(wpilib.OpMode):
                    pass
            """,
            "opmodes/subclass_only.py": """
                from pathlib import Path
                from samplebot.external_base import ExternalBaseMode

                Path(__file__).with_name("colliding-child-imported").touch()

                class ExternalChildMode(ExternalBaseMode):
                    pass
            """,
        },
    )
    foreign_path = tmp_path / "foreign_subclass_only.py"
    foreign_path.write_text("")
    module_name = "samplebot.opmodes.subclass_only"
    spec = importlib.util.spec_from_file_location(module_name, foreign_path)
    foreign_module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = foreign_module
    spec.loader.exec_module(foreign_module)

    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "External Base"
    ]
    assert not (package_dir / "opmodes" / "colliding-child-imported").exists()


def test_automatic_publication_bypasses_python_override():
    @autonomous
    class AutomaticMode(OpMode):
        pass

    class OverrideRobot(OpModeRobot):
        def publish_opmodes(self):
            raise AssertionError("automatic publication called Python override")

    OverrideRobot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "AutomaticMode"
    ]


def test_opmode_robot_registers_explicitly_imported_decorated_opmode(
    monkeypatch, tmp_path
):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib as wpi
        import samplebot.external_mode

        class Robot(wpi.OpModeRobot):
            def __init__(self):
                super().__init__()
        """,
        {
            "external_mode.py": """
                from wpilib import PeriodicOpMode
                from wpilib.opmodes import teleop

                @teleop
                class ImportedMode(PeriodicOpMode):
                    pass
            """,
        },
    )

    robot_module.Robot()

    options = wsim.DriverStationSim.get_opmode_options()
    assert [option.name for option in options] == ["ImportedMode"]


def test_opmode_robot_deduplicates_explicit_and_discovered_opmode(
    monkeypatch, tmp_path
):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib
        import samplebot.opmodes.overlap_mode

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "opmodes/overlap_mode.py": """
                import wpilib

                @wpilib.teleop
                class OverlapMode(wpilib.OpMode):
                    pass
            """,
        },
    )

    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "OverlapMode"
    ]


def test_opmode_robot_prunes_failed_explicit_import(monkeypatch, tmp_path):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        try:
            import samplebot.failed_mode
        except RuntimeError:
            pass

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "failed_mode.py": """
                import wpilib

                @wpilib.autonomous
                class FailedMode(wpilib.OpMode):
                    pass

                raise RuntimeError("expected explicit import failure")
            """,
        },
    )

    assert "samplebot.failed_mode" not in sys.modules
    robot_module.Robot()

    assert not wsim.DriverStationSim.get_opmode_options()


def test_opmode_robot_replaces_reloaded_class_generation(monkeypatch, tmp_path):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib
        import samplebot.external_mode

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "external_mode.py": """
                import wpilib

                @wpilib.utility
                class ReloadedMode(wpilib.OpMode):
                    pass
            """,
        },
    )
    mode_module = sys.modules["samplebot.external_mode"]
    old_class = mode_module.ReloadedMode
    importlib.reload(mode_module)

    robot_module.Robot()

    options = wsim.DriverStationSim.get_opmode_options()
    assert [option.name for option in options] == ["ReloadedMode"]
    assert opmode_impl.decorated_opmodes() == (mode_module.ReloadedMode,)
    assert mode_module.ReloadedMode is not old_class


def test_opmode_robot_prunes_failed_reload_generation(monkeypatch, tmp_path):
    package_dir, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib
        import samplebot.external_mode

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "external_mode.py": """
                import wpilib

                @wpilib.utility
                class ReloadedMode(wpilib.OpMode):
                    pass
            """,
        },
    )
    mode_path = package_dir / "external_mode.py"
    mode_path.write_text(
        "import wpilib\n\n"
        "class ReloadedMode(wpilib.OpMode):\n    pass\n\n"
        'raise RuntimeError("expected reload failure")\n'
    )
    for bytecode_path in (package_dir / "__pycache__").glob("external_mode.*.pyc"):
        bytecode_path.unlink()
    importlib.invalidate_caches()
    mode_module = sys.modules["samplebot.external_mode"]
    with pytest.raises(RuntimeError, match="expected reload failure"):
        importlib.reload(mode_module)

    robot_module.Robot()

    assert not wsim.DriverStationSim.get_opmode_options()
    assert opmode_impl.decorated_opmodes() == ()
    assert "_wpilib_opmode_metadata" not in mode_module.ReloadedMode.__dict__


def test_opmode_robot_rejects_decorated_non_leaf(caplog):
    @autonomous
    class BaseMode(OpMode):
        pass

    class MiddleMode(BaseMode):
        pass

    class LeafMode(MiddleMode):
        pass

    class MinimalRobot(OpModeRobot):
        pass

    MinimalRobot()

    assert not wsim.DriverStationSim.get_opmode_options()
    assert "BaseMode" in caplog.text
    assert "MiddleMode" in caplog.text

    class CommonMode(OpMode):
        pass

    @utility
    class LeafUtilityMode(CommonMode):
        pass

    MinimalRobot()

    options = wsim.DriverStationSim.get_opmode_options()
    assert [option.name for option in options] == ["LeafUtilityMode"]


def test_opmode_robot_reports_invalid_metadata_per_class(monkeypatch, tmp_path, caplog):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "opmodes/metadata_modes.py": """
                import wpilib

                @wpilib.autonomous(name=1)
                class BadName(wpilib.OpMode):
                    pass

                @wpilib.teleop(group=None)
                class BadGroup(wpilib.OpMode):
                    pass

                @wpilib.utility(description=[])
                class BadDescription(wpilib.OpMode):
                    pass

                @wpilib.autonomous(text_color="white")
                class BadTextColor(wpilib.OpMode):
                    pass

                @wpilib.teleop(background_color=object())
                class BadBackgroundColor(wpilib.OpMode):
                    pass

                @wpilib.utility(name="Good")
                class GoodMode(wpilib.OpMode):
                    pass
            """,
        },
    )

    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "Good"
    ]
    for class_name, field_name in (
        ("BadName", "name"),
        ("BadGroup", "group"),
        ("BadDescription", "description"),
        ("BadTextColor", "text_color"),
        ("BadBackgroundColor", "background_color"),
    ):
        assert f"samplebot.opmodes.metadata_modes.{class_name}" in caplog.text
        assert field_name in caplog.text


@pytest.mark.parametrize("overridden_method", ["add_opmode", "add_opmode_factory"])
@pytest.mark.parametrize("colored", [False, True])
def test_opmode_robot_automatic_registration_bypasses_overrides(
    monkeypatch, caplog, overridden_method, colored
):
    @utility(
        text_color=Color.WHITE if colored else None,
        background_color=Color.BLACK if colored else None,
    )
    class AutomaticMode(OpMode):
        pass

    class ManualMode(OpMode):
        pass

    class Robot(OpModeRobot):
        def __init__(self):
            super().__init__()
            self.registered = []

    original = getattr(OpModeRobot, overridden_method)

    def register(self, *args, **kwargs):
        self.registered.append(args)
        return original(self, *args, **kwargs)

    monkeypatch.setattr(Robot, overridden_method, register)

    robot = Robot()

    options = wsim.DriverStationSim.get_opmode_options()
    assert [option.name for option in options] == ["AutomaticMode"]
    assert options[0].text_color == (0xFFFFFF if colored else -1)
    assert options[0].background_color == (0x000000 if colored else -1)
    assert robot.registered == []
    assert "Could not register" not in caplog.text

    # Explicit registration still dispatches through the user's overrides.
    robot.add_opmode(ManualMode, RobotMode.UTILITY, "ManualMode")
    robot.publish_opmodes()
    assert len(robot.registered) == 1
    assert sorted(
        option.name for option in wsim.DriverStationSim.get_opmode_options()
    ) == ["AutomaticMode", "ManualMode"]


def test_opmode_robot_continues_after_per_class_registration_failure(
    monkeypatch, tmp_path, caplog
):
    original = OpModeRobotBase.add_opmode_factory

    def register(robot, mode, name, *args):
        if name == "BadMode":
            raise RuntimeError("expected registration failure")
        return original(robot, mode, name, *args)

    monkeypatch.setattr(OpModeRobotBase, "add_opmode_factory", register)
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "opmodes/modes.py": """
                import wpilib

                @wpilib.autonomous
                class BadMode(wpilib.OpMode):
                    pass

                @wpilib.teleop
                class GoodMode(wpilib.OpMode):
                    pass
            """,
        },
    )

    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "GoodMode"
    ]
    assert "samplebot.opmodes.modes.BadMode" in caplog.text
    assert "expected registration failure" in caplog.text


def test_opmode_robot_registers_explicit_base_with_unimported_scanned_subclass(
    monkeypatch, tmp_path, caplog
):
    package_dir, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib
        import samplebot.external_base

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "external_base.py": """
                import wpilib

                @wpilib.autonomous
                class ExternalBaseMode(wpilib.OpMode):
                    pass
            """,
            "opmodes/subclass_only.py": """
                from pathlib import Path
                from samplebot.external_base import ExternalBaseMode

                Path(__file__).with_name("external-subclass-imported").touch()

                class ExternalChildMode(ExternalBaseMode):
                    pass
            """,
        },
    )

    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "ExternalBaseMode"
    ]
    assert not (package_dir / "opmodes" / "external-subclass-imported").exists()
    assert "must not be subclassed" not in caplog.text


@pytest.mark.parametrize("import_child", [False, True])
@pytest.mark.parametrize(
    "child_source, registers",
    [
        pytest.param("Base = object\nclass Child(Base): pass", True, id="assignment"),
        pytest.param(
            "Base = Other = object\nclass Child(Base): pass", True, id="chained"
        ),
        pytest.param(
            "Base, Other = object, object\nclass Child(Base): pass",
            True,
            id="unpacking",
        ),
        pytest.param(
            "[Base, *Other] = [object, object]\nclass Child(Base): pass",
            True,
            id="starred-unpacking",
        ),
        pytest.param(
            "Base: type = object\nclass Child(Base): pass", True, id="annotated"
        ),
        pytest.param(
            "class Replacement:\n    def __ror__(self, other): return object\n"
            "Base |= Replacement()\nclass Child(Base): pass",
            True,
            id="augmented",
        ),
        pytest.param(
            "(Base := object)\nclass Child(Base): pass", True, id="named-expression"
        ),
        pytest.param(
            "class Container:\n    Base = object\n    class Child(Base): pass",
            True,
            id="class-scope-shadow",
        ),
        pytest.param(
            "def make_child():\n    Base = object\n    class Child(Base): pass",
            True,
            id="function-scope-shadow",
        ),
        pytest.param(
            "from types import SimpleNamespace\n"
            "import samplebot.external_base as modes\n"
            "modes = SimpleNamespace(Base=object)\nclass Child(modes.Base): pass",
            True,
            id="module-alias",
        ),
        pytest.param(
            "for Base in [object]: pass\nclass Child(Base): pass",
            True,
            id="for-target",
        ),
        pytest.param(
            "from contextlib import nullcontext\n"
            "with nullcontext(object) as Base:\n    class Child(Base): pass",
            True,
            id="with-target",
        ),
        pytest.param(
            "def make_child(Base):\n    class Child(Base): pass\n    return Child\n"
            "Child = make_child(object)",
            True,
            id="parameter",
        ),
        pytest.param(
            "if False: Base = object\nclass Child(Base): pass",
            False,
            id="false-branch",
        ),
        pytest.param(
            "try: pass\nexcept Exception: Base = object\nclass Child(Base): pass",
            False,
            id="exception-path",
        ),
        pytest.param("class Child(Base): pass", False, id="unchanged-import"),
        pytest.param(
            "class Child(Base): pass\nBase = object", False, id="later-assignment"
        ),
        pytest.param(
            "Base: type\nclass Child(Base): pass", False, id="annotation-only"
        ),
        pytest.param(
            "def unrelated():\n    Base = object\nclass Child(Base): pass",
            False,
            id="unrelated-scope",
        ),
        pytest.param(
            "unused = lambda: (Base := object)\nclass Child(Base): pass",
            False,
            id="unrelated-lambda-scope",
        ),
        pytest.param(
            "unused = lambda arg=(Base := object): arg\nclass Child(Base): pass",
            True,
            id="lambda-default-in-outer-scope",
        ),
        pytest.param(
            "Base = object\nfrom samplebot.external_base import Base\n"
            "class Child(Base): pass",
            False,
            id="reimport",
        ),
    ],
)
def test_opmode_robot_checks_runtime_subclasses(
    monkeypatch, tmp_path, caplog, child_source, registers, import_child
):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib
        import samplebot.external_base

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "external_base.py": """
                import wpilib

                @wpilib.autonomous
                class Base(wpilib.OpMode):
                    pass
            """,
            "opmodes/subclass_only.py": (
                "from samplebot.external_base import Base\n" + child_source + "\n"
            ),
        },
    )

    if import_child:
        importlib.import_module("samplebot.opmodes.subclass_only")

    robot_module.Robot()

    should_register = not import_child or registers
    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == (
        ["Base"] if should_register else []
    )
    assert ("must not be subclassed" in caplog.text) is not should_register
    assert ("samplebot.opmodes.subclass_only" in sys.modules) is import_child


@pytest.mark.parametrize("import_child", [False, True])
@pytest.mark.parametrize("import_name", ["Base", "*"])
def test_opmode_robot_checks_subclasses_only_in_imported_modules(
    monkeypatch, tmp_path, caplog, import_child, import_name
):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "opmodes/base_mode.py": """
                import wpilib

                @wpilib.autonomous
                class Base(wpilib.OpMode):
                    pass
            """,
            "opmodes/subclass_only.py": (
                f"from .base_mode import {import_name}\nclass Child(Base): pass\n"
            ),
        },
    )
    if import_child:
        importlib.import_module("samplebot.opmodes.subclass_only")

    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == (
        [] if import_child else ["Base"]
    )
    assert ("must not be subclassed" in caplog.text) is import_child
    assert ("samplebot.opmodes.subclass_only" in sys.modules) is import_child


def test_opmode_robot_continues_after_candidate_import_failure(
    monkeypatch, tmp_path, caplog
):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib as wpi

        class Robot(wpi.OpModeRobot):
            pass
        """,
        {
            "opmodes/bad_mode.py": """
                from wpilib import PeriodicOpMode, teleop

                @teleop
                class BadMode(PeriodicOpMode):
                    pass

                raise RuntimeError("expected candidate import failure")
            """,
            "opmodes/good_mode.py": """
                from wpilib import PeriodicOpMode, utility

                @utility
                class GoodMode(PeriodicOpMode):
                    pass
            """,
        },
    )

    robot_module.Robot()

    options = wsim.DriverStationSim.get_opmode_options()
    assert {option.name for option in options} == {"GoodMode"}
    assert "bad_mode" in caplog.text
    assert "expected candidate import failure" in caplog.text


def test_opmode_robot_retains_transitively_imported_candidate_after_failure(
    monkeypatch, tmp_path, caplog
):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib as wpi

        class Robot(wpi.OpModeRobot):
            pass
        """,
        {
            "opmodes/bad_mode.py": """
                from samplebot.opmodes import good_mode
                from wpilib import PeriodicOpMode, teleop

                @teleop
                class BadMode(PeriodicOpMode):
                    pass

                raise RuntimeError("expected candidate import failure")
            """,
            "opmodes/good_mode.py": """
                from wpilib import PeriodicOpMode, utility

                @utility
                class GoodMode(PeriodicOpMode):
                    pass
            """,
        },
    )

    robot_module.Robot()

    options = wsim.DriverStationSim.get_opmode_options()
    assert {option.name for option in options} == {"GoodMode"}
    assert "bad_mode" in caplog.text
    assert "expected candidate import failure" in caplog.text


def test_opmode_robot_rolls_back_nested_failed_candidates(
    monkeypatch, tmp_path, caplog
):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib as wpi

        class Robot(wpi.OpModeRobot):
            pass
        """,
        {
            "opmodes/bad_a.py": """
                from samplebot.opmodes import bad_b
                from wpilib import PeriodicOpMode, teleop

                @teleop
                class BadAMode(PeriodicOpMode):
                    pass
            """,
            "opmodes/bad_b.py": """
                from wpilib import PeriodicOpMode, utility

                @utility
                class BadBMode(PeriodicOpMode):
                    pass

                raise RuntimeError("expected nested candidate import failure")
            """,
        },
    )

    robot_module.Robot()

    assert not wsim.DriverStationSim.get_opmode_options()
    assert "bad_a" in caplog.text
    assert "Could not import OpMode module samplebot.opmodes.bad_b" in caplog.text
    assert "expected nested candidate import failure" in caplog.text


def test_opmode_robot_discovers_encoded_python_source(monkeypatch, tmp_path):
    package_dir, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {"opmodes/encoded_mode.py": ""},
    )
    (package_dir / "opmodes" / "encoded_mode.py").write_bytes(
        b"# -*- coding: latin-1 -*-\n"
        b"import wpilib\n\n"
        b"@wpilib.autonomous(name='Caf\xe9')\n"
        b"class EncodedMode(wpilib.OpMode):\n    pass\n"
    )

    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "Caf\N{LATIN SMALL LETTER E WITH ACUTE}"
    ]


def test_opmode_robot_continues_after_source_decode_failure(
    monkeypatch, tmp_path, caplog
):
    package_dir, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "opmodes/bad_encoding.py": "",
            "opmodes/good_mode.py": """
                import wpilib

                @wpilib.utility
                class GoodMode(wpilib.OpMode):
                    pass
            """,
        },
    )
    (package_dir / "opmodes" / "bad_encoding.py").write_bytes(
        b"# coding: utf-8\n\xff\n"
    )

    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "GoodMode"
    ]
    assert "bad_encoding.py" in caplog.text


def test_opmode_robot_continues_after_source_read_failure(
    monkeypatch, tmp_path, caplog
):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib

        class Robot(wpilib.OpModeRobot):
            pass
        """,
        {
            "opmodes/unreadable.py": "",
            "opmodes/good_mode.py": """
                import wpilib

                @wpilib.utility
                class GoodMode(wpilib.OpMode):
                    pass
            """,
        },
    )
    real_open = opmode_impl.tokenize.open

    def open_source(filename):
        if Path(filename).name == "unreadable.py":
            raise OSError("expected source read failure")
        return real_open(filename)

    monkeypatch.setattr(opmode_impl.tokenize, "open", open_source)
    robot_module.Robot()

    assert [option.name for option in wsim.DriverStationSim.get_opmode_options()] == [
        "GoodMode"
    ]
    assert "unreadable.py" in caplog.text
    assert "expected source read failure" in caplog.text


def test_opmode_robot_continues_after_candidate_parse_failure(
    monkeypatch, tmp_path, caplog
):
    _, robot_module = import_robot_package(
        monkeypatch,
        tmp_path,
        """
        import wpilib as wpi

        class Robot(wpi.OpModeRobot):
            pass
        """,
        {
            "opmodes/bad_syntax.py": """
                from wpilib import PeriodicOpMode, teleop

                @teleop
                class BrokenMode(PeriodicOpMode)
                    pass
            """,
            "opmodes/good_mode.py": """
                from wpilib import PeriodicOpMode, utility

                @utility
                class GoodMode(PeriodicOpMode):
                    pass
            """,
        },
    )

    robot_module.Robot()

    options = wsim.DriverStationSim.get_opmode_options()
    assert {option.name for option in options} == {"GoodMode"}
    assert "bad_syntax.py" in caplog.text
    assert "expected ':'" in caplog.text


class MockOpMode(OpMode):
    def __init__(self):
        super().__init__()
        self.disabled_periodic_count = 0
        self.opmode_run_count = 0
        self.opmode_stop_count = 0

    def disabled_periodic(self):
        self.disabled_periodic_count += 1

    def opmode_run(self, opmode_id: int):
        self.opmode_run_count += 1

    def opmode_stop(self):
        self.opmode_stop_count += 1


class OneArgOpMode(OpMode):
    def __init__(self, robot):
        super().__init__()

    def opmode_run(self, opmode_id: int):
        pass

    def opmode_stop(self):
        pass


class MockRobot(OpModeRobot):
    def __init__(self):
        super().__init__()
        self.driver_station_connected_count = 0
        self.none_periodic_count = 0
        self.periodic_count = 0

    def driver_station_connected(self):
        self.driver_station_connected_count += 1

    def none_periodic(self):
        self.none_periodic_count += 1

    def robot_periodic(self):
        self.periodic_count += 1


def test_robot_loop_refreshes_getter_backed_tunables():
    robot = MockRobot()
    backend = tunables.MockTunableBackend()
    value = [1]
    robot_thread = threading.Thread(target=robot.start_competition, daemon=True)

    tunables.TunableRegistry.reset()
    try:
        tunables.TunableRegistry.register_backend("", backend)
        tunables.get_table().publish_int(
            "getter",
            lambda: value[0],
            lambda tuned: value.__setitem__(0, tuned),
        )

        value[0] = 7
        robot_thread.start()
        wsim.wait_for_program_start()
        wsim.DriverStationSim.set_enabled(False)
        wsim.DriverStationSim.notify_new_data()
        wsim.step_timing(0.02)

        assert backend.get_value("/getter") == 7
    finally:
        robot.end_competition()
        if robot_thread.is_alive():
            robot_thread.join()
        tunables.TunableRegistry.reset()


@pytest.fixture(autouse=True)
def sim_timing_setup():
    wsim.pause_timing()
    wsim.set_program_started(False)
    yield
    wsim.resume_timing()
    RobotState.clear_opmodes()


def test_add_opmode():
    class MyMockRobot(MockRobot):
        def __init__(self):
            super().__init__()
            self.add_opmode(
                MockOpMode,
                RobotMode.AUTONOMOUS,
                "NoArgOpMode-Auto",
                "Group",
                "Description",
                Color.WHITE,
                Color.BLACK,
            )
            self.add_opmode(
                OneArgOpMode,
                RobotMode.UTILITY,
                "OneArgOpMode-Utility",
                "Group",
                "Description",
                Color.WHITE,
                Color.BLACK,
            )
            self.add_opmode(MockOpMode, RobotMode.TELEOPERATED, "NoArgOpMode")
            self.add_opmode(OneArgOpMode, RobotMode.TELEOPERATED, "OneArgOpMode")
            self.publish_opmodes()

    robot = MyMockRobot()
    options = wsim.DriverStationSim.get_opmode_options()

    assert len(options) == 4

    opt_map = {opt.name: opt for opt in options}

    auto_opt = opt_map["NoArgOpMode-Auto"]
    assert auto_opt.group == "Group"
    assert auto_opt.description == "Description"
    assert auto_opt.text_color == 0xFFFFFF
    assert auto_opt.background_color == 0x000000

    tele_opt = opt_map["NoArgOpMode"]
    assert tele_opt.group == ""
    assert tele_opt.description == ""
    assert tele_opt.text_color == -1
    assert tele_opt.background_color == -1


def test_clear_opmodes():
    class MyMockRobot(MockRobot):
        def __init__(self):
            super().__init__()
            self.add_opmode(MockOpMode, RobotMode.TELEOPERATED, "NoArgOpMode")
            self.publish_opmodes()

    robot = MyMockRobot()
    robot.clear_opmodes()

    options = wsim.DriverStationSim.get_opmode_options()
    assert len(options) == 0


def test_remove_opmode():
    class MyMockRobot(MockRobot):
        def __init__(self):
            super().__init__()
            self.add_opmode(MockOpMode, RobotMode.TELEOPERATED, "NoArgOpMode")
            self.add_opmode(OneArgOpMode, RobotMode.TELEOPERATED, "OneArgOpMode")
            self.publish_opmodes()

    robot = MyMockRobot()
    robot.remove_opmode(RobotMode.TELEOPERATED, "NoArgOpMode")
    robot.publish_opmodes()

    options = wsim.DriverStationSim.get_opmode_options()
    assert len(options) == 1
    assert options[0].name == "OneArgOpMode"


@pytest.fixture
def periodic_robot_test_fixture():
    class MyMockRobot(MockRobot):
        def __init__(self):
            super().__init__()
            self.add_opmode(MockOpMode, RobotMode.TELEOPERATED, "NoArgOpMode")
            self.publish_opmodes()

    robot = MyMockRobot()

    robot_thread = threading.Thread(target=robot.start_competition)
    robot_thread.start()

    yield robot

    robot.end_competition()
    robot_thread.join()


# @pytest.mark.xfail(reason="wpilib bug")
def test_none_periodic(periodic_robot_test_fixture):
    robot = periodic_robot_test_fixture

    wsim.wait_for_program_start()

    # Time step to get periodic calls on 20 ms robot loop
    wsim.step_timing(0.110)

    assert robot.none_periodic_count == 5


def test_robot_periodic(periodic_robot_test_fixture):
    PERIOD = 0.020  # 20 ms

    robot = periodic_robot_test_fixture

    wsim.wait_for_program_start()

    # robot_periodic should be called regardless of state
    assert robot.periodic_count == 0

    # Time step to get periodic calls on 20 ms robot loop
    wsim.step_timing(PERIOD)
    assert robot.periodic_count == 1

    # Additional time steps should continue calling robot_periodic
    wsim.step_timing(PERIOD)
    assert robot.periodic_count == 2
