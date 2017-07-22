/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/controller/StateSpaceControllerCoeffs.h>
#include <frc/controller/StateSpaceLoop.h>
#include <frc/controller/StateSpaceObserverCoeffs.h>
#include <frc/controller/StateSpacePlantCoeffs.h>

frc::StateSpacePlantCoeffs<2, 1, 1> MakeSingleJointedArmPlantCoeffs();
frc::StateSpaceControllerCoeffs<2, 1, 1> MakeSingleJointedArmControllerCoeffs();
frc::StateSpaceObserverCoeffs<2, 1, 1> MakeSingleJointedArmObserverCoeffs();
frc::StateSpaceLoop<2, 1, 1> MakeSingleJointedArmLoop();
