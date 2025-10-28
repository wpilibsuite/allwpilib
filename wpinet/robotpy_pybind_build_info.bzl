# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def wpinet_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPINET_HEADER_GEN = [
        struct(
            class_name = "PortForwarder",
            yml_file = "semiwrap/PortForwarder.yml",
            header_root = "$(execpath :robotpy-native-wpinet.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpinet.copy_headers)/wpi/net/PortForwarder.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::net::PortForwarder", "wpi__net__PortForwarder.hpp"),
            ],
        ),
        struct(
            class_name = "WebServer",
            yml_file = "semiwrap/WebServer.yml",
            header_root = "$(execpath :robotpy-native-wpinet.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpinet.copy_headers)/wpi/net/WebServer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::net::WebServer", "wpi__net__WebServer.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpinet.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "wpinet.casters.pkl",
        dep_file = "wpinet.casters.d",
    )

    gen_libinit(
        name = "wpinet.gen_lib_init",
        output_file = "src/main/python/wpinet/_init__wpinet.py",
        modules = ["native.wpinet._init_robotpy_native_wpinet", "wpiutil._init__wpiutil"],
    )

    gen_pkgconf(
        name = "wpinet.gen_pkgconf",
        libinit_py = "wpinet._init__wpinet",
        module_pkg_name = "wpinet._wpinet",
        output_file = "wpinet.pc",
        pkg_name = "wpinet",
        install_path = "src/main/python/wpinet",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpinet/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpinet.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPINET_HEADER_GEN],
        libname = "_wpinet",
        output_file = "semiwrap_init.wpinet._wpinet.hpp",
    )

    run_header_gen(
        name = "wpinet",
        casters_pickle = "wpinet.casters.pkl",
        header_gen_config = WPINET_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpinet",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpinet",
        install_path = "src/main/python/wpinet/",
        extension_name = "_wpinet",
        generated_srcs = [":wpinet.generated_srcs"],
        semiwrap_header = [":wpinet.gen_modinit_hpp"],
        deps = [
            ":wpinet.tmpl_hdrs",
            ":wpinet.trampoline_hdrs",
            "//wpinet:wpinet",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//wpinet:shared/wpinet",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpinet.generated_files",
        srcs = [
            "wpinet.gen_modinit_hpp.gen",
            "wpinet.header_gen_files",
            "wpinet.gen_pkgconf",
            "wpinet.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "wpinet.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/wpinet/wpinet.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/wpinet/**"], exclude = ["src/main/python/wpinet/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/main/python/wpinet/**/*.py"]) + [
            "src/main/python/wpinet/_init__wpinet.py",
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/wpinet/_wpinet",
            ":wpinet.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//wpinet:robotpy-native-wpinet",
            "//wpiutil:robotpy-wpiutil",
        ],
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//wpinet:robotpy-native-wpinet.copy_headers",
        ],
        package_root_file = "src/main/python/wpinet/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//wpinet:robotpy-native-wpinet.copy_headers",
        ],
        package_root_file = "src/main/python/wpinet/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
