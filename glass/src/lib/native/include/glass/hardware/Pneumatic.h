// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>
#include <utility>

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

class PneumaticControlModel : public Model {
 public:
  virtual CompressorModel* GetCompressor() = 0;

  virtual void ForEachSolenoid(
      wpi::function_ref<void(SolenoidModel& model, int index)> func) = 0;

  virtual std::string_view GetName() = 0;
};

class PneumaticControlsModel : public Model {
 public:
  virtual void ForEachPneumaticControl(
      wpi::function_ref<void(PneumaticControlModel& model, int index)>
          func) = 0;
};

struct AllPneumaticControlsModel : public Model {
  AllPneumaticControlsModel(std::unique_ptr<PneumaticControlsModel> pcms,
                            std::unique_ptr<PneumaticControlsModel> phs)
      : pcms{std::move(pcms)}, phs{std::move(phs)} {};
  std::unique_ptr<PneumaticControlsModel> pcms;
  std::unique_ptr<PneumaticControlsModel> phs;
  void Update() override {
    pcms->Update();
    phs->Update();
  };
  bool Exists() override { return true; }
};

bool DisplayPneumaticControlSolenoids(PneumaticControlModel* model, int index,
                                      bool outputsEnabled);
void DisplayPneumaticControlsSolenoids(
    PneumaticControlsModel* model, bool outputsEnabled,
    std::string_view noneMsg = "No solenoids");

void DisplayCompressorDevice(CompressorModel* model, int index,
                             bool outputsEnabled);
void DisplayCompressorsDevice(PneumaticControlsModel* model,
                              bool outputsEnabled);

}  // namespace glass
