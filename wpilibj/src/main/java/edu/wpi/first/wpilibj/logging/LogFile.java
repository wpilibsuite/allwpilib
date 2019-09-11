/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.logging;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

/**
 * A LogFile writes text to log in a file.
 */
public class LogFile {
  private final String m_filePrefix;
  private final String m_fileExtension;
  private OutputStream m_file;
  private Date m_time;

  /**
   * Instantiate a LogFile passing in its prefix and its extension.
   *
   * <p>If you want the file to be saved in a existing directory, you can add its
   * path before the file prefix. Exemple : to save the file in a usb stick on
   * the roborio ("/media/sda1/") : LogFile("/media/sda1/log").
   *
   * @param filePrefix The prefix of the LogFile.
   * @param fileExtension The extension of the LogFile (without dot).
   */
  public LogFile(String filePrefix, String fileExtension) {
    m_filePrefix = filePrefix;
    m_fileExtension = fileExtension;

    m_time = new Date(0);
    String fileName = createFilename(m_time);
    try {
      m_file = Files.newOutputStream(Paths.get(fileName));
    } catch (IOException ex) {
      System.out.println(ex.getMessage());
    }
  }

  /**
   * Instantiate a LogFile passing in its prefix.
   *
   * <p>If you want the file to be saved in a existing directory, you can add its
   * path before the file prefix. Exemple : to save the file in a usb stick on
   * the roborio ("/media/sda1/") : LogFile("/media/sda1/log").
   *
   * @param filePrefix The prefix of the LogFile.
   */
  public LogFile(String filePrefix) {
    this(filePrefix, "txt");
  }

  /**
   * Instantiate a LogFile.
   */
  public LogFile() {
    this("log", "txt");
  }

  /**
   * Write text in the logFile.
   *
   * @param text The text to be logged in the file.
   */
  public void log(String text) {
    try {
      m_file.write(text.getBytes());
    } catch (IOException ex) {
      System.out.println(ex.getMessage());
    }
    updateFilename();
  }

  /**
   * Check if the time has changed of more than 24 hours.
   * Change the filename if the condition is met.
   */
  private void updateFilename() {
    Date newTime = new Date();
    // If the difference between the two timestamps is more than 24 hours
    if (newTime.getTime() - m_time.getTime() > 86400000) {
      File oldName = new File(createFilename(m_time));
      File newName = new File(createFilename(newTime));
      oldName.renameTo(newName);

      try {
        m_file = Files.newOutputStream(newName.toPath(), StandardOpenOption.APPEND);
      } catch (IOException ex) {
        System.out.println(ex.getMessage());
      }
    }

    m_time = newTime;
  }

  /**
   * Create a filename with a time.
   *
   * @param time The time that is saved in the filename.
   * @return The filename at the format "{filePrefix}-{date/time}.txt".
   */
  private String createFilename(Date time) {
    // Get current date/time, format is YYYY-MM-DD.HH_mm_ss
    SimpleDateFormat formater = new SimpleDateFormat("yyyy-MM-dd.HH_mm_ss", Locale.getDefault());

    return m_filePrefix + "-" + formater.format(time) + "." + m_fileExtension;
  }

}
