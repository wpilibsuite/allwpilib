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
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

/**
 * A CSVLogFile is a LogFile that writes to a file the contents of each of its
 * registered {@link CSVLogCell}'s.
 *
 * <p>For the CSVLogFile to write log informations, you must call
 * {@link #periodic()} periodically.
 */
public class CSVLogFile extends LogFile{
  private final List<CSVLogCell> m_cells;
  private final CSVLogCell m_timestampCell;

  /**
   * Instantiate a LogFile passing in its prefix and its extension.
   *
   * If you want the file to be saved in a existing directory, you can add its
   * path before the file prefix. Exemple : to save the file in a usb stick on
   * the roborio ("/media/sda1/") : LogFile("/media/sda1/log").
   *
   * @param filePrefix The prefix of the LogFile.
   */
  public CSVLogFile(String filePrefix) {
    super(filePrefix, "csv");
    m_cells = new ArrayList<CSVLogCell>();
    m_timestampCell = new CSVLogCell("Timestamp (ms)");
    registerCell(m_timestampCell);
  }

  /**
   * Instantiate a CSVLogFile.
   */
  public CSVLogFile() {
    this("log");
  }

  /**
   * Register a new column to be logged. You will figure this value out
   * in {@link CSVLogCell#getContent()}.
   *
   * <p>A CSVLogCell can only be registered if the spreadsheet is inactive:
   * see {@link #start()} and {@link #stop()} to activate or inactivate it.
   *
   * @param cell The CSVLogCell to register.
   */
  public void registerCell(CSVLogCell cell) {
    if (isActive()) {
      System.out.println("You can't add a new cell when the spreadsheet is active: "
          + cell.getName());
    } else {
      m_cells.add(cell);
    }
  }

  /**
   * Start the CSVLogFile: open a new file and write the column headers.
   *
   * <p>It causes the CSVLogFile to be "active".
   */
  public void start() {
    super.start();

    for (CSVLogCell cell : m_cells) {
      log("\"" + cell.getName() + "\",");
    }
    log("\n");
  }

  /**
   * Call it regularly. If the CSVLogFile is active, write a row of the file.
   */
  public void periodic() {
    if (isActive()) {
      m_timestampCell.log(System.currentTimeMillis());
      writeRow();
    }
  }

  /**
   * Write a row of the file.
   */
  private void writeRow() {
    for (CSVLogCell cell : m_cells) {
      log("\"" + cell.getContent() + "\",");
    }
    log("\n");
  }
}
