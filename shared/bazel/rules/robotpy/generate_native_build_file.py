import argparse
import json
import pathlib

import tomli
from jinja2 import BaseLoader, Environment
from packaging.markers import Marker

from shared.bazel.rules.robotpy.generation_utils import (
    fixup_native_lib_name,
    fixup_python_dep_name,
    fixup_root_package_name,
)
from shared.bazel.rules.robotpy.hatchlib_native_port.config import PcFileConfig
from shared.bazel.rules.robotpy.hatchlib_native_port.validate import parse_input


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--project_cfg")
    parser.add_argument("--output_file")
    parser.add_argument("--third_party_dirs", nargs="+")
    parser.add_argument("--native_srcs_root")
    parser.add_argument("--generated_include_target", default=None)
    parser.add_argument("--generated_include_root")
    parser.add_argument("--package_name", required=True)
    args = parser.parse_args()

    with open(args.project_cfg, "rb") as fp:
        raw_config = tomli.load(fp)

    def double_quotes(data):
        if data:
            return json.dumps(data)
        return None

    def get_pc_dep(library):
        base_project = library.replace("robotpy-native-", "").replace("-", "_")
        wpilib_project = fixup_root_package_name(base_project)
        native_library = fixup_native_lib_name(library)
        return f"//{wpilib_project}:native/{base_project}/{native_library}.pc"

    def get_python_dep(library):
        base_project = library.replace("robotpy-native-", "").replace("-", "_")
        wpilib_project = fixup_root_package_name(base_project)
        return f"//{fixup_root_package_name(wpilib_project)}:{fixup_python_dep_name(library)}"

    env = Environment(loader=BaseLoader)
    env.filters["double_quotes"] = double_quotes
    env.filters["get_pc_dep"] = get_pc_dep
    env.filters["get_python_dep"] = get_python_dep
    env.filters["strip_src_prefix"] = lambda x: (
        pathlib.PurePath(x).as_posix().removeprefix("src/")
    )
    template = env.from_string(BUILD_FILE_TEMPLATE)

    nativelib_config = raw_config["tool"]["hatch"]["build"]["hooks"]["nativelib"]
    project_name = (
        raw_config["project"]["name"].replace("robotpy-native-", "").replace("-", "_")
    )
    root_package = fixup_root_package_name(project_name)
    pc_files = []

    local_pc_names = set()
    for i, raw_pc in enumerate(nativelib_config.get("pcfile", [])):
        pcfile = parse_input(
            raw_pc,
            PcFileConfig,
            "pyproject.toml",
            f"tool.hatch.build.hooks.nativelib.pcfile[{i}]",
        )
        if pcfile.enable_if and not Marker(pcfile.enable_if).evaluate():
            continue
        pc_files.append(pcfile)
        local_pc_names.add(pcfile.get_pc_path().name[:-3])

    requires = set()
    for pcfile in pc_files:
        if pcfile.requires:
            for dep in pcfile.requires:
                if dep not in local_pc_names:
                    requires.add(dep)

    maven_downloads = raw_config["tool"]["hatch"]["build"]["hooks"]["robotpy"][
        "maven_lib_download"
    ]

    third_party_dirs = args.third_party_dirs or []
    replace_prefix_keys = []
    if args.native_srcs_root:
        replace_prefix_keys.append(
            args.package_name + "/" + args.native_srcs_root + "include"
        )
        for d in third_party_dirs:
            replace_prefix_keys.append(
                args.package_name
                + "/"
                + args.native_srcs_root
                + "thirdparty/"
                + d
                + "/include"
            )

    if args.generated_include_target:
        replace_prefix_keys.append(root_package + "/" + args.generated_include_root)
    replace_prefix_keys.sort()

    with open(args.output_file, "w", newline="\n") as f:
        f.write(
            template.render(
                raw_project_config=raw_config["project"],
                nativelib_config=nativelib_config,
                root_package=root_package,
                maven_downloads=maven_downloads,
                third_party_dirs=args.third_party_dirs or [],
                pc_files=pc_files,
                requires=requires,
                project_name=project_name,
                generated_include_target=args.generated_include_target,
                native_srcs_root=args.native_srcs_root,
                replace_prefix_keys=replace_prefix_keys,
            )
        )


BUILD_FILE_TEMPLATE = """# THIS FILE IS AUTO GENERATED

load("@bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "copy_native_file", "generate_native_files", "robotpy_library")

def define_native_wrapper(name, pyproject_toml = None):
    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = {% if native_srcs_root %}native.glob(["{{native_srcs_root}}include/**"]){% else %}[]{% endif %}{% if generated_include_target %} + ["{{generated_include_target}}"]{% endif %}{% if third_party_dirs %} + native.glob([
        {%- for dir in third_party_dirs %}
            "{{native_srcs_root}}thirdparty/{{dir}}/include/**",
        {%- endfor %}
        ]){%- endif %},
        out = "native/{{project_name}}/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
        {%- for key in replace_prefix_keys %}
            "{{key}}": "",
            {%- endfor %}
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    libinit_files = [{% for pcfile in pc_files %}"{{pcfile.get_init_module_path() | strip_src_prefix}}"{% if not loop.last %}, {% endif %}{%- endfor %}]

    generate_native_files(
        name = name,
        pyproject_toml = pyproject_toml,
        pc_deps = [
        {%- for dep in requires | sort %}
            "{{dep | get_pc_dep}}",
        {%- endfor %}
        ],
        libinit_files = libinit_files,
        pc_files = [{% for pcfile in pc_files %}"{{pcfile.pcfile | strip_src_prefix}}"{% if not loop.last %}, {% endif %}{%- endfor %}],
    )
    {%- for maven_info in maven_downloads %}
    {%- for lib in maven_info["libs"] %}

    copy_native_file(
        name = "{{lib}}",
        library = "shared/{{lib}}",
        base_path = "native/{{project_name}}/",
    )
    {%- endfor %}
    {%- endfor %}

    robotpy_library(
        name = name,
        distribution = "{{raw_project_config.name}}",
        srcs = libinit_files,
        data = [
            name + ".pc_wrapper",
    {%- for maven_info in maven_downloads %}
    {%- for lib in maven_info["libs"] %}
            ":{{lib}}.copy_lib",
    {%- endfor %}
    {%- endfor %}
            "{}.copy_headers".format(name),
        ],
        deps = [
        {%- for dep in requires | sort %}
            "{{dep | get_python_dep}}",
        {%- endfor %}
        ],
        summary = "{{raw_project_config.description}}",
        requires = {{raw_project_config.dependencies | double_quotes}},
        python_requires = "{{raw_project_config["requires-python"]}}",
        strip_path_prefixes = ["{{root_package}}"],
        entry_points = {
            "pkg_config": [
            {%- for pcfile in pc_files %}
                "{{pcfile.name}} = native.{{project_name}}",
            {%- endfor %}
            ],
        },
    )

"""

if __name__ == "__main__":
    main()
