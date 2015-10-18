/*
 * WPILIb.h
 *
 *  Created on: May 29, 2014
 *      Author: alex
 */
#pragma once

#define SIMULATION "gazebo"

#include <iostream>
#include "string.h"

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

#include "SmartDashboard/SendableChooser.h"
#include "SmartDashboard/SmartDashboard.h"

#include "AnalogGyro.h"
#include "AnalogInput.h"
#include "AnalogPotentiometer.h"
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
#include "Solenoid.h"
#include "SpeedController.h"
#include "Talon.h"
#include "Victor.h"
#include "interfaces/Potentiometer.h"
