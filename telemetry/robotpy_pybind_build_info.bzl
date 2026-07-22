# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def telemetry_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = []):
    NAME_TRANSFORMS = [
        "--name-transform-default",
        "snake_case",
        "--name-transform-enum-value",
        "CAPS_CASE",
        "--name-transform-known-word",
        "3V3",
        "--name-transform-known-word",
        "5V",
        "--name-transform-known-word",
        "CAN",
        "--name-transform-known-word",
        "CPU",
        "--name-transform-known-word",
        "DS",
        "--name-transform-known-word",
        "FMS",
        "--name-transform-known-word",
        "FPGA",
        "--name-transform-known-word",
        "HAL",
        "--name-transform-known-word",
        "HTTP",
        "--name-transform-known-word",
        "I2C",
        "--name-transform-known-word",
        "IMU",
        "--name-transform-known-word",
        "JNI",
        "--name-transform-known-word",
        "JSON",
        "--name-transform-known-word",
        "mDNS",
        "--name-transform-known-word",
        "NT",
        "--name-transform-known-word",
        "OpMode",
        "--name-transform-known-word",
        "PCM",
        "--name-transform-known-word",
        "PDH",
        "--name-transform-known-word",
        "PDP",
        "--name-transform-known-word",
        "PID",
        "--name-transform-known-word",
        "POVs",
        "--name-transform-known-word",
        "PWM",
        "--name-transform-known-word",
        "RIO",
        "--name-transform-known-word",
        "SPI",
        "--name-transform-known-word",
        "URI",
        "--name-transform-known-word",
        "URL",
        "--name-transform-known-word",
        "USB",
        "--name-transform-known-word",
        "VIn",
    ]

    TELEMETRY_HEADER_GEN = [
        struct(
            class_name = "TelemetryEntry",
            yml_file = "semiwrap/TelemetryEntry.yml",
            header_root = "$(execpath :robotpy-native-telemetry.copy_headers)",
            header_file = "$(execpath :robotpy-native-telemetry.copy_headers)/wpi/telemetry/TelemetryEntry.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::TelemetryEntry", "wpi__TelemetryEntry.hpp"),
            ],
        ),
        struct(
            class_name = "TelemetryBackend",
            yml_file = "semiwrap/TelemetryBackend.yml",
            header_root = "$(execpath :robotpy-native-telemetry.copy_headers)",
            header_file = "$(execpath :robotpy-native-telemetry.copy_headers)/wpi/telemetry/TelemetryBackend.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::TelemetryBackend", "wpi__TelemetryBackend.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "telemetry.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "telemetry.casters.pkl",
        dep_file = "telemetry.casters.d",
    )

    gen_libinit(
        name = "telemetry.gen_lib_init",
        output_file = "src/main/python/telemetry/_init__telemetry.py",
        modules = ["native.telemetry._init_robotpy_native_telemetry", "wpiutil._init__wpiutil"],
    )

    gen_pkgconf(
        name = "telemetry.gen_pkgconf",
        libinit_py = "telemetry._init__telemetry",
        module_pkg_name = "telemetry._telemetry",
        output_file = "telemetry.pc",
        pkg_name = "telemetry",
        install_path = "src/main/python/telemetry",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/telemetry/__init__.py",
    )

    gen_modinit_hpp(
        name = "telemetry.gen_modinit_hpp",
        input_dats = [x.class_name for x in TELEMETRY_HEADER_GEN],
        libname = "_telemetry",
        output_file = "semiwrap_init.telemetry._telemetry.hpp",
    )

    run_header_gen(
        name = "telemetry",
        casters_pickle = "telemetry.casters.pkl",
        header_gen_config = TELEMETRY_HEADER_GEN,
        trampoline_subpath = "src/main/python/telemetry",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//telemetry:robotpy-native-telemetry.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        name_transforms = NAME_TRANSFORMS,
    )

    create_pybind_library(
        name = "telemetry",
        install_path = "src/main/python/telemetry/",
        extension_name = "_telemetry",
        generated_srcs = [":telemetry.generated_srcs"],
        semiwrap_header = [":telemetry.gen_modinit_hpp"],
        deps = [
            ":telemetry.tmpl_hdrs",
            ":telemetry.trampoline_hdrs",
            "//telemetry:telemetry",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//telemetry:shared/telemetry",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "telemetry.generated_files",
        srcs = [
            "telemetry.gen_modinit_hpp.gen",
            "telemetry.header_gen_files",
            "telemetry.gen_pkgconf",
            "telemetry.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "telemetry.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/telemetry/telemetry.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/telemetry/**"], exclude = ["src/main/python/telemetry/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/telemetry/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-telemetry",
        srcs = native.glob(["src/main/python/telemetry/**/*.py"]) + [
            "src/main/python/telemetry/_init__telemetry.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/telemetry/_telemetry",
            ":telemetry.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//telemetry:robotpy-native-telemetry",
            "//wpiutil:robotpy-wpiutil",
        ],
        strip_path_prefixes = ["telemetry/src/main/python", "telemetry"],
        summary = "Binary wrapper for WPILib telemetry library",
        project_urls = {"Source code": "https://github.com/robotpy/mostrobotpy"},
        author_email = "RobotPy Development Team <robotpy@googlegroups.com>",
        requires = ["robotpy-native-telemetry==0.0.0", "robotpy-wpiutil==0.0.0"],
        python_requires = ">=3.11",
        entry_points = {
            "pkg_config": ["telemetry = telemetry"],
        },
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//telemetry:robotpy-native-telemetry.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/telemetry/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//telemetry:robotpy-native-telemetry.copy_headers",
        ],
        package_root_file = "src/main/python/telemetry/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
