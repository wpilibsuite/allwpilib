/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/controller/PeriodVariantLoop.h>
#include <frc/controller/PeriodVariantObserverCoeffs.h>
#include <frc/controller/PeriodVariantPlantCoeffs.h>
#include <frc/controller/StateSpaceControllerCoeffs.h>

frc::PeriodVariantPlantCoeffs<2, 1, 1> MakeElevatorPlantCoeffs();
frc::StateSpaceControllerCoeffs<2, 1, 1> MakeElevatorControllerCoeffs();
frc::PeriodVariantObserverCoeffs<2, 1, 1> MakeElevatorObserverCoeffs();
frc::PeriodVariantLoop<2, 1, 1> MakeElevatorLoop();
