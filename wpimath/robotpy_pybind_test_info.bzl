# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def wpimath_test_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIMATH_TEST_HEADER_GEN = [
        struct(
            class_name = "module",
            yml_file = "semiwrap/module.yml",
            header_root = "wpimath/src/test/python/cpp/wpimath_test/include",
            header_file = "wpimath/src/test/python/cpp/wpimath_test/include/module.h",
            tmpl_class_names = [],
            trampolines = [
                ("SomeClass", "__SomeClass.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpimath_test.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "wpimath_test.casters.pkl",
        dep_file = "wpimath_test.casters.d",
    )

    gen_libinit(
        name = "wpimath_test.gen_lib_init",
        output_file = "src/test/python/cpp/wpimath_test/_init__wpimath_test.py",
        modules = ["wpimath._init__wpimath"],
    )

    gen_pkgconf(
        name = "wpimath_test.gen_pkgconf",
        libinit_py = "wpimath_test._init__wpimath_test",
        module_pkg_name = "wpimath_test._wpimath_test",
        output_file = "wpimath_test.pc",
        pkg_name = "wpimath_test",
        install_path = "src/test/python/cpp/wpimath_test",
        project_file = "src/test/python/cpp/pyproject.toml",
        package_root = "src/test/python/cpp/wpimath_test/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpimath_test.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIMATH_TEST_HEADER_GEN],
        libname = "_wpimath_test",
        output_file = "semiwrap_init.wpimath_test._wpimath_test.hpp",
    )

    run_header_gen(
        name = "wpimath_test",
        casters_pickle = "wpimath_test.casters.pkl",
        header_gen_config = WPIMATH_TEST_HEADER_GEN,
        trampoline_subpath = "src/test/python/cpp/wpimath_test",
        deps = header_to_dat_deps,
        local_native_libraries = [
        ],
        yml_prefix = "src/test/python/cpp/",
    )

    create_pybind_library(
        name = "wpimath_test",
        install_path = "src/test/python/cpp/wpimath_test/",
        extension_name = "_wpimath_test",
        generated_srcs = [":wpimath_test.generated_srcs"],
        semiwrap_header = [":wpimath_test.gen_modinit_hpp"],
        deps = [
            ":wpimath_test.tmpl_hdrs",
            ":wpimath_test.trampoline_hdrs",
            "//wpimath:wpimath",
            "//wpimath:wpimath_pybind_library",
        ],
        dynamic_deps = [
            "//wpimath:shared/wpimath",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpimath_test.generated_files",
        srcs = [
            "wpimath_test.gen_modinit_hpp.gen",
            "wpimath_test.header_gen_files",
            "wpimath_test.gen_pkgconf",
            "wpimath_test.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "wpimath_test.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/test/python/cpp/wpimath_test/wpimath_test.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/test/python/cpp/wpimath_test/**"], exclude = ["src/test/python/cpp/wpimath_test/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/test/python/cpp/wpimath_test/**/*.py"]) + [
            "src/test/python/cpp/wpimath_test/_init__wpimath_test.py",
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/test/python/cpp/wpimath_test/_wpimath_test",
            ":wpimath_test.trampoline_hdr_files",
        ],
        imports = ["src/test/python/cpp"],
        deps = [
        ],
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/test/python/cpp/semiwrap",
        extra_hdrs = native.glob(["src/test/python/cpp/**/*.h"], allow_empty = True) + [
        ],
        package_root_file = "src/test/python/cpp/wpimath_test/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/test/python/cpp/pyproject.toml",
        yaml_files = native.glob(["src/test/python/cpp/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/test/python/cpp/**/*.h"], allow_empty = True) + [
        ],
        package_root_file = "src/test/python/cpp/wpimath_test/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/test/python/cpp/pyproject.toml",
    )
