# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "copy_native_file", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")

def halsim_gui_ext_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    HALSIM_GUI_EXT_HEADER_GEN = [
    ]

    resolve_casters(
        name = "halsim_gui_ext.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "halsim_gui_ext.casters.pkl",
        dep_file = "halsim_gui_ext.casters.d",
    )

    gen_libinit(
        name = "halsim_gui_ext.gen_lib_init",
        output_file = "src/main/python/halsim_gui/_ext/_init__halsim_gui_ext.py",
        modules = ["hal._init__wpiHal", "wpimath._init__wpimath", "ntcore._init__ntcore"],
    )

    gen_pkgconf(
        name = "halsim_gui_ext.gen_pkgconf",
        libinit_py = "halsim_gui._ext._init__halsim_gui_ext",
        module_pkg_name = "halsim_gui._ext._halsim_gui_ext",
        output_file = "halsim_gui_ext.pc",
        pkg_name = "halsim_gui_ext",
        install_path = "src/main/python/halsim_gui/_ext",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/halsim_gui/_ext/__init__.py",
    )

    gen_modinit_hpp(
        name = "halsim_gui_ext.gen_modinit_hpp",
        input_dats = [x.class_name for x in HALSIM_GUI_EXT_HEADER_GEN],
        libname = "_halsim_gui_ext",
        output_file = "semiwrap_init.halsim_gui._ext._halsim_gui_ext.hpp",
    )

    run_header_gen(
        name = "halsim_gui_ext",
        casters_pickle = "halsim_gui_ext.casters.pkl",
        header_gen_config = HALSIM_GUI_EXT_HEADER_GEN,
        trampoline_subpath = "src/main/python/halsim_gui/_ext",
        deps = header_to_dat_deps,
        local_native_libraries = [
        ],
    )

    create_pybind_library(
        name = "halsim_gui_ext",
        install_path = "src/main/python/halsim_gui/_ext/",
        extension_name = "_halsim_gui_ext",
        generated_srcs = [],
        semiwrap_header = [":halsim_gui_ext.gen_modinit_hpp"],
        deps = [
            ":halsim_gui_ext.tmpl_hdrs",
            ":halsim_gui_ext.trampoline_hdrs",
            "//hal:wpiHal",
            "//hal:wpihal_pybind_library",
            "//ntcore:ntcore",
            "//ntcore:ntcore_pybind_library",
            "//simulation/halsim_gui:halsim_gui",
            "//wpimath:wpimath",
            "//wpimath:wpimath_pybind_library",
        ],
        dynamic_deps = [
            "//hal:shared/wpiHal",
            "//simulation/halsim_gui:shared/halsim_gui",
            "//ntcore:shared/ntcore",
            "//wpimath:shared/wpimath",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "halsim_gui_ext.generated_files",
        srcs = [
            "halsim_gui_ext.gen_modinit_hpp.gen",
            "halsim_gui_ext.header_gen_files",
            "halsim_gui_ext.gen_pkgconf",
            "halsim_gui_ext.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "halsim_gui_ext.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/halsim_gui/_ext/halsim_gui_ext.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/halsim_gui/**"], exclude = ["src/main/python/halsim_gui/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/halsim_gui/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    copy_native_file(
        name = "halsim_gui",
        library = ":shared/halsim_gui",
        base_path = "src/main/python/halsim_gui/",
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/main/python/halsim_gui/**/*.py"]) + [
            "src/main/python/halsim_gui/_ext/_init__halsim_gui_ext.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/halsim_gui/_ext/_halsim_gui_ext",
            ":halsim_gui_ext.trampoline_hdr_files",
            ":halsim_gui.copy_lib",
        ],
        imports = ["src/main/python"],
        deps = [
            "//hal:robotpy-hal",
            "//ntcore:pyntcore",
            "//wpimath:robotpy-wpimath",
            "//wpiutil:robotpy-wpiutil",
        ],
        visibility = ["//visibility:public"],
    )
