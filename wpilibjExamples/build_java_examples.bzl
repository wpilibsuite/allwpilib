load("@rules_java//java:defs.bzl", "java_binary", "java_library")
load("@rules_pkg//:mappings.bzl", "pkg_files")
load("@rules_pkg//:pkg.bzl", "pkg_zip")
load("//shared/bazel/rules:java_rules.bzl", "wpilib_java_junit5_test")

_EXAMPLE_DEPS = [
    "//allwpilib-java/src/main/java",
    "//apriltag/src/main/java",
    "//cameraserver/src/main/java",
    "//cscore/src/main/java",
    "//drivers/src/main/java",
    "//fields/src/main/java",
    "//hal/src/main/java",
    "//ntcore/src/main/java",
    "//telemetry/src/main/java",
    "//tunables/src/main/java",
    "//wpimath/src/main/java",
    "//wpilibj/src/main/java",
    "//commandsv2/src/main/java",
    "//commandsv3/src/main/java",
    "//wpiutil/src/main/java",
    "//romiVendordep/src/main/java",
    "//xrpVendordep/src/main/java",
    "//wpiunits/src/main/java",
    "//epilogue-runtime/src/main/java",
    "@bzlmodrio-opencv//libraries/java/opencv",
]

def first_level_folders(paths, prefix):
    output = {}
    for path in paths:
        rel = path[len(prefix):]
        output[rel.split("/", 1)[0]] = True
    return sorted(output.keys())

def _package_type(package_type):
    pkg_files(
        name = package_type + "-pkg",
        srcs = native.glob(["src/main/java/org/wpilib/" + package_type + "/**"]),
        strip_prefix = "src/main/java/org/wpilib",
    )

    pkgs = ["//:license_pkg_files", ":" + package_type + "-pkg"]
    if package_type in ["examples", "templates"]:
        pkg_files(
            name = package_type + "-main-pkg",
            srcs = ["src/main/java/org/wpilib/Main.java"],
            strip_prefix = "src/main/java/org/wpilib",
        )
        pkgs.append(package_type + "-main-pkg")

    if package_type == "examples":
        pkg_files(
            name = package_type + "-tests-pkg",
            srcs = native.glob(["src/test/java/org/wpilib/" + package_type + "/**"]),
            strip_prefix = "src/test/java/org/wpilib/" + package_type,
            prefix = "examples_test",
        )
        pkgs.append(package_type + "-tests-pkg")

    pkg_zip(
        name = package_type + "-zip",
        srcs = pkgs,
        tags = ["manual"],
    )

def build_java_sources():
    # The module descriptor lists all generated Epilogue services, so compile the
    # complete module once and share it across the per-folder targets.
    java_library(
        name = "examples-java",
        srcs = native.glob(["src/main/java/**/*.java"]),
        plugins = ["//epilogue-processor/src/main/java:plugin"],
        deps = _EXAMPLE_DEPS,
        tags = ["wpi-example"],
    )

def build_examples(folders, halsim_deps):
    _package_type("examples")

    for folder in folders:
        java_binary(
            name = folder + "-example",
            main_class = "org/wpilib/examples/" + folder + "/Main",
            runtime_deps = [":examples-java"],
            tags = ["wpi-example"],
        )

def build_commands(folders):
    _package_type("commands")

    for folder in folders:
        java_library(
            name = folder + "-command",
            exports = [":examples-java"],
            tags = ["wpi-example"],
        )

def build_snippets(folders):
    _package_type("snippets")

    for folder in folders:
        java_library(
            name = folder + "-snippet",
            exports = [":examples-java"],
            tags = ["wpi-example"],
        )

def build_templates(folders):
    _package_type("templates")

    for folder in folders:
        java_library(
            name = folder + "-template",
            exports = [":examples-java"],
            tags = ["wpi-example"],
        )

def build_tests(example_test_folders, snippet_test_folders):
    for folder in example_test_folders:
        wpilib_java_junit5_test(
            name = folder + "-test",
            srcs = native.glob(["src/test/java/org/wpilib/examples/" + folder + "/**/*.java"]),
            deps = [":examples-java"] + _EXAMPLE_DEPS,
            tags = ["wpi-example"],
        )

    for folder in snippet_test_folders:
        wpilib_java_junit5_test(
            name = folder + "-test",
            srcs = native.glob(["src/test/java/org/wpilib/snippets/" + folder + "/**/*.java"]),
            deps = [":examples-java"] + _EXAMPLE_DEPS,
            tags = ["wpi-example"],
        )
