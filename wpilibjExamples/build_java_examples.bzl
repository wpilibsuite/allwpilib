load("@rules_java//java:defs.bzl", "java_binary", "java_library")
load("@rules_pkg//:mappings.bzl", "pkg_files")
load("@rules_pkg//:pkg.bzl", "pkg_zip")
load("//wpilibjExamples:example_projects.bzl", "COMMANDS_V2_FOLDERS", "EXAMPLES_FOLDERS", "SNIPPETS_FOLDERS", "TEMPLATES_FOLDERS")

def _package_type(package_type):
    pkg_files(
        name = package_type + "-pkg",
        srcs = native.glob(["src/main/java/edu/wpi/first/wpilibj/" + package_type + "/**"]),
        strip_prefix = "src/main/java/edu/wpi/first/wpilibj",
    )

    pkgs = ["//:license_pkg_files", ":" + package_type + "-pkg"]
    if package_type == "examples":
        pkg_files(
            name = package_type + "-tests-pkg",
            srcs = native.glob(["src/test/java/edu/wpi/first/wpilibj/" + package_type + "/**"]),
            strip_prefix = "src/test/java/edu/wpi/first/wpilibj/" + package_type,
            prefix = "examples_test",
        )
        pkgs.append(package_type + "-tests-pkg")

    pkg_zip(
        name = package_type + "-zip",
        srcs = pkgs,
        tags = ["manual", "no-remote"],
    )

    native.genrule(
        name = package_type + "-publshing-bundle",
        srcs = [":" + package_type + "-zip"],
        outs = [package_type + "-maven-info.json"],
        cmd = "$(locations //shared/bazel/rules/publishing:generate_maven_bundle) --output_file=$(OUTS) --maven_infos $(locations :" + package_type + "-zip),edu.wpi.first.wpilibj," + package_type + ", ",
        tools = ["//shared/bazel/rules/publishing:generate_maven_bundle"],
        visibility = ["//visibility:public"],
        tags = ["manual"],
    )

def build_examples(halsim_deps):
    _package_type("examples")

    for folder in EXAMPLES_FOLDERS:
        java_binary(
            name = folder + "-example",
            srcs = native.glob(["src/main/java/edu/wpi/first/wpilibj/examples/" + folder + "/**/*.java"]),
            main_class = "edu/wpi/first/wpilibj/examples/" + folder + "/Main",
            plugins = [
                "//epilogue-processor:plugin",
            ],
            deps = [
                "//apriltag:apriltag-java",
                "//cameraserver:cameraserver-java",
                "//cscore:cscore-java",
                "//hal:hal-java",
                "//ntcore:networktables-java",
                "//wpimath:wpimath-java",
                "//wpilibj:wpilibj",
                "//wpilibNewCommands:wpilibNewCommands-java",
                "//wpiutil:wpiutil-java",
                "//romiVendordep:romi-java",
                "//xrpVendordep:xrp-java",
                "//wpiunits",
                "//epilogue-runtime:epilogue",
                "@bzlmodrio-opencv//libraries/java/opencv",
            ],
            tags = ["wpi-example"],
        )

def build_commands():
    _package_type("commands")

    for folder in COMMANDS_V2_FOLDERS:
        java_library(
            name = folder + "-command",
            srcs = native.glob(["src/main/java/edu/wpi/first/wpilibj/commands/" + folder + "/**/*.java"]),
            deps = [
                "//hal:hal-java",
                "//wpilibj:wpilibj",
                "//wpilibNewCommands:wpilibNewCommands-java",
                "//wpimath:wpimath-java",
            ],
            tags = ["wpi-example"],
        )

def build_snippets():
    _package_type("snippets")

    for folder in SNIPPETS_FOLDERS:
        java_library(
            name = folder + "-snippet",
            srcs = native.glob(["src/main/java/edu/wpi/first/wpilibj/snippets/" + folder + "/**/*.java"]),
            deps = [
                "//wpilibj:wpilibj",
            ],
            tags = ["wpi-example"],
        )

def build_templates():
    _package_type("templates")

    for folder in TEMPLATES_FOLDERS:
        java_library(
            name = folder + "-template",
            srcs = native.glob(["src/main/java/edu/wpi/first/wpilibj/templates/" + folder + "/**/*.java"]),
            deps = [
                "//hal:hal-java",
                "//wpilibj:wpilibj",
                "//wpilibNewCommands:wpilibNewCommands-java",
                "//wpimath:wpimath-java",
                "//wpiutil:wpiutil-java",
                "//xrpVendordep:xrp-java",
            ],
            tags = ["wpi-example"],
        )
