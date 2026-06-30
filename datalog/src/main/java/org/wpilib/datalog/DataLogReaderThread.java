// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.Consumer;
import org.wpilib.datalog.DataLogRecord.MetadataRecordData;
import org.wpilib.datalog.DataLogRecord.StartRecordData;

/**
 * DataLogReaderThread reads logs on a background thread via {@link DataLogReader} in order to
 * associate human-readable metadata with log records.
 */
public class DataLogReaderThread implements AutoCloseable {
  private final Thread m_thread;
  private final ReentrantLock m_mutex;
  private final DataLogReader m_reader;
  private final AtomicBoolean m_done = new AtomicBoolean(false);
  private final AtomicBoolean m_active = new AtomicBoolean(true);
  private final AtomicInteger m_numRecords = new AtomicInteger(0);

  /**
   * Creates a new DataLogReaderThread.
   *
   * @param reader A DataLogReader instance for the log file that should be read.
   */
  public DataLogReaderThread(DataLogReader reader) {
    m_mutex = new ReentrantLock();
    m_reader = reader;
    m_entriesById = new HashMap<>();
    m_entriesByName = new HashMap<>();
    m_thread = new Thread(this::readMain);
    m_thread.start();
  }

  private void readMain() {
    for (DataLogRecord record : m_reader) {
      if (!m_active.get()) {
        break;
      }
      m_numRecords.incrementAndGet();
      if (record.isStart()) {
        DataLogReaderEntry data = new DataLogReaderEntry(record.getStartData());
        m_mutex.lock();
        try {
          DataLogReaderEntry oldEntry = m_entriesById.put(data.entry, data);
          if (oldEntry != null) {
            System.out.println("...DUPLICATE entry ID, overriding");
          }
          boolean isNew = m_entriesByName.containsKey(data.name);
          m_entriesByName.put(data.name, data);
          if (isNew) {
            data.m_ranges.add(
                new DataLogReaderRange(
                    m_reader.iterator(), new DataLogIterator(m_reader, Integer.MAX_VALUE)));
          }
        } finally {
          m_mutex.unlock();
        }
      } else if (record.isFinish()) {
        m_mutex.lock();
        try {
          var readerEntry = m_entriesById.get(record.getEntry());
          if (readerEntry == null) {
            System.out.println("...ID not found");
          } else {
            readerEntry.m_ranges.getLast().m_end = m_reader.iterator();
            m_entriesById.remove(record.getEntry());
          }
        } finally {
          m_mutex.unlock();
        }
      } else if (record.isSetMetadata()) {
        MetadataRecordData data = record.getSetMetadataData();
        m_mutex.lock();
        try {
          var readerEntry = m_entriesById.get(data.entry);
          if (readerEntry == null) {
            System.out.println("...ID not found");
          } else {
            readerEntry.metadata = data.metadata;
          }
        } finally {
          m_mutex.unlock();
        }
      } else if (record.isControl()) {
        System.out.println("Unrecognized control record");
      }
    }

    m_done.set(true);
  }

  /**
   * Returns the number of records contained in the given DataLog.
   *
   * @return The number of records in this DataLog.
   */
  public int getNumRecords() {
    return m_numRecords.get();
  }

  /**
   * Returns the number of distinct entries in the DataLog.
   *
   * @return The number of distinct entries in the DataLog.
   */
  public int getNumEntries() {
    m_mutex.lock();
    try {
      return m_entriesByName.size();
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Executes a user-provided function on the each uniquely-named entry from the DataLog.
   *
   * @param func A function that accepts a <code>DataLogReaderEntry</code> that will be called on
   *     every entry with a unique name.
   */
  public void forEachEntryName(Consumer<DataLogReaderEntry> func) {
    m_mutex.lock();
    try {
      for (var kv : m_entriesByName.entrySet()) {
        func.accept(kv.getValue());
      }
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Checks if the background thread has finished processing the entire log.
   *
   * @return A boolean representing if the background thread has finished processing the entire log.
   */
  public boolean isDone() {
    return m_done.get();
  }

  /**
   * Returns the <code>DataLogReader</code> object associated with the log.
   *
   * @return The <code>DataLogReader</code> object associated with the log.
   */
  public DataLogReader getReader() {
    return m_reader;
  }

  /**
   * Fetches the entry with the given id.
   *
   * @param entry Id number of the desired entry, which is associated with all of its records.
   * @return The DataLogReaderEntry associated with that entry id.
   */
  DataLogReaderEntry getEntry(int entry) {
    m_mutex.lock();
    try {
      if (m_entriesById.containsKey(entry)) {
        return m_entriesById.get(entry);
      } else {
        return null;
      }
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Fetches the entry with the given name.
   *
   * @param name Name string of an entry.
   * @return The DataLogReaderEntry associated with that name.
   */
  DataLogReaderEntry getEntry(String name) {
    m_mutex.lock();
    try {
      if (m_entriesByName.containsKey(name)) {
        return m_entriesByName.get(name);
      } else {
        return null;
      }
    } finally {
      m_mutex.unlock();
    }
  }

  private final HashMap<Integer, DataLogReaderEntry> m_entriesById;
  private final HashMap<String, DataLogReaderEntry> m_entriesByName;

  /**
   * DataLogReader Entry class, which associates an entry's ID with its name, type, and metadata in
   * a persistent way.
   */
  public static class DataLogReaderEntry extends StartRecordData {
    private final List<DataLogReaderRange> m_ranges;

    /**
     * Returns the list of ranges for which this entry is valid.
     *
     * @return List of DataLogReaderRange for which this entry is valid
     */
    public List<DataLogReaderRange> getRanges() {
      return List.copyOf(m_ranges);
    }

    public DataLogReaderEntry(int entry, String name, String type, String metadata) {
      super(entry, name, type, metadata);
      m_ranges = new ArrayList<>();
    }

    public DataLogReaderEntry(StartRecordData startData) {
      super(startData.entry, startData.name, startData.type, startData.metadata);
      m_ranges = new ArrayList<>();
    }
  }

  /** Range of records during which an entry is valid. */
  public static class DataLogReaderRange {
    public DataLogIterator m_begin;
    public DataLogIterator m_end;

    public DataLogReaderRange(DataLogIterator begin, DataLogIterator end) {
      m_begin = begin;
      m_end = end;
    }
  }

  @Override
  public void close() {
    m_active.set(false);
    try {
      m_thread.join();
    } catch (InterruptedException e) {
      System.out.println("DataLog Reader Thread was interrupted");
    }
  }
}
