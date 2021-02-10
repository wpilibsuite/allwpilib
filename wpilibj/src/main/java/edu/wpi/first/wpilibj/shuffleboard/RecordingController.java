// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.DriverStation;

/** Controls Shuffleboard recordings via NetworkTables. */
final class RecordingController {
  private static final String kRecordingTableName = "/Shuffleboard/.recording/";
  private static final String kRecordingControlKey = kRecordingTableName + "RecordData";
  private static final String kRecordingFileNameFormatKey = kRecordingTableName + "FileNameFormat";
  private static final String kEventMarkerTableName = kRecordingTableName + "events";

  private final NetworkTableEntry m_recordingControlEntry;
  private final NetworkTableEntry m_recordingFileNameFormatEntry;
  private final NetworkTable m_eventsTable;

  RecordingController(NetworkTableInstance ntInstance) {
    m_recordingControlEntry = ntInstance.getEntry(kRecordingControlKey);
    m_recordingFileNameFormatEntry = ntInstance.getEntry(kRecordingFileNameFormatKey);
    m_eventsTable = ntInstance.getTable(kEventMarkerTableName);
  }

  public void startRecording() {
    m_recordingControlEntry.setBoolean(true);
  }

  public void stopRecording() {
    m_recordingControlEntry.setBoolean(false);
  }

  public void setRecordingFileNameFormat(String format) {
    m_recordingFileNameFormatEntry.setString(format);
  }

  public void clearRecordingFileNameFormat() {
    m_recordingFileNameFormatEntry.delete();
  }

  public void addEventMarker(String name, String description, EventImportance importance) {
    if (name == null || name.isEmpty()) {
      DriverStation.reportError("Shuffleboard event name was not specified", true);
      return;
    }

    if (importance == null) {
      DriverStation.reportError("Shuffleboard event importance was null", true);
      return;
    }

    String eventDescription = description == null ? "" : description;

    m_eventsTable
        .getSubTable(name)
        .getEntry("Info")
        .setStringArray(new String[] {eventDescription, importance.getSimpleName()});
  }
}
