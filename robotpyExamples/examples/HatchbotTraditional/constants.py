#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

#
# The constants module is a convenience place for teams to hold robot-wide
# numerical or boolean constants. Don't use this for any other purpose!
#

import math
import wpilib

# Motors
LEFT_MOTOR1_PORT = 0
LEFT_MOTOR2_PORT = 1
RIGHT_MOTOR1_PORT = 2
RIGHT_MOTOR2_PORT = 3

# Encoders
LEFT_ENCODER_PORTS = (0, 1)
RIGHT_ENCODER_PORTS = (2, 3)
LEFT_ENCODER_REVERSED = False
RIGHT_ENCODER_REVERSED = True

ENCODER_CPR = 1024
WHEEL_DIAMETER_INCHES = 6
# Assumes the encoders are directly mounted on the wheel shafts
ENCODER_DISTANCE_PER_PULSE = (WHEEL_DIAMETER_INCHES * math.pi) / ENCODER_CPR

# Hatch
HATCH_SOLENOID_MODULE_TYPE = wpilib.PneumaticsModuleType.CTRE_PCM
HATCH_SOLENOID_MODULE = 0
HATCH_SOLENOID_PORTS = (0, 1)

# Autonomous
AUTO_DRIVE_DISTANCE_INCHES = 60
AUTO_BACKUP_DISTANCE_INCHES = 20
AUTO_DRIVE_VELOCITY = 0.5

# Operator Interface
DRIVER_CONTROLLER_PORT = 0

# Physical parameters
DRIVE_TRAIN_MOTOR_COUNT = 2
TRACK_WIDTH = 0.381 * 2
GEARING_RATIO = 8
WHEEL_RADIUS = 0.0508

# kEncoderResolution = -
