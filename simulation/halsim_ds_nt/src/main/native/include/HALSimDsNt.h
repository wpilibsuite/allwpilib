/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include <mockdata/DriverStationData.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/mutex.h>

enum HALSimDSNT_Mode { teleop, auton, test };

class HALSimDSNT {
 public:
  std::shared_ptr<nt::NetworkTable> rootTable, modeTable, allianceTable;
  enum HALSimDSNT_Mode currentMode;
  bool isEnabled, lastIsEnabled, isEstop;
  std::atomic<bool> isAllianceRed, running;
  std::atomic<double> currentMatchTime, timingHz, allianceStation;
  std::thread loopThread;
  wpi::mutex modeMutex;

  void Initialize();
  void HandleModePress(enum HALSimDSNT_Mode mode, bool isPressed);
  void UpdateModeButtons();
  void DoModeUpdate();
  void DoAllianceUpdate();
  void LoopFunc();
  void Flush();
};
