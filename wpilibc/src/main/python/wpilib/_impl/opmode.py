from __future__ import annotations

import ast
from collections.abc import Callable
from dataclasses import dataclass
from functools import partial
import importlib
from importlib.machinery import PathFinder
import inspect
from pathlib import Path
import sys
import tokenize
from typing import Any, TypeVar

from hal import RobotMode
from wpiutil import Color
from wpilib._wpilib import OpMode, OpModeRobotBase, RobotState

from .report_error import report_error


@dataclass(frozen=True)
class OpModeMetadata:
    mode: RobotMode
    name: str
    group: str
    description: str
    text_color: Color | None
    background_color: Color | None


_decorated_opmodes: list[type[OpMode]] = []
_OpModeT = TypeVar("_OpModeT", bound=OpMode)


def attach_metadata(
    cls: type[_OpModeT],
    *,
    mode: RobotMode,
    name: str = "",
    group: str = "",
    description: str = "",
    text_color: Color | None = None,
    background_color: Color | None = None,
) -> type[_OpModeT]:
    if not inspect.isclass(cls) or not issubclass(cls, OpMode):
        raise TypeError("opmode decorator must be applied to an OpMode subclass")
    if "_wpilib_opmode_metadata" in cls.__dict__:
        raise ValueError("multiple opmode decorators are not allowed")

    cls._wpilib_opmode_metadata = OpModeMetadata(
        mode,
        cls.__name__ if name == "" else name,
        group,
        description,
        text_color,
        background_color,
    )
    identity = (cls.__module__, cls.__qualname__)
    if "<locals>" not in cls.__qualname__:
        _decorated_opmodes[:] = [
            decorated_cls
            for decorated_cls in _decorated_opmodes
            if (decorated_cls.__module__, decorated_cls.__qualname__) != identity
        ]
    _decorated_opmodes.append(cls)
    return cls


def _is_current_class_generation(cls: type[OpMode]) -> bool:
    module = sys.modules.get(cls.__module__)
    if module is None:
        return False
    parts = cls.__qualname__.split(".")
    if "<locals>" in parts:
        return True

    current = module.__dict__.get(parts[0])
    for part in parts[1:]:
        namespace = getattr(current, "__dict__", None)
        if namespace is None:
            return False
        current = namespace.get(part)
    return current is cls


def decorated_opmodes() -> tuple[type[OpMode], ...]:
    _decorated_opmodes[:] = [
        cls for cls in _decorated_opmodes if _is_current_class_generation(cls)
    ]
    return tuple(_decorated_opmodes)


def _tree_has_opmode_decorator(tree: ast.Module) -> bool:
    decorator_names = {"autonomous", "teleop", "utility"}
    imported_decorators: set[str] = set()
    module_aliases: set[tuple[str, ...]] = set()

    for node in ast.walk(tree):
        if isinstance(node, ast.Import):
            for imported in node.names:
                if imported.name not in {"wpilib", "wpilib.opmodes"}:
                    continue
                if imported.asname is not None:
                    module_aliases.add((imported.asname,))
                else:
                    module_aliases.add(tuple(imported.name.split(".")))
                    # An unaliased dotted import also binds the root package.
                    module_aliases.add((imported.name.split(".")[0],))
        elif isinstance(node, ast.ImportFrom) and node.module in {
            "wpilib",
            "wpilib.opmodes",
        }:
            for imported in node.names:
                if node.module == "wpilib" and imported.name == "opmodes":
                    module_aliases.add((imported.asname or imported.name,))
                elif imported.name == "*":
                    imported_decorators.update(decorator_names)
                elif imported.name in decorator_names:
                    imported_decorators.add(imported.asname or imported.name)

    def attribute_path(node: ast.expr) -> tuple[str, ...] | None:
        parts: list[str] = []
        while isinstance(node, ast.Attribute):
            parts.append(node.attr)
            node = node.value
        if not isinstance(node, ast.Name):
            return None
        parts.append(node.id)
        return tuple(reversed(parts))

    for node in ast.walk(tree):
        if not isinstance(node, ast.ClassDef):
            continue
        for decorator in node.decorator_list:
            if isinstance(decorator, ast.Call):
                decorator = decorator.func
            if isinstance(decorator, ast.Name):
                if decorator.id in imported_decorators:
                    return True
            elif isinstance(decorator, ast.Attribute):
                path = attribute_path(decorator)
                if (
                    path is not None
                    and path[-1] in decorator_names
                    and path[:-1] in module_aliases
                ):
                    return True
    return False


def _has_opmode_decorator(source: str, filename: str) -> bool:
    return _tree_has_opmode_decorator(ast.parse(source, filename=filename))


@dataclass(frozen=True)
class _ScannedModule:
    name: str
    path: Path
    tree: ast.Module


def _scan_modules(package_dir: Path, package_name: str) -> list[_ScannedModule]:
    modules: list[_ScannedModule] = []
    for source_path in sorted(package_dir.rglob("*.py")):
        try:
            with tokenize.open(source_path) as source_file:
                source = source_file.read()
            tree = ast.parse(source, filename=str(source_path))
        except (OSError, UnicodeError, SyntaxError) as exc:
            report_error(f"Could not read or parse OpMode module {source_path}: {exc}")
            continue

        relative_path = source_path.relative_to(package_dir)
        is_package = relative_path.name == "__init__.py"
        module_parts = (
            relative_path.parent.parts
            if is_package
            else relative_path.with_suffix("").parts
        )
        modules.append(
            _ScannedModule(
                ".".join((package_name, *module_parts)),
                source_path,
                tree,
            )
        )
    return modules


def _path_from_origin(origin: str | None) -> Path | None:
    if origin is None or origin in {"built-in", "frozen"}:
        return None
    return Path(origin).resolve()


def _loaded_origin(module_name: str) -> Path | None:
    module = sys.modules.get(module_name)
    if module is None:
        return None
    origin = getattr(getattr(module, "__spec__", None), "origin", None)
    return _path_from_origin(origin or getattr(module, "__file__", None))


def _resolved_origin(module_name: str) -> Path | None:
    search_path = None
    spec = None
    parts = module_name.split(".")
    for index in range(len(parts)):
        current_name = ".".join(parts[: index + 1])
        loaded = sys.modules.get(current_name)
        if loaded is not None:
            spec = getattr(loaded, "__spec__", None)
        else:
            try:
                spec = PathFinder.find_spec(current_name, search_path)
            except KeyError:
                namespace_paths = [
                    str(Path(entry) / parts[index])
                    for entry in (search_path or sys.path)
                    if (Path(entry) / parts[index]).is_dir()
                ]
                if not namespace_paths or index == len(parts) - 1:
                    return None
                search_path = namespace_paths
                continue
        if spec is None:
            return None
        search_locations = spec.submodule_search_locations
        if search_locations is None:
            search_path = None
        else:
            search_path = list(getattr(search_locations, "_path", search_locations))
    return _path_from_origin(spec.origin)


def _expected_origins(
    scanned_module: _ScannedModule, package_dir: Path, package_name: str
) -> dict[str, Path]:
    expected: dict[str, Path] = {}
    relative = scanned_module.path.relative_to(package_dir)
    directories = relative.parent.parts
    for index in range(len(directories) + 1):
        package_path = package_dir.joinpath(*directories[:index])
        package_init = package_path / "__init__.py"
        if package_init.is_file():
            module_name = ".".join((package_name, *directories[:index]))
            expected[module_name] = package_init.resolve()
    expected[scanned_module.name] = scanned_module.path.resolve()
    return expected


def _has_origin_collision(
    scanned_module: _ScannedModule, package_dir: Path, package_name: str
) -> bool:
    expected_origins = _expected_origins(scanned_module, package_dir, package_name)
    for module_name, expected_path in expected_origins.items():
        if module_name not in sys.modules:
            continue
        actual_path = _loaded_origin(module_name)
        if actual_path != expected_path:
            report_error(
                f"Refusing to import OpMode module {scanned_module.name} from "
                f"scanned path {scanned_module.path}: preloaded module "
                f"{module_name} has origin {actual_path}"
            )
            return True

    resolved_path = _resolved_origin(scanned_module.name)
    if resolved_path is not None and resolved_path != scanned_module.path.resolve():
        report_error(
            f"Refusing to import OpMode module {scanned_module.name} from scanned "
            f"path {scanned_module.path}: module resolves to {resolved_path}"
        )
        return True
    return False


def _import_candidates(
    modules: list[_ScannedModule], package_dir: Path, package_name: str
) -> None:
    for scanned_module in modules:
        if not _tree_has_opmode_decorator(scanned_module.tree):
            continue
        module_name = scanned_module.name
        if _has_origin_collision(scanned_module, package_dir, package_name):
            continue
        registry_before = list(_decorated_opmodes)
        origin_mismatch = False
        try:
            imported_module = importlib.import_module(module_name)
            if _loaded_origin(module_name) != scanned_module.path.resolve():
                origin_mismatch = True
                raise ImportError(
                    f"imported origin {getattr(imported_module, '__file__', None)} "
                    f"does not match scanned path {scanned_module.path}"
                )
        except Exception as exc:
            new_loaded_classes = [
                cls
                for cls in _decorated_opmodes
                if cls not in registry_before
                and cls.__module__ in sys.modules
                and not (origin_mismatch and cls.__module__ == module_name)
            ]
            _decorated_opmodes[:] = registry_before + new_loaded_classes
            report_error(
                f"Could not import OpMode module {module_name}: {exc}",
                print_trace=True,
            )


def register_opmode(
    robot: OpModeRobotBase,
    opmode_cls: type,
    mode: RobotMode,
    name: str,
    group: str | None = None,
    description: str | None = None,
    text_color: Color | None = None,
    background_color: Color | None = None,
    *,
    add_factory: Callable[..., None] | None = None,
) -> None:
    # Discovery runs before the subclass constructor has finished. Use the
    # native registration method unless an explicit manual call supplies its
    # own factory registration method.
    if add_factory is None:
        add_factory = partial(OpModeRobotBase.add_opmode_factory, robot)

    def make_opmode_instance() -> OpMode:
        # Try to instantiate with robot argument first.
        try:
            return opmode_cls(robot)
        except TypeError:
            return opmode_cls()

    if text_color is None or background_color is None:
        add_factory(mode, name, group or "", description or "", make_opmode_instance)
    else:
        add_factory(
            mode,
            name,
            group or "",
            description or "",
            text_color,
            background_color,
            make_opmode_instance,
        )


def _find_subclass(cls: type[OpMode]) -> str | None:
    subclasses = cls.__subclasses__()
    if subclasses:
        subclass = subclasses[0]
        return f"{subclass.__module__}.{subclass.__qualname__}"
    return None


def _invalid_metadata(metadata: OpModeMetadata) -> str | None:
    for field_name in ("name", "group", "description"):
        if not isinstance(getattr(metadata, field_name), str):
            return f"{field_name} must be a string"
    for field_name in ("text_color", "background_color"):
        value = getattr(metadata, field_name)
        if value is not None and not isinstance(value, Color):
            return f"{field_name} must be a wpiutil.Color or None"
    return None


def discover_and_register(robot: OpModeRobotBase) -> None:
    robot_source = Path(inspect.getfile(type(robot)))
    package_dir = robot_source.parent / "opmodes"
    if package_dir.is_dir():
        robot_module = type(robot).__module__
        robot_package = (
            robot_module
            if robot_source.name == "__init__.py"
            else robot_module.rpartition(".")[0]
        )
        package_name = f"{robot_package}.opmodes" if robot_package else "opmodes"
        scanned_modules = _scan_modules(package_dir, package_name)
        _import_candidates(scanned_modules, package_dir, package_name)

    registered: set[tuple[type[OpMode], RobotMode]] = set()
    registrations: list[tuple[type[OpMode], OpModeMetadata]] = []
    for cls in decorated_opmodes():
        metadata = cls.__dict__.get("_wpilib_opmode_metadata")
        if metadata is None or not issubclass(cls, OpMode):
            continue
        key = (cls, metadata.mode)
        if key in registered:
            continue
        registered.add(key)
        class_name = f"{cls.__module__}.{cls.__qualname__}"
        metadata_error = _invalid_metadata(metadata)
        if metadata_error is not None:
            report_error(f"Invalid OpMode metadata for {class_name}: {metadata_error}")
            continue
        subclass = _find_subclass(cls)
        if subclass is not None:
            report_error(
                f"Decorated OpMode {cls.__module__}.{cls.__qualname__} must not be "
                f"subclassed; found subclass {subclass}"
            )
            continue
        registrations.append((cls, metadata))

    for cls, metadata in registrations:
        try:
            register_opmode(
                robot,
                cls,
                metadata.mode,
                metadata.name,
                metadata.group,
                metadata.description,
                metadata.text_color,
                metadata.background_color,
            )
        except Exception as exc:
            report_error(
                f"Could not register decorated OpMode "
                f"{cls.__module__}.{cls.__qualname__}: {exc}",
                print_trace=True,
            )
    RobotState.publish_opmodes()
