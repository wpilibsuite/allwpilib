# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def tunable_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = []):
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

    TUNABLE_HEADER_GEN = [
        struct(
            class_name = "TunableBackend",
            yml_file = "semiwrap/TunableBackend.yml",
            header_root = "$(execpath :robotpy-native-tunable.copy_headers)",
            header_file = "$(execpath :robotpy-native-tunable.copy_headers)/wpi/tunable/TunableBackend.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::TunableBackend", "wpi__TunableBackend.hpp"),
                ("wpi::TunableBackend::PublishedTunable", "wpi__TunableBackend__PublishedTunable.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "tunable.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "tunable.casters.pkl",
        dep_file = "tunable.casters.d",
    )

    gen_libinit(
        name = "tunable.gen_lib_init",
        output_file = "src/main/python/tunable/_init__tunable.py",
        modules = ["native.tunable._init_robotpy_native_tunable", "wpiutil._init__wpiutil"],
    )

    gen_pkgconf(
        name = "tunable.gen_pkgconf",
        libinit_py = "tunable._init__tunable",
        module_pkg_name = "tunable._tunable",
        output_file = "tunable.pc",
        pkg_name = "tunable",
        install_path = "src/main/python/tunable",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/tunable/__init__.py",
    )

    gen_modinit_hpp(
        name = "tunable.gen_modinit_hpp",
        input_dats = [x.class_name for x in TUNABLE_HEADER_GEN],
        libname = "_tunable",
        output_file = "semiwrap_init.tunable._tunable.hpp",
    )

    run_header_gen(
        name = "tunable",
        casters_pickle = "tunable.casters.pkl",
        header_gen_config = TUNABLE_HEADER_GEN,
        trampoline_subpath = "src/main/python/tunable",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//tunable:robotpy-native-tunable.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        name_transforms = NAME_TRANSFORMS,
    )

    create_pybind_library(
        name = "tunable",
        install_path = "src/main/python/tunable/",
        extension_name = "_tunable",
        generated_srcs = [":tunable.generated_srcs"],
        semiwrap_header = [":tunable.gen_modinit_hpp"],
        deps = [
            ":tunable.tmpl_hdrs",
            ":tunable.trampoline_hdrs",
            "//tunable:tunable",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//tunable:shared/tunable",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "tunable.generated_files",
        srcs = [
            "tunable.gen_modinit_hpp.gen",
            "tunable.header_gen_files",
            "tunable.gen_pkgconf",
            "tunable.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "tunable.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/tunable/tunable.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/tunable/**"], exclude = ["src/main/python/tunable/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/tunable/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-tunable",
        srcs = native.glob(["src/main/python/tunable/**/*.py"]) + [
            "src/main/python/tunable/_init__tunable.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/tunable/_tunable",
            ":tunable.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//tunable:robotpy-native-tunable",
            "//wpiutil:robotpy-wpiutil",
        ],
        strip_path_prefixes = ["tunable/src/main/python", "tunable"],
        summary = "Binary wrapper for WPILib tunable library",
        project_urls = {"Source code": "https://github.com/robotpy/mostrobotpy"},
        author_email = "RobotPy Development Team <robotpy@googlegroups.com>",
        requires = ["robotpy-native-tunable==0.0.0", "robotpy-wpiutil==0.0.0"],
        python_requires = ">=3.11",
        entry_points = {
            "pkg_config": ["tunable = tunable"],
        },
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//tunable:robotpy-native-tunable.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/tunable/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//tunable:robotpy-native-tunable.copy_headers",
        ],
        package_root_file = "src/main/python/tunable/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
