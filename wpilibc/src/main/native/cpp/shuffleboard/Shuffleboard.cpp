// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/Shuffleboard.h"

#include <networktables/NetworkTableInstance.h>

#include "frc/shuffleboard/ShuffleboardTab.h"

using namespace frc;

void Shuffleboard::Update() {
  GetInstance().Update();
}

ShuffleboardTab& Shuffleboard::GetTab(std::string_view title) {
  return GetInstance().GetTab(title);
}

void Shuffleboard::SelectTab(int index) {
  GetInstance().SelectTab(index);
}

void Shuffleboard::SelectTab(std::string_view title) {
  GetInstance().SelectTab(title);
}

void Shuffleboard::EnableActuatorWidgets() {
  GetInstance().EnableActuatorWidgets();
}

void Shuffleboard::DisableActuatorWidgets() {
  // Need to update to make sure the sendable builders are initialized
  Update();
  GetInstance().DisableActuatorWidgets();
}

void Shuffleboard::StartRecording() {
  GetRecordingController().StartRecording();
}

void Shuffleboard::StopRecording() {
  GetRecordingController().StopRecording();
}

void Shuffleboard::SetRecordingFileNameFormat(std::string_view format) {
  GetRecordingController().SetRecordingFileNameFormat(format);
}

void Shuffleboard::ClearRecordingFileNameFormat() {
  GetRecordingController().ClearRecordingFileNameFormat();
}

void Shuffleboard::AddEventMarker(std::string_view name,
                                  std::string_view description,
                                  ShuffleboardEventImportance importance) {
  GetRecordingController().AddEventMarker(name, description, importance);
}

void Shuffleboard::AddEventMarker(std::string_view name,
                                  ShuffleboardEventImportance importance) {
  AddEventMarker(name, "", importance);
}

detail::ShuffleboardInstance& Shuffleboard::GetInstance() {
  static detail::ShuffleboardInstance inst(
      nt::NetworkTableInstance::GetDefault());
  return inst;
}

detail::RecordingController& Shuffleboard::GetRecordingController() {
  static detail::RecordingController inst(
      nt::NetworkTableInstance::GetDefault());
  return inst;
}
