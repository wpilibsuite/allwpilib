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

ShuffleboardTab& Shuffleboard::GetTab(wpi::StringRef title) {
  return GetInstance().GetTab(title);
}

void Shuffleboard::SelectTab(int index) {
  GetInstance().SelectTab(index);
}

void Shuffleboard::SelectTab(wpi::StringRef title) {
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

void Shuffleboard::SetRecordingFileNameFormat(wpi::StringRef format) {
  GetRecordingController().SetRecordingFileNameFormat(format);
}

void Shuffleboard::ClearRecordingFileNameFormat() {
  GetRecordingController().ClearRecordingFileNameFormat();
}

void Shuffleboard::AddEventMarker(wpi::StringRef name,
                                  wpi::StringRef description,
                                  ShuffleboardEventImportance importance) {
  GetRecordingController().AddEventMarker(name, description, importance);
}

void Shuffleboard::AddEventMarker(wpi::StringRef name,
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
