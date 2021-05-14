load("@wpi_bazel_rules//rules:cc.bzl", "wpilib_cc_library")
load("@wpi_bazel_rules//rules:halsim_binary.bzl", "wpilib_cc_halsim_binary")

EXAMPLE_FOLDERS = [
    "AddressableLED",
    "ArcadeDrive",
    "ArcadeDriveXboxController",
    "ArmBot",
    "ArmBotOffboard",
    "ArmSimulation",
    "AxisCameraSample",
    "CANPDP",
    "DifferentialDriveBot",
    "DifferentialDrivePoseEstimator",
    "DMA",
    "DriveDistanceOffboard",
    "DutyCycleEncoder",
    "DutyCycleInput",
    "ElevatorProfiledPID",
    "ElevatorSimulation",
    "ElevatorTrapezoidProfile",
    "Encoder",
    "Frisbeebot",
    "GearsBot",
    "GettingStarted",
    "Gyro",
    "GyroDriveCommands",
    "GyroMecanum",
    "HatchbotInlined",
    "HatchbotTraditional",
    "HidRumble",
    "IntermediateVision",
    "MecanumBot",
    "MecanumControllerCommand",
    "MecanumDrive",
    "MecanumDrivePoseEstimator",
    "Mechanism2d",
    "MotorControl",
    "MotorControlEncoder",
    "PacGoat",
    "PotentiometerPID",
    "QuickVision",
    "RamseteCommand",
    "RamseteController",
    "Relay",
    "RomiReference",
    "SchedulerEventLogging",
    "SelectCommand",
    "ShuffleBoard",
    "SimpleDifferentialDriveSimulation",
    "Solenoid",
    "StateSpaceArm",
    "StateSpaceDifferentialDriveSimulation",
    "StateSpaceElevator",
    "StateSpaceFlywheel",
    "StateSpaceFlywheelSysId",
    "SwerveBot",
    "SwerveControllerCommand",
    "SwerveDrivePoseEstimator",
    "TankDriveXboxController",
    "Ultrasonic",
    "UltrasonicPID",
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
    "oldcommandbased",
    "robotbaseskeleton",
    "timed",
    "timedskeleton",
]

def build_examples(halsim_deps = []):
    for folder in EXAMPLE_FOLDERS:
        wpilib_cc_library(
            name = folder + "-examples-headers",
            hdrs = native.glob(["src/main/cpp/examples/" + folder + "/include/**/*.h"]),
            strip_include_prefix = "src/main/cpp/examples/" + folder + "/include",
            tags = ["wpi-example"],
        )

        wpilib_cc_halsim_binary(
            name = folder + "-example",
            srcs = native.glob(["src/main/cpp/examples/" + folder + "/cpp/**/*.cpp"]),
            #            hdrs = native.glob(["src/main/cpp/examples/" + folder + "/include/**/*.h"]),
            raw_deps = [
                "//wpilibNewCommands:cpp",
                "//wpilibOldCommands:cpp",
                folder + "-examples-headers",
            ],
            wpi_shared_deps = [
                "//wpilibc:wpilibc",
            ],
            tags = ["wpi-example"],
            halsim_deps = halsim_deps,
        )

def build_commands():
    for folder in COMMANDS_FOLDERS:
        wpilib_cc_library(
            name = folder + "-command",
            srcs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.h"]),
            raw_deps = [
                "//wpilibc:wpilibc",
                "//wpilibNewCommands:cpp",
                "//wpilibOldCommands:cpp",
            ],
            strip_include_prefix = "src/main/cpp/commands/" + folder,
            tags = ["wpi-example"],
        )

def build_templates():
    for folder in TEMPLATES_FOLDERS:
        wpilib_cc_library(
            name = folder + "-template",
            srcs = native.glob(["src/main/cpp/templates/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/templates/" + folder + "/**/*.h"]),
            raw_deps = [
                "//wpilibc:wpilibc",
                "//wpilibNewCommands:cpp",
                "//wpilibOldCommands:cpp",
            ],
            strip_include_prefix = "src/main/cpp/templates/" + folder + "/include",
            tags = ["wpi-example"],
        )
