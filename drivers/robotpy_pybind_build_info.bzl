# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:robotpy_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def drivers_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = []):
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

    DRIVERS_HEADER_GEN = [
        struct(
            class_name = "GoBildaPinpoint",
            yml_file = "semiwrap/GoBildaPinpoint.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/odometry/GoBildaPinpoint.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::GoBildaPinpoint", "wpi__GoBildaPinpoint.hpp"),
            ],
        ),
        struct(
            class_name = "Koors40",
            yml_file = "semiwrap/Koors40.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/Koors40.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Koors40", "wpi__Koors40.hpp"),
            ],
        ),
        struct(
            class_name = "PWMSparkFlex",
            yml_file = "semiwrap/PWMSparkFlex.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/PWMSparkFlex.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMSparkFlex", "wpi__PWMSparkFlex.hpp"),
            ],
        ),
        struct(
            class_name = "PWMSparkMax",
            yml_file = "semiwrap/PWMSparkMax.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/PWMSparkMax.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMSparkMax", "wpi__PWMSparkMax.hpp"),
            ],
        ),
        struct(
            class_name = "PWMTalonFX",
            yml_file = "semiwrap/PWMTalonFX.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/PWMTalonFX.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMTalonFX", "wpi__PWMTalonFX.hpp"),
            ],
        ),
        struct(
            class_name = "PWMTalonSRX",
            yml_file = "semiwrap/PWMTalonSRX.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/PWMTalonSRX.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMTalonSRX", "wpi__PWMTalonSRX.hpp"),
            ],
        ),
        struct(
            class_name = "PWMVenom",
            yml_file = "semiwrap/PWMVenom.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/PWMVenom.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMVenom", "wpi__PWMVenom.hpp"),
            ],
        ),
        struct(
            class_name = "PWMVictorSPX",
            yml_file = "semiwrap/PWMVictorSPX.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/PWMVictorSPX.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMVictorSPX", "wpi__PWMVictorSPX.hpp"),
            ],
        ),
        struct(
            class_name = "Spark",
            yml_file = "semiwrap/Spark.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/Spark.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Spark", "wpi__Spark.hpp"),
            ],
        ),
        struct(
            class_name = "SparkMini",
            yml_file = "semiwrap/SparkMini.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/SparkMini.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SparkMini", "wpi__SparkMini.hpp"),
            ],
        ),
        struct(
            class_name = "Talon",
            yml_file = "semiwrap/Talon.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/Talon.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Talon", "wpi__Talon.hpp"),
            ],
        ),
        struct(
            class_name = "VictorSP",
            yml_file = "semiwrap/VictorSP.yml",
            header_root = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib-drivers.copy_headers)/wpi/drivers/motor/VictorSP.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::VictorSP", "wpi__VictorSP.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "drivers.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "drivers.casters.pkl",
        dep_file = "drivers.casters.d",
    )

    gen_libinit(
        name = "drivers.gen_lib_init",
        output_file = "src/main/python/wpilib_drivers/_init__drivers.py",
        modules = ["native.wpilib_drivers._init_robotpy_native_wpilib_drivers", "wpilib._init__wpilib"],
    )

    gen_pkgconf(
        name = "drivers.gen_pkgconf",
        libinit_py = "wpilib_drivers._init__drivers",
        module_pkg_name = "wpilib_drivers._drivers",
        output_file = "drivers.pc",
        pkg_name = "drivers",
        install_path = "src/main/python/wpilib_drivers",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpilib_drivers/__init__.py",
    )

    gen_modinit_hpp(
        name = "drivers.gen_modinit_hpp",
        input_dats = [x.class_name for x in DRIVERS_HEADER_GEN],
        libname = "_drivers",
        output_file = "semiwrap_init.wpilib_drivers._drivers.hpp",
    )

    run_header_gen(
        name = "drivers",
        casters_pickle = "drivers.casters.pkl",
        header_gen_config = DRIVERS_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpilib_drivers",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//drivers:robotpy-native-wpilib-drivers.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//telemetry:robotpy-native-telemetry.copy_headers",
            "//tunables:robotpy-native-tunables.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        name_transforms = NAME_TRANSFORMS,
    )

    create_pybind_library(
        name = "drivers",
        install_path = "src/main/python/wpilib_drivers/",
        extension_name = "_drivers",
        generated_srcs = [":drivers.generated_srcs"],
        semiwrap_header = [":drivers.gen_modinit_hpp"],
        deps = [
            ":drivers.tmpl_hdrs",
            ":drivers.trampoline_hdrs",
            "//drivers:drivers",
            "//wpilibc:wpilib_pybind_library",
            "//wpilibc:wpilibc",
        ],
        dynamic_deps = [
            "//drivers:shared/drivers",
            "//wpilibc:shared/wpilibc",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "drivers.generated_files",
        srcs = [
            "drivers.gen_modinit_hpp.gen",
            "drivers.header_gen_files",
            "drivers.gen_pkgconf",
            "drivers.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = [], extra_pybind_hdrs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "drivers.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/wpilib_drivers/drivers.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/wpilib_drivers/**"], exclude = ["src/main/python/wpilib_drivers/**/*.py"]),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/wpilib_drivers/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        distribution = "wpilib-drivers",
        srcs = native.glob(["src/main/python/wpilib_drivers/**/*.py"]) + [
            "src/main/python/wpilib_drivers/_init__drivers.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/wpilib_drivers/_drivers",
            ":drivers.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//drivers:robotpy-native-wpilib-drivers",
            "//wpilibc:robotpy-wpilib",
        ],
        strip_path_prefixes = ["drivers/src/main/python", "drivers"],
        summary = "RobotPy bindings for WPILib third-party drivers",
        project_urls = {"Source code": "https://github.com/robotpy/mostrobotpy"},
        author_email = "RobotPy Development Team <robotpy@googlegroups.com>",
        requires = ["robotpy-native-wpilib-drivers==0.0.0", "wpilib==0.0.0"],
        python_requires = ">=3.11",
        entry_points = {
            "pkg_config": ["drivers = wpilib_drivers"],
        },
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = extra_pybind_hdrs + [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//drivers:robotpy-native-wpilib-drivers.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//telemetry:robotpy-native-telemetry.copy_headers",
            "//tunables:robotpy-native-tunables.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/wpilib_drivers/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = extra_pybind_hdrs + [
            "//drivers:robotpy-native-wpilib-drivers.copy_headers",
        ],
        package_root_file = "src/main/python/wpilib_drivers/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
