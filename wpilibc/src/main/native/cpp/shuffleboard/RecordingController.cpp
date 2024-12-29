// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/RecordingController.h"

#include <string>

#include "frc/Errors.h"

using namespace frc;
using namespace frc::detail;

RecordingController::RecordingController(nt::NetworkTableInstance ntInstance) {
  m_recordingControlEntry =
      ntInstance.GetBooleanTopic("/Shuffleboard/.recording/RecordData")
          .Publish();
  m_recordingFileNameFormatEntry =
      ntInstance.GetStringTopic("/Shuffleboard/.recording/FileNameFormat")
          .Publish();
  m_eventsTable = ntInstance.GetTable("/Shuffleboard/.recording/events");
}

void RecordingController::StartRecording() {
  m_recordingControlEntry.Set(true);
}

void RecordingController::StopRecording() {
  m_recordingControlEntry.Set(false);
}

void RecordingController::SetRecordingFileNameFormat(std::string_view format) {
  m_recordingFileNameFormatEntry.Set(format);
}

void RecordingController::ClearRecordingFileNameFormat() {
  m_recordingFileNameFormatEntry.Set("");
}

void RecordingController::AddEventMarker(
    std::string_view name, std::string_view description,
    ShuffleboardEventImportance importance) {
  if (name.empty()) {
    FRC_ReportError(err::Error, "Shuffleboard event name was not specified");
    return;
  }
  m_eventsTable->GetSubTable(name)->GetEntry("Info").SetStringArray(
      {{std::string{description},
        std::string{ShuffleboardEventImportanceName(importance)}}});
}
