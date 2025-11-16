# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def apriltag_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    APRILTAG_HEADER_GEN = [
        struct(
            class_name = "AprilTag",
            yml_file = "semiwrap/AprilTag.yml",
            header_root = "$(execpath :robotpy-native-apriltag.copy_headers)",
            header_file = "$(execpath :robotpy-native-apriltag.copy_headers)/wpi/apriltag/AprilTag.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::apriltag::AprilTag", "wpi__apriltag__AprilTag.hpp"),
            ],
        ),
        struct(
            class_name = "AprilTagDetection",
            yml_file = "semiwrap/AprilTagDetection.yml",
            header_root = "$(execpath :robotpy-native-apriltag.copy_headers)",
            header_file = "$(execpath :robotpy-native-apriltag.copy_headers)/wpi/apriltag/AprilTagDetection.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::apriltag::AprilTagDetection", "wpi__apriltag__AprilTagDetection.hpp"),
                ("wpi::apriltag::AprilTagDetection::Point", "wpi__apriltag__AprilTagDetection__Point.hpp"),
            ],
        ),
        struct(
            class_name = "AprilTagDetector",
            yml_file = "semiwrap/AprilTagDetector.yml",
            header_root = "$(execpath :robotpy-native-apriltag.copy_headers)",
            header_file = "$(execpath :robotpy-native-apriltag.copy_headers)/wpi/apriltag/AprilTagDetector.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::apriltag::AprilTagDetector", "wpi__apriltag__AprilTagDetector.hpp"),
                ("wpi::apriltag::AprilTagDetector::Config", "wpi__apriltag__AprilTagDetector__Config.hpp"),
                ("wpi::apriltag::AprilTagDetector::QuadThresholdParameters", "wpi__apriltag__AprilTagDetector__QuadThresholdParameters.hpp"),
                ("wpi::apriltag::AprilTagDetector::Results", "wpi__apriltag__AprilTagDetector__Results.hpp"),
            ],
        ),
        struct(
            class_name = "AprilTagFieldLayout",
            yml_file = "semiwrap/AprilTagFieldLayout.yml",
            header_root = "$(execpath :robotpy-native-apriltag.copy_headers)",
            header_file = "$(execpath :robotpy-native-apriltag.copy_headers)/wpi/apriltag/AprilTagFieldLayout.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::apriltag::AprilTagFieldLayout", "wpi__apriltag__AprilTagFieldLayout.hpp"),
            ],
        ),
        struct(
            class_name = "AprilTagFields",
            yml_file = "semiwrap/AprilTagFields.yml",
            header_root = "$(execpath :robotpy-native-apriltag.copy_headers)",
            header_file = "$(execpath :robotpy-native-apriltag.copy_headers)/wpi/apriltag/AprilTagFields.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "AprilTagPoseEstimate",
            yml_file = "semiwrap/AprilTagPoseEstimate.yml",
            header_root = "$(execpath :robotpy-native-apriltag.copy_headers)",
            header_file = "$(execpath :robotpy-native-apriltag.copy_headers)/wpi/apriltag/AprilTagPoseEstimate.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::apriltag::AprilTagPoseEstimate", "wpi__apriltag__AprilTagPoseEstimate.hpp"),
            ],
        ),
        struct(
            class_name = "AprilTagPoseEstimator",
            yml_file = "semiwrap/AprilTagPoseEstimator.yml",
            header_root = "$(execpath :robotpy-native-apriltag.copy_headers)",
            header_file = "$(execpath :robotpy-native-apriltag.copy_headers)/wpi/apriltag/AprilTagPoseEstimator.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::apriltag::AprilTagPoseEstimator", "wpi__apriltag__AprilTagPoseEstimator.hpp"),
                ("wpi::apriltag::AprilTagPoseEstimator::Config", "wpi__apriltag__AprilTagPoseEstimator__Config.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "apriltag.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "apriltag.casters.pkl",
        dep_file = "apriltag.casters.d",
    )

    gen_libinit(
        name = "apriltag.gen_lib_init",
        output_file = "src/main/python/robotpy_apriltag/_init__apriltag.py",
        modules = ["native.apriltag._init_robotpy_native_apriltag", "wpiutil._init__wpiutil", "wpimath._init__wpimath"],
    )

    gen_pkgconf(
        name = "apriltag.gen_pkgconf",
        libinit_py = "robotpy_apriltag._init__apriltag",
        module_pkg_name = "robotpy_apriltag._apriltag",
        output_file = "apriltag.pc",
        pkg_name = "apriltag",
        install_path = "src/main/python/robotpy_apriltag",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/robotpy_apriltag/__init__.py",
    )

    gen_modinit_hpp(
        name = "apriltag.gen_modinit_hpp",
        input_dats = [x.class_name for x in APRILTAG_HEADER_GEN],
        libname = "_apriltag",
        output_file = "semiwrap_init.robotpy_apriltag._apriltag.hpp",
    )

    run_header_gen(
        name = "apriltag",
        casters_pickle = "apriltag.casters.pkl",
        header_gen_config = APRILTAG_HEADER_GEN,
        trampoline_subpath = "src/main/python/robotpy_apriltag",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//apriltag:robotpy-native-apriltag.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "apriltag",
        install_path = "src/main/python/robotpy_apriltag/",
        extension_name = "_apriltag",
        generated_srcs = [":apriltag.generated_srcs"],
        semiwrap_header = [":apriltag.gen_modinit_hpp"],
        deps = [
            ":apriltag.tmpl_hdrs",
            ":apriltag.trampoline_hdrs",
            "//apriltag:apriltag",
            "//wpimath:wpimath",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//apriltag:shared/apriltag",
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "apriltag.generated_files",
        srcs = [
            "apriltag.gen_modinit_hpp.gen",
            "apriltag.header_gen_files",
            "apriltag.gen_pkgconf",
            "apriltag.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "apriltag.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/robotpy_apriltag/apriltag.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/robotpy_apriltag/**"], exclude = ["src/main/python/robotpy_apriltag/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/robotpy_apriltag/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/main/python/robotpy_apriltag/**/*.py"]) + [
            "src/main/python/robotpy_apriltag/_init__apriltag.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/robotpy_apriltag/_apriltag",
            ":apriltag.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//apriltag:robotpy-native-apriltag",
            "//wpimath:robotpy-wpimath",
            "//wpiutil:robotpy-wpiutil",
        ],
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//apriltag:robotpy-native-apriltag.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/robotpy_apriltag/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//apriltag:robotpy-native-apriltag.copy_headers",
        ],
        package_root_file = "src/main/python/robotpy_apriltag/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
