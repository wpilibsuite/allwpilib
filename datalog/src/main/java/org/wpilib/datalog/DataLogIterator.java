// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Objects;
import java.util.function.Consumer;

/** DataLogReader iterator. */
public class DataLogIterator implements Iterator<DataLogRecord> {
  DataLogIterator(DataLogReader reader, int pos) {
    m_reader = reader;
    m_pos = pos;
  }

  @Override
  public void forEachRemaining(Consumer<? super DataLogRecord> action) {
    Objects.requireNonNull(action);
    while (hasNext()) {
      action.accept(next());
    }
  }

  @Override
  public boolean hasNext() {
    if (m_next == null && !m_done) {
      try {
        m_next = m_reader.getRecordInfo(m_pos);
      } catch (NoSuchElementException ex) {
        m_done = true;
      }
    }
    return m_next != null;
  }

  @Override
  public DataLogRecord next() {
    if (!hasNext()) {
      throw new NoSuchElementException();
    }
    DataLogRecord record = m_next.record;
    m_pos = m_next.nextPos;
    m_next = null;
    return record;
  }

  private final DataLogReader m_reader;
  private int m_pos;
  private DataLogReader.RecordInfo m_next;
  private boolean m_done;
}
