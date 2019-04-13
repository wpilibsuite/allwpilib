/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/RecordingController.h"

#include "frc/DriverStation.h"

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

void RecordingController::SetRecordingFileNameFormat(wpi::StringRef format) {
  m_recordingFileNameFormatEntry.SetString(format);
}

void RecordingController::ClearRecordingFileNameFormat() {
  m_recordingFileNameFormatEntry.Delete();
}

void RecordingController::AddEventMarker(
    wpi::StringRef name, wpi::StringRef description,
    ShuffleboardEventImportance importance) {
  if (name.empty()) {
    DriverStation::ReportError("Shuffleboard event name was not specified");
    return;
  }
  auto arr = wpi::ArrayRef<std::string>{
      description, ShuffleboardEventImportanceName(importance)};
  m_eventsTable->GetSubTable(name)->GetEntry("Info").SetStringArray(arr);
}
