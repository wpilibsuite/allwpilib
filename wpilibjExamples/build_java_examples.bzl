load("@rules_java//java:defs.bzl", "java_library")
load("@wpi_bazel_rules//rules:halsim_binary.bzl", "wpilib_java_halsim_binary")

EXAMPLES_FOLDERS = [
    "addressableled",
    "arcadedrive",
    "arcadedrivexboxcontroller",
    "armbot",
    "armbotoffboard",
    "armsimulation",
    "axiscamera",
    "canpdp",
    "differentialdrivebot",
    "differentialdriveposeestimator",
    "drivedistanceoffboard",
    "dutycycleencoder",
    "dutycycleinput",
    "elevatorprofiledpid",
    "elevatorsimulation",
    "elevatortrapezoidprofile",
    "encoder",
    "frisbeebot",
    "gearsbot",
    "gettingstarted",
    "gyro",
    "gyrodrivecommands",
    "gyromecanum",
    "hatchbotinlined",
    "hatchbottraditional",
    "hidrumble",
    "intermediatevision",
    "mecanumbot",
    "mecanumcontrollercommand",
    "mecanumdrive",
    "mecanumdriveposeestimator",
    "mechanism2d",
    "motorcontrol",
    "motorcontrolencoder",
    "pacgoat",
    "potentiometerpid",
    "quickvision",
    "ramsetecommand",
    "ramsetecontroller",
    "relay",
    "romireference",
    "schedulereventlogging",
    "selectcommand",
    "shuffleboard",
    "simpledifferentialdrivesimulation",
    "solenoid",
    "statespacearm",
    "statespacedifferentialdrivesimulation",
    "statespaceelevator",
    "statespaceflywheel",
    "statespaceflywheelsysid",
    "swervebot",
    "swervecontrollercommand",
    "swervesdriveposeestimator",
    "tankdrive",
    "tankdrivexboxcontroller",
    "ultrasonic",
    "ultrasonicpid",
]

COMMANDS_FOLDERS = [
    "command",
    "command2",
    "commandgroup",
    "emptyclass",
    "instant",
    "instantcommand",
    "parallelcommandgroup",
    "paralleldeadlinegroup",
    "parallelracegroup",
    "pidcommand",
    "pidsubsystem",
    "pidsubsystem2",
    "profiledpidcommand",
    "profiledpidsubsystem",
    "sequentialcommandgroup",
    "subsystem",
    "subsystem2",
    "timed",
    "trapezoidprofilecommand",
    "trapezoidprofilesubsystem",
    "trigger",
]

TEMPLATES_FOLDERS = [
    "commandbased",
    "educational",
    "oldcommandbased",
    "robotbaseskeleton",
    "romicommandbased",
    "romitimed",
    "timed",
    "timedskeleton",
]

def build_examples(halsim_deps):
    for folder in EXAMPLES_FOLDERS:
        java_library(
            name = folder + "-example-lib",
            srcs = native.glob(["src/main/java/edu/wpi/first/wpilibj/examples/" + folder + "/**/*.java"]),
            deps = [
                "//wpilibj:java",
                "//wpilibNewCommands:java",
                "//wpilibOldCommands:java",
            ],
            tags = ["wpi-example"],
        )

        wpilib_java_halsim_binary(
            name = folder + "-example",
            main_class = "edu/wpi/first/wpilibj/examples/" + folder + "/Main",
            runtime_deps = [":" + folder + "-example-lib"],
            wpi_shared_deps = [
                "//cscore:cscorejni",
                "//cscore:cscore",
                "//ntcore:ntcorejni",
                "//ntcore:ntcore",
                "//wpimath:wpimathjni",
                "//wpimath:wpimath",
                "//wpiutil:wpiutiljni",
                "//wpiutil:wpiutil",
                "//hal:wpiHaljni",
                "//hal:wpiHal",
            ],
            halsim_deps = halsim_deps,
            raw_jni_deps = ["@local_opencv//:opencv-cpp-shared-libs"],
            tags = ["wpi-example"],
        )

def build_commands():
    for folder in COMMANDS_FOLDERS:
        java_library(
            name = folder + "-example",
            srcs = native.glob(["src/main/java/edu/wpi/first/wpilibj/commands/" + folder + "/**/*.java"]),
            deps = [
                "//wpilibj:java",
                "//wpilibNewCommands:java",
                "//wpilibOldCommands:java",
            ],
            tags = ["wpi-example"],
        )

def build_templates():
    for folder in TEMPLATES_FOLDERS:
        java_library(
            name = folder + "-template",
            srcs = native.glob(["src/main/java/edu/wpi/first/wpilibj/templates/" + folder + "/**/*.java"]),
            deps = [
                "//wpilibj:java",
                "//wpilibNewCommands:java",
                "//wpilibOldCommands:java",
            ],
            tags = ["wpi-example"],
        )
