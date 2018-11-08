/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/SmallVector.h>
#include <wpi/StringRef.h>

#include "frc/shuffleboard/ShuffleboardEventImportance.h"

namespace frc {
namespace detail {

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

}  // namespace detail
}  // namespace frc
