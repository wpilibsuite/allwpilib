// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.util.FileLogger;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.util.concurrent.Event;
import edu.wpi.first.util.datalog.DataLog;
import edu.wpi.first.util.datalog.DataLogBackgroundWriter;
import edu.wpi.first.util.datalog.IntegerLogEntry;
import edu.wpi.first.util.datalog.StringLogEntry;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Random;

/**
 * Centralized data log that provides automatic data log file management. It automatically cleans up
 * old files when disk space is low and renames the file based either on current date/time or (if
 * available) competition match number. The data file will be saved to a USB flash drive in a folder
 * named "logs" if one is attached, or to /home/lvuser/logs otherwise.
 *
 * <p>Log files are initially named "FRC_TBD_{random}.wpilog" until the DS connects. After the DS
 * connects, the log file is renamed to "FRC_yyyyMMdd_HHmmss.wpilog" (where the date/time is UTC).
 * If the FMS is connected and provides a match number, the log file is renamed to
 * "FRC_yyyyMMdd_HHmmss_{event}_{match}.wpilog".
 *
 * <p>On startup, all existing FRC_TBD log files are deleted. If there is less than 50 MB of free
 * space on the target storage, FRC_ log files are deleted (oldest to newest) until there is 50 MB
 * free OR there are 10 files remaining.
 *
 * <p>By default, all NetworkTables value changes are stored to the data log.
 */
public final class DataLogManager {
  private static DataLogBackgroundWriter m_log;
  private static boolean m_stopped;
  private static String m_logDir;
  private static boolean m_filenameOverride;
  private static Thread m_thread;
  private static final ZoneId m_utc = ZoneId.of("UTC");
  private static final DateTimeFormatter m_timeFormatter =
      DateTimeFormatter.ofPattern("yyyyMMdd_HHmmss").withZone(m_utc);
  private static boolean m_ntLoggerEnabled = true;
  private static int m_ntEntryLogger;
  private static int m_ntConnLogger;
  private static boolean m_consoleLoggerEnabled = true;
  private static FileLogger m_consoleLogger;
  private static StringLogEntry m_messageLog;

  // if less than this much free space, delete log files until there is this much free space
  // OR there are this many files remaining.
  private static final long kFreeSpaceThreshold = 50000000L;
  private static final int kFileCountThreshold = 10;

  private DataLogManager() {}

  /** Start data log manager with default directory location. */
  public static synchronized void start() {
    start("", "", 0.25);
  }

  /**
   * Start data log manager. The parameters have no effect if the data log manager was already
   * started (e.g. by calling another static function).
   *
   * @param dir if not empty, directory to use for data log storage
   */
  public static synchronized void start(String dir) {
    start(dir, "", 0.25);
  }

  /**
   * Start data log manager. The parameters have no effect if the data log manager was already
   * started (e.g. by calling another static function).
   *
   * @param dir if not empty, directory to use for data log storage
   * @param filename filename to use; if none provided, the filename is automatically generated
   */
  public static synchronized void start(String dir, String filename) {
    start(dir, filename, 0.25);
  }

  /**
   * Start data log manager. The parameters have no effect if the data log manager was already
   * started (e.g. by calling another static function).
   *
   * @param dir if not empty, directory to use for data log storage
   * @param filename filename to use; if none provided, the filename is automatically generated
   * @param period time between automatic flushes to disk, in seconds; this is a time/storage
   *     tradeoff
   */
  public static synchronized void start(String dir, String filename, double period) {
    if (m_log == null) {
      m_logDir = makeLogDir(dir);
      m_filenameOverride = !filename.isEmpty();

      // Delete all previously existing FRC_TBD_*.wpilog files. These only exist when the robot
      // never connects to the DS, so they are very unlikely to have useful data and just clutter
      // the filesystem.
      File[] files =
          new File(m_logDir)
              .listFiles((d, name) -> name.startsWith("FRC_TBD_") && name.endsWith(".wpilog"));
      if (files != null) {
        for (File file : files) {
          if (!file.delete()) {
            System.err.println("DataLogManager: could not delete " + file);
          }
        }
      }
      m_log = new DataLogBackgroundWriter(m_logDir, makeLogFilename(filename), period);
      m_messageLog = new StringLogEntry(m_log, "messages");

      // Log all NT entries and connections
      if (m_ntLoggerEnabled) {
        startNtLog();
      }
      // Log console output
      if (m_consoleLoggerEnabled) {
        startConsoleLog();
      }
    } else if (m_stopped) {
      m_log.setFilename(makeLogFilename(filename));
      m_log.resume();
      m_stopped = false;
    }

    if (m_thread == null) {
      m_thread = new Thread(DataLogManager::logMain, "DataLogDS");
      m_thread.setDaemon(true);
      m_thread.start();
    }
  }

  /** Stop data log manager. */
  public static synchronized void stop() {
    if (m_thread != null) {
      m_thread.interrupt();
      m_thread = null;
    }
    if (m_log != null) {
      m_log.stop();
      m_stopped = true;
    }
  }

  /**
   * Log a message to the "messages" entry. The message is also printed to standard output (followed
   * by a newline).
   *
   * @param message message
   */
  public static synchronized void log(String message) {
    if (m_messageLog == null) {
      start();
    }
    m_messageLog.append(message);
    System.out.println(message);
  }

  /**
   * Get the managed data log (for custom logging). Starts the data log manager if not already
   * started.
   *
   * @return data log
   */
  public static synchronized DataLog getLog() {
    if (m_log == null) {
      start();
    }
    return m_log;
  }

  /**
   * Get the log directory.
   *
   * @return log directory, or empty string if logging not yet started
   */
  public static synchronized String getLogDir() {
    if (m_logDir == null) {
      return "";
    }
    return m_logDir;
  }

  /**
   * Enable or disable logging of NetworkTables data. Note that unlike the network interface for
   * NetworkTables, this will capture every value change. Defaults to enabled.
   *
   * @param enabled true to enable, false to disable
   */
  public static synchronized void logNetworkTables(boolean enabled) {
    boolean wasEnabled = m_ntLoggerEnabled;
    m_ntLoggerEnabled = enabled;
    if (m_log == null) {
      start();
      return;
    }
    if (enabled && !wasEnabled) {
      startNtLog();
    } else if (!enabled && wasEnabled) {
      stopNtLog();
    }
  }

  /**
   * Enable or disable logging of the console output. Defaults to enabled.
   *
   * @param enabled true to enable, false to disable
   */
  public static synchronized void logConsoleOutput(boolean enabled) {
    boolean wasEnabled = m_consoleLoggerEnabled;
    m_consoleLoggerEnabled = enabled;
    if (m_log == null) {
      start();
      return;
    }
    if (enabled && !wasEnabled) {
      startConsoleLog();
    } else if (!enabled && wasEnabled) {
      stopConsoleLog();
    }
  }

  private static String makeLogDir(String dir) {
    if (!dir.isEmpty()) {
      return dir;
    }

    if (RobotBase.isReal()) {
      try {
        // prefer a mounted USB drive if one is accessible
        Path usbDir = Paths.get("/u").toRealPath();
        if (Files.isWritable(usbDir)) {
          if (!new File("/u/logs").mkdir()) {
            // ignored
          }
          HAL.report(tResourceType.kResourceType_DataLogManager, tInstances.kDataLogLocation_USB);
          return "/u/logs";
        }
      } catch (IOException ex) {
        // ignored
      }
      if (RobotBase.getRuntimeType() == RuntimeType.kRoboRIO) {
        DriverStation.reportWarning(
            "DataLogManager: Logging to RoboRIO 1 internal storage is not recommended!"
                + " Plug in a FAT32 formatted flash drive!",
            false);
      }
      if (!new File("/home/lvuser/logs").mkdir()) {
        // ignored
      }
      HAL.report(tResourceType.kResourceType_DataLogManager, tInstances.kDataLogLocation_Onboard);
      return "/home/lvuser/logs";
    }
    String logDir = Filesystem.getOperatingDirectory().getAbsolutePath() + "/logs";
    if (!new File(logDir).mkdir()) {
      // ignored
    }
    return logDir;
  }

  private static String makeLogFilename(String filenameOverride) {
    if (!filenameOverride.isEmpty()) {
      return filenameOverride;
    }
    Random rnd = new Random();
    StringBuilder filename = new StringBuilder();
    filename.append("FRC_TBD_");
    for (int i = 0; i < 4; i++) {
      filename.append(String.format("%04x", rnd.nextInt(0x10000)));
    }
    filename.append(".wpilog");
    return filename.toString();
  }

  private static void startNtLog() {
    NetworkTableInstance inst = NetworkTableInstance.getDefault();
    m_ntEntryLogger = inst.startEntryDataLog(m_log, "", "NT:");
    m_ntConnLogger = inst.startConnectionDataLog(m_log, "NTConnection");
  }

  private static void stopNtLog() {
    NetworkTableInstance.stopEntryDataLog(m_ntEntryLogger);
    NetworkTableInstance.stopConnectionDataLog(m_ntConnLogger);
  }

  private static void startConsoleLog() {
    if (RobotBase.isReal()) {
      m_consoleLogger = new FileLogger("/home/lvuser/FRC_UserProgram.log", m_log, "console");
    }
  }

  private static void stopConsoleLog() {
    if (RobotBase.isReal()) {
      m_consoleLogger.close();
    }
  }

  private static void logMain() {
    // based on free disk space, scan for "old" FRC_*.wpilog files and remove
    {
      File logDir = new File(m_logDir);
      long freeSpace = logDir.getUsableSpace();
      if (freeSpace < kFreeSpaceThreshold) {
        // Delete oldest FRC_*.wpilog files (ignore FRC_TBD_*.wpilog as we just created one)
        File[] files =
            logDir.listFiles(
                (dir, name) ->
                    name.startsWith("FRC_")
                        && name.endsWith(".wpilog")
                        && !name.startsWith("FRC_TBD_"));
        if (files != null) {
          Arrays.sort(files, Comparator.comparingLong(File::lastModified));
          int count = files.length;
          for (File file : files) {
            --count;
            if (count < kFileCountThreshold) {
              break;
            }
            long length = file.length();
            if (file.delete()) {
              DriverStation.reportWarning("DataLogManager: Deleted " + file.getName(), false);
              freeSpace += length;
              if (freeSpace >= kFreeSpaceThreshold) {
                break;
              }
            } else {
              System.err.println("DataLogManager: could not delete " + file);
            }
          }
        }
      } else if (freeSpace < 2 * kFreeSpaceThreshold) {
        DriverStation.reportWarning(
            "DataLogManager: Log storage device has "
                + freeSpace / 1000000
                + " MB of free space remaining! Logs will get deleted below "
                + kFreeSpaceThreshold / 1000000
                + " MB of free space. "
                + "Consider deleting logs off the storage device.",
            false);
      }
    }

    int timeoutCount = 0;
    boolean paused = false;
    int dsAttachCount = 0;
    int fmsAttachCount = 0;
    boolean dsRenamed = m_filenameOverride;
    boolean fmsRenamed = m_filenameOverride;
    int sysTimeCount = 0;
    IntegerLogEntry sysTimeEntry =
        new IntegerLogEntry(
            m_log, "systemTime", "{\"source\":\"DataLogManager\",\"format\":\"time_t_us\"}");

    Event newDataEvent = new Event();
    DriverStation.provideRefreshedDataEventHandle(newDataEvent.getHandle());
    while (!Thread.interrupted()) {
      boolean timedOut;
      try {
        timedOut = WPIUtilJNI.waitForObjectTimeout(newDataEvent.getHandle(), 0.25);
      } catch (InterruptedException e) {
        break;
      }
      if (Thread.interrupted()) {
        break;
      }
      if (timedOut) {
        timeoutCount++;
        // pause logging after being disconnected for 10 seconds
        if (timeoutCount > 40 && !paused) {
          timeoutCount = 0;
          paused = true;
          m_log.pause();
        }
        continue;
      }
      // when we connect to the DS, resume logging
      timeoutCount = 0;
      if (paused) {
        paused = false;
        m_log.resume();
      }

      if (!dsRenamed) {
        // track DS attach
        if (DriverStation.isDSAttached()) {
          dsAttachCount++;
        } else {
          dsAttachCount = 0;
        }
        if (dsAttachCount > 50) { // 1 second
          if (RobotController.isSystemTimeValid()) {
            LocalDateTime now = LocalDateTime.now(m_utc);
            m_log.setFilename("FRC_" + m_timeFormatter.format(now) + ".wpilog");
            dsRenamed = true;
          } else {
            dsAttachCount = 0; // wait a bit and try again
          }
        }
      }

      if (!fmsRenamed) {
        // track FMS attach
        if (DriverStation.isFMSAttached()) {
          fmsAttachCount++;
        } else {
          fmsAttachCount = 0;
        }
        if (fmsAttachCount > 250) { // 5 seconds
          // match info comes through TCP, so we need to double-check we've
          // actually received it
          DriverStation.MatchType matchType = DriverStation.getMatchType();
          if (matchType != DriverStation.MatchType.None) {
            // rename per match info
            char matchTypeChar =
                switch (matchType) {
                  case Practice -> 'P';
                  case Qualification -> 'Q';
                  case Elimination -> 'E';
                  default -> '_';
                };
            m_log.setFilename(
                "FRC_"
                    + m_timeFormatter.format(LocalDateTime.now(m_utc))
                    + "_"
                    + DriverStation.getEventName()
                    + "_"
                    + matchTypeChar
                    + DriverStation.getMatchNumber()
                    + ".wpilog");
            fmsRenamed = true;
            dsRenamed = true; // don't override FMS rename
          }
        }
      }

      // Write system time every ~5 seconds
      sysTimeCount++;
      if (sysTimeCount >= 250) {
        sysTimeCount = 0;
        if (RobotController.isSystemTimeValid()) {
          sysTimeEntry.append(WPIUtilJNI.getSystemTime(), WPIUtilJNI.now());
        }
      }
    }
    newDataEvent.close();
  }
}
