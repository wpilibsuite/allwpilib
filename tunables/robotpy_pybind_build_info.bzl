# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def tunables_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = []):
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

    TUNABLES_HEADER_GEN = [
        struct(
            class_name = "ComplexTunable",
            yml_file = "semiwrap/ComplexTunable.yml",
            header_root = "$(execpath :robotpy-native-tunables.copy_headers)",
            header_file = "$(execpath :robotpy-native-tunables.copy_headers)/wpi/tunables/ComplexTunable.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::tunables::ComplexTunable", "wpi__tunables__ComplexTunable.hpp"),
            ],
        ),
        struct(
            class_name = "TunableBackend",
            yml_file = "semiwrap/TunableBackend.yml",
            header_root = "$(execpath :robotpy-native-tunables.copy_headers)",
            header_file = "$(execpath :robotpy-native-tunables.copy_headers)/wpi/tunables/TunableBackend.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::tunables::TunableBackend", "wpi__tunables__TunableBackend.hpp"),
                ("wpi::tunables::TunableBackend::PublishedTunable", "wpi__tunables__TunableBackend__PublishedTunable.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "tunables.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "tunables.casters.pkl",
        dep_file = "tunables.casters.d",
    )

    gen_libinit(
        name = "tunables.gen_lib_init",
        output_file = "src/main/python/tunables/_init__tunables.py",
        modules = ["native.tunables._init_robotpy_native_tunables", "wpiutil._init__wpiutil"],
    )

    gen_pkgconf(
        name = "tunables.gen_pkgconf",
        libinit_py = "tunables._init__tunables",
        module_pkg_name = "tunables._tunables",
        output_file = "tunables.pc",
        pkg_name = "tunables",
        install_path = "src/main/python/tunables",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/tunables/__init__.py",
    )

    gen_modinit_hpp(
        name = "tunables.gen_modinit_hpp",
        input_dats = [x.class_name for x in TUNABLES_HEADER_GEN],
        libname = "_tunables",
        output_file = "semiwrap_init.tunables._tunables.hpp",
    )

    run_header_gen(
        name = "tunables",
        casters_pickle = "tunables.casters.pkl",
        header_gen_config = TUNABLES_HEADER_GEN,
        trampoline_subpath = "src/main/python/tunables",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//tunables:robotpy-native-tunables.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        name_transforms = NAME_TRANSFORMS,
    )

    create_pybind_library(
        name = "tunables",
        install_path = "src/main/python/tunables/",
        extension_name = "_tunables",
        generated_srcs = [":tunables.generated_srcs"],
        semiwrap_header = [":tunables.gen_modinit_hpp"],
        deps = [
            ":tunables.tmpl_hdrs",
            ":tunables.trampoline_hdrs",
            "//tunables:tunables",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//tunables:shared/tunables",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "tunables.generated_files",
        srcs = [
            "tunables.gen_modinit_hpp.gen",
            "tunables.header_gen_files",
            "tunables.gen_pkgconf",
            "tunables.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = [], extra_pybind_hdrs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "tunables.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/tunables/tunables.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/tunables/**"], exclude = ["src/main/python/tunables/**/*.py"]),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/tunables/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-tunables",
        srcs = native.glob(["src/main/python/tunables/**/*.py"]) + [
            "src/main/python/tunables/_init__tunables.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/tunables/_tunables",
            ":tunables.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//tunables:robotpy-native-tunables",
            "//wpiutil:robotpy-wpiutil",
        ],
        strip_path_prefixes = ["tunables/src/main/python", "tunables"],
        summary = "Binary wrapper for WPILib tunables library",
        project_urls = {"Source code": "https://github.com/robotpy/mostrobotpy"},
        author_email = "RobotPy Development Team <robotpy@googlegroups.com>",
        requires = ["robotpy-native-tunables==0.0.0", "robotpy-wpiutil==0.0.0"],
        python_requires = ">=3.11",
        entry_points = {
            "pkg_config": ["tunables = tunables"],
        },
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = extra_pybind_hdrs + [
            "//tunables:robotpy-native-tunables.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/tunables/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = extra_pybind_hdrs + [
            "//tunables:robotpy-native-tunables.copy_headers",
        ],
        package_root_file = "src/main/python/tunables/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
