/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.DriverStation;

/**
 * Controls Shuffleboard recordings via NetworkTables.
 */
public final class RecordingController {
  private static final String kRecordingTableName = "/Shuffleboard/.recording/";
  private static final String kRecordingControlKey = kRecordingTableName + "RecordData";
  private static final String kRecordingFileNameFormatKey = kRecordingTableName + "FileNameFormat";
  private static final String kEventMarkerTableName = kRecordingTableName + "events";

  private static RecordingController s_defaultInstance;

  private final NetworkTableEntry m_recordingControlEntry;
  private final NetworkTableEntry m_recordingFileNameFormatEntry;
  private final NetworkTable m_eventsTable;

  /**
   * Creates a new RecordingController instance
   *
   * @param ntInstance the NetworkTables instance to use
   */
  public RecordingController(NetworkTableInstance ntInstance) {
    m_recordingControlEntry = ntInstance.getEntry(kRecordingControlKey);
    m_recordingFileNameFormatEntry = ntInstance.getEntry(kRecordingFileNameFormatKey);
    m_eventsTable = ntInstance.getTable(kEventMarkerTableName);
  }

  /**
   * Get global default instance.
   *
   * @return Global default instance
   */
  public static synchronized RecordingController getDefault() {
    if (s_defaultInstance == null) {
      s_defaultInstance = new RecordingController(NetworkTableInstance.getDefault());
    }
    return s_defaultInstance;
  }

  /**
   * Starts data recording on the dashboard. Has no effect if recording is already in progress.
   *
   * @see #stopRecording()
   */
  public void startRecording() {
    m_recordingControlEntry.setBoolean(true);
  }

  /**
   * Stops data recording on the dashboard. Has no effect if no recording is in progress.
   *
   * @see #startRecording()
   */
  public void stopRecording() {
    m_recordingControlEntry.setBoolean(false);
  }

  /**
   * Sets the file name format for new recording files to use. If recording is in progress when this
   * method is called, it will continue to use the same file. New recordings will use the format.
   *
   * <p>To avoid recording files overwriting each other, make sure to use unique recording file
   * names. File name formats accept templates for inserting the date and time when the recording
   * started with the {@code ${date}} and {@code ${time}} templates, respectively. For example,
   * the default format is {@code "recording-${time}"} and recording files created with it will have
   * names like {@code "recording-2018.01.15.sbr"}. Users are <strong>strongly</strong> recommended
   * to use the {@code ${time}} template to ensure unique file names.
   * </p>
   *
   * @param format the format for the
   * @see #clearRecordingFileNameFormat()
   */
  public void setRecordingFileNameFormat(String format) {
    m_recordingFileNameFormatEntry.setString(format);
  }

  /**
   * Clears the custom name format for recording files. New recordings will use the default format.
   *
   * @see #setRecordingFileNameFormat(String)
   */
  public void clearRecordingFileNameFormat() {
    m_recordingFileNameFormatEntry.delete();
  }

  /**
   * Notifies Shuffleboard of an event. Events can range from as trivial as a change in a command
   * state to as critical as a total power loss or component failure. If Shuffleboard is recording,
   * the event will also be recorded.
   *
   * <p>If {@code name} is {@code null} or empty, or {@code importance} is {@code null}, then
   * no event will be sent and an error will be printed to the driver station.
   *
   * @param name        the name of the event
   * @param description a description of the event
   * @param importance  the importance of the event
   */
  public void addEventMarker(String name, String description, EventImportance importance) {
    if (name == null || name.isEmpty()) {
      DriverStation.reportError(
          "Shuffleboard event name was not specified", true);
      return;
    }

    if (importance == null) {
      DriverStation.reportError(
          "Shuffleboard event importance was null", true);
      return;
    }

    String eventDescription = description == null ? "" : description;

    m_eventsTable.getSubTable(name)
        .getEntry("Info")
        .setStringArray(new String[]{eventDescription, importance.getSimpleName()});
  }
}
