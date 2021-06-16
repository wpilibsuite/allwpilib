// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/function_ref.h>

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
                          std::string_view noneMsg = "No solenoids");

void DisplayCompressorDevice(PCMModel* model, int index, bool outputsEnabled);
void DisplayCompressorDevice(CompressorModel* model, int index,
                             bool outputsEnabled);
void DisplayCompressorsDevice(PCMsModel* model, bool outputsEnabled);

}  // namespace glass
