import functools
import os
import pathlib
import platform
import sys
import typing as T

import pkgconf
import tomli
from packaging.markers import Marker

from shared.bazel.rules.robotpy.hatchlib_native_port.config import PcFileConfig
from shared.bazel.rules.robotpy.hatchlib_native_port.validate import parse_input
from shared.bazel.rules.robotpy.hack_pkgcfgs import hack_pkgconfig

# Port of https://github.com/robotpy/hatch-nativelib/blob/main/src/hatch_nativelib/plugin.py

INITPY_VARNAME = "pkgconf_pypi_initpy"

platform_sys = platform.system()
is_windows = platform_sys == "Windows"
is_macos = platform_sys == "Darwin"


class NativelibHook:
    def __init__(self, output_pcfile, output_libinit, config, metadata):
        self.output_pcfile = output_pcfile
        self.output_libinit = output_libinit

        self.config = config
        self.root_pth = output_pcfile.parent.parent.parent
        self.metadata = metadata

    def initialize(self):
        for pcfg in self._pcfiles:
            self._generate_pcfile(pcfg, {})

    def _get_pkg_from_path(self, path: pathlib.Path) -> str:
        rel = path.relative_to(self.root_pth)
        return str(rel).replace("/", ".").replace("\\", ".")

    def _generate_pcfile(
        self, pcfg: PcFileConfig, build_data: T.Dict[str, T.Any]
    ) -> pathlib.Path:

        pcfile_rel = pcfg.get_pc_path()
        pcfile = self.output_pcfile
        prefix_rel = pcfile_rel.parent
        prefix_path = pcfile.parent

        prefix = "${pcfiledir}"

        # variables first
        variables = {}
        variables["prefix"] = prefix

        if pcfg.includedir:
            increl = pathlib.PurePosixPath(pcfg.includedir).relative_to(
                prefix_rel.as_posix()
            )
            variables["includedir"] = f"${{prefix}}/{increl}"

        if pcfg.shared_libraries:
            if pcfg.libdir:
                librel = pathlib.PurePosixPath(pcfg.libdir).relative_to(
                    prefix_rel.as_posix()
                )
                variables["libdir"] = f"${{prefix}}/{librel}"
            else:
                variables["libdir"] = "${prefix}"

        if pcfg.variables:
            for n in ("prefix", "includedir", "libdir", INITPY_VARNAME):
                if n in pcfg.variables:
                    raise ValueError(f"variables may not contain {n}")

            variables.update(variables)

        # If there are libraries, generate _init_NAME.py for each
        if pcfg.shared_libraries:
            package = self._get_pkg_from_path(prefix_path)
            variables[INITPY_VARNAME] = f"{package}.{pcfg.get_init_module()}"
            self._generate_init_py(pcfg, prefix_path, build_data)

            # .. not documented but it works?
            # eps = self.metadata.core.entry_points.setdefault("pkg_config", {})
            # eps[pcfg.get_name()] = package

        contents = [f"{k}={v}" for k, v in variables.items()]
        contents.append("")

        description = pcfg.description
        if description is None:
            description = self.metadata["description"]

        if not description:
            raise ValueError(
                f"tool.hatch.build.hooks.nativelib.pcfile: description not provided for {pcfg.get_name()}"
            )

        contents += [
            f"Name: {pcfg.get_name()}",
            f"Description: {description}",
        ]

        version = pcfg.version or self.metadata["version"]
        if version:
            contents.append(f"Version: {version}")

        libs = []
        if pcfg.shared_libraries:
            libs.append("-L${libdir}")
            libs.extend(f"-l{lib}" for lib in pcfg.shared_libraries)

        cflags = []
        if pcfg.includedir:
            cflags.append("-I${includedir}")

        if pcfg.extra_cflags:
            cflags.append(pcfg.extra_cflags)

        if pcfg.requires:
            contents.append(f"Requires: {' '.join(pcfg.requires)}")

        if pcfg.requires_private:
            contents.append(f"Requires.private: {' '.join(pcfg.requires_private)}")

        if libs:
            contents.append(f"Libs: {' '.join(libs)}")

        if pcfg.libs_private:
            contents.append(f"Libs.private: {pcfg.libs_private}")

        if cflags:
            contents.append(f"Cflags: {' '.join(cflags)}")

        content = ("\n".join(contents)) + "\n"
        pcfile.parent.mkdir(parents=True, exist_ok=True)
        with open(pcfile, "w") as fp:
            fp.write(content)

        return pcfile

    def _generate_init_py(
        self,
        pcfg: PcFileConfig,
        prefix_path: pathlib.Path,
        build_data: T.Dict[str, T.Any],
    ):
        libinit_py_rel = pcfg.get_init_module_path()
        self.root_pth / libinit_py_rel

        libdir = prefix_path
        if pcfg.libdir:
            libdir = self.root_pth / pathlib.PurePosixPath(pcfg.libdir)
            libdir = pathlib.Path(str(libdir).replace("src/", "").replace("src\\", ""))

        lib_paths = []
        assert pcfg.shared_libraries is not None
        for lib in pcfg.shared_libraries:
            lib_path = libdir / self._make_shared_lib_fname(lib)
            lib_paths.append(lib_path)

        if pcfg.requires:
            requires = pcfg.requires
        else:
            requires = []

        _write_libinit_py(self.output_libinit, lib_paths, requires)

    def _make_shared_lib_fname(self, lib: str):
        if is_windows:
            return f"{lib}.dll"
        elif is_macos:
            return f"lib{lib}.dylib"
        else:
            return f"lib{lib}.so"

    @functools.cached_property
    def _pcfiles(self) -> T.List[PcFileConfig]:
        pcfiles = []
        for i, raw_pc in enumerate(self.config.get("pcfile", [])):
            pcfile = parse_input(
                raw_pc,
                PcFileConfig,
                "pyproject.toml",
                f"tool.hatch.build.hooks.nativelib.pcfile[{i}]",
            )

            if pcfile.enable_if and not Marker(pcfile.enable_if).evaluate():
                print(
                    f"{pcfile.pcfile} skipped because enable_if did not match current environment"
                )
                continue

            pcfiles.append(pcfile)

        return pcfiles


# TODO: this belongs in a separate script/api that can be used from multiple tools
def _write_libinit_py(
    init_py: pathlib.Path,
    libs: T.List[pathlib.Path],
    requires: T.List[str],
):
    """
    :param init_py: the _init module for the library(ies) that is written out
    :param libs: for each library that is being initialized, this is the
                 path to that library

    :param requires: other pkgconf packages that these libraries depend on.
                     Their init_py will be looked up and imported first.
    """

    contents = [
        "# This file is automatically generated, DO NOT EDIT",
        "# fmt: off",
        "",
    ]

    for req in requires:
        r = pkgconf.run_pkgconf(
            req, f"--variable={INITPY_VARNAME}", capture_output=True
        )
        # TODO: should this be a fatal error
        if r.returncode == 0:
            module = r.stdout.decode("utf-8").strip()  # type: ignore[arg-type, union-attr]
            contents.append(f"import {module}")
        else:
            raise Exception("Could not find ", req)

    if contents[-1] != "":
        contents.append("")

    if libs:
        contents += [
            "def __load_library():",
            "    from os.path import abspath, join, dirname, exists",
        ]

        if is_macos:
            contents += ["    from ctypes import CDLL, RTLD_GLOBAL"]
        else:
            contents += ["    from ctypes import cdll", ""]

        if len(libs) > 1:
            contents.append("    libs = []")

        contents.append("    root = abspath(dirname(__file__))")

        for lib in libs:
            rel = lib.relative_to(init_py.parent)
            components = ", ".join(map(repr, rel.parts))

            contents += [
                "",
                f"    lib_path = join(root, {components})",
                "",
                "    try:",
            ]

            if is_macos:
                load = "CDLL(lib_path, mode=RTLD_GLOBAL)"
            else:
                load = "cdll.LoadLibrary(lib_path)"

            if len(libs) > 1:
                contents.append(f"        libs.append({load})")
            else:
                contents.append(f"        return {load}")

            contents += [
                "    except FileNotFoundError:",
                f"        if not exists(lib_path):",
                f'            raise FileNotFoundError("{lib.name} was not found on your system. Is this package correctly installed?")',
            ]

            if is_windows:
                contents.append(
                    f'        raise Exception("{lib.name} could not be loaded. Do you have Visual Studio C++ Redistributible installed?")'
                )
            else:
                contents.append(
                    f'        raise FileNotFoundError("{lib.name} could not be loaded. There is a missing dependency.")'
                )

        if len(libs) > 1:
            contents += ["    return libs"]

        contents += ["", "__lib = __load_library()", ""]

    content = ("\n".join(contents)) + "\n"

    init_py.parent.mkdir(parents=True, exist_ok=True)
    with open(init_py, "w") as fp:
        fp.write(content)


def main():
    pyproject_toml = sys.argv[1]
    libinit_file = pathlib.Path(sys.argv[2])
    pc_file = pathlib.Path(sys.argv[3])
    pkgcfgs = [pathlib.Path(x) for x in sys.argv[4:]]

    hack_pkgconfig(pkgcfgs)

    with open(pyproject_toml, "rb") as fp:
        raw_config = tomli.load(fp)

    nativelib_cfg = raw_config["tool"]["hatch"]["build"]["hooks"]["nativelib"]
    metadata = raw_config["project"]

    generator = NativelibHook(pc_file, libinit_file, nativelib_cfg, metadata)
    generator.initialize()


if __name__ == "__main__":
    main()
