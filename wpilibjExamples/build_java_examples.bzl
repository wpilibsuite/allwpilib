load("@rules_java//java:defs.bzl", "java_binary", "java_library")
load("@rules_pkg//:mappings.bzl", "pkg_files")
load("@rules_pkg//:pkg.bzl", "pkg_zip")
load("//shared/bazel/rules:java_rules.bzl", "wpilib_java_junit5_test")
load("//wpilibjExamples:example_projects.bzl", "COMMANDS_V2_FOLDERS", "EXAMPLE_FOLDERS", "EXAMPLE_TESTS_FOLDERS", "SNIPPET_FOLDERS", "SNIPPET_TESTS_FOLDERS", "TEMPLATE_FOLDERS")

def _package_type(package_type):
    pkg_files(
        name = package_type + "-pkg",
        srcs = native.glob(["src/main/java/org/wpilib/" + package_type + "/**"]),
        strip_prefix = "src/main/java/org/wpilib",
    )

    pkgs = ["//:license_pkg_files", ":" + package_type + "-pkg"]
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

def build_examples(halsim_deps):
    _package_type("examples")

    for folder in EXAMPLE_FOLDERS:
        java_binary(
            name = folder + "-example",
            srcs = native.glob(["src/main/java/org/wpilib/examples/" + folder + "/**/*.java"]),
            main_class = "org/wpilib/examples/" + folder + "/Main",
            plugins = [
                "//epilogue-processor:plugin",
            ],
            deps = [
                "//apriltag:apriltag-java",
                "//cameraserver:cameraserver-java",
                "//cscore:cscore-java",
                "//hal:hal-java",
                "//ntcore:ntcore-java",
                "//wpimath:wpimath-java",
                "//wpilibj:wpilibj-java",
                "//commandsv2:commandsv2-java",
                "//wpiutil:wpiutil-java",
                "//romiVendordep:romiVendordep-java",
                "//xrpVendordep:xrpVendordep-java",
                "//wpiunits:wpiunits-java",
                "//epilogue-runtime:epilogue-java",
                "@bzlmodrio-opencv//libraries/java/opencv",
            ],
            tags = ["wpi-example"],
        )

def build_commands():
    _package_type("commands")

    for folder in COMMANDS_V2_FOLDERS:
        java_library(
            name = folder + "-command",
            srcs = native.glob(["src/main/java/org/wpilib/commands/" + folder + "/**/*.java"]),
            deps = [
                "//hal:hal-java",
                "//wpilibj:wpilibj-java",
                "//commandsv2:commandsv2-java",
                "//wpimath:wpimath-java",
            ],
            tags = ["wpi-example"],
        )

def build_snippets():
    _package_type("snippets")

    for folder in SNIPPET_FOLDERS:
        java_library(
            name = folder + "-snippet",
            srcs = native.glob(["src/main/java/org/wpilib/snippets/" + folder + "/**/*.java"]),
            deps = [
                "//apriltag:apriltag-java",
                "//cameraserver:cameraserver-java",
                "//cscore:cscore-java",
                "//hal:hal-java",
                "//ntcore:ntcore-java",
                "//wpimath:wpimath-java",
                "//wpilibj:wpilibj-java",
                "//commandsv2:commandsv2-java",
                "//wpiutil:wpiutil-java",
                "//romiVendordep:romiVendordep-java",
                "//xrpVendordep:xrpVendordep-java",
                "//wpiunits:wpiunits-java",
                "//epilogue-runtime:epilogue-java",
                "@bzlmodrio-opencv//libraries/java/opencv",
            ],
            tags = ["wpi-example"],
        )

def build_templates():
    _package_type("templates")

    for folder in TEMPLATE_FOLDERS:
        java_library(
            name = folder + "-template",
            srcs = native.glob(["src/main/java/org/wpilib/templates/" + folder + "/**/*.java"]),
            deps = [
                "//hal:hal-java",
                "//wpilibj:wpilibj-java",
                "//commandsv2:commandsv2-java",
                "//wpimath:wpimath-java",
                "//wpiutil:wpiutil-java",
                "//xrpVendordep:xrpVendordep-java",
            ],
            tags = ["wpi-example"],
        )

def build_tests():
    for folder in EXAMPLE_TESTS_FOLDERS:
        wpilib_java_junit5_test(
            name = folder + "-test",
            srcs = native.glob(["src/test/java/org/wpilib/examples/" + folder + "/**/*.java"]),
            deps = [
                ":" + folder + "-example",
                "//hal:hal-java",
                "//ntcore:ntcore-java",
                "//wpilibj:wpilibj-java",
                "//commandsv2:commandsv2-java",
                "//wpimath:wpimath-java",
                "//wpiutil:wpiutil-java",
            ],
            tags = ["wpi-example"],
        )

    for folder in SNIPPET_TESTS_FOLDERS:
        wpilib_java_junit5_test(
            name = folder + "-test",
            srcs = native.glob(["src/test/java/org/wpilib/snippets/" + folder + "/**/*.java"]),
            deps = [
                ":" + folder + "-snippet",
                "//hal:hal-java",
                "//ntcore:ntcore-java",
                "//wpilibj:wpilibj-java",
                "//commandsv2:commandsv2-java",
                "//wpimath:wpimath-java",
                "//wpiutil:wpiutil-java",
            ],
            tags = ["wpi-example"],
        )
