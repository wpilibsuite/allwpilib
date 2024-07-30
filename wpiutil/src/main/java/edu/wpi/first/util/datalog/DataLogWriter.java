// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.io.IOException;
import java.io.OutputStream;

/** A data log writer that flushes the data log to a file when flush() is called. */
public class DataLogWriter extends DataLog {
  /**
   * Construct a new Data Log.
   *
   * @param filename filename to use
   * @param extraHeader extra header data
   * @throws IOException if file cannot be opened
   */
  public DataLogWriter(String filename, String extraHeader) throws IOException {
    super(DataLogJNI.fgCreate(filename, extraHeader));
    m_os = null;
    m_buf = null;
  }

  /**
   * Construct a new Data Log.
   *
   * @param filename filename to use
   * @throws IOException if file cannot be opened
   */
  public DataLogWriter(String filename) throws IOException {
    this(filename, "");
  }

  /**
   * Construct a new Data Log with an output stream. Prefer the filename version if possible; this
   * is much slower!
   *
   * @param os output stream
   * @param extraHeader extra header data
   */
  public DataLogWriter(OutputStream os, String extraHeader) {
    super(DataLogJNI.fgCreateMemory(extraHeader));
    m_os = os;
    m_buf = new byte[kBufferSize];
  }

  /**
   * Construct a new Data Log with an output stream.
   *
   * @param os output stream
   */
  public DataLogWriter(OutputStream os) {
    this(os, "");
  }

  /** Explicitly flushes the log data to disk. */
  @Override
  public void flush() {
    DataLogJNI.flush(m_impl);
    if (m_os == null) {
      return;
    }
    try {
      int pos = 0;
      for (; ; ) {
        int qty = DataLogJNI.copyWriteBuffer(m_impl, m_buf, pos);
        if (qty == 0) {
          break;
        }
        pos += qty;
        m_os.write(m_buf, 0, qty);
      }
      m_os.flush();
    } catch (IOException e) {
      // ignore
    }
  }

  private static final int kBufferSize = 16 * 1024;

  private final OutputStream m_os;
  private final byte[] m_buf;
}
