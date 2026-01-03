# THIS FILE IS AUTO GENERATED

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "publish_casters", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def wpimath_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIMATH_HEADER_GEN = [
        struct(
            class_name = "ComputerVisionUtil",
            yml_file = "semiwrap/ComputerVisionUtil.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/util/ComputerVisionUtil.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "MathUtil",
            yml_file = "semiwrap/MathUtil.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/util/MathUtil.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
    ]

    resolve_casters(
        name = "wpimath.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json", ":src/main/python/wpimath/wpimath-casters.pybind11.json"],
        casters_pkl_file = "wpimath.casters.pkl",
        dep_file = "wpimath.casters.d",
    )

    gen_libinit(
        name = "wpimath.gen_lib_init",
        output_file = "src/main/python/wpimath/_init__wpimath.py",
        modules = ["native.wpimath._init_robotpy_native_wpimath", "wpiutil._init__wpiutil"],
    )

    gen_pkgconf(
        name = "wpimath.gen_pkgconf",
        libinit_py = "wpimath._init__wpimath",
        module_pkg_name = "wpimath._wpimath",
        output_file = "wpimath.pc",
        pkg_name = "wpimath",
        install_path = "src/main/python/wpimath",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpimath.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIMATH_HEADER_GEN],
        libname = "_wpimath",
        output_file = "semiwrap_init.wpimath._wpimath.hpp",
    )

    run_header_gen(
        name = "wpimath",
        casters_pickle = "wpimath.casters.pkl",
        header_gen_config = WPIMATH_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpimath",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpimath",
        install_path = "src/main/python/wpimath/",
        extension_name = "_wpimath",
        generated_srcs = [":wpimath.generated_srcs"],
        semiwrap_header = [":wpimath.gen_modinit_hpp"],
        deps = [
            ":wpimath.tmpl_hdrs",
            ":wpimath.trampoline_hdrs",
            "//wpimath:wpimath",
            "//wpimath:wpimath-casters",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpimath.generated_files",
        srcs = [
            "wpimath.gen_modinit_hpp.gen",
            "wpimath.header_gen_files",
            "wpimath.gen_pkgconf",
            "wpimath.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpimath_filter_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIMATH_FILTER_HEADER_GEN = [
        struct(
            class_name = "Debouncer",
            yml_file = "semiwrap/filter/Debouncer.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/filter/Debouncer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Debouncer", "wpi__math__Debouncer.hpp"),
            ],
        ),
        struct(
            class_name = "LinearFilter",
            yml_file = "semiwrap/filter/LinearFilter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/filter/LinearFilter.hpp",
            tmpl_class_names = [
                ("LinearFilter_tmpl1", "LinearFilter"),
            ],
            trampolines = [
                ("wpi::math::LinearFilter", "wpi__math__LinearFilter.hpp"),
            ],
        ),
        struct(
            class_name = "MedianFilter",
            yml_file = "semiwrap/filter/MedianFilter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/filter/MedianFilter.hpp",
            tmpl_class_names = [
                ("MedianFilter_tmpl1", "MedianFilter"),
            ],
            trampolines = [
                ("wpi::math::MedianFilter", "wpi__math__MedianFilter.hpp"),
            ],
        ),
        struct(
            class_name = "SlewRateLimiter",
            yml_file = "semiwrap/filter/SlewRateLimiter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/filter/SlewRateLimiter.hpp",
            tmpl_class_names = [
                ("SlewRateLimiter_tmpl1", "SlewRateLimiter"),
            ],
            trampolines = [
                ("wpi::math::SlewRateLimiter", "wpi__math__SlewRateLimiter.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpimath_filter.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json", ":src/main/python/wpimath/wpimath-casters.pybind11.json"],
        casters_pkl_file = "wpimath_filter.casters.pkl",
        dep_file = "wpimath_filter.casters.d",
    )

    gen_libinit(
        name = "wpimath_filter.gen_lib_init",
        output_file = "src/main/python/wpimath/filter/_init__filter.py",
        modules = ["native.wpimath._init_robotpy_native_wpimath", "wpimath._init__wpimath"],
    )

    gen_pkgconf(
        name = "wpimath_filter.gen_pkgconf",
        libinit_py = "wpimath.filter._init__filter",
        module_pkg_name = "wpimath.filter._filter",
        output_file = "wpimath_filter.pc",
        pkg_name = "wpimath_filter",
        install_path = "src/main/python/wpimath/filter",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpimath_filter.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIMATH_FILTER_HEADER_GEN],
        libname = "_filter",
        output_file = "semiwrap_init.wpimath.filter._filter.hpp",
    )

    run_header_gen(
        name = "wpimath_filter",
        casters_pickle = "wpimath_filter.casters.pkl",
        header_gen_config = WPIMATH_FILTER_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpimath/filter",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpimath_filter",
        install_path = "src/main/python/wpimath/filter/",
        extension_name = "_filter",
        generated_srcs = [":wpimath_filter.generated_srcs"],
        semiwrap_header = [":wpimath_filter.gen_modinit_hpp"],
        deps = [
            ":wpimath_filter.tmpl_hdrs",
            ":wpimath_filter.trampoline_hdrs",
            "//wpimath:wpimath",
            "//wpimath:wpimath-casters",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpimath_filter.generated_files",
        srcs = [
            "wpimath_filter.gen_modinit_hpp.gen",
            "wpimath_filter.header_gen_files",
            "wpimath_filter.gen_pkgconf",
            "wpimath_filter.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpimath_geometry_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIMATH_GEOMETRY_HEADER_GEN = [
        struct(
            class_name = "CoordinateAxis",
            yml_file = "semiwrap/geometry/CoordinateAxis.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/CoordinateAxis.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::CoordinateAxis", "wpi__math__CoordinateAxis.hpp"),
            ],
        ),
        struct(
            class_name = "CoordinateSystem",
            yml_file = "semiwrap/geometry/CoordinateSystem.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/CoordinateSystem.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::CoordinateSystem", "wpi__math__CoordinateSystem.hpp"),
            ],
        ),
        struct(
            class_name = "Ellipse2d",
            yml_file = "semiwrap/geometry/Ellipse2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Ellipse2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Ellipse2d", "wpi__math__Ellipse2d.hpp"),
            ],
        ),
        struct(
            class_name = "Pose2d",
            yml_file = "semiwrap/geometry/Pose2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Pose2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Pose2d", "wpi__math__Pose2d.hpp"),
            ],
        ),
        struct(
            class_name = "Pose3d",
            yml_file = "semiwrap/geometry/Pose3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Pose3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Pose3d", "wpi__math__Pose3d.hpp"),
            ],
        ),
        struct(
            class_name = "Quaternion",
            yml_file = "semiwrap/geometry/Quaternion.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Quaternion.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Quaternion", "wpi__math__Quaternion.hpp"),
            ],
        ),
        struct(
            class_name = "Rectangle2d",
            yml_file = "semiwrap/geometry/Rectangle2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Rectangle2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Rectangle2d", "wpi__math__Rectangle2d.hpp"),
            ],
        ),
        struct(
            class_name = "Rotation2d",
            yml_file = "semiwrap/geometry/Rotation2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Rotation2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Rotation2d", "wpi__math__Rotation2d.hpp"),
            ],
        ),
        struct(
            class_name = "Rotation3d",
            yml_file = "semiwrap/geometry/Rotation3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Rotation3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Rotation3d", "wpi__math__Rotation3d.hpp"),
            ],
        ),
        struct(
            class_name = "Transform2d",
            yml_file = "semiwrap/geometry/Transform2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Transform2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Transform2d", "wpi__math__Transform2d.hpp"),
            ],
        ),
        struct(
            class_name = "Transform3d",
            yml_file = "semiwrap/geometry/Transform3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Transform3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Transform3d", "wpi__math__Transform3d.hpp"),
            ],
        ),
        struct(
            class_name = "Translation2d",
            yml_file = "semiwrap/geometry/Translation2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Translation2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Translation2d", "wpi__math__Translation2d.hpp"),
            ],
        ),
        struct(
            class_name = "Translation3d",
            yml_file = "semiwrap/geometry/Translation3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Translation3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Translation3d", "wpi__math__Translation3d.hpp"),
            ],
        ),
        struct(
            class_name = "Twist2d",
            yml_file = "semiwrap/geometry/Twist2d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Twist2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Twist2d", "wpi__math__Twist2d.hpp"),
            ],
        ),
        struct(
            class_name = "Twist3d",
            yml_file = "semiwrap/geometry/Twist3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/geometry/Twist3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Twist3d", "wpi__math__Twist3d.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpimath_geometry.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json", ":src/main/python/wpimath/wpimath-casters.pybind11.json"],
        casters_pkl_file = "wpimath_geometry.casters.pkl",
        dep_file = "wpimath_geometry.casters.d",
    )

    gen_libinit(
        name = "wpimath_geometry.gen_lib_init",
        output_file = "src/main/python/wpimath/geometry/_init__geometry.py",
        modules = ["native.wpimath._init_robotpy_native_wpimath", "wpimath._init__wpimath"],
    )

    gen_pkgconf(
        name = "wpimath_geometry.gen_pkgconf",
        libinit_py = "wpimath.geometry._init__geometry",
        module_pkg_name = "wpimath.geometry._geometry",
        output_file = "wpimath_geometry.pc",
        pkg_name = "wpimath_geometry",
        install_path = "src/main/python/wpimath/geometry",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpimath_geometry.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIMATH_GEOMETRY_HEADER_GEN],
        libname = "_geometry",
        output_file = "semiwrap_init.wpimath.geometry._geometry.hpp",
    )

    run_header_gen(
        name = "wpimath_geometry",
        casters_pickle = "wpimath_geometry.casters.pkl",
        header_gen_config = WPIMATH_GEOMETRY_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpimath/geometry",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpimath_geometry",
        install_path = "src/main/python/wpimath/geometry/",
        extension_name = "_geometry",
        generated_srcs = [":wpimath_geometry.generated_srcs"],
        semiwrap_header = [":wpimath_geometry.gen_modinit_hpp"],
        deps = [
            ":wpimath_geometry.tmpl_hdrs",
            ":wpimath_geometry.trampoline_hdrs",
            "//wpimath:wpimath",
            "//wpimath:wpimath-casters",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpimath_geometry.generated_files",
        srcs = [
            "wpimath_geometry.gen_modinit_hpp.gen",
            "wpimath_geometry.header_gen_files",
            "wpimath_geometry.gen_pkgconf",
            "wpimath_geometry.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpimath_interpolation_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIMATH_INTERPOLATION_HEADER_GEN = [
        struct(
            class_name = "TimeInterpolatableBuffer",
            yml_file = "semiwrap/interpolation/TimeInterpolatableBuffer.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/interpolation/TimeInterpolatableBuffer.hpp",
            tmpl_class_names = [
                ("TimeInterpolatableBuffer_tmpl1", "TimeInterpolatablePose2dBuffer"),
                ("TimeInterpolatableBuffer_tmpl2", "TimeInterpolatablePose3dBuffer"),
                ("TimeInterpolatableBuffer_tmpl3", "TimeInterpolatableRotation2dBuffer"),
                ("TimeInterpolatableBuffer_tmpl4", "TimeInterpolatableRotation3dBuffer"),
                ("TimeInterpolatableBuffer_tmpl5", "TimeInterpolatableTranslation2dBuffer"),
                ("TimeInterpolatableBuffer_tmpl6", "TimeInterpolatableTranslation3dBuffer"),
                ("TimeInterpolatableBuffer_tmpl7", "TimeInterpolatableFloatBuffer"),
            ],
            trampolines = [
                ("wpi::math::TimeInterpolatableBuffer", "wpi__math__TimeInterpolatableBuffer.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpimath_interpolation.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json", ":src/main/python/wpimath/wpimath-casters.pybind11.json"],
        casters_pkl_file = "wpimath_interpolation.casters.pkl",
        dep_file = "wpimath_interpolation.casters.d",
    )

    gen_libinit(
        name = "wpimath_interpolation.gen_lib_init",
        output_file = "src/main/python/wpimath/interpolation/_init__interpolation.py",
        modules = ["native.wpimath._init_robotpy_native_wpimath", "wpimath.geometry._init__geometry"],
    )

    gen_pkgconf(
        name = "wpimath_interpolation.gen_pkgconf",
        libinit_py = "wpimath.interpolation._init__interpolation",
        module_pkg_name = "wpimath.interpolation._interpolation",
        output_file = "wpimath_interpolation.pc",
        pkg_name = "wpimath_interpolation",
        install_path = "src/main/python/wpimath/interpolation",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpimath_interpolation.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIMATH_INTERPOLATION_HEADER_GEN],
        libname = "_interpolation",
        output_file = "semiwrap_init.wpimath.interpolation._interpolation.hpp",
    )

    run_header_gen(
        name = "wpimath_interpolation",
        casters_pickle = "wpimath_interpolation.casters.pkl",
        header_gen_config = WPIMATH_INTERPOLATION_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpimath/interpolation",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpimath_interpolation",
        install_path = "src/main/python/wpimath/interpolation/",
        extension_name = "_interpolation",
        generated_srcs = [":wpimath_interpolation.generated_srcs"],
        semiwrap_header = [":wpimath_interpolation.gen_modinit_hpp"],
        deps = [
            ":wpimath_interpolation.tmpl_hdrs",
            ":wpimath_interpolation.trampoline_hdrs",
            "//wpimath:wpimath",
            "//wpimath:wpimath-casters",
            "//wpimath:wpimath_geometry_pybind_library",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpimath_interpolation.generated_files",
        srcs = [
            "wpimath_interpolation.gen_modinit_hpp.gen",
            "wpimath_interpolation.header_gen_files",
            "wpimath_interpolation.gen_pkgconf",
            "wpimath_interpolation.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpimath_kinematics_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIMATH_KINEMATICS_HEADER_GEN = [
        struct(
            class_name = "ChassisSpeeds",
            yml_file = "semiwrap/kinematics/ChassisSpeeds.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/ChassisSpeeds.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::ChassisSpeeds", "wpi__math__ChassisSpeeds.hpp"),
            ],
        ),
        struct(
            class_name = "ChassisAccelerations",
            yml_file = "semiwrap/kinematics/ChassisAccelerations.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/ChassisAccelerations.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::ChassisAccelerations", "wpi__math__ChassisAccelerations.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveKinematics",
            yml_file = "semiwrap/kinematics/DifferentialDriveKinematics.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveKinematics.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveKinematics", "wpi__math__DifferentialDriveKinematics.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveOdometry3d",
            yml_file = "semiwrap/kinematics/DifferentialDriveOdometry3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveOdometry3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveOdometry3d", "wpi__math__DifferentialDriveOdometry3d.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveOdometry",
            yml_file = "semiwrap/kinematics/DifferentialDriveOdometry.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveOdometry.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveOdometry", "wpi__math__DifferentialDriveOdometry.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveWheelPositions",
            yml_file = "semiwrap/kinematics/DifferentialDriveWheelPositions.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveWheelPositions.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveWheelPositions", "wpi__math__DifferentialDriveWheelPositions.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveWheelSpeeds",
            yml_file = "semiwrap/kinematics/DifferentialDriveWheelSpeeds.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveWheelSpeeds.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveWheelSpeeds", "wpi__math__DifferentialDriveWheelSpeeds.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveWheelAccelerations",
            yml_file = "semiwrap/kinematics/DifferentialDriveWheelAccelerations.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveWheelAccelerations", "wpi__math__DifferentialDriveWheelAccelerations.hpp"),
            ],
        ),
        struct(
            class_name = "Kinematics",
            yml_file = "semiwrap/kinematics/Kinematics.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/Kinematics.hpp",
            tmpl_class_names = [
                ("Kinematics_tmpl1", "DifferentialDriveKinematicsBase"),
                ("Kinematics_tmpl2", "MecanumDriveKinematicsBase"),
                ("Kinematics_tmpl3", "SwerveDrive2KinematicsBase"),
                ("Kinematics_tmpl4", "SwerveDrive3KinematicsBase"),
                ("Kinematics_tmpl5", "SwerveDrive4KinematicsBase"),
                ("Kinematics_tmpl6", "SwerveDrive6KinematicsBase"),
            ],
            trampolines = [
                ("wpi::math::Kinematics", "wpi__math__Kinematics.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveKinematics",
            yml_file = "semiwrap/kinematics/MecanumDriveKinematics.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveKinematics.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveKinematics", "wpi__math__MecanumDriveKinematics.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveOdometry",
            yml_file = "semiwrap/kinematics/MecanumDriveOdometry.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveOdometry.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveOdometry", "wpi__math__MecanumDriveOdometry.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveOdometry3d",
            yml_file = "semiwrap/kinematics/MecanumDriveOdometry3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveOdometry3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveOdometry3d", "wpi__math__MecanumDriveOdometry3d.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveWheelPositions",
            yml_file = "semiwrap/kinematics/MecanumDriveWheelPositions.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveWheelPositions.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveWheelPositions", "wpi__math__MecanumDriveWheelPositions.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveWheelSpeeds",
            yml_file = "semiwrap/kinematics/MecanumDriveWheelSpeeds.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveWheelSpeeds.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveWheelSpeeds", "wpi__math__MecanumDriveWheelSpeeds.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveWheelAccelerations",
            yml_file = "semiwrap/kinematics/MecanumDriveWheelAccelerations.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/MecanumDriveWheelAccelerations.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveWheelAccelerations", "wpi__math__MecanumDriveWheelAccelerations.hpp"),
            ],
        ),
        struct(
            class_name = "Odometry",
            yml_file = "semiwrap/kinematics/Odometry.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/Odometry.hpp",
            tmpl_class_names = [
                ("Odometry_tmpl1", "DifferentialDriveOdometryBase"),
                ("Odometry_tmpl2", "MecanumDriveOdometryBase"),
                ("Odometry_tmpl3", "SwerveDrive2OdometryBase"),
                ("Odometry_tmpl4", "SwerveDrive3OdometryBase"),
                ("Odometry_tmpl5", "SwerveDrive4OdometryBase"),
                ("Odometry_tmpl6", "SwerveDrive6OdometryBase"),
            ],
            trampolines = [
                ("wpi::math::Odometry", "wpi__math__Odometry.hpp"),
            ],
        ),
        struct(
            class_name = "Odometry3d",
            yml_file = "semiwrap/kinematics/Odometry3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/Odometry3d.hpp",
            tmpl_class_names = [
                ("Odometry3d_tmpl1", "DifferentialDriveOdometry3dBase"),
                ("Odometry3d_tmpl2", "MecanumDriveOdometry3dBase"),
                ("Odometry3d_tmpl3", "SwerveDrive2Odometry3dBase"),
                ("Odometry3d_tmpl4", "SwerveDrive3Odometry3dBase"),
                ("Odometry3d_tmpl5", "SwerveDrive4Odometry3dBase"),
                ("Odometry3d_tmpl6", "SwerveDrive6Odometry3dBase"),
            ],
            trampolines = [
                ("wpi::math::Odometry3d", "wpi__math__Odometry3d.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDriveKinematics",
            yml_file = "semiwrap/kinematics/SwerveDriveKinematics.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveDriveKinematics.hpp",
            tmpl_class_names = [
                ("SwerveDriveKinematics_tmpl1", "SwerveDrive2Kinematics"),
                ("SwerveDriveKinematics_tmpl2", "SwerveDrive3Kinematics"),
                ("SwerveDriveKinematics_tmpl3", "SwerveDrive4Kinematics"),
                ("SwerveDriveKinematics_tmpl4", "SwerveDrive6Kinematics"),
            ],
            trampolines = [
                ("wpi::math::SwerveDriveKinematics", "wpi__math__SwerveDriveKinematics.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDriveOdometry",
            yml_file = "semiwrap/kinematics/SwerveDriveOdometry.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveDriveOdometry.hpp",
            tmpl_class_names = [
                ("SwerveDriveOdometry_tmpl1", "SwerveDrive2Odometry"),
                ("SwerveDriveOdometry_tmpl2", "SwerveDrive3Odometry"),
                ("SwerveDriveOdometry_tmpl3", "SwerveDrive4Odometry"),
                ("SwerveDriveOdometry_tmpl4", "SwerveDrive6Odometry"),
            ],
            trampolines = [
                ("wpi::math::SwerveDriveOdometry", "wpi__math__SwerveDriveOdometry.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDriveOdometry3d",
            yml_file = "semiwrap/kinematics/SwerveDriveOdometry3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveDriveOdometry3d.hpp",
            tmpl_class_names = [
                ("SwerveDriveOdometry3d_tmpl1", "SwerveDrive2Odometry3d"),
                ("SwerveDriveOdometry3d_tmpl2", "SwerveDrive3Odometry3d"),
                ("SwerveDriveOdometry3d_tmpl3", "SwerveDrive4Odometry3d"),
                ("SwerveDriveOdometry3d_tmpl4", "SwerveDrive6Odometry3d"),
            ],
            trampolines = [
                ("wpi::math::SwerveDriveOdometry3d", "wpi__math__SwerveDriveOdometry3d.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveModulePosition",
            yml_file = "semiwrap/kinematics/SwerveModulePosition.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveModulePosition.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SwerveModulePosition", "wpi__math__SwerveModulePosition.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveModuleState",
            yml_file = "semiwrap/kinematics/SwerveModuleState.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveModuleState.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SwerveModuleState", "wpi__math__SwerveModuleState.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveModuleAcceleration",
            yml_file = "semiwrap/kinematics/SwerveModuleAcceleration.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/kinematics/SwerveModuleAcceleration.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SwerveModuleAcceleration", "wpi__math__SwerveModuleAcceleration.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpimath_kinematics.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json", ":src/main/python/wpimath/wpimath-casters.pybind11.json"],
        casters_pkl_file = "wpimath_kinematics.casters.pkl",
        dep_file = "wpimath_kinematics.casters.d",
    )

    gen_libinit(
        name = "wpimath_kinematics.gen_lib_init",
        output_file = "src/main/python/wpimath/kinematics/_init__kinematics.py",
        modules = ["native.wpimath._init_robotpy_native_wpimath", "wpimath.geometry._init__geometry"],
    )

    gen_pkgconf(
        name = "wpimath_kinematics.gen_pkgconf",
        libinit_py = "wpimath.kinematics._init__kinematics",
        module_pkg_name = "wpimath.kinematics._kinematics",
        output_file = "wpimath_kinematics.pc",
        pkg_name = "wpimath_kinematics",
        install_path = "src/main/python/wpimath/kinematics",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpimath_kinematics.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIMATH_KINEMATICS_HEADER_GEN],
        libname = "_kinematics",
        output_file = "semiwrap_init.wpimath.kinematics._kinematics.hpp",
    )

    run_header_gen(
        name = "wpimath_kinematics",
        casters_pickle = "wpimath_kinematics.casters.pkl",
        header_gen_config = WPIMATH_KINEMATICS_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpimath/kinematics",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpimath_kinematics",
        install_path = "src/main/python/wpimath/kinematics/",
        extension_name = "_kinematics",
        generated_srcs = [":wpimath_kinematics.generated_srcs"],
        semiwrap_header = [":wpimath_kinematics.gen_modinit_hpp"],
        deps = [
            ":wpimath_kinematics.tmpl_hdrs",
            ":wpimath_kinematics.trampoline_hdrs",
            "//wpimath:wpimath",
            "//wpimath:wpimath-casters",
            "//wpimath:wpimath_geometry_pybind_library",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpimath_kinematics.generated_files",
        srcs = [
            "wpimath_kinematics.gen_modinit_hpp.gen",
            "wpimath_kinematics.header_gen_files",
            "wpimath_kinematics.gen_pkgconf",
            "wpimath_kinematics.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpimath_spline_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIMATH_SPLINE_HEADER_GEN = [
        struct(
            class_name = "CubicHermiteSpline",
            yml_file = "semiwrap/spline/CubicHermiteSpline.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/CubicHermiteSpline.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::CubicHermiteSpline", "wpi__math__CubicHermiteSpline.hpp"),
            ],
        ),
        struct(
            class_name = "QuinticHermiteSpline",
            yml_file = "semiwrap/spline/QuinticHermiteSpline.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/QuinticHermiteSpline.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::QuinticHermiteSpline", "wpi__math__QuinticHermiteSpline.hpp"),
            ],
        ),
        struct(
            class_name = "Spline",
            yml_file = "semiwrap/spline/Spline.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/Spline.hpp",
            tmpl_class_names = [
                ("Spline_tmpl1", "Spline3"),
                ("Spline_tmpl2", "Spline5"),
            ],
            trampolines = [
                ("wpi::math::Spline", "wpi__math__Spline.hpp"),
                ("wpi::math::Spline::ControlVector", "wpi__math__Spline__ControlVector.hpp"),
            ],
        ),
        struct(
            class_name = "SplineHelper",
            yml_file = "semiwrap/spline/SplineHelper.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/SplineHelper.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SplineHelper", "wpi__math__SplineHelper.hpp"),
            ],
        ),
        struct(
            class_name = "SplineParameterizer",
            yml_file = "semiwrap/spline/SplineParameterizer.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/spline/SplineParameterizer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::SplineParameterizer", "wpi__math__SplineParameterizer.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpimath_spline.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json", ":src/main/python/wpimath/wpimath-casters.pybind11.json"],
        casters_pkl_file = "wpimath_spline.casters.pkl",
        dep_file = "wpimath_spline.casters.d",
    )

    gen_libinit(
        name = "wpimath_spline.gen_lib_init",
        output_file = "src/main/python/wpimath/spline/_init__spline.py",
        modules = ["native.wpimath._init_robotpy_native_wpimath", "wpimath.geometry._init__geometry"],
    )

    gen_pkgconf(
        name = "wpimath_spline.gen_pkgconf",
        libinit_py = "wpimath.spline._init__spline",
        module_pkg_name = "wpimath.spline._spline",
        output_file = "wpimath_spline.pc",
        pkg_name = "wpimath_spline",
        install_path = "src/main/python/wpimath/spline",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpimath_spline.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIMATH_SPLINE_HEADER_GEN],
        libname = "_spline",
        output_file = "semiwrap_init.wpimath.spline._spline.hpp",
    )

    run_header_gen(
        name = "wpimath_spline",
        casters_pickle = "wpimath_spline.casters.pkl",
        header_gen_config = WPIMATH_SPLINE_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpimath/spline",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpimath_spline",
        install_path = "src/main/python/wpimath/spline/",
        extension_name = "_spline",
        generated_srcs = [":wpimath_spline.generated_srcs"],
        semiwrap_header = [":wpimath_spline.gen_modinit_hpp"],
        deps = [
            ":wpimath_spline.tmpl_hdrs",
            ":wpimath_spline.trampoline_hdrs",
            "//wpimath:wpimath",
            "//wpimath:wpimath-casters",
            "//wpimath:wpimath_geometry_pybind_library",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpimath_spline.generated_files",
        srcs = [
            "wpimath_spline.gen_modinit_hpp.gen",
            "wpimath_spline.header_gen_files",
            "wpimath_spline.gen_pkgconf",
            "wpimath_spline.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpimath_controls_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPIMATH_CONTROLS_HEADER_GEN = [
        struct(
            class_name = "ArmFeedforward",
            yml_file = "semiwrap/controls/ArmFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ArmFeedforward.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::ArmFeedforward", "wpi__math__ArmFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "BangBangController",
            yml_file = "semiwrap/controls/BangBangController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/BangBangController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::BangBangController", "wpi__math__BangBangController.hpp"),
            ],
        ),
        struct(
            class_name = "ControlAffinePlantInversionFeedforward",
            yml_file = "semiwrap/controls/ControlAffinePlantInversionFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ControlAffinePlantInversionFeedforward.hpp",
            tmpl_class_names = [
                ("ControlAffinePlantInversionFeedforward_tmpl1", "ControlAffinePlantInversionFeedforward_1_1"),
                ("ControlAffinePlantInversionFeedforward_tmpl2", "ControlAffinePlantInversionFeedforward_2_1"),
                ("ControlAffinePlantInversionFeedforward_tmpl3", "ControlAffinePlantInversionFeedforward_2_2"),
            ],
            trampolines = [
                ("wpi::math::ControlAffinePlantInversionFeedforward", "wpi__math__ControlAffinePlantInversionFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveAccelerationLimiter",
            yml_file = "semiwrap/controls/DifferentialDriveAccelerationLimiter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/DifferentialDriveAccelerationLimiter.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveAccelerationLimiter", "wpi__math__DifferentialDriveAccelerationLimiter.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveFeedforward",
            yml_file = "semiwrap/controls/DifferentialDriveFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/DifferentialDriveFeedforward.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveFeedforward", "wpi__math__DifferentialDriveFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveWheelVoltages",
            yml_file = "semiwrap/controls/DifferentialDriveWheelVoltages.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/DifferentialDriveWheelVoltages.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveWheelVoltages", "wpi__math__DifferentialDriveWheelVoltages.hpp"),
            ],
        ),
        struct(
            class_name = "ElevatorFeedforward",
            yml_file = "semiwrap/controls/ElevatorFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ElevatorFeedforward.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::ElevatorFeedforward", "wpi__math__ElevatorFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "ImplicitModelFollower",
            yml_file = "semiwrap/controls/ImplicitModelFollower.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ImplicitModelFollower.hpp",
            tmpl_class_names = [
                ("ImplicitModelFollower_tmpl1", "ImplicitModelFollower_1_1"),
                ("ImplicitModelFollower_tmpl2", "ImplicitModelFollower_2_1"),
                ("ImplicitModelFollower_tmpl3", "ImplicitModelFollower_2_2"),
            ],
            trampolines = [
                ("wpi::math::ImplicitModelFollower", "wpi__math__ImplicitModelFollower.hpp"),
            ],
        ),
        struct(
            class_name = "LTVDifferentialDriveController",
            yml_file = "semiwrap/controls/LTVDifferentialDriveController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/LTVDifferentialDriveController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::LTVDifferentialDriveController", "wpi__math__LTVDifferentialDriveController.hpp"),
            ],
        ),
        struct(
            class_name = "LTVUnicycleController",
            yml_file = "semiwrap/controls/LTVUnicycleController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/LTVUnicycleController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::LTVUnicycleController", "wpi__math__LTVUnicycleController.hpp"),
            ],
        ),
        struct(
            class_name = "LinearPlantInversionFeedforward",
            yml_file = "semiwrap/controls/LinearPlantInversionFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/LinearPlantInversionFeedforward.hpp",
            tmpl_class_names = [
                ("LinearPlantInversionFeedforward_tmpl1", "LinearPlantInversionFeedforward_1_1"),
                ("LinearPlantInversionFeedforward_tmpl2", "LinearPlantInversionFeedforward_2_1"),
                ("LinearPlantInversionFeedforward_tmpl3", "LinearPlantInversionFeedforward_2_2"),
                ("LinearPlantInversionFeedforward_tmpl4", "LinearPlantInversionFeedforward_3_2"),
            ],
            trampolines = [
                ("wpi::math::LinearPlantInversionFeedforward", "wpi__math__LinearPlantInversionFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "LinearQuadraticRegulator",
            yml_file = "semiwrap/controls/LinearQuadraticRegulator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/LinearQuadraticRegulator.hpp",
            tmpl_class_names = [
                ("LinearQuadraticRegulator_tmpl1", "LinearQuadraticRegulator_1_1"),
                ("LinearQuadraticRegulator_tmpl2", "LinearQuadraticRegulator_2_1"),
                ("LinearQuadraticRegulator_tmpl3", "LinearQuadraticRegulator_2_2"),
                ("LinearQuadraticRegulator_tmpl4", "LinearQuadraticRegulator_3_2"),
            ],
            trampolines = [
                ("wpi::math::LinearQuadraticRegulator", "wpi__math__LinearQuadraticRegulator.hpp"),
            ],
        ),
        struct(
            class_name = "PIDController",
            yml_file = "semiwrap/controls/PIDController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/PIDController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::PIDController", "wpi__math__PIDController.hpp"),
            ],
        ),
        struct(
            class_name = "ProfiledPIDController",
            yml_file = "semiwrap/controls/ProfiledPIDController.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/ProfiledPIDController.hpp",
            tmpl_class_names = [
                ("ProfiledPIDController_tmpl1", "ProfiledPIDController"),
                ("ProfiledPIDController_tmpl2", "ProfiledPIDControllerRadians"),
            ],
            trampolines = [
                ("wpi::math::ProfiledPIDController", "wpi__math__ProfiledPIDController.hpp"),
            ],
        ),
        struct(
            class_name = "SimpleMotorFeedforward",
            yml_file = "semiwrap/controls/SimpleMotorFeedforward.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/controller/SimpleMotorFeedforward.hpp",
            tmpl_class_names = [
                ("SimpleMotorFeedforward_tmpl1", "SimpleMotorFeedforwardMeters"),
                ("SimpleMotorFeedforward_tmpl2", "SimpleMotorFeedforwardRadians"),
            ],
            trampolines = [
                ("wpi::math::SimpleMotorFeedforward", "wpi__math__SimpleMotorFeedforward.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDrivePoseEstimator",
            yml_file = "semiwrap/controls/DifferentialDrivePoseEstimator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/DifferentialDrivePoseEstimator.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDrivePoseEstimator", "wpi__math__DifferentialDrivePoseEstimator.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDrivePoseEstimator3d",
            yml_file = "semiwrap/controls/DifferentialDrivePoseEstimator3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/DifferentialDrivePoseEstimator3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDrivePoseEstimator3d", "wpi__math__DifferentialDrivePoseEstimator3d.hpp"),
            ],
        ),
        struct(
            class_name = "ExtendedKalmanFilter",
            yml_file = "semiwrap/controls/ExtendedKalmanFilter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/ExtendedKalmanFilter.hpp",
            tmpl_class_names = [
                ("ExtendedKalmanFilter_tmpl1", "ExtendedKalmanFilter_1_1_1"),
                ("ExtendedKalmanFilter_tmpl2", "ExtendedKalmanFilter_2_1_1"),
                ("ExtendedKalmanFilter_tmpl3", "ExtendedKalmanFilter_2_1_2"),
                ("ExtendedKalmanFilter_tmpl4", "ExtendedKalmanFilter_2_2_2"),
            ],
            trampolines = [
                ("wpi::math::ExtendedKalmanFilter", "wpi__math__ExtendedKalmanFilter.hpp"),
            ],
        ),
        struct(
            class_name = "KalmanFilter",
            yml_file = "semiwrap/controls/KalmanFilter.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/KalmanFilter.hpp",
            tmpl_class_names = [
                ("KalmanFilter_tmpl1", "KalmanFilter_1_1_1"),
                ("KalmanFilter_tmpl2", "KalmanFilter_2_1_1"),
                ("KalmanFilter_tmpl3", "KalmanFilter_2_1_2"),
                ("KalmanFilter_tmpl4", "KalmanFilter_2_2_2"),
                ("KalmanFilter_tmpl5", "KalmanFilter_3_2_3"),
            ],
            trampolines = [
                ("wpi::math::KalmanFilter", "wpi__math__KalmanFilter.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDrivePoseEstimator",
            yml_file = "semiwrap/controls/MecanumDrivePoseEstimator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/MecanumDrivePoseEstimator.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDrivePoseEstimator", "wpi__math__MecanumDrivePoseEstimator.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDrivePoseEstimator3d",
            yml_file = "semiwrap/controls/MecanumDrivePoseEstimator3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/MecanumDrivePoseEstimator3d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDrivePoseEstimator3d", "wpi__math__MecanumDrivePoseEstimator3d.hpp"),
            ],
        ),
        struct(
            class_name = "PoseEstimator",
            yml_file = "semiwrap/controls/PoseEstimator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/PoseEstimator.hpp",
            tmpl_class_names = [
                ("PoseEstimator_tmpl1", "DifferentialDrivePoseEstimatorBase"),
                ("PoseEstimator_tmpl2", "MecanumDrivePoseEstimatorBase"),
                ("PoseEstimator_tmpl3", "SwerveDrive2PoseEstimatorBase"),
                ("PoseEstimator_tmpl4", "SwerveDrive3PoseEstimatorBase"),
                ("PoseEstimator_tmpl5", "SwerveDrive4PoseEstimatorBase"),
                ("PoseEstimator_tmpl6", "SwerveDrive6PoseEstimatorBase"),
            ],
            trampolines = [
                ("wpi::math::PoseEstimator", "wpi__math__PoseEstimator.hpp"),
            ],
        ),
        struct(
            class_name = "PoseEstimator3d",
            yml_file = "semiwrap/controls/PoseEstimator3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/PoseEstimator3d.hpp",
            tmpl_class_names = [
                ("PoseEstimator3d_tmpl1", "DifferentialDrivePoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl2", "MecanumDrivePoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl3", "SwerveDrive2PoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl4", "SwerveDrive3PoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl5", "SwerveDrive4PoseEstimator3dBase"),
                ("PoseEstimator3d_tmpl6", "SwerveDrive6PoseEstimator3dBase"),
            ],
            trampolines = [
                ("wpi::math::PoseEstimator3d", "wpi__math__PoseEstimator3d.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDrivePoseEstimator",
            yml_file = "semiwrap/controls/SwerveDrivePoseEstimator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/SwerveDrivePoseEstimator.hpp",
            tmpl_class_names = [
                ("SwerveDrivePoseEstimator_tmpl1", "SwerveDrive2PoseEstimator"),
                ("SwerveDrivePoseEstimator_tmpl2", "SwerveDrive3PoseEstimator"),
                ("SwerveDrivePoseEstimator_tmpl3", "SwerveDrive4PoseEstimator"),
                ("SwerveDrivePoseEstimator_tmpl4", "SwerveDrive6PoseEstimator"),
            ],
            trampolines = [
                ("wpi::math::SwerveDrivePoseEstimator", "wpi__math__SwerveDrivePoseEstimator.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDrivePoseEstimator3d",
            yml_file = "semiwrap/controls/SwerveDrivePoseEstimator3d.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/estimator/SwerveDrivePoseEstimator3d.hpp",
            tmpl_class_names = [
                ("SwerveDrivePoseEstimator3d_tmpl1", "SwerveDrive2PoseEstimator3d"),
                ("SwerveDrivePoseEstimator3d_tmpl2", "SwerveDrive3PoseEstimator3d"),
                ("SwerveDrivePoseEstimator3d_tmpl3", "SwerveDrive4PoseEstimator3d"),
                ("SwerveDrivePoseEstimator3d_tmpl4", "SwerveDrive6PoseEstimator3d"),
            ],
            trampolines = [
                ("wpi::math::SwerveDrivePoseEstimator3d", "wpi__math__SwerveDrivePoseEstimator3d.hpp"),
            ],
        ),
        struct(
            class_name = "SimulatedAnnealing",
            yml_file = "semiwrap/controls/SimulatedAnnealing.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/optimization/SimulatedAnnealing.hpp",
            tmpl_class_names = [
                ("SimulatedAnnealing_tmpl1", "SimulatedAnnealing"),
            ],
            trampolines = [
                ("wpi::math::SimulatedAnnealing", "wpi__math__SimulatedAnnealing.hpp"),
            ],
        ),
        struct(
            class_name = "TravelingSalesman",
            yml_file = "semiwrap/controls/TravelingSalesman.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/path/TravelingSalesman.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TravelingSalesman", "wpi__math__TravelingSalesman.hpp"),
            ],
        ),
        struct(
            class_name = "LinearSystem",
            yml_file = "semiwrap/controls/LinearSystem.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/system/LinearSystem.hpp",
            tmpl_class_names = [
                ("LinearSystem_tmpl1", "LinearSystem_1_1_1"),
                ("LinearSystem_tmpl2", "LinearSystem_1_1_2"),
                ("LinearSystem_tmpl3", "LinearSystem_1_1_3"),
                ("LinearSystem_tmpl4", "LinearSystem_2_1_1"),
                ("LinearSystem_tmpl5", "LinearSystem_2_1_2"),
                ("LinearSystem_tmpl6", "LinearSystem_2_1_3"),
                ("LinearSystem_tmpl7", "LinearSystem_2_2_1"),
                ("LinearSystem_tmpl8", "LinearSystem_2_2_2"),
                ("LinearSystem_tmpl9", "LinearSystem_2_2_3"),
                ("LinearSystem_tmpl10", "LinearSystem_3_2_1"),
                ("LinearSystem_tmpl11", "LinearSystem_3_2_2"),
                ("LinearSystem_tmpl12", "LinearSystem_3_2_3"),
            ],
            trampolines = [
                ("wpi::math::LinearSystem", "wpi__math__LinearSystem.hpp"),
            ],
        ),
        struct(
            class_name = "LinearSystemLoop",
            yml_file = "semiwrap/controls/LinearSystemLoop.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/system/LinearSystemLoop.hpp",
            tmpl_class_names = [
                ("LinearSystemLoop_tmpl1", "LinearSystemLoop_1_1_1"),
                ("LinearSystemLoop_tmpl2", "LinearSystemLoop_2_1_1"),
                ("LinearSystemLoop_tmpl3", "LinearSystemLoop_2_1_2"),
                ("LinearSystemLoop_tmpl4", "LinearSystemLoop_2_2_2"),
                ("LinearSystemLoop_tmpl5", "LinearSystemLoop_3_2_3"),
            ],
            trampolines = [
                ("wpi::math::LinearSystemLoop", "wpi__math__LinearSystemLoop.hpp"),
            ],
        ),
        struct(
            class_name = "DCMotor",
            yml_file = "semiwrap/controls/DCMotor.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/system/plant/DCMotor.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DCMotor", "wpi__math__DCMotor.hpp"),
            ],
        ),
        struct(
            class_name = "LinearSystemId",
            yml_file = "semiwrap/controls/LinearSystemId.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/system/plant/LinearSystemId.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::LinearSystemId", "wpi__math__LinearSystemId.hpp"),
            ],
        ),
        struct(
            class_name = "ExponentialProfile",
            yml_file = "semiwrap/controls/ExponentialProfile.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/ExponentialProfile.hpp",
            tmpl_class_names = [
                ("ExponentialProfile_tmpl1", "ExponentialProfileMeterVolts"),
            ],
            trampolines = [
                ("wpi::math::ExponentialProfile", "wpi__math__ExponentialProfile.hpp"),
                ("wpi::math::ExponentialProfile::Constraints", "wpi__math__ExponentialProfile__Constraints.hpp"),
                ("wpi::math::ExponentialProfile::State", "wpi__math__ExponentialProfile__State.hpp"),
                ("wpi::math::ExponentialProfile::ProfileTiming", "wpi__math__ExponentialProfile__ProfileTiming.hpp"),
            ],
        ),
        struct(
            class_name = "Trajectory",
            yml_file = "semiwrap/controls/Trajectory.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/Trajectory.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::Trajectory", "wpi__math__Trajectory.hpp"),
                ("wpi::math::Trajectory::State", "wpi__math__Trajectory__State.hpp"),
            ],
        ),
        struct(
            class_name = "TrajectoryConfig",
            yml_file = "semiwrap/controls/TrajectoryConfig.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/TrajectoryConfig.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TrajectoryConfig", "wpi__math__TrajectoryConfig.hpp"),
            ],
        ),
        struct(
            class_name = "TrajectoryGenerator",
            yml_file = "semiwrap/controls/TrajectoryGenerator.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/TrajectoryGenerator.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TrajectoryGenerator", "wpi__math__TrajectoryGenerator.hpp"),
            ],
        ),
        struct(
            class_name = "TrajectoryParameterizer",
            yml_file = "semiwrap/controls/TrajectoryParameterizer.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/TrajectoryParameterizer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TrajectoryParameterizer", "wpi__math__TrajectoryParameterizer.hpp"),
            ],
        ),
        struct(
            class_name = "TrapezoidProfile",
            yml_file = "semiwrap/controls/TrapezoidProfile.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/TrapezoidProfile.hpp",
            tmpl_class_names = [
                ("TrapezoidProfile_tmpl1", "TrapezoidProfile"),
                ("TrapezoidProfile_tmpl2", "TrapezoidProfileRadians"),
            ],
            trampolines = [
                ("wpi::math::TrapezoidProfile", "wpi__math__TrapezoidProfile.hpp"),
                ("wpi::math::TrapezoidProfile::Constraints", "wpi__math__TrapezoidProfile__Constraints.hpp"),
                ("wpi::math::TrapezoidProfile::State", "wpi__math__TrapezoidProfile__State.hpp"),
                ("wpi::math::TrapezoidProfile::ProfileTiming", "wpi__math__TrapezoidProfile__ProfileTiming.hpp"),
            ],
        ),
        struct(
            class_name = "CentripetalAccelerationConstraint",
            yml_file = "semiwrap/controls/CentripetalAccelerationConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/CentripetalAccelerationConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::CentripetalAccelerationConstraint", "wpi__math__CentripetalAccelerationConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveKinematicsConstraint",
            yml_file = "semiwrap/controls/DifferentialDriveKinematicsConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/DifferentialDriveKinematicsConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveKinematicsConstraint", "wpi__math__DifferentialDriveKinematicsConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDriveVoltageConstraint",
            yml_file = "semiwrap/controls/DifferentialDriveVoltageConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/DifferentialDriveVoltageConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::DifferentialDriveVoltageConstraint", "wpi__math__DifferentialDriveVoltageConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "EllipticalRegionConstraint",
            yml_file = "semiwrap/controls/EllipticalRegionConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/EllipticalRegionConstraint.hpp",
            tmpl_class_names = [
                ("EllipticalRegionConstraint_tmpl1", "EllipticalRegionConstraint"),
            ],
            trampolines = [
                ("wpi::math::EllipticalRegionConstraint", "wpi__math__EllipticalRegionConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "MaxVelocityConstraint",
            yml_file = "semiwrap/controls/MaxVelocityConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/MaxVelocityConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MaxVelocityConstraint", "wpi__math__MaxVelocityConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDriveKinematicsConstraint",
            yml_file = "semiwrap/controls/MecanumDriveKinematicsConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/MecanumDriveKinematicsConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::MecanumDriveKinematicsConstraint", "wpi__math__MecanumDriveKinematicsConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "RectangularRegionConstraint",
            yml_file = "semiwrap/controls/RectangularRegionConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/RectangularRegionConstraint.hpp",
            tmpl_class_names = [
                ("RectangularRegionConstraint_tmpl1", "RectangularRegionConstraint"),
            ],
            trampolines = [
                ("wpi::math::RectangularRegionConstraint", "wpi__math__RectangularRegionConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "SwerveDriveKinematicsConstraint",
            yml_file = "semiwrap/controls/SwerveDriveKinematicsConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/SwerveDriveKinematicsConstraint.hpp",
            tmpl_class_names = [
                ("SwerveDriveKinematicsConstraint_tmpl1", "SwerveDrive2KinematicsConstraint"),
                ("SwerveDriveKinematicsConstraint_tmpl2", "SwerveDrive3KinematicsConstraint"),
                ("SwerveDriveKinematicsConstraint_tmpl3", "SwerveDrive4KinematicsConstraint"),
                ("SwerveDriveKinematicsConstraint_tmpl4", "SwerveDrive6KinematicsConstraint"),
            ],
            trampolines = [
                ("wpi::math::SwerveDriveKinematicsConstraint", "wpi__math__SwerveDriveKinematicsConstraint.hpp"),
            ],
        ),
        struct(
            class_name = "TrajectoryConstraint",
            yml_file = "semiwrap/controls/TrajectoryConstraint.yml",
            header_root = "$(execpath :robotpy-native-wpimath.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpimath.copy_headers)/wpi/math/trajectory/constraint/TrajectoryConstraint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::math::TrajectoryConstraint", "wpi__math__TrajectoryConstraint.hpp"),
                ("wpi::math::TrajectoryConstraint::MinMax", "wpi__math__TrajectoryConstraint__MinMax.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpimath_controls.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json", ":src/main/python/wpimath/wpimath-casters.pybind11.json"],
        casters_pkl_file = "wpimath_controls.casters.pkl",
        dep_file = "wpimath_controls.casters.d",
    )

    gen_libinit(
        name = "wpimath_controls.gen_lib_init",
        output_file = "src/main/python/wpimath/_controls/_init__controls.py",
        modules = ["native.wpimath._init_robotpy_native_wpimath", "wpimath._init__wpimath", "wpimath.geometry._init__geometry", "wpimath.kinematics._init__kinematics", "wpimath.spline._init__spline"],
    )

    gen_pkgconf(
        name = "wpimath_controls.gen_pkgconf",
        libinit_py = "wpimath._controls._init__controls",
        module_pkg_name = "wpimath._controls._controls",
        output_file = "wpimath_controls.pc",
        pkg_name = "wpimath_controls",
        install_path = "src/main/python/wpimath/_controls",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpimath_controls.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPIMATH_CONTROLS_HEADER_GEN],
        libname = "_controls",
        output_file = "semiwrap_init.wpimath._controls._controls.hpp",
    )

    run_header_gen(
        name = "wpimath_controls",
        casters_pickle = "wpimath_controls.casters.pkl",
        header_gen_config = WPIMATH_CONTROLS_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpimath/_controls",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpimath_controls",
        install_path = "src/main/python/wpimath/_controls/",
        extension_name = "_controls",
        generated_srcs = [":wpimath_controls.generated_srcs"],
        semiwrap_header = [":wpimath_controls.gen_modinit_hpp"],
        deps = [
            ":wpimath_controls.tmpl_hdrs",
            ":wpimath_controls.trampoline_hdrs",
            "//wpimath:wpimath",
            "//wpimath:wpimath-casters",
            "//wpimath:wpimath_geometry_pybind_library",
            "//wpimath:wpimath_kinematics_pybind_library",
            "//wpimath:wpimath_pybind_library",
            "//wpimath:wpimath_spline_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpimath_controls.generated_files",
        srcs = [
            "wpimath_controls.gen_modinit_hpp.gen",
            "wpimath_controls.header_gen_files",
            "wpimath_controls.gen_pkgconf",
            "wpimath_controls.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def publish_library_casters():
    publish_casters(
        name = "publish_casters",
        caster_name = "wpimath-casters",
        output_json = "src/main/python/wpimath/wpimath-casters.pybind11.json",
        output_pc = "src/main/python/wpimath/wpimath-casters.pc",
        project_config = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpimath/__init__.py",
        typecasters_srcs = native.glob(["src/main/python/wpimath/_impl/src/**", "src/main/python/wpimath/_impl/src/type_casters/**"]),
    )

    cc_library(
        name = "wpimath-casters",
        hdrs = native.glob(["src/main/python/wpimath/_impl/src/*.h", "src/main/python/wpimath/_impl/src/type_casters/*.h"]),
        includes = ["src/main/python/wpimath/_impl/src", "src/main/python/wpimath/_impl/src/type_casters"],
        visibility = ["//visibility:public"],
        tags = ["robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "wpimath.generated_files",
            "wpimath_filter.generated_files",
            "wpimath_geometry.generated_files",
            "wpimath_interpolation.generated_files",
            "wpimath_kinematics.generated_files",
            "wpimath_spline.generated_files",
            "wpimath_controls.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/wpimath/wpimath.pc",
            "src/main/python/wpimath/filter/wpimath_filter.pc",
            "src/main/python/wpimath/geometry/wpimath_geometry.pc",
            "src/main/python/wpimath/interpolation/wpimath_interpolation.pc",
            "src/main/python/wpimath/kinematics/wpimath_kinematics.pc",
            "src/main/python/wpimath/spline/wpimath_spline.pc",
            "src/main/python/wpimath/_controls/wpimath_controls.pc",
            "src/main/python/wpimath/wpimath-casters.pc",
            "src/main/python/wpimath/wpimath-casters.pybind11.json",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/wpimath/**"], exclude = ["src/main/python/wpimath/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/wpimath/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/main/python/wpimath/**/*.py"]) + [
            "src/main/python/wpimath/_init__wpimath.py",
            "src/main/python/wpimath/filter/_init__filter.py",
            "src/main/python/wpimath/geometry/_init__geometry.py",
            "src/main/python/wpimath/interpolation/_init__interpolation.py",
            "src/main/python/wpimath/kinematics/_init__kinematics.py",
            "src/main/python/wpimath/spline/_init__spline.py",
            "src/main/python/wpimath/_controls/_init__controls.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/wpimath/_wpimath",
            ":src/main/python/wpimath/filter/_filter",
            ":src/main/python/wpimath/geometry/_geometry",
            ":src/main/python/wpimath/interpolation/_interpolation",
            ":src/main/python/wpimath/kinematics/_kinematics",
            ":src/main/python/wpimath/spline/_spline",
            ":src/main/python/wpimath/_controls/_controls",
            ":wpimath.trampoline_hdr_files",
            ":wpimath_filter.trampoline_hdr_files",
            ":wpimath_geometry.trampoline_hdr_files",
            ":wpimath_interpolation.trampoline_hdr_files",
            ":wpimath_kinematics.trampoline_hdr_files",
            ":wpimath_spline.trampoline_hdr_files",
            ":wpimath_controls.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//wpimath:robotpy-native-wpimath",
            "//wpiutil:robotpy-wpiutil",
        ],
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/wpimath/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//wpimath:robotpy-native-wpimath.copy_headers",
        ],
        package_root_file = "src/main/python/wpimath/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
