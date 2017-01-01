/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * This is a simple circular buffer so we don't need to "bucket brigade" copy old values.
 */
public class CircularBuffer {
  private double[] m_data;

  // Index of element at front of buffer
  private int m_front = 0;

  // Number of elements used in buffer
  private int m_length = 0;

  /**
   * @param size The size of the circular buffer.
   */
  public CircularBuffer(int size) {
    m_data = new double[size];
    for (int i = 0; i < m_data.length; i++) {
      m_data[i] = 0.0;
    }
  }

  /**
   * Push new value onto front of the buffer. The value at the back is overwritten if the buffer is
   * full.
   */
  public void pushFront(double value) {
    if (m_data.length == 0) {
      return;
    }

    m_front = moduloDec(m_front);

    m_data[m_front] = value;

    if (m_length < m_data.length) {
      m_length++;
    }
  }

  /**
   * Push new value onto back of the buffer. The value at the front is overwritten if the buffer is
   * full.
   */
  public void pushBack(double value) {
    if (m_data.length == 0) {
      return;
    }

    m_data[(m_front + m_length) % m_data.length] = value;

    if (m_length < m_data.length) {
      m_length++;
    } else {
      // Increment front if buffer is full to maintain size
      m_front = moduloInc(m_front);
    }
  }

  /**
   * Pop value at front of buffer.
   *
   * @return value at front of buffer
   */
  public double popFront() {
    // If there are no elements in the buffer, do nothing
    if (m_length == 0) {
      return 0.0;
    }

    double temp = m_data[m_front];
    m_front = moduloInc(m_front);
    m_length--;
    return temp;
  }


  /**
   * Pop value at back of buffer.
   */
  public double popBack() {
    // If there are no elements in the buffer, do nothing
    if (m_length == 0) {
      return 0.0;
    }

    m_length--;
    return m_data[(m_front + m_length) % m_data.length];
  }

  /**
   * Resizes internal buffer to given size.
   *
   * <p>A new buffer is allocated because arrays are not resizable.
   */
  void resize(int size) {
    double[] newBuffer = new double[size];
    m_length = Math.min(m_length, size);
    for (int i = 0; i < m_length; i++) {
      newBuffer[i] = m_data[(m_front + i) % m_data.length];
    }
    m_data = newBuffer;
    m_front = 0;
  }

  /**
   * Sets internal buffer contents to zero.
   */
  public void reset() {
    for (int i = 0; i < m_data.length; i++) {
      m_data[i] = 0.0;
    }
    m_front = 0;
    m_length = 0;
  }

  /**
   * @return Element at index starting from front of buffer.
   */
  public double get(int index) {
    return m_data[(m_front + index) % m_data.length];
  }

  /**
   * Increment an index modulo the length of the m_data buffer.
   */
  private int moduloInc(int index) {
    return (index + 1) % m_data.length;
  }

  /**
   * Decrement an index modulo the length of the m_data buffer.
   */
  private int moduloDec(int index) {
    if (index == 0) {
      return m_data.length - 1;
    } else {
      return index - 1;
    }
  }
}
