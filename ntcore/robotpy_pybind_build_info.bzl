# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def ntcore_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    NTCORE_HEADER_GEN = [
        struct(
            class_name = "BooleanArrayTopic",
            yml_file = "semiwrap/BooleanArrayTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/BooleanArrayTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::BooleanArraySubscriber", "wpi__nt__BooleanArraySubscriber.hpp"),
                ("wpi::nt::BooleanArrayPublisher", "wpi__nt__BooleanArrayPublisher.hpp"),
                ("wpi::nt::BooleanArrayEntry", "wpi__nt__BooleanArrayEntry.hpp"),
                ("wpi::nt::BooleanArrayTopic", "wpi__nt__BooleanArrayTopic.hpp"),
            ],
        ),
        struct(
            class_name = "BooleanTopic",
            yml_file = "semiwrap/BooleanTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/BooleanTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::BooleanSubscriber", "wpi__nt__BooleanSubscriber.hpp"),
                ("wpi::nt::BooleanPublisher", "wpi__nt__BooleanPublisher.hpp"),
                ("wpi::nt::BooleanEntry", "wpi__nt__BooleanEntry.hpp"),
                ("wpi::nt::BooleanTopic", "wpi__nt__BooleanTopic.hpp"),
            ],
        ),
        struct(
            class_name = "DoubleArrayTopic",
            yml_file = "semiwrap/DoubleArrayTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/DoubleArrayTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::DoubleArraySubscriber", "wpi__nt__DoubleArraySubscriber.hpp"),
                ("wpi::nt::DoubleArrayPublisher", "wpi__nt__DoubleArrayPublisher.hpp"),
                ("wpi::nt::DoubleArrayEntry", "wpi__nt__DoubleArrayEntry.hpp"),
                ("wpi::nt::DoubleArrayTopic", "wpi__nt__DoubleArrayTopic.hpp"),
            ],
        ),
        struct(
            class_name = "DoubleTopic",
            yml_file = "semiwrap/DoubleTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/DoubleTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::DoubleSubscriber", "wpi__nt__DoubleSubscriber.hpp"),
                ("wpi::nt::DoublePublisher", "wpi__nt__DoublePublisher.hpp"),
                ("wpi::nt::DoubleEntry", "wpi__nt__DoubleEntry.hpp"),
                ("wpi::nt::DoubleTopic", "wpi__nt__DoubleTopic.hpp"),
            ],
        ),
        struct(
            class_name = "FloatArrayTopic",
            yml_file = "semiwrap/FloatArrayTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/FloatArrayTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::FloatArraySubscriber", "wpi__nt__FloatArraySubscriber.hpp"),
                ("wpi::nt::FloatArrayPublisher", "wpi__nt__FloatArrayPublisher.hpp"),
                ("wpi::nt::FloatArrayEntry", "wpi__nt__FloatArrayEntry.hpp"),
                ("wpi::nt::FloatArrayTopic", "wpi__nt__FloatArrayTopic.hpp"),
            ],
        ),
        struct(
            class_name = "FloatTopic",
            yml_file = "semiwrap/FloatTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/FloatTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::FloatSubscriber", "wpi__nt__FloatSubscriber.hpp"),
                ("wpi::nt::FloatPublisher", "wpi__nt__FloatPublisher.hpp"),
                ("wpi::nt::FloatEntry", "wpi__nt__FloatEntry.hpp"),
                ("wpi::nt::FloatTopic", "wpi__nt__FloatTopic.hpp"),
            ],
        ),
        struct(
            class_name = "GenericEntry",
            yml_file = "semiwrap/GenericEntry.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/GenericEntry.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::GenericSubscriber", "wpi__nt__GenericSubscriber.hpp"),
                ("wpi::nt::GenericPublisher", "wpi__nt__GenericPublisher.hpp"),
                ("wpi::nt::GenericEntry", "wpi__nt__GenericEntry.hpp"),
            ],
        ),
        struct(
            class_name = "IntegerArrayTopic",
            yml_file = "semiwrap/IntegerArrayTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/IntegerArrayTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::IntegerArraySubscriber", "wpi__nt__IntegerArraySubscriber.hpp"),
                ("wpi::nt::IntegerArrayPublisher", "wpi__nt__IntegerArrayPublisher.hpp"),
                ("wpi::nt::IntegerArrayEntry", "wpi__nt__IntegerArrayEntry.hpp"),
                ("wpi::nt::IntegerArrayTopic", "wpi__nt__IntegerArrayTopic.hpp"),
            ],
        ),
        struct(
            class_name = "IntegerTopic",
            yml_file = "semiwrap/IntegerTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/IntegerTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::IntegerSubscriber", "wpi__nt__IntegerSubscriber.hpp"),
                ("wpi::nt::IntegerPublisher", "wpi__nt__IntegerPublisher.hpp"),
                ("wpi::nt::IntegerEntry", "wpi__nt__IntegerEntry.hpp"),
                ("wpi::nt::IntegerTopic", "wpi__nt__IntegerTopic.hpp"),
            ],
        ),
        struct(
            class_name = "MultiSubscriber",
            yml_file = "semiwrap/MultiSubscriber.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/MultiSubscriber.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::MultiSubscriber", "wpi__nt__MultiSubscriber.hpp"),
            ],
        ),
        struct(
            class_name = "NTSendable",
            yml_file = "semiwrap/NTSendable.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/NTSendable.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::NTSendable", "wpi__nt__NTSendable.hpp"),
            ],
        ),
        struct(
            class_name = "NTSendableBuilder",
            yml_file = "semiwrap/NTSendableBuilder.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/NTSendableBuilder.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::NTSendableBuilder", "wpi__nt__NTSendableBuilder.hpp"),
            ],
        ),
        struct(
            class_name = "NetworkTable",
            yml_file = "semiwrap/NetworkTable.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/NetworkTable.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::NetworkTable", "wpi__nt__NetworkTable.hpp"),
            ],
        ),
        struct(
            class_name = "NetworkTableEntry",
            yml_file = "semiwrap/NetworkTableEntry.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/NetworkTableEntry.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::NetworkTableEntry", "wpi__nt__NetworkTableEntry.hpp"),
            ],
        ),
        struct(
            class_name = "NetworkTableInstance",
            yml_file = "semiwrap/NetworkTableInstance.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/NetworkTableInstance.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::NetworkTableInstance", "wpi__nt__NetworkTableInstance.hpp"),
            ],
        ),
        struct(
            class_name = "NetworkTableListener",
            yml_file = "semiwrap/NetworkTableListener.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/NetworkTableListener.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::NetworkTableListener", "wpi__nt__NetworkTableListener.hpp"),
                ("wpi::nt::NetworkTableListenerPoller", "wpi__nt__NetworkTableListenerPoller.hpp"),
            ],
        ),
        struct(
            class_name = "NetworkTableType",
            yml_file = "semiwrap/NetworkTableType.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/NetworkTableType.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "NetworkTableValue",
            yml_file = "semiwrap/NetworkTableValue.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/NetworkTableValue.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::Value", "wpi__nt__Value.hpp"),
            ],
        ),
        struct(
            class_name = "RawTopic",
            yml_file = "semiwrap/RawTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/RawTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::RawSubscriber", "wpi__nt__RawSubscriber.hpp"),
                ("wpi::nt::RawPublisher", "wpi__nt__RawPublisher.hpp"),
                ("wpi::nt::RawEntry", "wpi__nt__RawEntry.hpp"),
                ("wpi::nt::RawTopic", "wpi__nt__RawTopic.hpp"),
            ],
        ),
        struct(
            class_name = "StructTopic",
            yml_file = "semiwrap/StructTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/StructTopic.hpp",
            tmpl_class_names = [
                ("StructTopic_tmpl1", "StructSubscriber"),
                ("StructTopic_tmpl2", "StructPublisher"),
                ("StructTopic_tmpl3", "StructEntry"),
                ("StructTopic_tmpl4", "StructTopic"),
            ],
            trampolines = [
                ("wpi::nt::StructSubscriber", "wpi__nt__StructSubscriber.hpp"),
                ("wpi::nt::StructPublisher", "wpi__nt__StructPublisher.hpp"),
                ("wpi::nt::StructEntry", "wpi__nt__StructEntry.hpp"),
                ("wpi::nt::StructTopic", "wpi__nt__StructTopic.hpp"),
            ],
        ),
        struct(
            class_name = "StructArrayTopic",
            yml_file = "semiwrap/StructArrayTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/StructArrayTopic.hpp",
            tmpl_class_names = [
                ("StructArrayTopic_tmpl1", "StructArraySubscriber"),
                ("StructArrayTopic_tmpl2", "StructArrayPublisher"),
                ("StructArrayTopic_tmpl3", "StructArrayEntry"),
                ("StructArrayTopic_tmpl4", "StructArrayTopic"),
            ],
            trampolines = [
                ("wpi::nt::StructArraySubscriber", "wpi__nt__StructArraySubscriber.hpp"),
                ("wpi::nt::StructArrayPublisher", "wpi__nt__StructArrayPublisher.hpp"),
                ("wpi::nt::StructArrayEntry", "wpi__nt__StructArrayEntry.hpp"),
                ("wpi::nt::StructArrayTopic", "wpi__nt__StructArrayTopic.hpp"),
            ],
        ),
        struct(
            class_name = "StringArrayTopic",
            yml_file = "semiwrap/StringArrayTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/StringArrayTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::StringArraySubscriber", "wpi__nt__StringArraySubscriber.hpp"),
                ("wpi::nt::StringArrayPublisher", "wpi__nt__StringArrayPublisher.hpp"),
                ("wpi::nt::StringArrayEntry", "wpi__nt__StringArrayEntry.hpp"),
                ("wpi::nt::StringArrayTopic", "wpi__nt__StringArrayTopic.hpp"),
            ],
        ),
        struct(
            class_name = "StringTopic",
            yml_file = "semiwrap/StringTopic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/StringTopic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::StringSubscriber", "wpi__nt__StringSubscriber.hpp"),
                ("wpi::nt::StringPublisher", "wpi__nt__StringPublisher.hpp"),
                ("wpi::nt::StringEntry", "wpi__nt__StringEntry.hpp"),
                ("wpi::nt::StringTopic", "wpi__nt__StringTopic.hpp"),
            ],
        ),
        struct(
            class_name = "Topic",
            yml_file = "semiwrap/Topic.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/Topic.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::Topic", "wpi__nt__Topic.hpp"),
                ("wpi::nt::Subscriber", "wpi__nt__Subscriber.hpp"),
                ("wpi::nt::Publisher", "wpi__nt__Publisher.hpp"),
            ],
        ),
        struct(
            class_name = "ntcore_cpp",
            yml_file = "semiwrap/ntcore_cpp.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/ntcore_cpp.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::nt::EventFlags", "wpi__nt__EventFlags.hpp"),
                ("wpi::nt::TopicInfo", "wpi__nt__TopicInfo.hpp"),
                ("wpi::nt::ConnectionInfo", "wpi__nt__ConnectionInfo.hpp"),
                ("wpi::nt::ValueEventData", "wpi__nt__ValueEventData.hpp"),
                ("wpi::nt::LogMessage", "wpi__nt__LogMessage.hpp"),
                ("wpi::nt::TimeSyncEventData", "wpi__nt__TimeSyncEventData.hpp"),
                ("wpi::nt::Event", "wpi__nt__Event.hpp"),
                ("wpi::nt::PubSubOptions", "wpi__nt__PubSubOptions.hpp"),
                ("wpi::nt::meta::SubscriberOptions", "wpi__nt__meta__SubscriberOptions.hpp"),
                ("wpi::nt::meta::TopicPublisher", "wpi__nt__meta__TopicPublisher.hpp"),
                ("wpi::nt::meta::TopicSubscriber", "wpi__nt__meta__TopicSubscriber.hpp"),
                ("wpi::nt::meta::ClientPublisher", "wpi__nt__meta__ClientPublisher.hpp"),
                ("wpi::nt::meta::ClientSubscriber", "wpi__nt__meta__ClientSubscriber.hpp"),
                ("wpi::nt::meta::Client", "wpi__nt__meta__Client.hpp"),
            ],
        ),
        struct(
            class_name = "ntcore_cpp_types",
            yml_file = "semiwrap/ntcore_cpp_types.yml",
            header_root = "$(execpath :robotpy-native-ntcore.copy_headers)",
            header_file = "$(execpath :robotpy-native-ntcore.copy_headers)/wpi/nt/ntcore_cpp_types.hpp",
            tmpl_class_names = [
                ("ntcore_cpp_types_tmpl1", "TimestampedBoolean"),
                ("ntcore_cpp_types_tmpl2", "TimestampedInteger"),
                ("ntcore_cpp_types_tmpl3", "TimestampedFloat"),
                ("ntcore_cpp_types_tmpl4", "TimestampedDouble"),
                ("ntcore_cpp_types_tmpl5", "TimestampedString"),
                ("ntcore_cpp_types_tmpl6", "TimestampedRaw"),
                ("ntcore_cpp_types_tmpl7", "TimestampedBooleanArray"),
                ("ntcore_cpp_types_tmpl8", "TimestampedIntegerArray"),
                ("ntcore_cpp_types_tmpl9", "TimestampedFloatArray"),
                ("ntcore_cpp_types_tmpl10", "TimestampedDoubleArray"),
                ("ntcore_cpp_types_tmpl11", "TimestampedStringArray"),
                ("ntcore_cpp_types_tmpl12", "TimestampedStruct"),
                ("ntcore_cpp_types_tmpl13", "TimestampedStructArray"),
            ],
            trampolines = [
                ("wpi::nt::Timestamped", "wpi__nt__Timestamped.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "ntcore.resolve_casters",
        caster_deps = ["//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "ntcore.casters.pkl",
        dep_file = "ntcore.casters.d",
    )

    gen_libinit(
        name = "ntcore.gen_lib_init",
        output_file = "src/main/python/ntcore/_init__ntcore.py",
        modules = ["native.ntcore._init_robotpy_native_ntcore", "wpiutil._init__wpiutil", "wpinet._init__wpinet", "wpilog._init__wpilog"],
    )

    gen_pkgconf(
        name = "ntcore.gen_pkgconf",
        libinit_py = "ntcore._init__ntcore",
        module_pkg_name = "ntcore._ntcore",
        output_file = "ntcore.pc",
        pkg_name = "ntcore",
        install_path = "src/main/python/ntcore",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/ntcore/__init__.py",
    )

    gen_modinit_hpp(
        name = "ntcore.gen_modinit_hpp",
        input_dats = [x.class_name for x in NTCORE_HEADER_GEN],
        libname = "_ntcore",
        output_file = "semiwrap_init.ntcore._ntcore.hpp",
    )

    run_header_gen(
        name = "ntcore",
        casters_pickle = "ntcore.casters.pkl",
        header_gen_config = NTCORE_HEADER_GEN,
        trampoline_subpath = "src/main/python/ntcore",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "ntcore",
        install_path = "src/main/python/ntcore/",
        extension_name = "_ntcore",
        generated_srcs = [":ntcore.generated_srcs"],
        semiwrap_header = [":ntcore.gen_modinit_hpp"],
        deps = [
            ":ntcore.tmpl_hdrs",
            ":ntcore.trampoline_hdrs",
            "//datalog:datalog",
            "//datalog:wpilog_pybind_library",
            "//ntcore:ntcore",
            "//wpinet:wpinet",
            "//wpinet:wpinet_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//datalog:shared/datalog",
            "//ntcore:shared/ntcore",
            "//wpinet:shared/wpinet",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "ntcore.generated_files",
        srcs = [
            "ntcore.gen_modinit_hpp.gen",
            "ntcore.header_gen_files",
            "ntcore.gen_pkgconf",
            "ntcore.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "ntcore.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/ntcore/ntcore.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/ntcore/**"], exclude = ["src/main/python/ntcore/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/ntcore/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/main/python/ntcore/**/*.py"]) + [
            "src/main/python/ntcore/_init__ntcore.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/ntcore/_ntcore",
            ":ntcore.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//datalog:robotpy-wpilog",
            "//ntcore:robotpy-native-ntcore",
            "//wpinet:robotpy-wpinet",
            "//wpiutil:robotpy-wpiutil",
        ],
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/ntcore/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//ntcore:robotpy-native-ntcore.copy_headers",
        ],
        package_root_file = "src/main/python/ntcore/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
