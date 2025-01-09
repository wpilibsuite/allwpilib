// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.function.Consumer;

/** DataLogReader iterator. */
public class DataLogIterator implements Iterator<DataLogRecord> {
  DataLogIterator(DataLogReader reader, int pos) {
    m_reader = reader;
    m_pos = pos;
  }

  @Override
  public void forEachRemaining(Consumer<? super DataLogRecord> action) {
    int size = m_reader.size();
    for (; m_pos < size; m_pos = m_reader.getNextRecord(m_pos)) {
      DataLogRecord record;
      try {
        record = m_reader.getRecord(m_pos);
      } catch (NoSuchElementException ex) {
        break;
      }
      action.accept(record);
    }
  }

  @Override
  public boolean hasNext() {
    return (m_pos + 16) <= m_reader.size();
  }

  @Override
  public DataLogRecord next() {
    DataLogRecord record = m_reader.getRecord(m_pos);
    m_pos = m_reader.getNextRecord(m_pos);
    return record;
  }

  private final DataLogReader m_reader;
  private int m_pos;
}
