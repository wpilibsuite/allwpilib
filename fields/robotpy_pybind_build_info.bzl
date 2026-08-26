# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def fields_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = []):
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

    FIELDS_HEADER_GEN = [
        struct(
            class_name = "Field",
            yml_file = "semiwrap/Field.yml",
            header_root = "$(execpath :robotpy-native-fields.copy_headers)",
            header_file = "$(execpath :robotpy-native-fields.copy_headers)/wpi/fields/Field.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::fields::Field", "wpi__fields__Field.hpp"),
            ],
        ),
        struct(
            class_name = "FieldImage",
            yml_file = "semiwrap/FieldImage.yml",
            header_root = "$(execpath :robotpy-native-fields.copy_headers)",
            header_file = "$(execpath :robotpy-native-fields.copy_headers)/wpi/fields/FieldImage.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::fields::FieldImage", "wpi__fields__FieldImage.hpp"),
            ],
        ),
        struct(
            class_name = "FieldTag",
            yml_file = "semiwrap/FieldTag.yml",
            header_root = "$(execpath :robotpy-native-fields.copy_headers)",
            header_file = "$(execpath :robotpy-native-fields.copy_headers)/wpi/fields/FieldTag.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::fields::FieldTag", "wpi__fields__FieldTag.hpp"),
            ],
        ),
        struct(
            class_name = "fields",
            yml_file = "semiwrap/fields.yml",
            header_root = "$(execpath :robotpy-native-fields.copy_headers)",
            header_file = "$(execpath :robotpy-native-fields.copy_headers)/wpi/fields/fields.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
    ]

    resolve_casters(
        name = "fields.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "fields.casters.pkl",
        dep_file = "fields.casters.d",
    )

    gen_libinit(
        name = "fields.gen_lib_init",
        output_file = "src/main/python/robotpy_fields/_init__fields.py",
        modules = ["native.fields._init_robotpy_native_fields", "wpiutil._init__wpiutil", "wpimath._init__wpimath"],
    )

    gen_pkgconf(
        name = "fields.gen_pkgconf",
        libinit_py = "robotpy_fields._init__fields",
        module_pkg_name = "robotpy_fields._fields",
        output_file = "fields.pc",
        pkg_name = "fields",
        install_path = "src/main/python/robotpy_fields",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/robotpy_fields/__init__.py",
    )

    gen_modinit_hpp(
        name = "fields.gen_modinit_hpp",
        input_dats = [x.class_name for x in FIELDS_HEADER_GEN],
        libname = "_fields",
        output_file = "semiwrap_init.robotpy_fields._fields.hpp",
    )

    run_header_gen(
        name = "fields",
        casters_pickle = "fields.casters.pkl",
        header_gen_config = FIELDS_HEADER_GEN,
        trampoline_subpath = "src/main/python/robotpy_fields",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//fields:robotpy-native-fields.copy_headers",
            "//telemetry:robotpy-native-telemetry.copy_headers",
            "//tunables:robotpy-native-tunables.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        name_transforms = NAME_TRANSFORMS,
    )

    create_pybind_library(
        name = "fields",
        install_path = "src/main/python/robotpy_fields/",
        extension_name = "_fields",
        generated_srcs = [":fields.generated_srcs"],
        semiwrap_header = [":fields.gen_modinit_hpp"],
        deps = [
            ":fields.tmpl_hdrs",
            ":fields.trampoline_hdrs",
            "//fields:fields",
            "//wpimath:wpimath",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//fields:shared/fields",
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "fields.generated_files",
        srcs = [
            "fields.gen_modinit_hpp.gen",
            "fields.header_gen_files",
            "fields.gen_pkgconf",
            "fields.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = [], extra_pybind_hdrs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "fields.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/robotpy_fields/fields.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/robotpy_fields/**"], exclude = ["src/main/python/robotpy_fields/**/*.py"]),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/robotpy_fields/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        distribution = "robotpy-fields",
        srcs = native.glob(["src/main/python/robotpy_fields/**/*.py"]) + [
            "src/main/python/robotpy_fields/_init__fields.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/robotpy_fields/_fields",
            ":fields.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//fields:robotpy-native-fields",
            "//wpimath:robotpy-wpimath",
            "//wpiutil:robotpy-wpiutil",
        ],
        strip_path_prefixes = ["fields/src/main/python", "fields"],
        summary = "RobotPy bindings for WPILib's fields library",
        project_urls = {"Source code": "https://github.com/robotpy/mostrobotpy"},
        author_email = "RobotPy Development Team <robotpy@googlegroups.com>",
        requires = ["robotpy-native-fields==0.0.0", "robotpy-wpiutil==0.0.0", "robotpy-wpimath==0.0.0"],
        python_requires = ">=3.11",
        entry_points = {
            "pkg_config": ["fields = robotpy_fields"],
        },
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = extra_pybind_hdrs + [
            "//fields:robotpy-native-fields.copy_headers",
            "//telemetry:robotpy-native-telemetry.copy_headers",
            "//tunables:robotpy-native-tunables.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/robotpy_fields/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = extra_pybind_hdrs + [
            "//fields:robotpy-native-fields.copy_headers",
        ],
        package_root_file = "src/main/python/robotpy_fields/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
