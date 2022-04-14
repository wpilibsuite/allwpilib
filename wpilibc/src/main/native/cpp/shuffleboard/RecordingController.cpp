// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/RecordingController.h"

#include "frc/Errors.h"

using namespace frc;
using namespace frc::detail;

RecordingController::RecordingController(nt::NetworkTableInstance ntInstance)
    : m_recordingControlEntry(), m_recordingFileNameFormatEntry() {
  m_recordingControlEntry =
      ntInstance.GetEntry("/Shuffleboard/.recording/RecordData");
  m_recordingFileNameFormatEntry =
      ntInstance.GetEntry("/Shuffleboard/.recording/FileNameFormat");
  m_eventsTable = ntInstance.GetTable("/Shuffleboard/.recording/events");
}

void RecordingController::StartRecording() {
  m_recordingControlEntry.SetBoolean(true);
}

void RecordingController::StopRecording() {
  m_recordingControlEntry.SetBoolean(false);
}

void RecordingController::SetRecordingFileNameFormat(std::string_view format) {
  m_recordingFileNameFormatEntry.SetString(format);
}

void RecordingController::ClearRecordingFileNameFormat() {
  m_recordingFileNameFormatEntry.Delete();
}

void RecordingController::AddEventMarker(
    std::string_view name, std::string_view description,
    ShuffleboardEventImportance importance) {
  if (name.empty()) {
    FRC_ReportError(err::Error, "{}",
                    "Shuffleboard event name was not specified");
    return;
  }
  m_eventsTable->GetSubTable(name)->GetEntry("Info").SetStringArray(
      {std::string{description},
       std::string{ShuffleboardEventImportanceName(importance)}});
}
