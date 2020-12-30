// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/SmallVector.h>
#include <wpi/StringRef.h>

#include "frc/shuffleboard/ShuffleboardEventImportance.h"

namespace frc::detail {

class RecordingController final {
 public:
  explicit RecordingController(nt::NetworkTableInstance ntInstance);
  virtual ~RecordingController() = default;

  void StartRecording();
  void StopRecording();
  void SetRecordingFileNameFormat(wpi::StringRef format);
  void ClearRecordingFileNameFormat();

  void AddEventMarker(wpi::StringRef name, wpi::StringRef description,
                      ShuffleboardEventImportance importance);

 private:
  nt::NetworkTableEntry m_recordingControlEntry;
  nt::NetworkTableEntry m_recordingFileNameFormatEntry;
  std::shared_ptr<nt::NetworkTable> m_eventsTable;
};

}  // namespace frc::detail
