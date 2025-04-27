load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("//wpilibcExamples:example_projects.bzl", "COMMANDS_V2_FOLDERS", "EXAMPLE_FOLDERS", "SNIPPETS_FOLDERS", "TEMPLATES_FOLDERS", "TESTS_FOLDERS")

def build_examples(halsim_deps = []):
    for folder in EXAMPLE_FOLDERS:
        cc_library(
            name = folder + "-examples-headers",
            hdrs = native.glob(["src/main/cpp/examples/" + folder + "/include/**/*.h"]),
            strip_include_prefix = "src/main/cpp/examples/" + folder + "/include",
            tags = ["wpi-example"],
        )
        cc_binary(
            name = folder + "-example",
            srcs = native.glob(["src/main/cpp/examples/" + folder + "/cpp/**/*.cpp", "src/main/cpp/examples/" + folder + "/c/**/*.c"]),
            deps = [
                "//wpilibNewCommands:wpilibNewCommands.static",
                "//apriltag:apriltag.static",
                "//romiVendordep:romi-cpp.static",
                "//xrpVendordep:xrp-cpp",
                ":{}-examples-headers".format(folder),
            ],
            tags = ["wpi-example"],
        )

def build_commands():
    for folder in COMMANDS_V2_FOLDERS:
        cc_library(
            name = folder + "-command",
            srcs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.h"]),
            deps = [
                "//wpilibNewCommands:wpilibNewCommands.static",
            ],
            strip_include_prefix = "src/main/cpp/commands/" + folder,
            tags = ["wpi-example"],
        )

def build_snippets():
    for folder in SNIPPETS_FOLDERS:
        cc_library(
            name = folder + "-template",
            srcs = native.glob(["src/main/cpp/snippets/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/snippets/" + folder + "/**/*.h"]),
            deps = [
                "//wpilibNewCommands:wpilibNewCommands.static",
            ],
            strip_include_prefix = "src/main/cpp/snippets/" + folder + "/include",
            tags = ["wpi-example"],
        )

def build_templates():
    for folder in TEMPLATES_FOLDERS:
        cc_library(
            name = folder + "-template",
            srcs = native.glob(["src/main/cpp/templates/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/templates/" + folder + "/**/*.h"]),
            deps = [
                "//wpilibNewCommands:wpilibNewCommands.static",
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
            srcs = native.glob([example_test_folder + "/**/*.cpp", example_src_folder + "/cpp/**/*.cpp", example_src_folder + "/c/**/*.c"]),
            deps = [
                "//wpilibNewCommands:wpilibNewCommands.static",
                ":{}-examples-headers".format(folder),
                "//thirdparty/googletest:googletest.static",
            ],
            defines = ["RUNNING_FRC_TESTS=1"],
            tags = ["wpi-example", "no-tsan", "no-asan", "no-ubsan", "exclusive"],
        )
