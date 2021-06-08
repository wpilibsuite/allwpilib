// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/SmallVector.h>

#include "frc/shuffleboard/ShuffleboardEventImportance.h"

namespace frc::detail {

class RecordingController final {
 public:
  explicit RecordingController(nt::NetworkTableInstance ntInstance);
  virtual ~RecordingController() = default;

  void StartRecording();
  void StopRecording();
  void SetRecordingFileNameFormat(std::string_view format);
  void ClearRecordingFileNameFormat();

  void AddEventMarker(std::string_view name, std::string_view description,
                      ShuffleboardEventImportance importance);

 private:
  nt::NetworkTableEntry m_recordingControlEntry;
  nt::NetworkTableEntry m_recordingFileNameFormatEntry;
  std::shared_ptr<nt::NetworkTable> m_eventsTable;
};

}  // namespace frc::detail
