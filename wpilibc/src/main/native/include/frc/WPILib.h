// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

// clang-format off
#ifdef _MSC_VER
#pragma message("warning: Including this header drastically increases compilation times and is bad style. Include only what you use instead.")
#else
#warning "Including this header drastically increases compilation times and is bad style. Include only what you use instead."
#endif

// clang-format on

#if __has_include(<cameraserver/CameraServer.h>)
#include <cameraserver/CameraServer.h>
#include <vision/VisionRunner.h>
#endif

#include "frc/ADXL345_I2C.h"
#include "frc/ADXL345_SPI.h"
#include "frc/ADXL362.h"
#include "frc/ADXRS450_Gyro.h"
#include "frc/AnalogAccelerometer.h"
#include "frc/AnalogGyro.h"
#include "frc/AnalogInput.h"
#include "frc/AnalogOutput.h"
#include "frc/AnalogPotentiometer.h"
#include "frc/AnalogTrigger.h"
#include "frc/AnalogTriggerOutput.h"
#include "frc/BuiltInAccelerometer.h"
#include "frc/Compressor.h"
#include "frc/Counter.h"
#include "frc/DMC60.h"
#include "frc/DigitalInput.h"
#include "frc/DigitalOutput.h"
#include "frc/DigitalSource.h"
#include "frc/DoubleSolenoid.h"
#include "frc/DriverStation.h"
#include "frc/Encoder.h"
#include "frc/ErrorBase.h"
#include "frc/GearTooth.h"
#include "frc/GenericHID.h"
#include "frc/I2C.h"
#include "frc/InterruptableSensorBase.h"
#include "frc/IterativeRobot.h"
#include "frc/Jaguar.h"
#include "frc/Joystick.h"
#include "frc/NidecBrushless.h"
#include "frc/Notifier.h"
#include "frc/PIDController.h"
#include "frc/PIDOutput.h"
#include "frc/PIDSource.h"
#include "frc/PWM.h"
#include "frc/PWMSpeedController.h"
#include "frc/PWMTalonSRX.h"
#include "frc/PWMVictorSPX.h"
#include "frc/PowerDistributionPanel.h"
#include "frc/Preferences.h"
#include "frc/Relay.h"
#include "frc/RobotBase.h"
#include "frc/RobotController.h"
#include "frc/RobotDrive.h"
#include "frc/SD540.h"
#include "frc/SPI.h"
#include "frc/SensorUtil.h"
#include "frc/SerialPort.h"
#include "frc/Servo.h"
#include "frc/Solenoid.h"
#include "frc/Spark.h"
#include "frc/SpeedController.h"
#include "frc/SpeedControllerGroup.h"
#include "frc/Talon.h"
#include "frc/Threads.h"
#include "frc/TimedRobot.h"
#include "frc/Timer.h"
#include "frc/Ultrasonic.h"
#include "frc/Utility.h"
#include "frc/Victor.h"
#include "frc/VictorSP.h"
#include "frc/WPIErrors.h"
#include "frc/XboxController.h"
#if __has_include("frc/buttons/InternalButton.h")
#include "frc/buttons/InternalButton.h"
#include "frc/buttons/JoystickButton.h"
#include "frc/buttons/NetworkButton.h"
#include "frc/commands/Command.h"
#include "frc/commands/CommandGroup.h"
#include "frc/commands/PIDCommand.h"
#include "frc/commands/PIDSubsystem.h"
#include "frc/commands/PrintCommand.h"
#include "frc/commands/StartCommand.h"
#include "frc/commands/Subsystem.h"
#include "frc/commands/WaitCommand.h"
#include "frc/commands/WaitForChildren.h"
#include "frc/commands/WaitUntilCommand.h"
#endif
#include "frc/drive/DifferentialDrive.h"
#include "frc/drive/KilloughDrive.h"
#include "frc/drive/MecanumDrive.h"
#include "frc/filters/LinearDigitalFilter.h"
#include "frc/interfaces/Accelerometer.h"
#include "frc/interfaces/Gyro.h"
#include "frc/interfaces/Potentiometer.h"
#include "frc/smartdashboard/SendableChooser.h"
#include "frc/smartdashboard/SmartDashboard.h"
