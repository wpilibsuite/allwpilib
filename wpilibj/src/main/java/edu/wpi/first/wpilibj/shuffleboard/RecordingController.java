// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.wpilibj.shuffleboard;

import org.wpilib.networktables.BooleanPublisher;
import org.wpilib.networktables.NetworkTable;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.StringPublisher;
import org.wpilib.wpilibj.DriverStation;

/** Controls Shuffleboard recordings via NetworkTables. */
final class RecordingController {
  private static final String kRecordingTableName = "/Shuffleboard/.recording/";
  private static final String kRecordingControlKey = kRecordingTableName + "RecordData";
  private static final String kRecordingFileNameFormatKey = kRecordingTableName + "FileNameFormat";
  private static final String kEventMarkerTableName = kRecordingTableName + "events";

  private final BooleanPublisher m_recordingControlEntry;
  private final StringPublisher m_recordingFileNameFormatEntry;
  private final NetworkTable m_eventsTable;

  RecordingController(NetworkTableInstance ntInstance) {
    m_recordingControlEntry = ntInstance.getBooleanTopic(kRecordingControlKey).publish();
    m_recordingFileNameFormatEntry =
        ntInstance.getStringTopic(kRecordingFileNameFormatKey).publish();
    m_eventsTable = ntInstance.getTable(kEventMarkerTableName);
  }

  public void startRecording() {
    m_recordingControlEntry.set(true);
  }

  public void stopRecording() {
    m_recordingControlEntry.set(false);
  }

  public void setRecordingFileNameFormat(String format) {
    m_recordingFileNameFormatEntry.set(format);
  }

  public void clearRecordingFileNameFormat() {
    m_recordingFileNameFormatEntry.set("");
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
