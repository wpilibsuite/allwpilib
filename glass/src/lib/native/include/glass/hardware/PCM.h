/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/STLExtras.h>
#include <wpi/StringRef.h>

#include "glass/Model.h"

namespace glass {

class DataSource;

class CompressorModel : public Model {
 public:
  virtual DataSource* GetRunningData() = 0;
  virtual DataSource* GetEnabledData() = 0;
  virtual DataSource* GetPressureSwitchData() = 0;
  virtual DataSource* GetCurrentData() = 0;

  virtual void SetRunning(bool val) = 0;
  virtual void SetEnabled(bool val) = 0;
  virtual void SetPressureSwitch(bool val) = 0;
  virtual void SetCurrent(double val) = 0;
};

class SolenoidModel : public Model {
 public:
  virtual DataSource* GetOutputData() = 0;

  virtual void SetOutput(bool val) = 0;
};

class PCMModel : public Model {
 public:
  virtual CompressorModel* GetCompressor() = 0;

  virtual void ForEachSolenoid(
      wpi::function_ref<void(SolenoidModel& model, int index)> func) = 0;
};

class PCMsModel : public Model {
 public:
  virtual void ForEachPCM(
      wpi::function_ref<void(PCMModel& model, int index)> func) = 0;
};

bool DisplayPCMSolenoids(PCMModel* model, int index, bool outputsEnabled);
void DisplayPCMsSolenoids(PCMsModel* model, bool outputsEnabled,
                          wpi::StringRef noneMsg = "No solenoids");

void DisplayCompressorDevice(PCMModel* model, int index, bool outputsEnabled);
void DisplayCompressorDevice(CompressorModel* model, int index,
                             bool outputsEnabled);
void DisplayCompressorsDevice(PCMsModel* model, bool outputsEnabled);

}  // namespace glass
