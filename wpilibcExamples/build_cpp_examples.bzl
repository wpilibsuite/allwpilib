load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("@rules_pkg//:mappings.bzl", "pkg_files")
load("@rules_pkg//:pkg.bzl", "pkg_zip")
load("//wpilibcExamples:example_projects.bzl", "COMMANDS_V2_FOLDERS", "EXAMPLE_FOLDERS", "SNIPPETS_FOLDERS", "TEMPLATES_FOLDERS", "TESTS_FOLDERS")

def _package_type(package_type):
    pkg_files(
        name = package_type + "-pkg",
        srcs = native.glob(["src/main/cpp/" + package_type + "/**"]),
        strip_prefix = "src/main/cpp",
    )

    pkgs = ["//:license_pkg_files", ":" + package_type + "-pkg"]
    if package_type == "examples":
        pkg_files(
            name = package_type + "-tests-pkg",
            srcs = native.glob(["src/test/cpp/" + package_type + "/**"]),
            strip_prefix = "src/test/cpp/" + package_type,
            prefix = "examples_test",
        )
        pkgs.append(package_type + "-tests-pkg")

    pkg_zip(
        name = package_type + "-zip",
        srcs = pkgs,
        tags = ["no-remote"],
    )

def build_examples(halsim_deps = []):
    _package_type("examples")

    for folder in EXAMPLE_FOLDERS:
        cc_library(
            name = folder + "-examples-headers",
            hdrs = native.glob(["src/main/cpp/examples/" + folder + "/include/**/*.h"], allow_empty = True),
            strip_include_prefix = "src/main/cpp/examples/" + folder + "/include",
            tags = ["wpi-example"],
        )
        cc_binary(
            name = folder + "-example",
            srcs = native.glob(["src/main/cpp/examples/" + folder + "/cpp/**/*.cpp", "src/main/cpp/examples/" + folder + "/c/**/*.c"], allow_empty = True),
            deps = [
                "//wpilibNewCommands",
                "//apriltag",
                "//romiVendordep",
                "//xrpVendordep",
                "//cameraserver",
                ":{}-examples-headers".format(folder),
            ],
            tags = ["wpi-example"],
        )

def build_commands():
    _package_type("commands")

    for folder in COMMANDS_V2_FOLDERS:
        cc_library(
            name = folder + "-command",
            srcs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.h"]),
            deps = [
                "//wpilibNewCommands",
            ],
            strip_include_prefix = "src/main/cpp/commands/" + folder,
            tags = ["wpi-example"],
        )

def build_snippets():
    _package_type("snippets")

    for folder in SNIPPETS_FOLDERS:
        cc_library(
            name = folder + "-template",
            srcs = native.glob(["src/main/cpp/snippets/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/snippets/" + folder + "/**/*.h"], allow_empty = True),
            deps = [
                "//wpilibNewCommands",
            ],
            strip_include_prefix = "src/main/cpp/snippets/" + folder + "/include",
            tags = ["wpi-example"],
        )

def build_templates():
    _package_type("templates")

    for folder in TEMPLATES_FOLDERS:
        cc_library(
            name = folder + "-template",
            srcs = native.glob(["src/main/cpp/templates/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/templates/" + folder + "/**/*.h"]),
            deps = [
                "//wpilibNewCommands",
            ],
            strip_include_prefix = "src/main/cpp/templates/" + folder + "/include",
            tags = ["wpi-example"],
        )

def build_tests():
    for folder in TESTS_FOLDERS:
        example_src_folder = "src/main/cpp/examples/" + folder
        example_test_folder = "src/test/cpp/examples/" + folder
        cc_test(
            name = folder + "-test",
            size = "small",
            srcs = native.glob([example_test_folder + "/**/*.cpp", example_src_folder + "/cpp/**/*.cpp", example_src_folder + "/c/**/*.c"], allow_empty = True),
            deps = [
                "//wpilibNewCommands",
                ":{}-examples-headers".format(folder),
                "//thirdparty/googletest",
            ],
            defines = ["RUNNING_FRC_TESTS=1"],
            tags = ["wpi-example", "no-tsan", "no-asan", "no-ubsan", "exclusive"],
        )
