import argparse
import json

import tomli
from jinja2 import BaseLoader, Environment

from shared.bazel.rules.robotpy.generation_utils import (
    fixup_python_dep_name,
    fixup_root_package_name,
    fixup_shared_lib_name,
)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--project_cfg")
    parser.add_argument("--output_file")
    parser.add_argument("--third_party_dirs", nargs="+")
    args = parser.parse_args()

    with open(args.project_cfg, "rb") as fp:
        raw_config = tomli.load(fp)

    def double_quotes(data):
        if data:
            return json.dumps(data)
        return None

    def get_pc_dep(library):
        base_project = library.replace("robotpy-native-", "")
        wpilib_project = fixup_root_package_name(base_project)
        return f"//{wpilib_project}:native/{base_project}/{library}.pc"

    def get_python_dep(library):
        base_project = library.replace("robotpy-native-", "")
        wpilib_project = fixup_root_package_name(base_project)
        return f"//{fixup_root_package_name(wpilib_project)}:{fixup_python_dep_name(library)}"

    env = Environment(loader=BaseLoader)
    env.filters["double_quotes"] = double_quotes
    env.filters["get_pc_dep"] = get_pc_dep
    env.filters["get_python_dep"] = get_python_dep
    template = env.from_string(BUILD_FILE_TEMPLATE)

    nativelib_config = raw_config["tool"]["hatch"]["build"]["hooks"]["nativelib"]
    project_name = nativelib_config["pcfile"][0]["name"]
    root_package = fixup_root_package_name(project_name)
    shared_library_name = fixup_shared_lib_name(project_name)
    with open(args.output_file, "w") as f:
        f.write(
            template.render(
                raw_project_config=raw_config["project"],
                nativelib_config=nativelib_config,
                root_package=root_package,
                shared_library_name=shared_library_name,
                third_party_dirs=args.third_party_dirs or [],
            )
        )


BUILD_FILE_TEMPLATE = """# THIS FILE IS AUTO GENERATED

load("@aspect_bazel_lib//lib:copy_to_directory.bzl", "copy_to_directory")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "native_wrappery_library")

def define_native_wrapper(name, pyproject_toml = None):
    copy_to_directory(
        name = "{}.copy_headers".format(name),
        srcs = native.glob(["src/main/native/include/**"]) + native.glob(["src/generated/main/native/include/**"], allow_empty = True){% if third_party_dirs %} + native.glob([
        {%- for dir in third_party_dirs %}
            "src/main/native/thirdparty/{{dir}}/include/**",
        {%- endfor %}
        ]){%- endif %},
        out = "native/{{nativelib_config.pcfile[0].name}}/include",
        root_paths = ["src/main/native/include/"],
        replace_prefixes = {
            "{{root_package}}/src/generated/main/native/include": "",
            "{{root_package}}/src/main/native/include": "",
            {%- for dir in third_party_dirs %}
            "{{root_package}}/src/main/native/thirdparty/{{dir}}/include": "",
            {%- endfor %}
        },
        verbose = False,
        visibility = ["//visibility:public"],
    )

    native_wrappery_library(
        name = name,
        pyproject_toml = pyproject_toml or "src/main/python/native-pyproject.toml",
        libinit_file = "native/{{nativelib_config.pcfile[0].name}}/_init_{{raw_project_config.name.replace("-", "_")}}.py",
        pc_file = "native/{{nativelib_config.pcfile[0].name}}/{{raw_project_config.name}}.pc",
        pc_deps = [
        {%- for dep in nativelib_config.pcfile[0].requires | sort %}
            "{{dep | get_pc_dep}}",
        {%- endfor %}
        ],
        deps = [
        {%- for dep in nativelib_config.pcfile[0].requires | sort %}
            "{{dep | get_python_dep}}",
        {%- endfor %}
        ],
        headers = "{}.copy_headers".format(name),
        native_shared_library = "shared/{{shared_library_name}}",
        install_path = "native/{{nativelib_config.pcfile[0].name}}/",
        strip_path_prefixes = ["{{root_package}}"],
        requires = {{raw_project_config.dependencies | double_quotes}},
        summary = "{{raw_project_config.description}}",
        entry_points = {
            "pkg_config": [
                "{{nativelib_config.pcfile[0].name}} = native.{{nativelib_config.pcfile[0].name}}",
            ],
        },
    )

"""

if __name__ == "__main__":
    main()
