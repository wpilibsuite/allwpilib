load("@rules_java//java:defs.bzl", "java_binary", "java_library")
load("//shared/bazel/rules:java_rules.bzl", "wpilib_java_junit5_test")
load("//wpilibjExamples:example_projects.bzl", "COMMANDS_V2_FOLDERS", "EXAMPLES_FOLDERS", "SNIPPETS_FOLDERS", "TEMPLATES_FOLDERS", "TEST_FOLDERS")

def build_examples(halsim_deps):
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

def build_tests():
    for folder in TEST_FOLDERS:
        wpilib_java_junit5_test(
            name = folder + "-test",
            srcs = native.glob(["src/test/java/edu/wpi/first/wpilibj/examples/" + folder + "/**/*.java"]),
            deps = [
                ":" + folder + "-example",
                "//hal:hal-java",
                "//ntcore:networktables-java",
                "//wpilibj:wpilibj",
                "//wpilibNewCommands:wpilibNewCommands-java",
                "//wpimath:wpimath-java",
                "//wpiutil:wpiutil-java",
            ],
            tags = ["wpi-example"],
        )
