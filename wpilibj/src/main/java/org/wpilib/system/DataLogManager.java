// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

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
import org.wpilib.datalog.DataLog;
import org.wpilib.datalog.DataLogBackgroundWriter;
import org.wpilib.datalog.FileLogger;
import org.wpilib.datalog.IntegerLogEntry;
import org.wpilib.datalog.StringLogEntry;
import org.wpilib.driverstation.DriverStationErrors;
import org.wpilib.driverstation.MatchState;
import org.wpilib.driverstation.MatchType;
import org.wpilib.driverstation.RobotState;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.framework.RobotBase;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.util.Alert;
import org.wpilib.util.UsageReporting;
import org.wpilib.util.WPIUtilJNI;
import org.wpilib.util.concurrent.Event;

/**
 * Centralized data log that provides automatic data log file management. It automatically cleans up
 * old files when disk space is low and renames the file based either on current date/time or (if
 * available) competition match number. The data file will be saved to a USB flash drive in a folder
 * named "logs" if one is attached, or to /home/systemcore/logs otherwise.
 *
 * <p>Log files are initially named "WPILIB_TBD_{random}.wpilog" until the DS connects. After the DS
 * connects, the log file is renamed to "WPILIB_yyyyMMdd_HHmmss.wpilog" (where the date/time is
 * UTC). If the FMS is connected and provides a match number, the log file is renamed to
 * "WPILIB_yyyyMMdd_HHmmss_{event}_{match}.wpilog".
 *
 * <p>On startup, all existing WPILIB_TBD log files are deleted. If there is less than 200 MB of
 * free space on the target storage, non-match WPILIB_ log files are deleted (oldest to newest)
 * until there is 200 MB free OR there are 10 files remaining. Competition match logs are not
 * deleted. An alert is active while less than 400 MB is available, checked every 5 seconds. An
 * informational alert indicates when automatic directory selection falls back to SystemCore
 * internal storage.
 *
 * <p>By default, all NetworkTables value changes are stored to the data log.
 */
public final class DataLogManager {
  private static DataLogBackgroundWriter m_log;
  private static boolean m_stopped;
  private static String m_logDir;
  private static boolean m_internalStorage;
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
  private static final long FREE_SPACE_THRESHOLD = 200000000L;
  private static final int FILE_COUNT_THRESHOLD = 10;

  static boolean isMatchLog(String filename) {
    int extensionIndex = filename.lastIndexOf(".wpilog");
    if (extensionIndex != filename.length() - 7) {
      return false;
    }

    int matchTypeSeparator = filename.lastIndexOf('_', extensionIndex - 1);
    if (matchTypeSeparator < 0 || matchTypeSeparator + 2 >= extensionIndex) {
      return false;
    }

    char matchType = filename.charAt(matchTypeSeparator + 1);
    if (matchType != 'P' && matchType != 'Q' && matchType != 'E' && matchType != 'T') {
      return false;
    }
    for (int i = matchTypeSeparator + 2; i < extensionIndex; i++) {
      if (filename.charAt(i) < '0' || filename.charAt(i) > '9') {
        return false;
      }
    }
    return true;
  }

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

      // Delete all previously existing WPILIB_TBD_*.wpilog files. These only exist when the robot
      // never connects to the DS, so they are very unlikely to have useful data and just clutter
      // the filesystem.
      File[] files =
          new File(m_logDir)
              .listFiles((d, name) -> name.startsWith("WPILIB_TBD_") && name.endsWith(".wpilog"));
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
      // Wait for the thread to release its alerts before another start can reuse their IDs.
      boolean interrupted = false;
      while (m_thread.isAlive()) {
        try {
          m_thread.join();
        } catch (InterruptedException ex) {
          interrupted = true;
        }
      }
      if (interrupted) {
        Thread.currentThread().interrupt();
      }
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
          UsageReporting.reportUsage("DataLogManager", "USB");
          return "/u/logs";
        }
      } catch (IOException ex) {
        // ignored
      }
      if (!new File("/home/systemcore/logs").mkdir()) {
        // ignored
      }
      UsageReporting.reportUsage("DataLogManager", "Onboard");
      m_internalStorage = true;
      return "/home/systemcore/logs";
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
    StringBuilder filename = new StringBuilder(18);
    filename.append("WPILIB_TBD_");
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
      m_consoleLogger = new FileLogger("/home/systemcore/WPILIB_UserProgram.log", m_log, "console");
    }
  }

  private static void stopConsoleLog() {
    if (RobotBase.isReal()) {
      m_consoleLogger.close();
    }
  }

  static void updateFreeSpaceAlert(Alert alert, long freeSpace) {
    boolean lowSpace = freeSpace < 2 * FREE_SPACE_THRESHOLD;
    if (lowSpace) {
      alert.setText(
          "DataLogManager: Log storage device has "
              + freeSpace / 1000000
              + " MB of free space remaining! Old logs are deleted at startup below "
              + FREE_SPACE_THRESHOLD / 1000000
              + " MB of free space. Consider deleting logs off the storage device.");
    }
    alert.set(lowSpace);
  }

  private static void checkFreeSpace(Alert alert) {
    try {
      updateFreeSpaceAlert(alert, Files.getFileStore(Path.of(m_logDir)).getUsableSpace());
    } catch (IOException ex) {
      // Keep the last known state if disk space cannot be read.
    }
  }

  private static void logMain() {
    try (var internalStorageAlert =
            new Alert(
                "DataLogManager",
                "internalStorage",
                "DataLogManager: Logging to SystemCore internal storage. Attach a USB "
                    + "drive to store logs externally.",
                Alert.Level.LOW);
        var freeSpaceAlert = new Alert("DataLogManager", "lowSpace", "", Alert.Level.MEDIUM)) {
      internalStorageAlert.set(m_internalStorage);
      logMain(freeSpaceAlert);
    }
  }

  private static void logMain(Alert freeSpaceAlert) {
    // based on free disk space, scan for "old" WPILIB_*.wpilog files and remove
    {
      File logDir = new File(m_logDir);
      long freeSpace = logDir.getUsableSpace();
      if (freeSpace < FREE_SPACE_THRESHOLD) {
        // Delete oldest non-match WPILIB_*.wpilog files. Ignore temporary and match logs.
        File[] files =
            logDir.listFiles(
                (dir, name) ->
                    name.startsWith("WPILIB_")
                        && name.endsWith(".wpilog")
                        && !name.startsWith("WPILIB_TBD_")
                        && !isMatchLog(name));
        if (files != null) {
          Arrays.sort(files, Comparator.comparingLong(File::lastModified));
          int count = files.length;
          for (File file : files) {
            --count;
            if (count < FILE_COUNT_THRESHOLD) {
              break;
            }
            long length = file.length();
            if (file.delete()) {
              DriverStationErrors.reportWarning("DataLogManager: Deleted " + file.getName(), false);
              freeSpace += length;
              if (freeSpace >= FREE_SPACE_THRESHOLD) {
                break;
              }
            } else {
              System.err.println("DataLogManager: could not delete " + file);
            }
          }
        }
      }
    }
    checkFreeSpace(freeSpaceAlert);
    long nextSpaceCheck = System.nanoTime() + 5_000_000_000L;

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
    DriverStationBackend.provideRefreshedDataEventHandle(newDataEvent.getHandle());
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
      long spaceCheckTime = System.nanoTime();
      if (spaceCheckTime - nextSpaceCheck >= 0) {
        checkFreeSpace(freeSpaceAlert);
        nextSpaceCheck = spaceCheckTime + 5_000_000_000L;
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
        if (RobotState.isDSAttached()) {
          dsAttachCount++;
        } else {
          dsAttachCount = 0;
        }
        if (dsAttachCount > 50) { // 1 second
          if (RobotController.isSystemTimeValid()) {
            LocalDateTime now = LocalDateTime.now(m_utc);
            m_log.setFilename("WPILIB_" + m_timeFormatter.format(now) + ".wpilog");
            dsRenamed = true;
          } else {
            dsAttachCount = 0; // wait a bit and try again
          }
        }
      }

      if (!fmsRenamed) {
        // track FMS attach
        if (RobotState.isFMSAttached()) {
          fmsAttachCount++;
        } else {
          fmsAttachCount = 0;
        }
        if (fmsAttachCount > 250) { // 5 seconds
          // match info comes through TCP, so we need to double-check we've
          // actually received it
          MatchType matchType = MatchState.getMatchType();
          if (matchType != MatchType.NONE) {
            // rename per match info
            char matchTypeChar =
                switch (matchType) {
                  case PRACTICE -> 'P';
                  case QUALIFICATION -> 'Q';
                  case ELIMINATION -> 'E';
                  default -> 'T';
                };
            m_log.setFilename(
                "WPILIB_"
                    + m_timeFormatter.format(LocalDateTime.now(m_utc))
                    + "_"
                    + MatchState.getEventName()
                    + "_"
                    + matchTypeChar
                    + MatchState.getMatchNumber()
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
          sysTimeEntry.append(WPIUtilJNI.getSystemTime() / 1000L, WPIUtilJNI.now());
        }
      }
    }
    newDataEvent.close();
  }
}
