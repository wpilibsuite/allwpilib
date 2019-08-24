/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.logging;

import java.util.ArrayList;
import java.util.List;

/**
 * A CSVLogFile is a LogFile that writes to a file the contents of each of its
 * registered {@link CSVLogCell}'s.
 *
 * <p>For the CSVLogFile to write log informations, you must call
 * {@link #periodic()} periodically.
 */
public class CSVLogFile extends LogFile {
  private final List<CSVLogCell> m_cells;
  private final CSVLogCell m_timestampCell;
  private boolean m_active;

  /**
   * Instantiate a LogFile passing in its prefix and its extension.
   *
   * <p>If you want the file to be saved in a existing directory, you can add its
   * path before the file prefix. Exemple : to save the file in a usb stick on
   * the roborio ("/media/sda1/") : LogFile("/media/sda1/log").
   *
   * @param filePrefix The prefix of the LogFile.
   */
  public CSVLogFile(String filePrefix) {
    super(filePrefix, "csv");
    m_cells = new ArrayList<CSVLogCell>();
    m_timestampCell = new CSVLogCell("Timestamp (ms)");
    m_active = false;
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
   * see {@link #start()} to activate it.
   *
   * @param cell The CSVLogCell to register.
   */
  public void registerCell(CSVLogCell cell) {
    if (m_active) {
      System.out.println("You can't add a new cell when the spreadsheet is active: "
          + cell.getName());
    } else {
      m_cells.add(cell);
    }
  }

  /**
   * Write the column headers.
   *
   * <p>It causes the CSVLogFile to be "active".
   */
  public void start() {
    if (m_active) {
      System.out.println("This table has already been initialized");
      return;
    }

    for (CSVLogCell cell : m_cells) {
      log("\"" + cell.getName() + "\",");
    }
    log("\n");

    m_active = true;
  }

  /**
   * Call it regularly. If the CSVLogFile is active, write a row of the file.
   */
  public void periodic() {
    if (m_active) {
      m_timestampCell.log(System.currentTimeMillis());
      writeRow();
    }
  }

  /**
   * Return true if the LogFile is active.
   *
   * @return true if the LogFile is active.
   */
  public boolean isActive() {
    return m_active;
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
