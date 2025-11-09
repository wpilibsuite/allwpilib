# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def xrp_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    XRP_HEADER_GEN = [
        struct(
            class_name = "XRPGyro",
            yml_file = "semiwrap/XRPGyro.yml",
            header_root = "$(execpath :robotpy-native-xrp.copy_headers)",
            header_file = "$(execpath :robotpy-native-xrp.copy_headers)/wpi/xrp/XRPGyro.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::xrp::XRPGyro", "wpi__xrp__XRPGyro.hpp"),
            ],
        ),
        struct(
            class_name = "XRPMotor",
            yml_file = "semiwrap/XRPMotor.yml",
            header_root = "$(execpath :robotpy-native-xrp.copy_headers)",
            header_file = "$(execpath :robotpy-native-xrp.copy_headers)/wpi/xrp/XRPMotor.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::xrp::XRPMotor", "wpi__xrp__XRPMotor.hpp"),
            ],
        ),
        struct(
            class_name = "XRPOnBoardIO",
            yml_file = "semiwrap/XRPOnBoardIO.yml",
            header_root = "$(execpath :robotpy-native-xrp.copy_headers)",
            header_file = "$(execpath :robotpy-native-xrp.copy_headers)/wpi/xrp/XRPOnBoardIO.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::xrp::XRPOnBoardIO", "wpi__xrp__XRPOnBoardIO.hpp"),
            ],
        ),
        struct(
            class_name = "XRPRangefinder",
            yml_file = "semiwrap/XRPRangefinder.yml",
            header_root = "$(execpath :robotpy-native-xrp.copy_headers)",
            header_file = "$(execpath :robotpy-native-xrp.copy_headers)/wpi/xrp/XRPRangefinder.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::xrp::XRPRangefinder", "wpi__xrp__XRPRangefinder.hpp"),
            ],
        ),
        struct(
            class_name = "XRPReflectanceSensor",
            yml_file = "semiwrap/XRPReflectanceSensor.yml",
            header_root = "$(execpath :robotpy-native-xrp.copy_headers)",
            header_file = "$(execpath :robotpy-native-xrp.copy_headers)/wpi/xrp/XRPReflectanceSensor.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::xrp::XRPReflectanceSensor", "wpi__xrp__XRPReflectanceSensor.hpp"),
            ],
        ),
        struct(
            class_name = "XRPServo",
            yml_file = "semiwrap/XRPServo.yml",
            header_root = "$(execpath :robotpy-native-xrp.copy_headers)",
            header_file = "$(execpath :robotpy-native-xrp.copy_headers)/wpi/xrp/XRPServo.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::xrp::XRPServo", "wpi__xrp__XRPServo.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "xrp.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "xrp.casters.pkl",
        dep_file = "xrp.casters.d",
    )

    gen_libinit(
        name = "xrp.gen_lib_init",
        output_file = "src/main/python/xrp/_init__xrp.py",
        modules = ["native.xrp._init_robotpy_native_xrp", "wpilib._init__wpilib", "wpimath.geometry._init__geometry"],
    )

    gen_pkgconf(
        name = "xrp.gen_pkgconf",
        libinit_py = "xrp._init__xrp",
        module_pkg_name = "xrp._xrp",
        output_file = "xrp.pc",
        pkg_name = "xrp",
        install_path = "src/main/python/xrp",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/xrp/__init__.py",
    )

    gen_modinit_hpp(
        name = "xrp.gen_modinit_hpp",
        input_dats = [x.class_name for x in XRP_HEADER_GEN],
        libname = "_xrp",
        output_file = "semiwrap_init.xrp._xrp.hpp",
    )

    run_header_gen(
        name = "xrp",
        casters_pickle = "xrp.casters.pkl",
        header_gen_config = XRP_HEADER_GEN,
        trampoline_subpath = "src/main/python/xrp",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
            "//xrpVendordep:robotpy-native-xrp.copy_headers",
        ],
    )

    create_pybind_library(
        name = "xrp",
        install_path = "src/main/python/xrp/",
        extension_name = "_xrp",
        generated_srcs = [":xrp.generated_srcs"],
        semiwrap_header = [":xrp.gen_modinit_hpp"],
        deps = [
            ":xrp.tmpl_hdrs",
            ":xrp.trampoline_hdrs",
            "//wpilibc:wpilib_pybind_library",
            "//wpilibc:wpilibc",
            "//wpimath:wpimath",
            "//wpimath:wpimath_geometry_pybind_library",
            "//xrpVendordep:xrpVendordep",
        ],
        dynamic_deps = [
            "//wpilibc:shared/wpilibc",
            "//wpimath:shared/wpimath",
            "//xrpVendordep:shared/xrpVendordep",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "xrp.generated_files",
        srcs = [
            "xrp.gen_modinit_hpp.gen",
            "xrp.header_gen_files",
            "xrp.gen_pkgconf",
            "xrp.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "xrp.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/xrp/xrp.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/xrp/**"], exclude = ["src/main/python/xrp/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/xrp/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/main/python/xrp/**/*.py"]) + [
            "src/main/python/xrp/_init__xrp.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/xrp/_xrp",
            ":xrp.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//wpilibc:robotpy-wpilib",
            "//xrpVendordep:robotpy-native-xrp",
        ],
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
            "//xrpVendordep:robotpy-native-xrp.copy_headers",
        ],
        package_root_file = "src/main/python/xrp/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//xrpVendordep:robotpy-native-xrp.copy_headers",
        ],
        package_root_file = "src/main/python/xrp/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
