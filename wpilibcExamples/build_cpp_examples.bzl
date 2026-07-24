load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("@rules_pkg//:mappings.bzl", "pkg_files")
load("@rules_pkg//:pkg.bzl", "pkg_zip")

def first_level_folders(paths, prefix):
    output = {}
    for path in paths:
        rel = path[len(prefix):]
        output[rel.split("/", 1)[0]] = True
    return sorted(output.keys())

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
        tags = ["manual"],
    )

def build_examples(folders, halsim_deps = []):
    _package_type("examples")

    for folder in folders:
        cc_library(
            name = folder + "-examples-headers",
            hdrs = native.glob(["src/main/cpp/examples/" + folder + "/include/**/*.hpp"], allow_empty = True),
            strip_include_prefix = "src/main/cpp/examples/" + folder + "/include",
            tags = ["wpi-example"],
        )
        cc_binary(
            name = folder + "-example",
            srcs = native.glob(["src/main/cpp/examples/" + folder + "/cpp/**/*.cpp", "src/main/cpp/examples/" + folder + "/c/**/*.c"], allow_empty = True),
            deps = [
                "//apriltag",
                "//commandsv2",
                "//romiVendordep",
                "//xrpVendordep",
                ":{}-examples-headers".format(folder),
            ],
            tags = ["wpi-example"],
        )

def build_commands(folders):
    _package_type("commands")

    for folder in folders:
        cc_library(
            name = folder + "-command",
            srcs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.hpp"]),
            deps = [
                "//commandsv2",
            ],
            strip_include_prefix = "src/main/cpp/commands/" + folder,
            tags = ["wpi-example"],
        )

def build_snippets(folders):
    _package_type("snippets")

    for folder in folders:
        cc_library(
            name = folder + "-snippets-headers",
            hdrs = native.glob(["src/main/cpp/snippets/" + folder + "/include/**/*.hpp"], allow_empty = True),
            strip_include_prefix = "src/main/cpp/snippets/" + folder + "/include",
            tags = ["wpi-example"],
        )
        cc_library(
            name = folder + "-snippet",
            srcs = native.glob(["src/main/cpp/snippets/" + folder + "/**/*.cpp"]),
            deps = [
                "//apriltag",
                "//commandsv2",
                "//cameraserver",
                ":{}-snippets-headers".format(folder),
            ],
            strip_include_prefix = "src/main/cpp/snippets/" + folder + "/include",
            tags = ["wpi-example"],
        )

def build_templates(folders):
    _package_type("templates")

    for folder in folders:
        cc_library(
            name = folder + "-template",
            srcs = native.glob(["src/main/cpp/templates/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/templates/" + folder + "/**/*.hpp"]),
            deps = [
                "//commandsv2",
            ],
            strip_include_prefix = "src/main/cpp/templates/" + folder + "/include",
            tags = ["wpi-example"],
        )

def build_tests(example_test_folders, snippet_test_folders):
    for folder in example_test_folders:
        example_src_folder = "src/main/cpp/examples/" + folder
        example_test_folder = "src/test/cpp/examples/" + folder
        cc_test(
            name = folder + "-test",
            size = "small",
            srcs = native.glob([example_test_folder + "/**/*.cpp", example_src_folder + "/cpp/**/*.cpp", example_src_folder + "/c/**/*.c"], allow_empty = True),
            deps = [
                "//commandsv2",
                ":{}-examples-headers".format(folder),
                "//thirdparty/googletest",
            ],
            defines = ["RUNNING_WPILIB_TESTS=1"],
            tags = ["wpi-example", "no-tsan", "no-asan", "no-ubsan", "exclusive"],
        )
    for folder in snippet_test_folders:
        snippet_src_folder = "src/main/cpp/snippets/" + folder
        snippet_test_folder = "src/test/cpp/snippets/" + folder
        cc_test(
            name = folder + "-test",
            size = "small",
            srcs = native.glob([snippet_test_folder + "/**/*.cpp", snippet_src_folder + "/**/*.cpp"], allow_empty = True),
            deps = [
                "//commandsv2",
                ":{}-snippets-headers".format(folder),
                "//thirdparty/googletest",
            ],
            defines = ["RUNNING_WPILIB_TESTS=1"],
            tags = ["wpi-example", "no-tsan", "no-asan", "no-ubsan", "exclusive"],
        )
