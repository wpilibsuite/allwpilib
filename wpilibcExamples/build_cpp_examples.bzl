load("//shared/bazel/rules:cc_rules.bzl", "wpilib_cc_binary", "wpilib_cc_library")

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
    "DigitalCommunication",
    "DMA",
    "DriveDistanceOffboard",
    "DutyCycleEncoder",
    "DutyCycleInput",
    "ElevatorProfiledPID",
    "ElevatorSimulation",
    "ElevatorTrapezoidProfile",
    "Encoder",
    "EventLoop",
    "Frisbeebot",
    "GearsBot",
    "GettingStarted",
    "Gyro",
    "GyroDriveCommands",
    "GyroMecanum",
    # "HAL",
    "HatchbotInlined",
    "HatchbotTraditional",
    "HidRumble",
    "I2CCommunication",
    "IntermediateVision",
    "MecanumBot",
    "MecanumControllerCommand",
    "MecanumDrive",
    "MecanumDrivePoseEstimator",
    "Mechanism2d",
    "MotorControl",
    "MotorControlEncoder",
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
    "TankDrive",
    "TankDriveXboxController",
    "Ultrasonic",
    "UltrasonicPID",
]

COMMANDS_V2_FOLDERS = [
    "command2",
    "emptyclass",
    "instantcommand",
    "parallelcommandgroup",
    "paralleldeadlinegroup",
    "parallelracegroup",
    "pidcommand",
    "pidsubsystem2",
    "profiledpidcommand",
    "profiledpidsubsystem",
    "sequentialcommandgroup",
    "subsystem2",
    "trapezoidprofilecommand",
    "trapezoidprofilesubsystem",
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

        wpilib_cc_binary(
            name = folder + "-example",
            srcs = native.glob(["src/main/cpp/examples/" + folder + "/cpp/**/*.cpp", "src/main/cpp/examples/" + folder + "/c/**/*.c"]),
            deps = [
                "//wpilibNewCommands/src/main/native:wpilibNewCommands.shared",
                ":{}-examples-headers".format(folder),
            ],
            tags = ["wpi-example"],
        )

def build_commands():
    for folder in COMMANDS_V2_FOLDERS:
        wpilib_cc_library(
            name = folder + "-command",
            srcs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.cpp"]),
            hdrs = native.glob(["src/main/cpp/commands/" + folder + "/**/*.h"]),
            deps = [
                "//wpilibNewCommands/src/main/native:wpilibNewCommands.shared",
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
            deps = [
                "//wpilibNewCommands/src/main/native:wpilibNewCommands.shared",
            ],
            strip_include_prefix = "src/main/cpp/templates/" + folder + "/include",
            tags = ["wpi-example"],
        )
