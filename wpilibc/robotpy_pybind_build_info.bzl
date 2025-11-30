# THIS FILE IS AUTO GENERATED

load("//shared/bazel/rules/gen:gen-version-file.bzl", "generate_version_file")
load("//shared/bazel/rules/robotpy:pybind_rules.bzl", "create_pybind_library", "robotpy_library")
load("//shared/bazel/rules/robotpy:semiwrap_helpers.bzl", "gen_libinit", "gen_modinit_hpp", "gen_pkgconf", "resolve_casters", "run_header_gen")
load("//shared/bazel/rules/robotpy:semiwrap_tool_helpers.bzl", "scan_headers", "update_yaml_files")

def wpilib_event_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPILIB_EVENT_HEADER_GEN = [
        struct(
            class_name = "BooleanEvent",
            yml_file = "semiwrap/event/BooleanEvent.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/event/BooleanEvent.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::BooleanEvent", "wpi__BooleanEvent.hpp"),
            ],
        ),
        struct(
            class_name = "EventLoop",
            yml_file = "semiwrap/event/EventLoop.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/event/EventLoop.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::EventLoop", "wpi__EventLoop.hpp"),
            ],
        ),
        struct(
            class_name = "NetworkBooleanEvent",
            yml_file = "semiwrap/event/NetworkBooleanEvent.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/event/NetworkBooleanEvent.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::NetworkBooleanEvent", "wpi__NetworkBooleanEvent.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpilib_event.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "wpilib_event.casters.pkl",
        dep_file = "wpilib_event.casters.d",
    )

    gen_libinit(
        name = "wpilib_event.gen_lib_init",
        output_file = "src/main/python/wpilib/event/_init__event.py",
        modules = ["native.wpilib._init_robotpy_native_wpilib", "wpimath.filter._init__filter"],
    )

    gen_pkgconf(
        name = "wpilib_event.gen_pkgconf",
        libinit_py = "wpilib.event._init__event",
        module_pkg_name = "wpilib.event._event",
        output_file = "wpilib_event.pc",
        pkg_name = "wpilib_event",
        install_path = "src/main/python/wpilib/event",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpilib/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpilib_event.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPILIB_EVENT_HEADER_GEN],
        libname = "_event",
        output_file = "semiwrap_init.wpilib.event._event.hpp",
    )

    run_header_gen(
        name = "wpilib_event",
        casters_pickle = "wpilib_event.casters.pkl",
        header_gen_config = WPILIB_EVENT_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpilib/event",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpilib_event",
        install_path = "src/main/python/wpilib/event/",
        extension_name = "_event",
        generated_srcs = [":wpilib_event.generated_srcs"],
        semiwrap_header = [":wpilib_event.gen_modinit_hpp"],
        deps = [
            ":wpilib_event.tmpl_hdrs",
            ":wpilib_event.trampoline_hdrs",
            "//wpilibc:wpilibc",
            "//wpimath:wpimath",
            "//wpimath:wpimath_filter_pybind_library",
        ],
        dynamic_deps = [
            "//wpilibc:shared/wpilibc",
            "//wpimath:shared/wpimath",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpilib_event.generated_files",
        srcs = [
            "wpilib_event.gen_modinit_hpp.gen",
            "wpilib_event.header_gen_files",
            "wpilib_event.gen_pkgconf",
            "wpilib_event.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpilib_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPILIB_HEADER_GEN = [
        struct(
            class_name = "Filesystem",
            yml_file = "semiwrap/Filesystem.yml",
            header_root = "wpilibc/src/main/python/wpilib/src",
            header_file = "wpilibc/src/main/python/wpilib/src/rpy/Filesystem.h",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "MotorControllerGroup",
            yml_file = "semiwrap/MotorControllerGroup.yml",
            header_root = "wpilibc/src/main/python/wpilib/src",
            header_file = "wpilibc/src/main/python/wpilib/src/rpy/MotorControllerGroup.h",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PyMotorControllerGroup", "wpi__PyMotorControllerGroup.hpp"),
            ],
        ),
        struct(
            class_name = "Notifier",
            yml_file = "semiwrap/Notifier.yml",
            header_root = "wpilibc/src/main/python/wpilib/src",
            header_file = "wpilibc/src/main/python/wpilib/src/rpy/Notifier.h",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PyNotifier", "wpi__PyNotifier.hpp"),
            ],
        ),
        struct(
            class_name = "ExpansionHub",
            yml_file = "semiwrap/ExpansionHub.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/ExpansionHub.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::ExpansionHub", "wpi__ExpansionHub.hpp"),
            ],
        ),
        struct(
            class_name = "ExpansionHubMotor",
            yml_file = "semiwrap/ExpansionHubMotor.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/ExpansionHubMotor.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::ExpansionHubMotor", "wpi__ExpansionHubMotor.hpp"),
            ],
        ),
        struct(
            class_name = "ExpansionHubPidConstants",
            yml_file = "semiwrap/ExpansionHubPidConstants.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/ExpansionHubPidConstants.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::ExpansionHubPidConstants", "wpi__ExpansionHubPidConstants.hpp"),
            ],
        ),
        struct(
            class_name = "ExpansionHubServo",
            yml_file = "semiwrap/ExpansionHubServo.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/ExpansionHubServo.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::ExpansionHubServo", "wpi__ExpansionHubServo.hpp"),
            ],
        ),
        struct(
            class_name = "DSControlWord",
            yml_file = "semiwrap/DSControlWord.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/driverstation/DSControlWord.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::DSControlWord", "wpi__DSControlWord.hpp"),
            ],
        ),
        struct(
            class_name = "DriverStation",
            yml_file = "semiwrap/DriverStation.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/driverstation/DriverStation.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::DriverStation", "wpi__DriverStation.hpp"),
                ("wpi::DriverStation::TouchpadFinger", "wpi__DriverStation__TouchpadFinger.hpp"),
            ],
        ),
        struct(
            class_name = "Gamepad",
            yml_file = "semiwrap/Gamepad.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/driverstation/Gamepad.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Gamepad", "wpi__Gamepad.hpp"),
                ("wpi::Gamepad::Button", "wpi__Gamepad__Button.hpp"),
                ("wpi::Gamepad::Axis", "wpi__Gamepad__Axis.hpp"),
            ],
        ),
        struct(
            class_name = "GenericHID",
            yml_file = "semiwrap/GenericHID.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/driverstation/GenericHID.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::GenericHID", "wpi__GenericHID.hpp"),
            ],
        ),
        struct(
            class_name = "Joystick",
            yml_file = "semiwrap/Joystick.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/driverstation/Joystick.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Joystick", "wpi__Joystick.hpp"),
            ],
        ),
        struct(
            class_name = "PS4Controller",
            yml_file = "semiwrap/PS4Controller.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/driverstation/PS4Controller.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PS4Controller", "wpi__PS4Controller.hpp"),
                ("wpi::PS4Controller::Button", "wpi__PS4Controller__Button.hpp"),
                ("wpi::PS4Controller::Axis", "wpi__PS4Controller__Axis.hpp"),
            ],
        ),
        struct(
            class_name = "PS5Controller",
            yml_file = "semiwrap/PS5Controller.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/driverstation/PS5Controller.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PS5Controller", "wpi__PS5Controller.hpp"),
                ("wpi::PS5Controller::Button", "wpi__PS5Controller__Button.hpp"),
                ("wpi::PS5Controller::Axis", "wpi__PS5Controller__Axis.hpp"),
            ],
        ),
        struct(
            class_name = "StadiaController",
            yml_file = "semiwrap/StadiaController.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/driverstation/StadiaController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::StadiaController", "wpi__StadiaController.hpp"),
                ("wpi::StadiaController::Button", "wpi__StadiaController__Button.hpp"),
                ("wpi::StadiaController::Axis", "wpi__StadiaController__Axis.hpp"),
            ],
        ),
        struct(
            class_name = "XboxController",
            yml_file = "semiwrap/XboxController.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/driverstation/XboxController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::XboxController", "wpi__XboxController.hpp"),
                ("wpi::XboxController::Button", "wpi__XboxController__Button.hpp"),
                ("wpi::XboxController::Axis", "wpi__XboxController__Axis.hpp"),
            ],
        ),
        struct(
            class_name = "IterativeRobotBase",
            yml_file = "semiwrap/IterativeRobotBase.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/framework/IterativeRobotBase.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::IterativeRobotBase", "wpi__IterativeRobotBase.hpp"),
            ],
        ),
        struct(
            class_name = "RobotBase",
            yml_file = "semiwrap/RobotBase.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/framework/RobotBase.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::RobotBase", "wpi__RobotBase.hpp"),
            ],
        ),
        struct(
            class_name = "RobotState",
            yml_file = "semiwrap/RobotState.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/framework/RobotState.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::RobotState", "wpi__RobotState.hpp"),
            ],
        ),
        struct(
            class_name = "TimedRobot",
            yml_file = "semiwrap/TimedRobot.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/framework/TimedRobot.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::TimedRobot", "wpi__TimedRobot.hpp"),
            ],
        ),
        struct(
            class_name = "TimesliceRobot",
            yml_file = "semiwrap/TimesliceRobot.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/framework/TimesliceRobot.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::TimesliceRobot", "wpi__TimesliceRobot.hpp"),
            ],
        ),
        struct(
            class_name = "ADXL345_I2C",
            yml_file = "semiwrap/ADXL345_I2C.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/accelerometer/ADXL345_I2C.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::ADXL345_I2C", "wpi__ADXL345_I2C.hpp"),
                ("wpi::ADXL345_I2C::AllAxes", "wpi__ADXL345_I2C__AllAxes.hpp"),
            ],
        ),
        struct(
            class_name = "AnalogAccelerometer",
            yml_file = "semiwrap/AnalogAccelerometer.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/accelerometer/AnalogAccelerometer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::AnalogAccelerometer", "wpi__AnalogAccelerometer.hpp"),
            ],
        ),
        struct(
            class_name = "CAN",
            yml_file = "semiwrap/CAN.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/bus/CAN.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::CAN", "wpi__CAN.hpp"),
            ],
        ),
        struct(
            class_name = "I2C",
            yml_file = "semiwrap/I2C.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/bus/I2C.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::I2C", "wpi__I2C.hpp"),
            ],
        ),
        struct(
            class_name = "SerialPort",
            yml_file = "semiwrap/SerialPort.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/bus/SerialPort.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SerialPort", "wpi__SerialPort.hpp"),
            ],
        ),
        struct(
            class_name = "AnalogInput",
            yml_file = "semiwrap/AnalogInput.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/discrete/AnalogInput.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::AnalogInput", "wpi__AnalogInput.hpp"),
            ],
        ),
        struct(
            class_name = "CounterBase",
            yml_file = "semiwrap/CounterBase.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/discrete/CounterBase.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::CounterBase", "wpi__CounterBase.hpp"),
            ],
        ),
        struct(
            class_name = "DigitalInput",
            yml_file = "semiwrap/DigitalInput.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/discrete/DigitalInput.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::DigitalInput", "wpi__DigitalInput.hpp"),
            ],
        ),
        struct(
            class_name = "DigitalOutput",
            yml_file = "semiwrap/DigitalOutput.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/discrete/DigitalOutput.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::DigitalOutput", "wpi__DigitalOutput.hpp"),
            ],
        ),
        struct(
            class_name = "PWM",
            yml_file = "semiwrap/PWM.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/discrete/PWM.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWM", "wpi__PWM.hpp"),
            ],
        ),
        struct(
            class_name = "OnboardIMU",
            yml_file = "semiwrap/OnboardIMU.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/imu/OnboardIMU.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::OnboardIMU", "wpi__OnboardIMU.hpp"),
            ],
        ),
        struct(
            class_name = "AddressableLED",
            yml_file = "semiwrap/AddressableLED.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/led/AddressableLED.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::AddressableLED", "wpi__AddressableLED.hpp"),
                ("wpi::AddressableLED::LEDData", "wpi__AddressableLED__LEDData.hpp"),
            ],
        ),
        struct(
            class_name = "LEDPattern",
            yml_file = "semiwrap/LEDPattern.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/led/LEDPattern.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::LEDPattern", "wpi__LEDPattern.hpp"),
                ("wpi::LEDPattern::LEDReader", "wpi__LEDPattern__LEDReader.hpp"),
            ],
        ),
        struct(
            class_name = "Koors40",
            yml_file = "semiwrap/Koors40.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/Koors40.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Koors40", "wpi__Koors40.hpp"),
            ],
        ),
        struct(
            class_name = "MotorController",
            yml_file = "semiwrap/MotorController.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/MotorController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::MotorController", "wpi__MotorController.hpp"),
            ],
        ),
        struct(
            class_name = "MotorSafety",
            yml_file = "semiwrap/MotorSafety.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/MotorSafety.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::MotorSafety", "wpi__MotorSafety.hpp"),
            ],
        ),
        struct(
            class_name = "PWMMotorController",
            yml_file = "semiwrap/PWMMotorController.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/PWMMotorController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMMotorController", "wpi__PWMMotorController.hpp"),
            ],
        ),
        struct(
            class_name = "PWMSparkFlex",
            yml_file = "semiwrap/PWMSparkFlex.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/PWMSparkFlex.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMSparkFlex", "wpi__PWMSparkFlex.hpp"),
            ],
        ),
        struct(
            class_name = "PWMSparkMax",
            yml_file = "semiwrap/PWMSparkMax.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/PWMSparkMax.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMSparkMax", "wpi__PWMSparkMax.hpp"),
            ],
        ),
        struct(
            class_name = "PWMTalonFX",
            yml_file = "semiwrap/PWMTalonFX.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/PWMTalonFX.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMTalonFX", "wpi__PWMTalonFX.hpp"),
            ],
        ),
        struct(
            class_name = "PWMTalonSRX",
            yml_file = "semiwrap/PWMTalonSRX.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/PWMTalonSRX.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMTalonSRX", "wpi__PWMTalonSRX.hpp"),
            ],
        ),
        struct(
            class_name = "PWMVenom",
            yml_file = "semiwrap/PWMVenom.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/PWMVenom.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMVenom", "wpi__PWMVenom.hpp"),
            ],
        ),
        struct(
            class_name = "PWMVictorSPX",
            yml_file = "semiwrap/PWMVictorSPX.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/PWMVictorSPX.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PWMVictorSPX", "wpi__PWMVictorSPX.hpp"),
            ],
        ),
        struct(
            class_name = "Spark",
            yml_file = "semiwrap/Spark.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/Spark.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Spark", "wpi__Spark.hpp"),
            ],
        ),
        struct(
            class_name = "SparkMini",
            yml_file = "semiwrap/SparkMini.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/SparkMini.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SparkMini", "wpi__SparkMini.hpp"),
            ],
        ),
        struct(
            class_name = "Talon",
            yml_file = "semiwrap/Talon.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/Talon.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Talon", "wpi__Talon.hpp"),
            ],
        ),
        struct(
            class_name = "VictorSP",
            yml_file = "semiwrap/VictorSP.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/motor/VictorSP.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::VictorSP", "wpi__VictorSP.hpp"),
            ],
        ),
        struct(
            class_name = "Compressor",
            yml_file = "semiwrap/Compressor.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/pneumatic/Compressor.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Compressor", "wpi__Compressor.hpp"),
            ],
        ),
        struct(
            class_name = "CompressorConfigType",
            yml_file = "semiwrap/CompressorConfigType.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/pneumatic/CompressorConfigType.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "DoubleSolenoid",
            yml_file = "semiwrap/DoubleSolenoid.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/pneumatic/DoubleSolenoid.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::DoubleSolenoid", "wpi__DoubleSolenoid.hpp"),
            ],
        ),
        struct(
            class_name = "PneumaticHub",
            yml_file = "semiwrap/PneumaticHub.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/pneumatic/PneumaticHub.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PneumaticHub", "wpi__PneumaticHub.hpp"),
                ("wpi::PneumaticHub::Version", "wpi__PneumaticHub__Version.hpp"),
                ("wpi::PneumaticHub::Faults", "wpi__PneumaticHub__Faults.hpp"),
                ("wpi::PneumaticHub::StickyFaults", "wpi__PneumaticHub__StickyFaults.hpp"),
            ],
        ),
        struct(
            class_name = "PneumaticsBase",
            yml_file = "semiwrap/PneumaticsBase.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/pneumatic/PneumaticsBase.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PneumaticsBase", "wpi__PneumaticsBase.hpp"),
            ],
        ),
        struct(
            class_name = "PneumaticsControlModule",
            yml_file = "semiwrap/PneumaticsControlModule.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/pneumatic/PneumaticsControlModule.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PneumaticsControlModule", "wpi__PneumaticsControlModule.hpp"),
            ],
        ),
        struct(
            class_name = "PneumaticsModuleType",
            yml_file = "semiwrap/PneumaticsModuleType.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/pneumatic/PneumaticsModuleType.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "Solenoid",
            yml_file = "semiwrap/Solenoid.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/pneumatic/Solenoid.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Solenoid", "wpi__Solenoid.hpp"),
            ],
        ),
        struct(
            class_name = "PowerDistribution",
            yml_file = "semiwrap/PowerDistribution.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/power/PowerDistribution.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::PowerDistribution", "wpi__PowerDistribution.hpp"),
                ("wpi::PowerDistribution::Version", "wpi__PowerDistribution__Version.hpp"),
                ("wpi::PowerDistribution::Faults", "wpi__PowerDistribution__Faults.hpp"),
                ("wpi::PowerDistribution::StickyFaults", "wpi__PowerDistribution__StickyFaults.hpp"),
            ],
        ),
        struct(
            class_name = "SharpIR",
            yml_file = "semiwrap/SharpIR.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/range/SharpIR.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SharpIR", "wpi__SharpIR.hpp"),
            ],
        ),
        struct(
            class_name = "AnalogEncoder",
            yml_file = "semiwrap/AnalogEncoder.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/rotation/AnalogEncoder.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::AnalogEncoder", "wpi__AnalogEncoder.hpp"),
            ],
        ),
        struct(
            class_name = "AnalogPotentiometer",
            yml_file = "semiwrap/AnalogPotentiometer.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/rotation/AnalogPotentiometer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::AnalogPotentiometer", "wpi__AnalogPotentiometer.hpp"),
            ],
        ),
        struct(
            class_name = "DutyCycle",
            yml_file = "semiwrap/DutyCycle.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/rotation/DutyCycle.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::DutyCycle", "wpi__DutyCycle.hpp"),
            ],
        ),
        struct(
            class_name = "DutyCycleEncoder",
            yml_file = "semiwrap/DutyCycleEncoder.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/rotation/DutyCycleEncoder.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::DutyCycleEncoder", "wpi__DutyCycleEncoder.hpp"),
            ],
        ),
        struct(
            class_name = "Encoder",
            yml_file = "semiwrap/Encoder.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/hardware/rotation/Encoder.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Encoder", "wpi__Encoder.hpp"),
            ],
        ),
        struct(
            class_name = "DriverStationModeThread",
            yml_file = "semiwrap/DriverStationModeThread.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/internal/DriverStationModeThread.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::internal::DriverStationModeThread", "wpi__internal__DriverStationModeThread.hpp"),
            ],
        ),
        struct(
            class_name = "Field2d",
            yml_file = "semiwrap/Field2d.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/Field2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Field2d", "wpi__Field2d.hpp"),
            ],
        ),
        struct(
            class_name = "FieldObject2d",
            yml_file = "semiwrap/FieldObject2d.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/FieldObject2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::FieldObject2d", "wpi__FieldObject2d.hpp"),
            ],
        ),
        struct(
            class_name = "Mechanism2d",
            yml_file = "semiwrap/Mechanism2d.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/Mechanism2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Mechanism2d", "wpi__Mechanism2d.hpp"),
            ],
        ),
        struct(
            class_name = "MechanismLigament2d",
            yml_file = "semiwrap/MechanismLigament2d.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/MechanismLigament2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::MechanismLigament2d", "wpi__MechanismLigament2d.hpp"),
            ],
        ),
        struct(
            class_name = "MechanismObject2d",
            yml_file = "semiwrap/MechanismObject2d.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/MechanismObject2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::MechanismObject2d", "wpi__MechanismObject2d.hpp"),
            ],
        ),
        struct(
            class_name = "MechanismRoot2d",
            yml_file = "semiwrap/MechanismRoot2d.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/MechanismRoot2d.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::MechanismRoot2d", "wpi__MechanismRoot2d.hpp"),
            ],
        ),
        struct(
            class_name = "SendableBuilderImpl",
            yml_file = "semiwrap/SendableBuilderImpl.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/SendableBuilderImpl.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SendableBuilderImpl", "wpi__SendableBuilderImpl.hpp"),
            ],
        ),
        struct(
            class_name = "SendableChooser",
            yml_file = "semiwrap/SendableChooser.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/SendableChooser.hpp",
            tmpl_class_names = [
                ("SendableChooser_tmpl1", "SendableChooser"),
            ],
            trampolines = [
                ("wpi::SendableChooser", "wpi__SendableChooser.hpp"),
            ],
        ),
        struct(
            class_name = "SendableChooserBase",
            yml_file = "semiwrap/SendableChooserBase.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/SendableChooserBase.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SendableChooserBase", "wpi__SendableChooserBase.hpp"),
            ],
        ),
        struct(
            class_name = "SmartDashboard",
            yml_file = "semiwrap/SmartDashboard.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/smartdashboard/SmartDashboard.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SmartDashboard", "wpi__SmartDashboard.hpp"),
            ],
        ),
        struct(
            class_name = "SysIdRoutineLog",
            yml_file = "semiwrap/SysIdRoutineLog.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/sysid/SysIdRoutineLog.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sysid::SysIdRoutineLog", "wpi__sysid__SysIdRoutineLog.hpp"),
                ("wpi::sysid::SysIdRoutineLog::MotorLog", "wpi__sysid__SysIdRoutineLog__MotorLog.hpp"),
            ],
        ),
        struct(
            class_name = "DataLogManager",
            yml_file = "semiwrap/DataLogManager.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/system/DataLogManager.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::DataLogManager", "wpi__DataLogManager.hpp"),
            ],
        ),
        struct(
            class_name = "Errors",
            yml_file = "semiwrap/Errors.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/system/Errors.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "RobotController",
            yml_file = "semiwrap/RobotController.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/system/RobotController.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::CANStatus", "wpi__CANStatus.hpp"),
                ("wpi::RobotController", "wpi__RobotController.hpp"),
            ],
        ),
        struct(
            class_name = "RuntimeType",
            yml_file = "semiwrap/RuntimeType.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/system/RuntimeType.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "SystemServer",
            yml_file = "semiwrap/SystemServer.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/system/SystemServer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SystemServer", "wpi__SystemServer.hpp"),
            ],
        ),
        struct(
            class_name = "Threads",
            yml_file = "semiwrap/Threads.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/system/Threads.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "Timer",
            yml_file = "semiwrap/Timer.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/system/Timer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Timer", "wpi__Timer.hpp"),
            ],
        ),
        struct(
            class_name = "Tracer",
            yml_file = "semiwrap/Tracer.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/system/Tracer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Tracer", "wpi__Tracer.hpp"),
            ],
        ),
        struct(
            class_name = "Watchdog",
            yml_file = "semiwrap/Watchdog.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/system/Watchdog.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Watchdog", "wpi__Watchdog.hpp"),
            ],
        ),
        struct(
            class_name = "Alert",
            yml_file = "semiwrap/Alert.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/util/Alert.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Alert", "wpi__Alert.hpp"),
            ],
        ),
        struct(
            class_name = "Preferences",
            yml_file = "semiwrap/Preferences.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/util/Preferences.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Preferences", "wpi__Preferences.hpp"),
            ],
        ),
        struct(
            class_name = "SensorUtil",
            yml_file = "semiwrap/SensorUtil.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/util/SensorUtil.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SensorUtil", "wpi__SensorUtil.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpilib.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "wpilib.casters.pkl",
        dep_file = "wpilib.casters.d",
    )

    gen_libinit(
        name = "wpilib.gen_lib_init",
        output_file = "src/main/python/wpilib/_init__wpilib.py",
        modules = ["native.wpilib._init_robotpy_native_wpilib", "hal._init__wpiHal", "wpiutil._init__wpiutil", "ntcore._init__ntcore", "wpimath._init__wpimath", "wpimath.geometry._init__geometry", "wpimath._controls._init__controls", "wpilib.event._init__event"],
    )

    gen_pkgconf(
        name = "wpilib.gen_pkgconf",
        libinit_py = "wpilib._init__wpilib",
        module_pkg_name = "wpilib._wpilib",
        output_file = "wpilib.pc",
        pkg_name = "wpilib",
        install_path = "src/main/python/wpilib",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpilib/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpilib.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPILIB_HEADER_GEN],
        libname = "_wpilib",
        output_file = "semiwrap_init.wpilib._wpilib.hpp",
    )

    run_header_gen(
        name = "wpilib",
        casters_pickle = "wpilib.casters.pkl",
        header_gen_config = WPILIB_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpilib",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        generation_defines = ["DYNAMIC_CAMERA_SERVER 1"],
    )

    create_pybind_library(
        name = "wpilib",
        install_path = "src/main/python/wpilib/",
        extension_name = "_wpilib",
        generated_srcs = [":wpilib.generated_srcs"],
        semiwrap_header = [":wpilib.gen_modinit_hpp"],
        deps = [
            ":wpilib.tmpl_hdrs",
            ":wpilib.trampoline_hdrs",
            "//hal:wpiHal",
            "//hal:wpihal_pybind_library",
            "//ntcore:ntcore",
            "//ntcore:ntcore_pybind_library",
            "//wpilibc:wpilib_event_pybind_library",
            "//wpilibc:wpilibc",
            "//wpimath:wpimath",
            "//wpimath:wpimath_controls_pybind_library",
            "//wpimath:wpimath_filter_pybind_library",
            "//wpimath:wpimath_geometry_pybind_library",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//hal:shared/wpiHal",
            "//ntcore:shared/ntcore",
            "//wpilibc:shared/wpilibc",
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
        local_defines = ["DYNAMIC_CAMERA_SERVER=1"],
    )

    native.filegroup(
        name = "wpilib.generated_files",
        srcs = [
            "wpilib.gen_modinit_hpp.gen",
            "wpilib.header_gen_files",
            "wpilib.gen_pkgconf",
            "wpilib.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpilib_counter_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPILIB_COUNTER_HEADER_GEN = [
        struct(
            class_name = "EdgeConfiguration",
            yml_file = "semiwrap/counter/EdgeConfiguration.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/counter/EdgeConfiguration.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "Tachometer",
            yml_file = "semiwrap/counter/Tachometer.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/counter/Tachometer.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::Tachometer", "wpi__Tachometer.hpp"),
            ],
        ),
        struct(
            class_name = "UpDownCounter",
            yml_file = "semiwrap/counter/UpDownCounter.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/counter/UpDownCounter.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::UpDownCounter", "wpi__UpDownCounter.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpilib_counter.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "wpilib_counter.casters.pkl",
        dep_file = "wpilib_counter.casters.d",
    )

    gen_libinit(
        name = "wpilib_counter.gen_lib_init",
        output_file = "src/main/python/wpilib/counter/_init__counter.py",
        modules = ["native.wpilib._init_robotpy_native_wpilib", "wpilib._init__wpilib"],
    )

    gen_pkgconf(
        name = "wpilib_counter.gen_pkgconf",
        libinit_py = "wpilib.counter._init__counter",
        module_pkg_name = "wpilib.counter._counter",
        output_file = "wpilib_counter.pc",
        pkg_name = "wpilib_counter",
        install_path = "src/main/python/wpilib/counter",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpilib/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpilib_counter.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPILIB_COUNTER_HEADER_GEN],
        libname = "_counter",
        output_file = "semiwrap_init.wpilib.counter._counter.hpp",
    )

    run_header_gen(
        name = "wpilib_counter",
        casters_pickle = "wpilib_counter.casters.pkl",
        header_gen_config = WPILIB_COUNTER_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpilib/counter",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpilib_counter",
        install_path = "src/main/python/wpilib/counter/",
        extension_name = "_counter",
        generated_srcs = [":wpilib_counter.generated_srcs"],
        semiwrap_header = [":wpilib_counter.gen_modinit_hpp"],
        deps = [
            ":wpilib_counter.tmpl_hdrs",
            ":wpilib_counter.trampoline_hdrs",
            "//hal:wpiHal",
            "//hal:wpihal_pybind_library",
            "//ntcore:ntcore",
            "//ntcore:ntcore_pybind_library",
            "//wpilibc:wpilib_event_pybind_library",
            "//wpilibc:wpilib_pybind_library",
            "//wpilibc:wpilibc",
            "//wpimath:wpimath",
            "//wpimath:wpimath_controls_pybind_library",
            "//wpimath:wpimath_filter_pybind_library",
            "//wpimath:wpimath_geometry_pybind_library",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//hal:shared/wpiHal",
            "//ntcore:shared/ntcore",
            "//wpilibc:shared/wpilibc",
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpilib_counter.generated_files",
        srcs = [
            "wpilib_counter.gen_modinit_hpp.gen",
            "wpilib_counter.header_gen_files",
            "wpilib_counter.gen_pkgconf",
            "wpilib_counter.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpilib_drive_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPILIB_DRIVE_HEADER_GEN = [
        struct(
            class_name = "DifferentialDrive",
            yml_file = "semiwrap/drive/DifferentialDrive.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/drive/DifferentialDrive.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::DifferentialDrive", "wpi__DifferentialDrive.hpp"),
                ("wpi::DifferentialDrive::WheelSpeeds", "wpi__DifferentialDrive__WheelSpeeds.hpp"),
            ],
        ),
        struct(
            class_name = "MecanumDrive",
            yml_file = "semiwrap/drive/MecanumDrive.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/drive/MecanumDrive.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::MecanumDrive", "wpi__MecanumDrive.hpp"),
                ("wpi::MecanumDrive::WheelSpeeds", "wpi__MecanumDrive__WheelSpeeds.hpp"),
            ],
        ),
        struct(
            class_name = "RobotDriveBase",
            yml_file = "semiwrap/drive/RobotDriveBase.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/drive/RobotDriveBase.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::RobotDriveBase", "wpi__RobotDriveBase.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpilib_drive.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "wpilib_drive.casters.pkl",
        dep_file = "wpilib_drive.casters.d",
    )

    gen_libinit(
        name = "wpilib_drive.gen_lib_init",
        output_file = "src/main/python/wpilib/drive/_init__drive.py",
        modules = ["native.wpilib._init_robotpy_native_wpilib", "wpilib._init__wpilib"],
    )

    gen_pkgconf(
        name = "wpilib_drive.gen_pkgconf",
        libinit_py = "wpilib.drive._init__drive",
        module_pkg_name = "wpilib.drive._drive",
        output_file = "wpilib_drive.pc",
        pkg_name = "wpilib_drive",
        install_path = "src/main/python/wpilib/drive",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpilib/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpilib_drive.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPILIB_DRIVE_HEADER_GEN],
        libname = "_drive",
        output_file = "semiwrap_init.wpilib.drive._drive.hpp",
    )

    run_header_gen(
        name = "wpilib_drive",
        casters_pickle = "wpilib_drive.casters.pkl",
        header_gen_config = WPILIB_DRIVE_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpilib/drive",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpilib_drive",
        install_path = "src/main/python/wpilib/drive/",
        extension_name = "_drive",
        generated_srcs = [":wpilib_drive.generated_srcs"],
        semiwrap_header = [":wpilib_drive.gen_modinit_hpp"],
        deps = [
            ":wpilib_drive.tmpl_hdrs",
            ":wpilib_drive.trampoline_hdrs",
            "//hal:wpiHal",
            "//hal:wpihal_pybind_library",
            "//ntcore:ntcore",
            "//ntcore:ntcore_pybind_library",
            "//wpilibc:wpilib_event_pybind_library",
            "//wpilibc:wpilib_pybind_library",
            "//wpilibc:wpilibc",
            "//wpimath:wpimath",
            "//wpimath:wpimath_controls_pybind_library",
            "//wpimath:wpimath_filter_pybind_library",
            "//wpimath:wpimath_geometry_pybind_library",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//hal:shared/wpiHal",
            "//ntcore:shared/ntcore",
            "//wpilibc:shared/wpilibc",
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpilib_drive.generated_files",
        srcs = [
            "wpilib_drive.gen_modinit_hpp.gen",
            "wpilib_drive.header_gen_files",
            "wpilib_drive.gen_pkgconf",
            "wpilib_drive.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def wpilib_simulation_extension(srcs = [], header_to_dat_deps = [], extra_hdrs = [], includes = [], extra_pyi_deps = []):
    WPILIB_SIMULATION_HEADER_GEN = [
        struct(
            class_name = "ADXL345Sim",
            yml_file = "semiwrap/simulation/ADXL345Sim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/ADXL345Sim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::ADXL345Sim", "wpi__sim__ADXL345Sim.hpp"),
            ],
        ),
        struct(
            class_name = "AddressableLEDSim",
            yml_file = "semiwrap/simulation/AddressableLEDSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/AddressableLEDSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::AddressableLEDSim", "wpi__sim__AddressableLEDSim.hpp"),
            ],
        ),
        struct(
            class_name = "AnalogEncoderSim",
            yml_file = "semiwrap/simulation/AnalogEncoderSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/AnalogEncoderSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::AnalogEncoderSim", "wpi__sim__AnalogEncoderSim.hpp"),
            ],
        ),
        struct(
            class_name = "AnalogInputSim",
            yml_file = "semiwrap/simulation/AnalogInputSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/AnalogInputSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::AnalogInputSim", "wpi__sim__AnalogInputSim.hpp"),
            ],
        ),
        struct(
            class_name = "BatterySim",
            yml_file = "semiwrap/simulation/BatterySim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/BatterySim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::BatterySim", "wpi__sim__BatterySim.hpp"),
            ],
        ),
        struct(
            class_name = "CTREPCMSim",
            yml_file = "semiwrap/simulation/CTREPCMSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/CTREPCMSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::CTREPCMSim", "wpi__sim__CTREPCMSim.hpp"),
            ],
        ),
        struct(
            class_name = "CallbackStore",
            yml_file = "semiwrap/simulation/CallbackStore.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/CallbackStore.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::CallbackStore", "wpi__sim__CallbackStore.hpp"),
            ],
        ),
        struct(
            class_name = "DCMotorSim",
            yml_file = "semiwrap/simulation/DCMotorSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/DCMotorSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::DCMotorSim", "wpi__sim__DCMotorSim.hpp"),
            ],
        ),
        struct(
            class_name = "DIOSim",
            yml_file = "semiwrap/simulation/DIOSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/DIOSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::DIOSim", "wpi__sim__DIOSim.hpp"),
            ],
        ),
        struct(
            class_name = "DifferentialDrivetrainSim",
            yml_file = "semiwrap/simulation/DifferentialDrivetrainSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/DifferentialDrivetrainSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::DifferentialDrivetrainSim", "wpi__sim__DifferentialDrivetrainSim.hpp"),
                ("wpi::sim::DifferentialDrivetrainSim::State", "wpi__sim__DifferentialDrivetrainSim__State.hpp"),
                ("wpi::sim::DifferentialDrivetrainSim::KitbotGearing", "wpi__sim__DifferentialDrivetrainSim__KitbotGearing.hpp"),
                ("wpi::sim::DifferentialDrivetrainSim::KitbotMotor", "wpi__sim__DifferentialDrivetrainSim__KitbotMotor.hpp"),
                ("wpi::sim::DifferentialDrivetrainSim::KitbotWheelSize", "wpi__sim__DifferentialDrivetrainSim__KitbotWheelSize.hpp"),
            ],
        ),
        struct(
            class_name = "DigitalPWMSim",
            yml_file = "semiwrap/simulation/DigitalPWMSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/DigitalPWMSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::DigitalPWMSim", "wpi__sim__DigitalPWMSim.hpp"),
            ],
        ),
        struct(
            class_name = "DoubleSolenoidSim",
            yml_file = "semiwrap/simulation/DoubleSolenoidSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/DoubleSolenoidSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::DoubleSolenoidSim", "wpi__sim__DoubleSolenoidSim.hpp"),
            ],
        ),
        struct(
            class_name = "DriverStationSim",
            yml_file = "semiwrap/simulation/DriverStationSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/DriverStationSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::DriverStationSim", "wpi__sim__DriverStationSim.hpp"),
            ],
        ),
        struct(
            class_name = "DutyCycleEncoderSim",
            yml_file = "semiwrap/simulation/DutyCycleEncoderSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/DutyCycleEncoderSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::DutyCycleEncoderSim", "wpi__sim__DutyCycleEncoderSim.hpp"),
            ],
        ),
        struct(
            class_name = "DutyCycleSim",
            yml_file = "semiwrap/simulation/DutyCycleSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/DutyCycleSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::DutyCycleSim", "wpi__sim__DutyCycleSim.hpp"),
            ],
        ),
        struct(
            class_name = "ElevatorSim",
            yml_file = "semiwrap/simulation/ElevatorSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/ElevatorSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::ElevatorSim", "wpi__sim__ElevatorSim.hpp"),
            ],
        ),
        struct(
            class_name = "EncoderSim",
            yml_file = "semiwrap/simulation/EncoderSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/EncoderSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::EncoderSim", "wpi__sim__EncoderSim.hpp"),
            ],
        ),
        struct(
            class_name = "FlywheelSim",
            yml_file = "semiwrap/simulation/FlywheelSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/FlywheelSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::FlywheelSim", "wpi__sim__FlywheelSim.hpp"),
            ],
        ),
        struct(
            class_name = "GamepadSim",
            yml_file = "semiwrap/simulation/GamepadSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/GamepadSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::GamepadSim", "wpi__sim__GamepadSim.hpp"),
            ],
        ),
        struct(
            class_name = "GenericHIDSim",
            yml_file = "semiwrap/simulation/GenericHIDSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/GenericHIDSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::GenericHIDSim", "wpi__sim__GenericHIDSim.hpp"),
            ],
        ),
        struct(
            class_name = "JoystickSim",
            yml_file = "semiwrap/simulation/JoystickSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/JoystickSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::JoystickSim", "wpi__sim__JoystickSim.hpp"),
            ],
        ),
        struct(
            class_name = "LinearSystemSim",
            yml_file = "semiwrap/simulation/LinearSystemSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/LinearSystemSim.hpp",
            tmpl_class_names = [
                ("LinearSystemSim_tmpl1", "LinearSystemSim_1_1_1"),
                ("LinearSystemSim_tmpl2", "LinearSystemSim_1_1_2"),
                ("LinearSystemSim_tmpl3", "LinearSystemSim_2_1_1"),
                ("LinearSystemSim_tmpl4", "LinearSystemSim_2_1_2"),
                ("LinearSystemSim_tmpl5", "LinearSystemSim_2_2_1"),
                ("LinearSystemSim_tmpl6", "LinearSystemSim_2_2_2"),
            ],
            trampolines = [
                ("wpi::sim::LinearSystemSim", "wpi__sim__LinearSystemSim.hpp"),
            ],
        ),
        struct(
            class_name = "PS4ControllerSim",
            yml_file = "semiwrap/simulation/PS4ControllerSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/PS4ControllerSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::PS4ControllerSim", "wpi__sim__PS4ControllerSim.hpp"),
            ],
        ),
        struct(
            class_name = "PS5ControllerSim",
            yml_file = "semiwrap/simulation/PS5ControllerSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/PS5ControllerSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::PS5ControllerSim", "wpi__sim__PS5ControllerSim.hpp"),
            ],
        ),
        struct(
            class_name = "PWMSim",
            yml_file = "semiwrap/simulation/PWMSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/PWMSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::PWMSim", "wpi__sim__PWMSim.hpp"),
            ],
        ),
        struct(
            class_name = "PneumaticsBaseSim",
            yml_file = "semiwrap/simulation/PneumaticsBaseSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/PneumaticsBaseSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::PneumaticsBaseSim", "wpi__sim__PneumaticsBaseSim.hpp"),
            ],
        ),
        struct(
            class_name = "PowerDistributionSim",
            yml_file = "semiwrap/simulation/PowerDistributionSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/PowerDistributionSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::PowerDistributionSim", "wpi__sim__PowerDistributionSim.hpp"),
            ],
        ),
        struct(
            class_name = "PWMMotorControllerSim",
            yml_file = "semiwrap/simulation/PWMMotorControllerSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/PWMMotorControllerSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::PWMMotorControllerSim", "wpi__sim__PWMMotorControllerSim.hpp"),
            ],
        ),
        struct(
            class_name = "REVPHSim",
            yml_file = "semiwrap/simulation/REVPHSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/REVPHSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::REVPHSim", "wpi__sim__REVPHSim.hpp"),
            ],
        ),
        struct(
            class_name = "RoboRioSim",
            yml_file = "semiwrap/simulation/RoboRioSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/RoboRioSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::RoboRioSim", "wpi__sim__RoboRioSim.hpp"),
            ],
        ),
        struct(
            class_name = "SendableChooserSim",
            yml_file = "semiwrap/simulation/SendableChooserSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/SendableChooserSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::SendableChooserSim", "wpi__sim__SendableChooserSim.hpp"),
            ],
        ),
        struct(
            class_name = "SharpIRSim",
            yml_file = "semiwrap/simulation/SharpIRSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/SharpIRSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::SharpIRSim", "wpi__SharpIRSim.hpp"),
            ],
        ),
        struct(
            class_name = "SimDeviceSim",
            yml_file = "semiwrap/simulation/SimDeviceSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/SimDeviceSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::SimDeviceSim", "wpi__sim__SimDeviceSim.hpp"),
            ],
        ),
        struct(
            class_name = "SimHooks",
            yml_file = "semiwrap/simulation/SimHooks.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/SimHooks.hpp",
            tmpl_class_names = [],
            trampolines = [],
        ),
        struct(
            class_name = "SingleJointedArmSim",
            yml_file = "semiwrap/simulation/SingleJointedArmSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/SingleJointedArmSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::SingleJointedArmSim", "wpi__sim__SingleJointedArmSim.hpp"),
            ],
        ),
        struct(
            class_name = "SolenoidSim",
            yml_file = "semiwrap/simulation/SolenoidSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/SolenoidSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::SolenoidSim", "wpi__sim__SolenoidSim.hpp"),
            ],
        ),
        struct(
            class_name = "StadiaControllerSim",
            yml_file = "semiwrap/simulation/StadiaControllerSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/StadiaControllerSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::StadiaControllerSim", "wpi__sim__StadiaControllerSim.hpp"),
            ],
        ),
        struct(
            class_name = "XboxControllerSim",
            yml_file = "semiwrap/simulation/XboxControllerSim.yml",
            header_root = "$(execpath :robotpy-native-wpilib.copy_headers)",
            header_file = "$(execpath :robotpy-native-wpilib.copy_headers)/wpi/simulation/XboxControllerSim.hpp",
            tmpl_class_names = [],
            trampolines = [
                ("wpi::sim::XboxControllerSim", "wpi__sim__XboxControllerSim.hpp"),
            ],
        ),
    ]

    resolve_casters(
        name = "wpilib_simulation.resolve_casters",
        caster_deps = ["//wpimath:src/main/python/wpimath/wpimath-casters.pybind11.json", "//wpiutil:src/main/python/wpiutil/wpiutil-casters.pybind11.json"],
        casters_pkl_file = "wpilib_simulation.casters.pkl",
        dep_file = "wpilib_simulation.casters.d",
    )

    gen_libinit(
        name = "wpilib_simulation.gen_lib_init",
        output_file = "src/main/python/wpilib/simulation/_init__simulation.py",
        modules = ["native.wpilib._init_robotpy_native_wpilib", "wpilib._init__wpilib", "wpimath._controls._init__controls", "wpimath.geometry._init__geometry", "wpimath.kinematics._init__kinematics"],
    )

    gen_pkgconf(
        name = "wpilib_simulation.gen_pkgconf",
        libinit_py = "wpilib.simulation._init__simulation",
        module_pkg_name = "wpilib.simulation._simulation",
        output_file = "wpilib_simulation.pc",
        pkg_name = "wpilib_simulation",
        install_path = "src/main/python/wpilib/simulation",
        project_file = "src/main/python/pyproject.toml",
        package_root = "src/main/python/wpilib/__init__.py",
    )

    gen_modinit_hpp(
        name = "wpilib_simulation.gen_modinit_hpp",
        input_dats = [x.class_name for x in WPILIB_SIMULATION_HEADER_GEN],
        libname = "_simulation",
        output_file = "semiwrap_init.wpilib.simulation._simulation.hpp",
    )

    run_header_gen(
        name = "wpilib_simulation",
        casters_pickle = "wpilib_simulation.casters.pkl",
        header_gen_config = WPILIB_SIMULATION_HEADER_GEN,
        trampoline_subpath = "src/main/python/wpilib/simulation",
        deps = header_to_dat_deps,
        local_native_libraries = [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
    )

    create_pybind_library(
        name = "wpilib_simulation",
        install_path = "src/main/python/wpilib/simulation/",
        extension_name = "_simulation",
        generated_srcs = [":wpilib_simulation.generated_srcs"],
        semiwrap_header = [":wpilib_simulation.gen_modinit_hpp"],
        deps = [
            ":wpilib_simulation.tmpl_hdrs",
            ":wpilib_simulation.trampoline_hdrs",
            "//hal:wpiHal",
            "//hal:wpihal_pybind_library",
            "//ntcore:ntcore",
            "//ntcore:ntcore_pybind_library",
            "//wpilibc:wpilib_event_pybind_library",
            "//wpilibc:wpilib_pybind_library",
            "//wpilibc:wpilibc",
            "//wpimath:wpimath",
            "//wpimath:wpimath_controls_pybind_library",
            "//wpimath:wpimath_filter_pybind_library",
            "//wpimath:wpimath_geometry_pybind_library",
            "//wpimath:wpimath_kinematics_pybind_library",
            "//wpimath:wpimath_pybind_library",
            "//wpiutil:wpiutil",
            "//wpiutil:wpiutil_pybind_library",
        ],
        dynamic_deps = [
            "//hal:shared/wpiHal",
            "//ntcore:shared/ntcore",
            "//wpilibc:shared/wpilibc",
            "//wpimath:shared/wpimath",
            "//wpiutil:shared/wpiutil",
        ],
        extra_hdrs = extra_hdrs,
        extra_srcs = srcs,
        includes = includes,
    )

    native.filegroup(
        name = "wpilib_simulation.generated_files",
        srcs = [
            "wpilib_simulation.gen_modinit_hpp.gen",
            "wpilib_simulation.header_gen_files",
            "wpilib_simulation.gen_pkgconf",
            "wpilib_simulation.gen_lib_init",
        ],
        tags = ["manual", "robotpy"],
    )

def define_pybind_library(name, pkgcfgs = []):
    # Helper used to generate all files with one target.
    native.filegroup(
        name = "{}.generated_files".format(name),
        srcs = [
            "wpilib_event.generated_files",
            "wpilib.generated_files",
            "wpilib_counter.generated_files",
            "wpilib_drive.generated_files",
            "wpilib_simulation.generated_files",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Files that will be included in the wheel as data deps
    native.filegroup(
        name = "{}.generated_pkgcfg_files".format(name),
        srcs = [
            "src/main/python/wpilib/event/wpilib_event.pc",
            "src/main/python/wpilib/wpilib.pc",
            "src/main/python/wpilib/counter/wpilib_counter.pc",
            "src/main/python/wpilib/drive/wpilib_drive.pc",
            "src/main/python/wpilib/simulation/wpilib_simulation.pc",
        ],
        tags = ["manual", "robotpy"],
        visibility = ["//visibility:public"],
    )

    # Contains all of the non-python files that need to be included in the wheel
    native.filegroup(
        name = "{}.extra_files".format(name),
        srcs = native.glob(["src/main/python/wpilib/**"], exclude = ["src/main/python/wpilib/**/*.py"], allow_empty = True),
        tags = ["manual", "robotpy"],
    )

    generate_version_file(
        name = "{}.generate_version".format(name),
        output_file = "src/main/python/wpilib/version.py",
        template = "//shared/bazel/rules/robotpy:version_template.in",
    )

    robotpy_library(
        name = name,
        srcs = native.glob(["src/main/python/wpilib/**/*.py"]) + [
            "src/main/python/wpilib/event/_init__event.py",
            "src/main/python/wpilib/_init__wpilib.py",
            "src/main/python/wpilib/counter/_init__counter.py",
            "src/main/python/wpilib/drive/_init__drive.py",
            "src/main/python/wpilib/simulation/_init__simulation.py",
            "{}.generate_version".format(name),
        ],
        data = [
            "{}.generated_pkgcfg_files".format(name),
            "{}.extra_files".format(name),
            ":src/main/python/wpilib/event/_event",
            ":src/main/python/wpilib/_wpilib",
            ":src/main/python/wpilib/counter/_counter",
            ":src/main/python/wpilib/drive/_drive",
            ":src/main/python/wpilib/simulation/_simulation",
            ":wpilib_event.trampoline_hdr_files",
            ":wpilib.trampoline_hdr_files",
            ":wpilib_counter.trampoline_hdr_files",
            ":wpilib_drive.trampoline_hdr_files",
            ":wpilib_simulation.trampoline_hdr_files",
        ],
        imports = ["src/main/python"],
        deps = [
            "//hal:robotpy-hal",
            "//ntcore:pyntcore",
            "//wpilibc:robotpy-native-wpilib",
            "//wpimath:robotpy-wpimath",
            "//wpiutil:robotpy-wpiutil",
        ],
        visibility = ["//visibility:public"],
    )

    update_yaml_files(
        name = "{}-update-yaml".format(name),
        yaml_output_directory = "src/main/python/semiwrap",
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//datalog:robotpy-native-datalog.copy_headers",
            "//hal:robotpy-native-wpihal.copy_headers",
            "//ntcore:robotpy-native-ntcore.copy_headers",
            "//wpilibc:robotpy-native-wpilib.copy_headers",
            "//wpimath:robotpy-native-wpimath.copy_headers",
            "//wpinet:robotpy-native-wpinet.copy_headers",
            "//wpiutil:robotpy-native-wpiutil.copy_headers",
        ],
        package_root_file = "src/main/python/wpilib/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
        yaml_files = native.glob(["src/main/python/semiwrap/**"]),
    )

    scan_headers(
        name = "{}-scan-headers".format(name),
        extra_hdrs = native.glob(["src/main/python/**/*.h"], allow_empty = True) + [
            "//wpilibc:robotpy-native-wpilib.copy_headers",
        ],
        package_root_file = "src/main/python/wpilib/__init__.py",
        pkgcfgs = pkgcfgs,
        pyproject_toml = "src/main/python/pyproject.toml",
    )
