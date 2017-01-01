/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#define SIMULATION "gazebo"

#include <cstring>
#include <iostream>

#include "AnalogGyro.h"
#include "AnalogInput.h"
#include "AnalogPotentiometer.h"
#include "Buttons/Button.h"
#include "Buttons/InternalButton.h"
#include "Buttons/JoystickButton.h"
#include "Buttons/NetworkButton.h"
#include "Buttons/Trigger.h"
#include "Commands/Command.h"
#include "Commands/CommandGroup.h"
#include "Commands/PIDCommand.h"
#include "Commands/PIDSubsystem.h"
#include "Commands/PrintCommand.h"
#include "Commands/Scheduler.h"
#include "Commands/StartCommand.h"
#include "Commands/Subsystem.h"
#include "Commands/WaitCommand.h"
#include "Commands/WaitForChildren.h"
#include "Commands/WaitUntilCommand.h"
#include "Counter.h"
#include "DigitalInput.h"
#include "DoubleSolenoid.h"
#include "Encoder.h"
#include "GenericHID.h"
#include "IterativeRobot.h"
#include "Jaguar.h"
#include "Joystick.h"
#include "LiveWindow/LiveWindow.h"
#include "PIDController.h"
#include "RobotBase.h"
#include "RobotDrive.h"
#include "SampleRobot.h"
#include "SmartDashboard/SendableChooser.h"
#include "SmartDashboard/SmartDashboard.h"
#include "Solenoid.h"
#include "SpeedController.h"
#include "Talon.h"
#include "Victor.h"
#include "XboxController.h"
#include "interfaces/Potentiometer.h"
