import dataclasses
import sys
import json
import os
import pathlib
import re
import shutil
import subprocess
from typing import Dict
import logging

"""
This script will refactor wpilib. It does this in multiple steps:
   1. Preprocess C++ and Java files to determine file moves, and calclate thier 
      respective changes to other files, i.e. how to change C++ include paths, or
      java package / import renames
   1b. Additional renames are applied as per this document https://docs.google.com/spreadsheets/d/1NXgby1njo7oZ9_Ezk8q6ez5VzPTXeLAVRSnB3M1TqR4/edit?gid=1323156307#gid=1323156307
       Note, some of the proposed renames require substantial build file modifications or need dependencies broken, and therefore are not covered with this script.
   2. Apply the pre-processed renames and include / import fixups
   3-N. Apply some manual fixes, as well as run scripts like the linters, upstream utils, pregenerated files, etc
   N-M. Apply namespace changes. This is a combination of automated find/replace as well as hand fixes.
"""

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

RUN_WITH_HPP = True
NEW_CC_FILE_SUFFIX = ".hpp" if RUN_WITH_HPP else ".h"

#####################################
# Preprocessing
#####################################
class RawConfig:
    """
    Raw config refers to information used soley for the preprocessing step. This includes the bulk subproject renames, as well as the 
    include / package renames and individual file moves. This information should only be used in the preprocessing phase
    """

    # Bulk project moves.
    PROJECT_RENAMES = [
        ("wpilibNewCommands/", "command/"),
        ("fieldImages/", "fields/"),
        ("datalogtool/", "tools/datalogtool/"),
        ("outlineviewer/", "tools/outlineviewer/"),
        ("processstarter/", "tools/processstarter/"),
        ("sysid/", "tools/sysid/"),
        ("wpical/", "tools/wpical/"),
        # TODO glass, wpilib[j/c]
    ]

    # This changes the paths within a C++ project. 
    # Format is (include path, original subpath, new subpath)
    CC_FOLDER_RENAMES = [
        ("wpiutil/src/main/native/include", "wpi", "wpi/util"),
        ("wpiutil/src/main/native/thirdparty/argparse/include", "wpi", "wpi/util"),
        # Leave debugging alone
        ("wpiutil/src/main/native/thirdparty/expected/include", "wpi", "wpi/util"),
        # Leave fmt alone
        ("wpiutil/src/main/native/thirdparty/json/include", "wpi", "wpi/util"),
        ("wpiutil/src/main/native/thirdparty/llvm/include", "wpi", "wpi/util"),
        ("wpiutil/src/main/native/thirdparty/mpack/include", "wpi", "wpi/util"),
        # Leave nanopb alone
        ("wpiutil/src/main/native/thirdparty/sigslot/include", "wpi", "wpi/util"),
        # Leave upb alone
        ("wpinet/src/main/native/include", "wpinet", "wpi/net"),
        ("wpinet/src/main/native/thirdparty/tcpsockets/include", "wpinet", "wpi/net"),
        # No datalog
        ("ntcore/src/main/native/include", "networktables", "wpi/ntcore"),
        ("ntcore/src/generated/main/native/include", "networktables", "wpi/ntcore"),
        ("hal/src/main/native/include", "hal", "wpi/hal"),
        ("wpimath/src/main/native/include", "frc", "wpi/math"),
        ("wpimath/src/main/native/include", "units", "wpi/units"),
        ("wpilibc/src/main/native/include", "frc", "wpi"),
        ("wpilibc/src/generated/main/native/include", "frc", "wpi"),
        ("apriltag/src/main/native/include", "frc", "wpi"),
        ("command/src/main/native/include", "frc2", "wpi"),
        ("command/src/generated/main/native/include", "frc2", "wpi"),
        ("fields/src/main/native/include", "fields", "wpi/fields"),
        ("cameraserver/src/main/native/include", "cameraserver", "wpi/cameraserver"),
        ("cameraserver/src/main/native/include", "vision", "wpi/vision"),
        ("glass/src/lib/native/include", "glass", "wpi/glass"),
        ("glass/src/libnt/native/include", "glass", "wpi/glass"),
        # wpigui root handled with CC_FILE_RENAMES
        # cscore root handled with CC_FILE_RENAMES
        ("romiVendordep/src/main/native/include", "frc/romi", "wpi/romi"),
        ("xrpVendordep/src/main/native/include", "frc/xrp", "wpi/xrp"),
    ]

    # Individual C++ file renames. 
    # In some cases this refers to files that were originally included at the root level that now belong under a folder, as well as
    # files moved to roughly match the java format in the spreadsheet.
    #
    # The format is [include_root: [(original path, new path), ...]]
    CC_FILE_RENAMES = [
        (
            pathlib.Path("ntcore/src/main/native/include"),
            [
                ("ntcore_test.h", "wpi/ntcore/ntcore_test" + NEW_CC_FILE_SUFFIX),
                ("ntcore.h", "wpi/ntcore/ntcore" + NEW_CC_FILE_SUFFIX),
                ("ntcore_c.h", "wpi/ntcore/ntcore_c.h"),
                ("ntcore_cpp.h", "wpi/ntcore/ntcore_cpp" + NEW_CC_FILE_SUFFIX),
            ],
        ),
        (
            pathlib.Path("hal/src/main/native/include"),
            [
                ("hal/UsageReporting.h", "wpi/hal/UsageReporting.h"),
            ],
        ),
        (
            pathlib.Path("ntcore/src/generated/main/native/include"),
            [
                ("ntcore_c_types.h", "wpi/ntcore/ntcore_c_types.h"),
                ("ntcore_cpp_types.h", "wpi/ntcore/ntcore_cpp_types" + NEW_CC_FILE_SUFFIX),
            ],
        ),
        (
            pathlib.Path("wpimath/src/main/native/include"),
            [
                ("wpimath/MathShared.h", "wpi/math/util/MathShared" + NEW_CC_FILE_SUFFIX),
                ("frc/DARE.h", "wpi/math/linalg/DARE" + NEW_CC_FILE_SUFFIX),
                ("frc/ct_matrix.h", "wpi/math/linalg/ct_matrix" + NEW_CC_FILE_SUFFIX),
                ("frc/EigenCore.h", "wpi/math/linalg/EigenCore" + NEW_CC_FILE_SUFFIX),
                ("frc/ComputerVisionUtil.h", "wpi/math/util/ComputerVisionUtil" + NEW_CC_FILE_SUFFIX),
                ("frc/MathUtil.h", "wpi/math/util/MathUtil" + NEW_CC_FILE_SUFFIX),
                ("frc/StateSpaceUtil.h", "wpi/math/util/StateSpaceUtil" + NEW_CC_FILE_SUFFIX),
            ],
        ),
        (
            pathlib.Path("cscore/src/main/native/include"),
            [
                ("cscore_cv.h", "wpi/cscore/cscore_cv" + NEW_CC_FILE_SUFFIX),
                ("cscore_cpp.h", "wpi/cscore/cscore_cpp" + NEW_CC_FILE_SUFFIX),
                ("cscore.h", "wpi/cscore/cscore" + NEW_CC_FILE_SUFFIX),
                ("cscore_raw.h", "wpi/cscore/cscore_raw" + NEW_CC_FILE_SUFFIX),
                ("cscore_c.h", "wpi/cscore/cscore_c.h"),
                ("cscore_oo.h", "wpi/cscore/cscore_oo" + NEW_CC_FILE_SUFFIX),
                ("cscore_runloop.h", "wpi/cscore/cscore_runloop" + NEW_CC_FILE_SUFFIX),
            ],
        ),
        (
            pathlib.Path("wpigui/src/main/native/include"),
            [
                ("wpigui_openurl.h", "wpi/gui/wpigui_openurl" + NEW_CC_FILE_SUFFIX),
                ("portable-file-dialogs.h", "wpi/gui/portable-file-dialogs" + NEW_CC_FILE_SUFFIX),
                ("wpigui_internal.h", "wpi/gui/wpigui_internal" + NEW_CC_FILE_SUFFIX),
                ("wpigui.h", "wpi/gui/wpigui" + NEW_CC_FILE_SUFFIX),
            ],
        ),
        (
            pathlib.Path("wpilibc/src/main/native/include"),
            [
                ("WPILibVersion.h", "wpi/system/WPILibVersion" + NEW_CC_FILE_SUFFIX),
                ("frc/DriverStation.h", "wpi/driverstation/DriverStation" + NEW_CC_FILE_SUFFIX),
                ("frc/GenericHID.h", "wpi/driverstation/GenericHID" + NEW_CC_FILE_SUFFIX),
                ("frc/Joystick.h", "wpi/driverstation/Joystick" + NEW_CC_FILE_SUFFIX),
                ("frc/ADXL345_I2C.h", "wpi/hardware/accelerometer/ADXL345_I2C" + NEW_CC_FILE_SUFFIX),
                ("frc/AnalogAccelerometer.h", "wpi/hardware/accelerometer/AnalogAccelerometer" + NEW_CC_FILE_SUFFIX),
                ("frc/CAN.h", "wpi/hardware/bus/CAN" + NEW_CC_FILE_SUFFIX),
                ("frc/I2C.h", "wpi/hardware/bus/I2C" + NEW_CC_FILE_SUFFIX),
                ("frc/SerialPort.h", "wpi/hardware/bus/SerialPort" + NEW_CC_FILE_SUFFIX),
                ("frc/AnalogInput.h", "wpi/hardware/discrete/AnalogInput" + NEW_CC_FILE_SUFFIX),
                ("frc/CounterBase.h", "wpi/hardware/discrete/CounterBase" + NEW_CC_FILE_SUFFIX),
                ("frc/DigitalInput.h", "wpi/hardware/discrete/DigitalInput" + NEW_CC_FILE_SUFFIX),
                ("frc/DigitalOutput.h", "wpi/hardware/discrete/DigitalOutput" + NEW_CC_FILE_SUFFIX),
                ("frc/PWM.h", "wpi/hardware/discrete/PWM" + NEW_CC_FILE_SUFFIX),
                ("frc/AddressableLED.h", "wpi/hardware/led/AddressableLED" + NEW_CC_FILE_SUFFIX),
                ("frc/LEDPattern.h", "wpi/hardware/led/LEDPattern" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/MotorController.h", "wpi/hardware/motor/MotorController" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/MotorControllerGroup.h", "wpi/hardware/motor/MotorControllerGroup" + NEW_CC_FILE_SUFFIX),
                ("frc/MotorSafety.h", "wpi/hardware/motor/MotorSafety" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/PWMMotorController.h", "wpi/hardware/motor/PWMMotorController" + NEW_CC_FILE_SUFFIX),
                ("frc/Compressor.h", "wpi/hardware/pneumatic/Compressor" + NEW_CC_FILE_SUFFIX),
                ("frc/CompressorConfigType.h", "wpi/hardware/pneumatic/CompressorConfigType" + NEW_CC_FILE_SUFFIX),
                ("frc/DoubleSolenoid.h", "wpi/hardware/pneumatic/DoubleSolenoid" + NEW_CC_FILE_SUFFIX),
                ("frc/PneumaticHub.h", "wpi/hardware/pneumatic/PneumaticHub" + NEW_CC_FILE_SUFFIX),
                ("frc/PneumaticsBase.h", "wpi/hardware/pneumatic/PneumaticsBase" + NEW_CC_FILE_SUFFIX),
                ("frc/PneumaticsControlModule.h", "wpi/hardware/pneumatic/PneumaticsControlModule" + NEW_CC_FILE_SUFFIX),
                ("frc/PneumaticsModuleType.h", "wpi/hardware/pneumatic/PneumaticsModuleType" + NEW_CC_FILE_SUFFIX),
                ("frc/Solenoid.h", "wpi/hardware/pneumatic/Solenoid" + NEW_CC_FILE_SUFFIX),
                ("frc/PowerDistribution.h", "wpi/hardware/power/PowerDistribution" + NEW_CC_FILE_SUFFIX),
                ("frc/AnalogEncoder.h", "wpi/hardware/rotation/AnalogEncoder" + NEW_CC_FILE_SUFFIX),
                ("frc/AnalogPotentiometer.h", "wpi/hardware/rotation/AnalogPotentiometer" + NEW_CC_FILE_SUFFIX),
                ("frc/DutyCycle.h", "wpi/hardware/rotation/DutyCycle" + NEW_CC_FILE_SUFFIX),
                ("frc/DutyCycleEncoder.h", "wpi/hardware/rotation/DutyCycleEncoder" + NEW_CC_FILE_SUFFIX),
                ("frc/Encoder.h", "wpi/hardware/rotation/Encoder" + NEW_CC_FILE_SUFFIX),
                ("frc/Servo.h", "wpi/hardware/servo/Servo" + NEW_CC_FILE_SUFFIX),
                ("frc/IterativeRobotBase.h", "wpi/opmode/IterativeRobotBase" + NEW_CC_FILE_SUFFIX),
                ("frc/RobotBase.h", "wpi/opmode/RobotBase" + NEW_CC_FILE_SUFFIX),
                ("frc/RobotState.h", "wpi/opmode/RobotState" + NEW_CC_FILE_SUFFIX),
                ("frc/TimedRobot.h", "wpi/opmode/TimedRobot" + NEW_CC_FILE_SUFFIX),
                ("frc/TimesliceRobot.h", "wpi/opmode/TimesliceRobot" + NEW_CC_FILE_SUFFIX),
                ("frc/DataLogManager.h", "wpi/system/DataLogManager" + NEW_CC_FILE_SUFFIX),
                ("frc/Filesystem.h", "wpi/system/Filesystem" + NEW_CC_FILE_SUFFIX),
                ("frc/Resource.h", "wpi/system/Resource" + NEW_CC_FILE_SUFFIX),
                ("frc/RobotController.h", "wpi/system/RobotController" + NEW_CC_FILE_SUFFIX),
                ("frc/ScopedTracer.h", "wpi/system/ScopedTracer" + NEW_CC_FILE_SUFFIX),
                ("frc/Threads.h", "wpi/system/Threads" + NEW_CC_FILE_SUFFIX),
                ("frc/Timer.h", "wpi/system/Timer" + NEW_CC_FILE_SUFFIX),
                ("frc/Tracer.h", "wpi/system/Tracer" + NEW_CC_FILE_SUFFIX),
                ("frc/WPILibVersion.h", "wpi/system/WPILibVersion" + NEW_CC_FILE_SUFFIX),
                ("frc/Watchdog.h", "wpi/system/Watchdog" + NEW_CC_FILE_SUFFIX),
            ],
        ),
        (
            pathlib.Path("wpilibc/src/generated/main/native/include"),
            [
                ("frc/StadiaController.h", "wpi/driverstation/StadiaController" + NEW_CC_FILE_SUFFIX),
                ("frc/PS4Controller.h", "wpi/driverstation/PS4Controller" + NEW_CC_FILE_SUFFIX),
                ("frc/PS5Controller.h", "wpi/driverstation/PS5Controller" + NEW_CC_FILE_SUFFIX),
                ("frc/XboxController.h", "wpi/driverstation/XboxController" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/PWMSparkFlex.h", "wpi/hardware/motor/PWMSparkFlex" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/SparkMini.h", "wpi/hardware/motor/SparkMini" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/PWMVictorSPX.h", "wpi/hardware/motor/PWMVictorSPX" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/Jaguar.h", "wpi/hardware/motor/Jaguar" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/Victor.h", "wpi/hardware/motor/Victor" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/DMC60.h", "wpi/hardware/motor/DMC60" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/VictorSP.h", "wpi/hardware/motor/VictorSP" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/PWMVenom.h", "wpi/hardware/motor/PWMVenom" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/Koors40.h", "wpi/hardware/motor/Koors40" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/SD540.h", "wpi/hardware/motor/SD540" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/Talon.h", "wpi/hardware/motor/Talon" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/Spark.h", "wpi/hardware/motor/Spark" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/PWMTalonSRX.h", "wpi/hardware/motor/PWMTalonSRX" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/PWMTalonFX.h", "wpi/hardware/motor/PWMTalonFX" + NEW_CC_FILE_SUFFIX),
                ("frc/motorcontrol/PWMSparkMax.h", "wpi/hardware/motor/PWMSparkMax" + NEW_CC_FILE_SUFFIX),
            ],
        ),
    ]

    # Package changes (and therefor folder moves) for java projects.
    # Format: (project, original package, new package)
    JAVA_PROJECT_REPLACMENTS = [
        ("wpiutil", "edu.wpi.first.util", "org.wpilib.util"),
        ("wpimath", "edu.wpi.first.math", "org.wpilib.math"),
        ("wpinet", "edu.wpi.first.net", "org.wpilib.net"),
        ("datalog", "edu.wpi.first.datalog", "org.wpilib.datalog"),
        ("command", "edu.wpi.first.wpilibj2.command", "org.wpilib.command"),
        ("ntcore", "edu.wpi.first.networktables", "org.wpilib.networktables"),
        ("fields", "edu.wpi.first.fields", "org.wpilib.fields"),
        ("hal", "edu.wpi.first.hal", "org.wpilib.hardware.hal"),
        ("wpiunits", "edu.wpi.first.units", "org.wpilib.units"),
        ("apriltag", "edu.wpi.first.apriltag", "org.wpilib.vision.apriltag"),
        ("epilogue-processor", "edu.wpi.first.epilogue.processor", "org.wpilib.epilogue.processor"),
        ("epilogue-runtime", "edu.wpi.first.epilogue", "org.wpilib.epilogue"),
        ("epilogue-runtime", "edu.wpi.first.epilogue.logging", "org.wpilib.epilogue.logging"),
        ("cscore", "edu.wpi.first.cscore", "org.wpilib.vision.camera"),
        ("cameraserver", "edu.wpi.first.vision", "org.wpilib.vision.process"),
        ("cameraserver", "edu.wpi.first.cameraserver", "org.wpilib.vision.stream"),
        ("xrpVendordep", "edu.wpi.first.wpilibj.xrp", "org.wpilib.xrp"),
        ("romiVendordep", "edu.wpi.first.wpilibj.romi", "org.wpilib.romi"),
        ("wpilibjExamples", "edu.wpi.first.wpilibj.commands", "org.wpilib.commands"),
        ("wpilibjExamples", "edu.wpi.first.wpilibj.examples", "org.wpilib.examples"),
        ("wpilibjExamples", "edu.wpi.first.wpilibj.snippets", "org.wpilib.snippets"),
        ("wpilibjExamples", "edu.wpi.first.wpilibj.templates", "org.wpilib.templates"),
        ("wpilibj", "edu.wpi.first.wpilibj", "org.wpilib"),
    ]

    # Individual java file moves
    # Format: [(project: [(original package, new package, class to move), ...])]
    JAVA_FILE_RENAMES = [
        (
            pathlib.Path("wpiutil"),
            [
                ("edu/wpi/first/util", "org/wpilib/util/runtime", "ClassPreloader"),
                ("edu/wpi/first/util", "org/wpilib/util/runtime", "CombinedRuntimeLoader"),
                ("edu/wpi/first/util", "org/wpilib/util/runtime", "MsvcRuntimeException"),
                ("edu/wpi/first/util", "org/wpilib/util/runtime", "RuntimeDetector"),
                ("edu/wpi/first/util", "org/wpilib/util/runtime", "RuntimeLoader"),
                ("edu/wpi/first/util", "org/wpilib/util/container", "DoubleCircularBuffer"),
                ("edu/wpi/first/util", "org/wpilib/util/container", "CircularBuffer"),
                ("edu/wpi/first/util", "org/wpilib/util/cleanup", "WPICleaner"),
                ("edu/wpi/first/util", "org/wpilib/util/concurrent", "EventVector"),
            ],
        ),
        (
            pathlib.Path("command"),
            [
                ("edu/wpi/first/wpilibj2", "org/wpilib", "MockHardwareExtension"),
            ],
        ),
        (
            pathlib.Path("wpimath"),
            [
                ("edu/wpi/first/math", "org/wpilib/math/linalg", "DARE"),
                ("edu/wpi/first/math", "org/wpilib/math/linalg", "VecBuilder"),
                ("edu/wpi/first/math", "org/wpilib/math/linalg", "Vector"),
                ("edu/wpi/first/math", "org/wpilib/math/linalg", "Matrix"),
                ("edu/wpi/first/math", "org/wpilib/math/linalg", "MatBuilder"),
                ("edu/wpi/first/math", "org/wpilib/math/util", "MathUtil"),
                ("edu/wpi/first/math", "org/wpilib/math/util", "StateSpaceUtil"),
                ("edu/wpi/first/math", "org/wpilib/math/util", "ComputerVisionUtil"),
                ("edu/wpi/first/math", "org/wpilib/math/util", "MathShared"),
                ("edu/wpi/first/math", "org/wpilib/math/util", "MathSharedStore"),
                ("edu/wpi/first/math", "org/wpilib/math/util", "Pair"),
                ("edu/wpi/first/math", "org/wpilib/math/util", "Num"),
                ("edu/wpi/first/math", "org/wpilib/math/util", "Nat"),
                ("edu/wpi/first/math", "org/wpilib/math/interpolation", "InterpolatingMatrixTreeMap"),
                ("edu/wpi/first/wpilibj", "org/wpilib", "ProtoTestBase"),
                ("edu/wpi/first/wpilibj", "org/wpilib", "StructTestBase"),
                ("edu/wpi/first/wpilibj", "org/wpilib", "UtilityClassTest"),
            ],
        ),
        (
            pathlib.Path("wpilibj"),
            [
                ("edu/wpi/first/wpilibj", "org/wpilib/driverstation", "DriverStation"),
                ("edu/wpi/first/wpilibj", "org/wpilib/driverstation", "GenericHID"),
                ("edu/wpi/first/wpilibj", "org/wpilib/driverstation", "Joystick"),
                ("edu/wpi/first/wpilibj", "org/wpilib/driverstation", "PS4Controller"),
                ("edu/wpi/first/wpilibj", "org/wpilib/driverstation", "PS5Controller"),
                ("edu/wpi/first/wpilibj", "org/wpilib/driverstation", "StadiaController"),
                ("edu/wpi/first/wpilibj", "org/wpilib/driverstation", "XboxController"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/bus", "CAN"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/bus", "I2C"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/bus", "SerialPort"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/accelerometer", "ADXL345_I2C"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/accelerometer", "AnalogAccelerometer"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/discrete", "AnalogInput"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/discrete", "CounterBase"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/discrete", "DigitalInput"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/discrete", "DigitalOutput"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/discrete", "PWM"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/led", "AddressableLED"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/led", "AddressableLEDBuffer"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/led", "AddressableLEDBufferView"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/led", "LEDPattern"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/led", "LEDReader"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/led", "LEDWriter"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "MotorController"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "MotorControllerGroup"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/motor", "MotorSafety"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "PWMMotorController"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/motor", "MotorSafety"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "DMC60"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "Jaguar"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "Koors40"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "PWMSparkFlex"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "PWMSparkMax"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "PWMTalonFX"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "PWMTalonSRX"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "PWMVenom"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "PWMVictorSPX"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "SD540"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "Spark"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "SparkMini"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "Talon"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "Victor"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "VictorSP"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "MockPWMMotorController"),
                ("edu/wpi/first/wpilibj/motorcontrol", "org/wpilib/hardware/motor", "MockMotorController"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "Compressor"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "CompressorConfigType"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "DoubleSolenoid"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "PneumaticHub"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "PneumaticsBase"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "PneumaticsControlModule"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "PneumaticsModuleType"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "Solenoid"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "DoubleSolenoidTestCTRE"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "DoubleSolenoidTestREV"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "SolenoidTestCTRE"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/pneumatic", "SolenoidTestREV"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/power", "PowerDistribution"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/rotation", "AnalogEncoder"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/rotation", "AnalogPotentiometer"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/rotation", "DutyCycle"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/rotation", "DutyCycleEncoder"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/rotation", "Encoder"),
                ("edu/wpi/first/wpilibj", "org/wpilib/hardware/servo", "Servo"),
                ("edu/wpi/first/wpilibj", "org/wpilib/opmode", "IterativeRobotBase"),
                ("edu/wpi/first/wpilibj", "org/wpilib/opmode", "RobotBase"),
                ("edu/wpi/first/wpilibj", "org/wpilib/opmode", "RobotState"),
                ("edu/wpi/first/wpilibj", "org/wpilib/opmode", "TimedRobot"),
                ("edu/wpi/first/wpilibj", "org/wpilib/opmode", "TimesliceRobot"),
                ("edu/wpi/first/wpilibj", "org/wpilib/system", "DataLogManager"),
                ("edu/wpi/first/wpilibj", "org/wpilib/system", "Filesystem"),
                ("edu/wpi/first/wpilibj", "org/wpilib/system", "Resource"),
                ("edu/wpi/first/wpilibj", "org/wpilib/system", "RobotController"),
                ("edu/wpi/first/wpilibj", "org/wpilib/system", "Threads"),
                ("edu/wpi/first/wpilibj", "org/wpilib/system", "Timer"),
                ("edu/wpi/first/wpilibj", "org/wpilib/system", "Tracer"),
                ("edu/wpi/first/wpilibj", "org/wpilib/system", "Watchdog"),
            ],
        ),
    ]

    # These describe files / folders that need to be moved, but aren't necessarily directly tied to the
    # methodology of the java / C++ moves.
    # Format: (original file/folder, new file/folder)
    GENERIC_RENAMES = [
        ("apriltag/src/main/native/resources/edu/wpi/first", "apriltag/src/main/native/resources/org/wpilib/vision"),
        ("apriltag/src/test/resources/edu/wpi/first", "apriltag/src/test/resources/org/wpilib/vision"),
        ("command/src/generate/main/native/cpp/frc2/", "command/src/generate/main/native/cpp/wpi/"),
        ("command/src/generate/main/native/include/frc2/", "command/src/generate/main/native/include/wpi/"),
        ("command/wpilibnewcommands-config.cmake.in", "command/command-config.cmake.in"),
        ("command/src/generated/main/native/cpp/frc2/", "command/src/generated/main/native/cpp/wpi/"),
        ("command/src/test/native/cpp/frc2/", "command/src/test/native/cpp/wpi/"),
        ("command/src/generate/main/native/include/wpi/command/button/commandhid.h.jinja", "command/src/generate/main/native/include/wpi/command/button/commandhid.hpp.jinja"),
        ("fields/fieldimages-config.cmake.in", "fields/fields-config.cmake.in"),
        ("fields/src/main/native/resources/edu/wpi/first", "fields/src/main/native/resources/org/wpilib"),
        ("fields/src/test/native/resources/edu/wpi/first", "fields/src/test/native/resources/org/wpilib"),
        ("ntcore/src/generate/main/native/include/networktables/Topic.h.jinja", "ntcore/src/generate/main/native/include/wpi/ntcore/Topic" + NEW_CC_FILE_SUFFIX + ".jinja"),
        ("ntcore/src/generate/main/native/include/ntcore_cpp_types.h.jinja", "ntcore/src/generate/main/native/include/ntcore_cpp_types.hpp.jinja"),
        ("/home/pjreiniger/git/allwpilib/wpilibj/src/test/resources/edu/wpi/first/wpilibj/", "/home/pjreiniger/git/allwpilib/wpilibj/src/test/resources/org/wpilib/"),
        ("wpilibc/src/generate/main/native/include/frc/", "wpilibc/src/generate/main/native/include/wpi/"),
        ("wpilibc/src/generate/main/native/include/wpi/hid.h.jinja", "wpilibc/src/generate/main/native/include/wpi/driverstation/hid" + NEW_CC_FILE_SUFFIX + ".jinja"),
        ("wpilibc/src/generate/main/native/include/wpi/motorcontroller/pwm_motor_controller.h.jinja", "wpilibc/src/generate/main/native/include/wpi/hardware/motor/pwm_motor_controller" + NEW_CC_FILE_SUFFIX + ".jinja"),
        ("wpilibc/src/generate/main/native/include/wpi/simulation/hidsim.h.jinja", "wpilibc/src/generate/main/native/include/wpi/simulation/hidsim.hpp.jinja"),
    ]

@dataclasses.dataclass
class PreprocessedConfig:
    cc_file_renames: int
    cc_include_replacements: int
    java_pkg_renames: int
    java_file_renames: Dict[str, str]
    java_class_package_overrides: Dict[str, str]

    def write_json(self, f):
        return json.dump(
            dict(
                cc_file_renames=self.cc_file_renames,
                cc_include_replacements=self.cc_include_replacements,
                java_file_renames=self.java_file_renames,
                java_pkg_renames=self.java_pkg_renames,
                java_class_package_overrides=self.java_class_package_overrides,
            ),
            f,
            indent=4,
        )


def crawl_for_renames(
    file_renames, include_root, subpath, old_pkg_as_dir, new_pkg_as_dir
):
    for root, _, files in os.walk(include_root + subpath):
        for f in files:
            original_file = pathlib.Path(root) / f
            new_file = str((pathlib.Path(root) / f)).replace(
                old_pkg_as_dir, new_pkg_as_dir
            )
            if str(original_file) != new_file:
                file_renames[str(original_file)] = new_file


def maybe_rename_java_file(class_import_renames, class_package_overrides, file_renames, project, old_pkg, new_pkg, subpath, class_name):

    old_pkg_as_dir = old_pkg.replace(".", "/")
    new_pkg_as_dir = new_pkg.replace(".", "/")
    
    original_file = (
        pathlib.Path(project)
        / subpath
        / old_pkg_as_dir
        / (class_name + ".java")
    )
    new_file = (
        pathlib.Path(project)
        / subpath
        / new_pkg_as_dir
        / (class_name + ".java")
    )
    if original_file.exists():
        class_import_renames[old_pkg + "." + class_name] = (
            new_pkg + "." + class_name
        )
        class_package_overrides[str(new_file)] = new_pkg
        file_renames[str(original_file)] = str(new_file)

def preprocess_java_renames():

    full_package_replacements = {}

    file_renames = {}

    for project, old_pkg, new_pkg in RawConfig.JAVA_PROJECT_REPLACMENTS:
        logging.info(f"Preprocessing java project {project}")
        old_pkg_as_dir = old_pkg.replace(".", "/")
        new_pkg_as_dir = new_pkg.replace(".", "/")
        full_package_replacements[old_pkg] = new_pkg
        full_package_replacements[old_pkg.replace(".", "_")] = new_pkg.replace(".", "_")
        full_package_replacements[old_pkg_as_dir] = new_pkg_as_dir

        include_root = project
        crawl_for_renames(file_renames, include_root, "/src/main/java", old_pkg_as_dir, new_pkg_as_dir)
        crawl_for_renames(file_renames, include_root, "/src/dev/java", old_pkg_as_dir, new_pkg_as_dir)
        crawl_for_renames(file_renames, include_root, "/src/generated/main/java", old_pkg_as_dir, new_pkg_as_dir,)
        crawl_for_renames(file_renames, include_root, "/src/test/java", old_pkg_as_dir, new_pkg_as_dir)

    class_import_renames = {}
    class_package_overrides = {}
    for project, replacements in RawConfig.JAVA_FILE_RENAMES:
        for old_pkg, new_pkg, class_name in replacements:
            old_pkg = old_pkg.replace("/", ".")
            new_pkg = new_pkg.replace("/", ".")

            maybe_rename_java_file(class_import_renames, class_package_overrides, file_renames, project, old_pkg, new_pkg, "src/main/java", class_name)
            maybe_rename_java_file(class_import_renames, class_package_overrides, file_renames, project, old_pkg, new_pkg, "src/test/java", class_name)
            maybe_rename_java_file(class_import_renames, class_package_overrides, file_renames, project, old_pkg, new_pkg, "src/test/java", class_name + "Test")
            maybe_rename_java_file(class_import_renames, class_package_overrides, file_renames, project, old_pkg, new_pkg, "src/generated/main/java", class_name)

    package_replacements = {}
    package_replacements.update(class_import_renames)
    package_replacements.update(full_package_replacements)

    return package_replacements, file_renames, class_package_overrides


def _preprocess_cc_file(original_dir, new_dir, original_file, include_root):
    original_rel = original_file.relative_to(include_root)
    
    if original_file.suffix == ".h" and "thirdparty" not in str(original_file):
        destination_file = include_root / (
            str(original_rel).replace(original_dir, new_dir) + "pp"
        )
    else:
        destination_file = include_root / str(original_rel).replace(
            original_dir, new_dir
        )
    destination_rel = destination_file.relative_to(include_root)

    return original_file, destination_file, original_rel, destination_rel


def preprocess_cc_renames(preprocessor_file):
    file_renames = {}
    include_replacements = {}

    for include_root, original, new in RawConfig.CC_FOLDER_RENAMES:
        include_root = pathlib.Path(include_root)
        logging.info(f"Preprocessing {include_root}")

        for root, _, files in os.walk(include_root):
            root = pathlib.Path(root)
            for f in files:
                original_file = root / f

                if original not in str(root):
                    logging.debug(f"Skipping preprocessing for {original_file}")
                    continue

                res = _preprocess_cc_file(original, new, original_file, include_root)
                if res:
                    original_file, destination_file, original_rel, destination_rel = res

                    file_renames[str(original_file)] = str(destination_file)
                    include_replacements[str(original_rel)] = str(destination_rel)

    for include_root, replacements in RawConfig.CC_FILE_RENAMES:
        include_root = pathlib.Path(include_root)
        for original, new in replacements:
            original_file = include_root / original
            destination_file = include_root / new
            file_renames[str(original_file)] = str(destination_file)
            include_replacements[original] = str(new)

    # These include replacements are mostly due to inculdes that did not have fully qualified names.
    include_replacements["EventLoop.h"] = "EventLoop" + NEW_CC_FILE_SUFFIX
    include_replacements["PneumaticsBase.h"] = "PneumaticsBase" + NEW_CC_FILE_SUFFIX
    include_replacements["EdgeConfiguration.h"] = "EdgeConfiguration" + NEW_CC_FILE_SUFFIX

    include_replacements["HandlesInternal.h"] = "HandlesInternal" + NEW_CC_FILE_SUFFIX
    include_replacements["util/Color.h"] = "wpi/util/Color" + NEW_CC_FILE_SUFFIX
    include_replacements["Color.h"] = "wpi/util/Color" + NEW_CC_FILE_SUFFIX
    include_replacements["util/Color8Bit.h"] = "wpi/util/Color8Bit" + NEW_CC_FILE_SUFFIX
    include_replacements["BooleanEvent.h"] = "wpi/event/BooleanEvent" + NEW_CC_FILE_SUFFIX
    include_replacements["../../include/frc/controller/BangBangController.h"] = "wpi/math/controller/BangBangController" + NEW_CC_FILE_SUFFIX
    include_replacements["MechanismObject2d.h"] = "wpi/smartdashboard/MechanismObject2d" + NEW_CC_FILE_SUFFIX

    include_replacements["Odometry.h"] = "wpi/math/kinematics/Odometry" + NEW_CC_FILE_SUFFIX
    include_replacements["Odometry3d.h"] = "wpi/math/kinematics/Odometry3d" + NEW_CC_FILE_SUFFIX
    include_replacements["SwerveDriveKinematics.h"] = "wpi/math/kinematics/SwerveDriveKinematics" + NEW_CC_FILE_SUFFIX
    include_replacements["SwerveModulePosition.h"] = "wpi/math/kinematics/SwerveModulePosition" + NEW_CC_FILE_SUFFIX
    include_replacements["SwerveModuleState.h"] = "wpi/math/kinematics/SwerveModuleState" + NEW_CC_FILE_SUFFIX
    include_replacements["Trigger.h"] = "wpi/command/button/Trigger" + NEW_CC_FILE_SUFFIX

    return file_renames, include_replacements



############################################
# File renaming / fixups
############################################
def crawl_and_replace(dir_to_crawl, file_filter, replacement_callback):

    excluded_dirs = [".venv", ".git", "build", ".gradle"]

    for root, dirs, files in os.walk(dir_to_crawl):
        dirs[:] = [d for d in dirs if d not in excluded_dirs and "bazel-" not in d]

        for f in files:

            full_file = os.path.join(root, f)
            if not file_filter(full_file):
                continue

            with open(full_file) as fs:
                contents = fs.read()

            contents = replacement_callback(contents)

            with open(full_file, "w") as of:
                of.write(contents)


def rename_projects():
    for _, new in RawConfig.PROJECT_RENAMES:
        if os.path.exists(new):
            shutil.rmtree(new)

    for original, new in RawConfig.PROJECT_RENAMES:
        logging.info(f"Moving project {original} -> {new}")
        original = pathlib.Path(original)
        new = pathlib.Path(new)

        new.parent.mkdir(parents=True, exist_ok=True)
        original.rename(new)

    _make_commit("SCRIPT Move subprojects")


def fixup_project_renames():
    def fixup_filter(full_file):
        suffix = full_file.split(".")[-1]
        return suffix not in ["pyc", "jar", "gz", "png", "jpg", "icns", "ico", "avi", "mp4", "bat"]

    def fixup_impl(contents):
        contents = contents.replace("wpilibNewCommands", "command")
        contents = contents.replace("wpilibnewcommands", "command")
        contents = contents.replace("fieldImages", "fields")
        contents = contents.replace("fieldimages", "fields")

        return contents

    crawl_and_replace(".", fixup_filter, fixup_impl)
    _make_commit("SCRIPT fixup project rename")


def run_cc_renames(pp_config: PreprocessedConfig):

    for original, new in pp_config.cc_file_renames.items():
        new = pathlib.Path(new)
        new.parent.mkdir(parents=True, exist_ok=True)
        if pathlib.Path(original).exists():
            shutil.move(original, new)

    _make_commit("SCRIPT Move cc files")


def run_cc_include_fixup(pp_config):
    def cc_replacement_filter(full_file):
        if full_file.endswith("/pyproject.toml"):
            return True

        suffix = full_file.split(".")[-1]
        return suffix in ["c", "cpp", "h", "hpp", "jinja", "mm"]

    def cc_replacement_impl(contents):
        for old_pkg, new_pkg in pp_config.cc_include_replacements.items():
            contents = contents.replace(f'"{old_pkg}"', f'"{new_pkg}"')
            contents = contents.replace(f"<{old_pkg}>", f"<{new_pkg}>")

        return contents

    crawl_and_replace(".", cc_replacement_filter, cc_replacement_impl)

    _make_commit("SCRIPT Run cc include replacements")


def generic_renames():
    logging.info("Running generic renames")
    for original, new in RawConfig.GENERIC_RENAMES:
        logging.debug(f"  {original} -> {new}")
        if os.path.exists(original):
            pathlib.Path(new).parent.mkdir(parents=True, exist_ok=True)
            shutil.move(original, new)

    _make_commit("SCRIPT Generic Renames")



def run_java_renames(pp_config: PreprocessedConfig):

    for original_file, new_file in pp_config.java_file_renames.items():
        pathlib.Path(new_file).parent.mkdir(parents=True, exist_ok=True)
        if pathlib.Path(original_file).exists():
            shutil.move(original_file, new_file)

    _make_commit("SCRIPT Move java files")


def run_java_fixup_imports(pp_config: PreprocessedConfig):
    logging.info("Fixing java imports")

    def java_replacement_filter(full_file):
        if full_file.endswith("/CMakeLists.txt"):
            return True
        if "ntcore/src/generate/types.json" in full_file:
            return True
        if "styleguide" in full_file:
            return True
        suffix = full_file.split(".")[-1]
        return suffix in ["java", "cpp", "jinja", "proto", "Extension"]

    def java_replacement_impl(contents):
        for old_pkg, new_pkg in pp_config.java_pkg_renames.items():
            contents = contents.replace(old_pkg, new_pkg)

        return contents

    crawl_and_replace(".", java_replacement_filter, java_replacement_impl)

    for filename, new_pkg in pp_config.java_class_package_overrides.items():
        with open(filename, "r") as f:
            contents = f.read()

        contents = re.sub("package .*;", f"package {new_pkg};", contents)

        with open(filename, "w") as f:
            f.write(contents)

    _make_commit("SCRIPT Run java package replacements")


############################################
# Namespace Changing
############################################

# The namespace replacemnt can be tricky; since some original namespaces are shared between subprojects 
# (wpiutil and wpinet both were wpi::, wpimath and wpilibc were both in frc::), you cannot trivially 
# do bulk replacements. Furthermore, since there is no conistency in how namespaces are denoted in cpp files
# (some do 'using namespace frc' while others do 'namespace frc {...}), or when fully qualified names were used
# some hackiness is required.
#
# This approach attempts to do as many bulk replacements as possible, but must first do select replacements of 
# items known a priori. For example, frc::Pose2d (or simply Pose2d) is known to have been moved to 
# the wpi::math::Pose2d namespace

def wpiutil_namespaced_classes_and_functions(namespaced_only=True):

    # This is used only by wpinet. Since it was in the same wpi:: namespace and lots of usages in code
    # did not use a fully qualified name, these items will be replaced in wpinet when they appears by themselves.
    include_space_items = [
        "DenseMap",
        "Logger",
        "SmallDenseMap",
        "SmallPtrSet",
        "SmallSet",
        "SmallString",
        "SmallVector",
        "SmallVectorImpl",
        "StringMap",
        
        "drop_back",
        "drop_front",
        "ends_with", 
        "ends_with_lower",
        "equals_lower", 
        "ltrim",
        "rsplit",
        "rtrim",
        "starts_with",
        "substr",
        "take_back",
        "take_front",
        "hexDigitValue",
        "parse_integer",

        "SafeThread",
        "SafeThreadOwner",
        "PromiseFactory",
        
        "raw_istream",
        "raw_ostream",
        "raw_svector_ostream",
        "raw_uvector_ostream",
        "raw_string_ostream",
        "raw_fd_istream",
        "raw_fd_ostream",
        "safe_malloc",
        "safe_calloc",

        "SHA1",
        
        "future<",
        "promise<",
        "SpanEq",
        "Base64Encode",
        "UidVector",
    ]

    # These items will only be relaced if they are in the form wpi::CLASS_NAME
    namespaced_only_items = [
        "contains",
        "contains_lower",
        "slice",
        "split",
        "trim",
        "remove_prefix",
        "remove_suffix",
        "to_string_view",
        "make_string",
        "parse_float",
        "find_lower",
        "ct_string",
        "hexdigit",
        "isDigit",
        "isHexDigit",
        "isPrint",
        "format_to_n_c_str",
        "PrintTo",
        "UnescapeCString",
        "NullDeleter",
        "DecayedDerivedFrom",

        "Event",
        
        "function_ref",
        "array",

        "mutex",
        "recursive_spinlock",
        "recursive_mutex",
        "priority_queue",
        "condition_variable",
        "spinlock",
        "circular_buffer",
        "unique_function",
        "insert_sorted",
        "FastQueue",
        "MemoryBuffer",
        "array",
        "empty_array",
        "expected",
        "unexpected",
        "byteswap",
        "for_each",
        "Base64Decode",
        "EventVector",
        "GetStackTrace",
        "Lerp",
        "DecodeLimits",
        "NumToCtString",
        "CallbackListenerData",
        "CallbackThread",
        "CallbackManager",
        "DenseMap",
        "CreateEvent",

        "print",
        "is_constexpr",
        "sgn",
        
        "json",
        
        "WPI_LOG_INFO",
        "WPI_LOG_CRITICAL",
        "WPI_LOG_ERROR",
        "WPI_LOG_WARNING",
        "WPI_LOG_DEBUG",
        "WPI_LOG_DEBUG1",
        "WPI_LOG_DEBUG2",
        "WPI_LOG_DEBUG3",
        "WPI_LOG_DEBUG4",

        "sys::windows",

        "UTF16",
        "convertUTF16ToUTF8String",
        "Now",
        "SetNowImpl",
        "NowDefault",
        "GetSystemTime",

        "RawFrame",
        "SetFrameData",
        "SignalObject",
        "SetEvent",
        "SafeThreadEvent",
        "WaitForObject",
        "WaitForObjects",

        "iterator_range",

        "GetTypeName",
        "Concat",
        "Struct",
        "Protobuf",
        "ProtobufSerializable",
        "StructSerializable",
        "ProtobufMessage",
        "ForEachStructSchema",
        "StructArrayBuffer",
        "MakeStructArrayTypeString",
        "UnpackStructInto",
        "GetStructTypeString",
        "UnpackStruct",
        "PackStruct",
        "GetStructSize",
        "UnpackCallback",
        "PackCallback",
        "StdVectorUnpackCallback",
        "ProtoInputStream",
        "ProtoOutputStream",
        "HasNestedStruct",
        "WpiArrayUnpackCallback",
        "UnpackStructArray",
        "PackStructArray",
        "StructFieldDescriptor",
        "DynamicStruct",
        "StructFieldType",
        "StructDescriptorDatabase",
        
        "Sendable",
        "SendableBuilder",
        "SendableHelper",
        "SendableRegistry",

        "support::endian",
        "endianness::native",
        "sig::ScopedConnection",
        "sig::Signal",
        "sig::Signal_mt",
        "sig::SignalBase",
        "SetSignalObject",
    ]

    output = []

    for item in namespaced_only_items + include_space_items:
        output.append((r"(\b)wpi::" + item + r"(\b)", r"\1wpi::util::" + item + r"\2"))
    
    for item in include_space_items: 
        output.append((r"([ !\(])" + item + r"(\b)", r"\1wpi::util::" + item + r"\2"))

    output.append(("wpi::java", "wpi::util::java"))
    output.append(("wpi::kHandleType", "wpi::util::kHandleType"))
    output.append((" support::endian::", " wpi::util::support::endian::"))
    return output


def wpimath_namespaced_classes_and_functions():
    items = [
        "Pose2d",
        "Pose3d",
        "Quaternion",
        "Rotation2d",
        "Rotation3d",
        "Transform2d",
        "Transform3d",
        "Translation2d",
        "Translation3d",
        "LinearSystem",
        "LinearSystemLoop",
        "InputModulus",
        "DCMotor",
        "Matrixd",
        "Vectord",
        "MakeWhiteNoiseVector",
        "DesaturateInputVector",
        "PIDController",
        "ProfiledPIDController",
        "FloorMod",
        "ChassisSpeeds",
        "DifferentialDriveKinematics",
        "DifferentialDriveKinematicsConstraint",
        "DifferentialDriveWheelSpeeds",
        "DifferentialDriveOdometry",
        "DifferentialDrivePoseEstimator",
        "MecanumDriveWheelPositions",
        "MecanumDriveWheelSpeeds",
        "MecanumDriveKinematics",
        "MecanumDriveOdometry",
        "MecanumDrivePoseEstimator",
        "SwerveModuleState",
        "SwerveModulePosition",
        "SwerveDriveKinematics",
        "SwerveDriveOdometry",
        "SwerveDrivePoseEstimator",
        "LinearPlantInversionFeedforward",
        "LinearQuadraticRegulator",
        "SimpleMotorFeedforward",
        "ElevatorFeedforward",
        "BangBangController",
        "LTVUnicycleController",
        "Trajectory",
        "TrajectoryConfig",
        "TrajectoryGenerator",
        "DiscretizeAB",
        "RKDP",
        "LinearFilter",
        "LinearSystem",
        "LinearSystemId",
        "MedianFilter",
        "Debouncer",
        "ApplyDeadband",
        "CopySignPow",
        "Debouncer",
        "TrapezoidProfile",
        "ExponentialProfile",
        "SlewRateLimiter",
        "KalmanFilter",
        "Debouncer",
        "TravelingSalesman",
    ]

    output = []
    
    for item in items:
        output.append((r"(\b)frc::" + item + r"(\b)", r"\1wpi::math::" + item + r"\2"))
        output.append((r"([ \b{<!\(])" + item + r"(\b)", r"\1wpi::math::" + item + r"\2"))
        output.append((r"^" + item + r"(\b)", r"\1wpi::math::" + item))

    output.append(("wpi::math::wpi::math::", "wpi::math::"))
    output.append(("wpi::math::wpi::math::", "wpi::math::"))

    return output

def wpinet_namespaced_classes_and_functions():

    output = []
    output.append(("wpi::WebSocket", "wpi::net::WebSocket")),
    output.append(("wpi::DsClient", "wpi::net::DsClient")),
    output.append(("wpi::EventLoopRunner", "wpi::net::EventLoopRunner")),
    output.append(("wpi::GetHostname", "wpi::net::GetHostname")),
    output.append(("wpi::HttpConnection", "wpi::net::HttpConnection")),
    output.append(("wpi::HttpLocation", "wpi::net::HttpLocation")),
    output.append(("wpi::HttpRequest", "wpi::net::HttpRequest")),
    output.append(("wpi::HttpWebSocketServerConnection", "wpi::net::HttpWebSocketServerConnection")),
    output.append(("wpi::ParallelTcpConnector", "wpi::net::ParallelTcpConnector")),
    output.append(("wpi::NetworkStream", "wpi::net::NetworkStream")),
    output.append(("wpi::raw_uv_ostream", "wpi::net::raw_uv_ostream")),
    output.append(("wpi::TCPAcceptor", "wpi::net::TCPAcceptor")),
    output.append(("wpi::TCPConnector", "wpi::net::TCPConnector")),
    output.append(("wpi::UnescapeURI", "wpi::net::UnescapeURI")),
    output.append(("wpi::UrlParser", "wpi::net::UrlParser")),
    output.append(("wpi::raw_socket_istream", "wpi::net::raw_socket_istream")),
    output.append(("wpi::raw_socket_ostream", "wpi::net::raw_socket_ostream")),
    output.append(("wpi::uv", "wpi::net::uv")),
    output.append(("wpi::NetworkAcceptor", "wpi::net::NetworkAcceptor")),
    output.append(("wpi::WebSocket", "wpi::net::WebSocket")),
    output.append(("wpi::HTTP_CONNECT", "wpi::net::HTTP_CONNECT")),
    output.append(("wpi::HTTP_GET", "wpi::net::HTTP_GET")),
    output.append(("wpi::MimeTypeFromPath", "wpi::net::MimeTypeFromPath")),
    output.append(("wpi::EscapeURI", "wpi::net::EscapeURI")),
    output.append(("wpi::http_method_str", "wpi::net::http_method_str")),
    
    output.append(("!FindMultipartBoundary", "!wpi::net::FindMultipartBoundary")),
    output.append(("!ParseHttpHeaders", "!wpi::net::ParseHttpHeaders")),

    return output
    
def ntcore_namespaced_classes_and_functions():

    output = []
    output.append((r"(\b)nt::", r"\1wpi::nt::")),
    
    output.append(("wpi::wpi::nt::", "wpi::nt::")),

    return output


def wpiunits_namespaced_classes_and_functions():

    output = []
    output.append((r"(\b)units::", r"\1wpi::units::")),
    output.append((r"^units::", r"wpi::units::")),
    
    output.append(("wpi::wpi::units::", "wpi::units::")),

    return output




def common_fwd_def_namespace_replacements():
    """
    This function attempts to greatly reduce the number of fwd defs that have to be manually fixed
    """
    output = []


    output.append(("""namespace wpi {
template <typename T>
class wpi::util::SmallVectorImpl;
}  // namespace wpi""", """namespace wpi::util {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi"""))

    output.append(("""namespace wpi {
class wpi::util::Logger;
}  // namespace wpi""", """namespace wpi::util {
class Logger;
}  // namespace wpi"""))

    output.append(("""namespace wpi {
class wpi::util::raw_ostream;
}  // namespace wpi""", """namespace wpi::util {
class raw_ostream;
}  // namespace wpi"""))

    output.append(("""namespace wpi {
class wpi::util::Logger;
template <typename T>
class wpi::util::SmallVectorImpl;
}  // namespace wpi""", """namespace wpi::util {
class Logger;
template <typename T>
class SmallVectorImpl;
}  // namespace wpi"""))

    output.append(("""namespace wpi {
class wpi::util::Logger;
class wpi::util::raw_ostream;
}  // namespace wpi""", """namespace wpi::util {
class Logger;
class raw_ostream;
}  // namespace wpi"""))

    output.append(("""namespace wpi {
template <typename T>
class wpi::util::SmallVectorImpl;
}  // namespace wpi""", """namespace wpi::util {
template <typename T>
class SmallVectorImpl;
}  // namespace wpi"""))

    output.append(("""namespace wpi {
class wpi::util::raw_istream;
}  // namespace wpi""", """namespace wpi::util {
class raw_istream;
}  // namespace wpi"""))

    output.append(("""namespace wpi {
class wpi::util::raw_ostream;
class wpi::util::Logger;
}  // namespace wpi""", """namespace wpi::util {
class raw_ostream;
class Logger;
}  // namespace wpi"""))


    output.append(("class wpi::util::SmallVectorImpl;", "class SmallVectorImpl;"))
    output.append(("class wpi::util::Logger;", "class Logger;"))
    output.append(("class wpi::util::raw_ostream;", "class raw_ostream;"))
    output.append(("class wpi::util::raw_istream;", "class raw_istream;"))

    return output

NAMESPACE_PROJECT_REPLACEMENTS = [

    ##################
    # apriltag
    ##################
    (
        "apriltag",
        wpiutil_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + wpimath_namespaced_classes_and_functions() + [
            ("namespace frc", "namespace wpi::apriltag"),
            ("frc::", "wpi::apriltag::"),
        ],
    ),
    ##################
    # Benchmark
    ##################
    (
        "benchmark",
        wpiutil_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + wpimath_namespaced_classes_and_functions() + [],
    ),
    
    ##################
    # cameraserver
    ##################
    (
        "cameraserver/multiCameraServer", wpiutil_namespaced_classes_and_functions() + ntcore_namespaced_classes_and_functions() + [
            ("namespace frc", "namespace wpi"),
            ("frc::", "wpi::"),
        ],
    ),
    (
        "cameraserver/src/main/native/include/wpi/cameraserver",
        wpiutil_namespaced_classes_and_functions() + [
            ("namespace frc", "namespace wpi"),
            ("frc::", "wpi::"),
        ] ,
    ),
    (
        "cameraserver/src/main/native/cpp/cameraserver", wpiutil_namespaced_classes_and_functions() +  [
            ("namespace frc", "namespace wpi"),
            ("frc::", "wpi::"),
        ],
    ),
    (
        "cameraserver/src/main/native/include/wpi/vision",
        wpiutil_namespaced_classes_and_functions() + [
            ("namespace frc", "namespace wpi::vision"),
            ("frc::", "wpi::vision::"),
        ],
    ),
    (
        "cameraserver/src/main/native/cpp/vision", wpiutil_namespaced_classes_and_functions() + [
            ("namespace frc", "namespace wpi::vision"),
            ("frc::", "wpi::vision::"),
        ],
    ),

    ##################
    # command
    ##################
    (
        "command",
        wpiutil_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + wpimath_namespaced_classes_and_functions() + ntcore_namespaced_classes_and_functions() + [
            ("namespace frc2", "namespace wpi::cmd"),
            ("frc2::", "wpi::cmd::"),
            ("frc::", "wpi::"),
        ]
    ),

    ##################
    # cscore
    ##################
    (
        "cscore", wpiutil_namespaced_classes_and_functions() + wpinet_namespaced_classes_and_functions() + [
            ("namespace cs", "namespace wpi::cs"),
            ("cs::", "wpi::cs::"),

            # Undo duplicates
            ("wpi::wpi::cs::", "wpi::cs::"),
        ] + common_fwd_def_namespace_replacements(),
    ),

    ##################
    # datalog
    ##################
    ("datalog", wpiutil_namespaced_classes_and_functions() + [
        (" GetStructTypeString", " wpi::util::GetStructTypeString"),
        (" StructSerializable", " wpi::util::StructSerializable"),
        (" MakeStructArrayTypeString", " wpi::util::MakeStructArrayTypeString"),
        (" StructArrayBuffer", " wpi::util::StructArrayBuffer"),
        (" ForEachStructSchema", " wpi::util::ForEachStructSchema"),
        (r"([ \(])ProtobufMessage", r"\1wpi::util::ProtobufMessage"),
        (" ProtobufSerializable", " wpi::util::ProtobufSerializable"),
        ("<ProtobufSerializable", "<wpi::util::ProtobufSerializable"),
        (r"(\b)Struct(\b)", r"\1wpi::util::Struct\2"),
        (r"(\b)Protobuf(\b)", r"\1wpi::util::Protobuf\2"),
        (r"(\b)MemoryBuffer(\b)", r"\1wpi::util::MemoryBuffer\2"),

        # Undo
        ("wpi::util::wpi::util", "wpi::util"),
        ("wpi::util::Protobuf.hpp", "Protobuf.hpp"),
        ("wpi::util::Struct.hpp", "Struct.hpp"),
        ("wpi::util::MemoryBuffer.h", "MemoryBuffer.h"),
        ("wpi::wpi::util", "wpi::util"),
    ] + common_fwd_def_namespace_replacements()),
    
    ##################
    # developer robot
    ##################
    (
        "developerRobot",  [
            ("frc::", "wpi::"),
        ],
    ),
    ##################
    # fields
    ##################
    ("fields", [
            ("namespace fields", "namespace wpi::fields"),
            ("fields::", "wpi::fields::"),

            # Undo
            ("wpi::wpi::fields::", "wpi::fields::"),
    ]),
    
    ##################
    # glass
    ##################
    (
        "glass", wpiutil_namespaced_classes_and_functions() + ntcore_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + [
            ("namespace glass", "namespace wpi::glass"),
            ("glass::", "wpi::glass::"),
            (" fields::", " wpi::fields::"),

            ("frc::", "wpi::math::"),

            ("wpi::wpi::glass", "wpi::glass"),
            ("wpi::wpi::glass", "wpi::glass"),
        ] + common_fwd_def_namespace_replacements(),
    ),

    ##################
    # hal
    ##################
    (
        "hal", wpiutil_namespaced_classes_and_functions() + ntcore_namespaced_classes_and_functions() + wpinet_namespaced_classes_and_functions() + [
            ("namespace hal", "namespace wpi::hal"),
            ("hal::", "wpi::hal::"),
            
            ("wpi::wpi::hal::", "wpi::hal::"),
        ],
    ),

    ##################
    # ntcore
    ##################
    ("ntcore", wpiutil_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + wpinet_namespaced_classes_and_functions() + [
            ("namespace nt", "namespace wpi::nt"),
            (r"(\b)nt::", r"\1wpi::nt::"),

            
            ("wpi::wpi::nt::", "wpi::nt::"),

            ("wpi::uv::", "wpi::net::uv::"),
    ] + common_fwd_def_namespace_replacements()),
    
    ##################
    # ntcoreffi
    ##################
    (
        "ntcoreffi", wpiutil_namespaced_classes_and_functions() + wpinet_namespaced_classes_and_functions(),
    ),

    ##################
    # Romi Vendordep
    ##################
    (
        "romiVendordep", wpimath_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + [
            ("namespace frc", "namespace wpi::romi"),
            ("frc::", "wpi::"),
        ],
    ),
    
    ##################
    # Simulation
    ##################
    (
        "simulation",
        wpiutil_namespaced_classes_and_functions() + wpinet_namespaced_classes_and_functions() + ntcore_namespaced_classes_and_functions()
        + [
            ("([&\( <])glass::", r"\1wpi::glass::"),
            ("namespace glass {", r"namespace wpi::glass {"),
            ("::glass", "::wpi::glass"),
            ("^glass::", "wpi::glass::"),

            # Undo
            ("wpi::wpi::glass", "wpi::glass"),
        ],
    ),
    ##################
    # Tools
    ##################
    (
        "tools", [
            ("([&\( <])glass::", r"\1wpi::glass::"),
            ("namespace glass {", "namespace wpi::glass {"),
            ("^glass::", "wpi::glass::"),
        ] + wpiutil_namespaced_classes_and_functions() + wpimath_namespaced_classes_and_functions() + ntcore_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + [
            ("// wpi::util::Logger", "// Logger"),
        ],
    ),
    
    ##################
    # wpilibc
    ##################
    (
        "wpilibc",
        wpiutil_namespaced_classes_and_functions() + ntcore_namespaced_classes_and_functions() + wpimath_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + [
            ("namespace frc", "namespace wpi"),
            ("frc::", "wpi::"),
            (" hal::", " wpi::hal::"),

        ] + common_fwd_def_namespace_replacements(),
    ),
    
    ##################
    # wpilibcExamples
    ##################
    (
        "wpilibcExamples", wpiutil_namespaced_classes_and_functions() + wpimath_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + [
            ("frc::OnBoardIO", "wpi::romi::OnBoardIO"),
            ("frc::XRP", "wpi::xrp::XRP"),
            ("frc::Romi", "wpi::romi::Romi"),
            ("frc::April", "wpi::apriltag::April"),
            (" cs::", " wpi::cs::"),
            (r"([ {])nt::", r"\1wpi::nt::"),
            ("frc::ObjectToRobotPose", "wpi::math::ObjectToRobotPose"),
            
            ("frc2::", "wpi::cmd::"),
            ("frc::", "wpi::"),
        ],
    ),
    ##################
    # wpimath
    ##################
    (
        "wpimath", wpiutil_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + [
            ("namespace frc", "namespace wpi::math"),
            ("frc::", "wpi::math::"),
        ]
    ),
    ##################
    # wpimath
    ##################
    (
        "wpimath/src/main/native/include/wpi/units", wpiutil_namespaced_classes_and_functions() + [
            ("namespace units", "namespace wpi::units"),
            (r"(\b)units::", r"\1wpi::units::"),

            # Undo
            ("wpi::wpi::units", "wpi::units"),
        ]
    ),
    ##################
    # wpinet
    ##################
    (
        "wpinet",
        wpiutil_namespaced_classes_and_functions() + [
            ("namespace wpi", "namespace wpi::net"),
            ("wpi::", "wpi::net::"),

            # For undoing changes if run twice
            ("wpi::net::net::", "wpi::net::"),
            ("wpi::net::net", "wpi::net"),
            
            ("wpi::net::util", "wpi::util"),

            (r"\(!equals", "(!wpi::util::equals"),
            (r"\(equals", "(wpi::util::equals"),

            (r"(\b)wpi::net::sig::", r"\1wpi::util::sig::"),
            (" sig::Connection", " wpi::util::sig::Connection"),
            (" sig::Signal", " wpi::util::sig::Signal"),
            (" sig::ScopedConnection", " wpi::util::sig::ScopedConnection"),
            (r" split\(", " wpi::util::split("),
            (r" trim\(", " wpi::util::trim("),
            (r"\(function_ref<", "(wpi::util::function_ref<"),
            (r"<function_ref<", "<wpi::util::function_ref<"),

        ] + [
            # Misc cleanup
            ("HttpPathRef wpi::util::drop_front", "HttpPathRef drop_front"),
            ("loop wpi::util::Event loop", "loop Event loop"),
            ("@return wpi::util::Event", "@return Event"),
            ("Filesystem wpi::util::Event handle.", "Filesystem Event handle."),
            ("FS wpi::util::Event", "FS Event"),
            ("wpi::util::Event loop should", "Event loop should"),
            ("HANDLE wpi::util::Event", "HANDLE Event"),
            (r"wpi::util::Event loop\.", "Event loop."),
            ("logger wpi::util::Logger", "logger Logger"),
            ("@return wpi::util::Logger", "@return Logger"),
        ] + common_fwd_def_namespace_replacements(),
    ),
    ##################
    # wpiutil
    ##################
    (
        "wpiutil",
        [
            ("namespace wpi", "namespace wpi::util"),
            ("wpi::", "wpi::util::"),

            # For undoing changes if run twice
            ("wpi::util::util::", "wpi::util::"),
            ("wpi::util::util", "wpi::util"),
        ]
    ),
    ##################
    # XRP Vendordep
    ##################
    (
        "xrpVendordep", wpimath_namespaced_classes_and_functions() + wpimath_namespaced_classes_and_functions() + wpiunits_namespaced_classes_and_functions() + [
            ("namespace frc", "namespace wpi::xrp"),
            ("frc::XRP", "wpi::xrp::XRP"),
            ("frc::", "wpi::"),
        ],
    ),
]


def run_namespace_replacements():
    def namespace_replacement_filter(full_file):
        suffix = full_file.split(".")[-1]
        return suffix in ["cpp", "h", "hpp", "mm", "jinja"]

    def namespace_replacement_impl(contents):
        for origin, new in replacements:
            contents = re.sub(origin, new, contents, flags=re.MULTILINE)

        return contents

    for project, replacements in NAMESPACE_PROJECT_REPLACEMENTS:
        logging.info(f"Fixing namespaces for {project}")

        crawl_and_replace(
            project, namespace_replacement_filter, namespace_replacement_impl
        )

    _make_commit("SCRIPT namespace replacements")


##################################
# Util Functions
##################################


def _make_commit(msg):
    subprocess.check_call(["git", "add", "."])
    subprocess.check_call(["git", "commit", "-m", msg, "--quiet"])
    pass



def load_pp_config(preprocessor_file):
    with open(preprocessor_file, "r") as f:
        pp = json.load(f)

    return PreprocessedConfig(
        cc_file_renames=pp["cc_file_renames"],
        cc_include_replacements=pp["cc_include_replacements"],
        java_pkg_renames=pp["java_pkg_renames"],
        java_file_renames=pp["java_file_renames"],
        java_class_package_overrides=pp["java_class_package_overrides"],
    )



def run_java_spotless():
    subprocess.check_call(["./gradlew", "spotlessApply"])



def pregenerate_files():
    subprocess.check_call(["bazel", "run", "//wpiutil:robotpy-wpiutil-generator.generate_build_info"])
    subprocess.check_call(["bazel", "run", "//wpinet:robotpy-wpinet-generator.generate_build_info"])
    subprocess.check_call(["bazel", "run", "//:write_all"])

    _make_commit("SCRIPT: Pregenerate files")


def run_upstream_utils():
    libraries = ["llvm", "json"]

    # python_exe = sys.executable
    python_exe = "python3.12" # TODO

    for library in libraries:
        subprocess.check_call([python_exe, f"./upstream_utils/{library}.py", "clone"])
        subprocess.check_call([python_exe, f"./upstream_utils/{library}.py", "copy-src"])
        subprocess.check_call([python_exe, f"./upstream_utils/{library}.py", "format-patch"])
        
    _make_commit("SCRIPT run upstream utils")

def run_linters():
    subprocess.check_call(["./gradlew", "spotlessApply"])
    _make_commit("SCRIPT: Spotless Apply")

    subprocess.check_call(["wpiformat", "-f", "."])
    _make_commit("SCRIPT: wpiformat")


def apply_patch(patch):
    
    if RUN_WITH_HPP:
        full_patch_file = SCRIPT_DIR + "/with_hpp/" + patch
    subprocess.check_call(["git", "am", "-3", full_patch_file])


def main():
    logging.basicConfig(level=logging.DEBUG)

    os.chdir("../../allwpilib")
    if RUN_WITH_HPP:
        preprocessor_file = SCRIPT_DIR + "/with_hpp/refactor_layout_pp.json"

    preprocess = True

    rename_projects()
    fixup_project_renames()

    if preprocess:
        cc_file_renames, cc_include_replacements = preprocess_cc_renames(
            preprocessor_file
        )
        java_pkg_renames, java_file_renames, java_class_package_overrides = (
            preprocess_java_renames()
        )

        pp_config = PreprocessedConfig(
            cc_file_renames=cc_file_renames,
            cc_include_replacements=cc_include_replacements,
            java_pkg_renames=java_pkg_renames,
            java_file_renames=java_file_renames,
            java_class_package_overrides=java_class_package_overrides,
        )
        with open(preprocessor_file, "w") as f:
            pp_config.write_json(f)
    else:
        pp_config = load_pp_config(preprocessor_file)

    run_cc_renames(pp_config)
    run_java_renames(pp_config)
    generic_renames()

    run_java_fixup_imports(pp_config)
    run_cc_include_fixup(pp_config)

    apply_patch("0008-HAND-FIX-Fixup-bazel-files.patch")
    apply_patch("0009-HAND-FIX-Fixup-pregen-files.patch")
    pregenerate_files()

    apply_patch("0011-HAND-FIXES-Fixup-java-and-python-compilation.patch")
    apply_patch("0012-HAND-FIXES-Fixup-gradle-cmake-styleguide.patch")
    run_linters()
    
    apply_patch("0015-HAND-FIXES-Fix-upstream-util-scripts.patch")
    apply_patch("0016-HAND-FIXES-Fixup-remaining-rename-issues.patch")

    # At this point the code should compile cleanly with just the file moves
    # The next phase of refactoring is running the more complicated namespace
    # replacements
    apply_patch("0017-HAND-FIXES-Update-upstream-for-namespace-changes.patch")
    run_namespace_replacements()
    run_upstream_utils()
    apply_patch("0020-HAND-FIXES-Update-build-scripts-for-namespaces.patch")
    apply_patch("0021-HAND-FIXES-Manual-cleanup-of-namespaces.patch")
    apply_patch("0022-HAND-FIXES-Update-maven-info.patch")
    run_linters()



if __name__ == "__main__":
    main()
