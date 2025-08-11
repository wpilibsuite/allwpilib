# THIS FILE IS AUTO GENERATED

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "publish_casters", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers")

def wpiutil_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIUTIL_HEADER_GEN = [
        struct(
            class_name = "StackTrace",
            yml_file = "semiwrap/StackTrace.yml",
            header_root = "$(execpath :robotpy-native-wpiutil.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpiutil.copy_headers)/wpi/StackTrace.h",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "Synchronization",
            yml_file = "semiwrap/Synchronization.yml",
            header_root = "$(execpath :robotpy-native-wpiutil.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpiutil.copy_headers)/wpi/Synchronization.h",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "RawFrame",
            yml_file = "semiwrap/RawFrame.yml",
            header_root = "$(execpath :robotpy-native-wpiutil.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpiutil.copy_headers)/wpi/RawFrame.h",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "Sendable",
            yml_file = "semiwrap/Sendable.yml",
            header_root = "$(execpath :robotpy-native-wpiutil.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpiutil.copy_headers)/wpi/sendable/Sendable.h",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Sendable", "wpi__Sendable.hpp"),
            ],
        ),
        struct(
            class_name = "SendableBuilder",
            yml_file = "semiwrap/SendableBuilder.yml",
            header_root = "$(execpath :robotpy-native-wpiutil.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpiutil.copy_headers)/wpi/sendable/SendableBuilder.h",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SendableBuilder", "wpi__SendableBuilder.hpp"),
            ],
        ),
        struct(
            class_name = "SendableRegistry",
            yml_file = "semiwrap/SendableRegistry.yml",
            header_root = "$(execpath :robotpy-native-wpiutil.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpiutil.copy_headers)/wpi/sendable/SendableRegistry.h",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SendableRegistry", "wpi__SendableRegistry.hpp"),
            ],
        ),
        struct(
            class_name = "WPyStruct",
            yml_file = "semiwrap/WPyStruct.yml",
            header_root = "wpiutil/src/main/python/wpiutil",
            header_file = "wpiutil/src/main/python/wpiutil/src/wpistruct/wpystruct_fns.h",
            tmpl_class_names = [],
            trampolines = [],
        ),
    ]

    resolve_casters(
        name = "wpiutil.resolve_casters",
        caster_deps = [":src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "wpiutil.casters.pkl",
        dep_file = "wpiutil.casters.d",
    )

    gen_libinit(
        name = "wpiutil.gen_lib_init",
        output_file = "src/main/python/wpiutil/_init__wpiutil.py",
        modules = ["native.wpiutil._init_robotpy_native_wpiutil"],
    )

    gen_pkgconf(
        name = "wpiutil.gen_pkgconf",
        libinit_py = "wpiutil._init__wpiutil",
        module_pkg_name = "wpiutil._wpiutil",
        output_file = "wpiutil.pc",
        pkg_name = "wpiutil",
        install_path = "src/main/python/wpiutil",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpiutil/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpiutil.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIUTIL_HEADER_GEN],
        libname = "_wpiutil",
        output_file = "semiwrap_init.wpiutil._wpiutil.hpp",
    )

    run_header_gen(
        name = "wpiutil",
        casters_pickle = "wpiutil.casters.pkl",
        header_gen_config = WPIUTIL_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpiutil",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpiutil",
        install_path = "src/main/python/wpiutil/",
        extension_name = "_wpiutil",
        generated_srcs = [":wpiutil.generated_srcs"],
        semiwrap_header = [":wpiutil.gen_modinit_hpp"],
        deps = [
            ":wpiutil.tmpl_hdrs",
            ":wpiutil.trampoline_hdrs",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil-casters",
        ],
        dynamic_deps = [
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpiutil.generated_files",
        srcs = [
            "wpiutil.gen_modinit_hpp.gen",
            "wpiutil.header_gen_files",
            "wpiutil.gen_pkgconf",
            "wpiutil.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def publish_library_casters():
    publish_casters(
        name = "publish_casters",
        caster_name = "wpiutil-casters",
        output_json = "src/main/python/wpiutil/wpiutil-casters.pybind11.json",
        output_pc = "src/main/python/wpiutil/wpiutil-casters.pc",
        project_config = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpiutil/__init__.py",
        typecasters_srcs = native.glob(["src/main/python/wpiutil/src/type_casters/**", "src/main/python/wpiutil/src/wpistruct/**"]),
    )

    cc_library(
        name = "wpiutil-casters",
        hdrs = native.glob(["src/main/python/wpiutil/src/type_casters/*.h", "src/main/python/wpiutil/src/wpistruct/*.h"]),
        includes = ["src/main/python/wpiutil/src/type_casters", "src/main/python/wpiutil/src/wpistruct"],
        visibility = ["//visibility:public"],
        tags = ["robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "wpiutil.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_data_files".format(name),
        srcs = [
            "src/main/python/wpiutil/wpiutil.pc",
            "src/main/python/wpiutil/wpiutil-casters.pc",
            "src/main/python/wpiutil/wpiutil-casters.pybind11.json",
        ],
        tags = ["manual", "robotpy"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/wpiutil/**"], exclude = ["src/main/python/wpiutil/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/main/python/wpiutil/**/*.py"]) + [
            "src/main/python/wpiutil/_init__wpiutil.py",
        ],
        data = [
            "{}.generated_data_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/wpiutil/_wpiutil",
            ":wpiutil.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//wpiutil:robotpy-native-wpiutil",
        ],
        strip_path_prefixes = ["wpiutil/src/main/python/"],
        summary = "Binary wrapper for FRC WPIUtil library",
        project_urls = {"Source code": "https://github.com/robotpy/mostrobotpy"},
        author_email = "RobotPy Development Team <robotpy@googlegroups.com>",
        requires = ["robotpy-native-wpiutil==2027.0.0a2"],
        entry_points = {
            "pkg_config": ["wpiutil-casters = wpiutil", "wpiutil = wpiutil"],
        },
        visibility = ["//visibility:public"],
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True),
        package_root_file = "src/main/python/wpiutil/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
