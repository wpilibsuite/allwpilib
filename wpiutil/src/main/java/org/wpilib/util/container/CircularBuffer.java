// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

/**
 * This is a simple circular buffer so we don't need to "bucket brigade" copy old values.
 *
 * @param <T> Buffer element type.
 */
public class CircularBuffer<T> {
  private T[] m_data;

  // Index of element at front of buffer
  private int m_front;

  // Number of elements used in buffer
  private int m_length;

  /**
   * Create a CircularBuffer with the provided size.
   *
   * @param size Maximum number of buffer elements.
   */
  @SuppressWarnings("unchecked")
  public CircularBuffer(int size) {
    m_data = (T[]) new Object[size];
  }

  /**
   * Returns number of elements in buffer.
   *
   * @return number of elements in buffer
   */
  public int size() {
    return m_length;
  }

  /**
   * Get value at front of buffer.
   *
   * @return value at front of buffer
   */
  public T getFirst() {
    return m_data[m_front];
  }

  /**
   * Get value at back of buffer.
   *
   * @return value at back of buffer
   * @throws IndexOutOfBoundsException if the index is out of range (index &lt; 0 || index &gt;=
   *     size())
   */
  public T getLast() {
    // If there are no elements in the buffer, do nothing
    if (m_length == 0) {
      throw new IndexOutOfBoundsException("getLast() called on an empty container");
    }

    return m_data[(m_front + m_length - 1) % m_data.length];
  }

  /**
   * Push new value onto front of the buffer. The value at the back is overwritten if the buffer is
   * full.
   *
   * @param value The value to push.
   */
  public void addFirst(T value) {
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
   *
   * @param value The value to push.
   */
  public void addLast(T value) {
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
   * @throws IndexOutOfBoundsException if the index is out of range (index &lt; 0 || index &gt;=
   *     size())
   */
  public T removeFirst() {
    // If there are no elements in the buffer, do nothing
    if (m_length == 0) {
      throw new IndexOutOfBoundsException("removeFirst() called on an empty container");
    }

    T temp = m_data[m_front];
    m_front = moduloInc(m_front);
    m_length--;
    return temp;
  }

  /**
   * Pop value at back of buffer.
   *
   * @return value at back of buffer
   * @throws IndexOutOfBoundsException if the index is out of range (index &lt; 0 || index &gt;=
   *     size())
   */
  public T removeLast() {
    // If there are no elements in the buffer, do nothing
    if (m_length == 0) {
      throw new IndexOutOfBoundsException("removeLast() called on an empty container");
    }

    m_length--;
    return m_data[(m_front + m_length) % m_data.length];
  }

  /**
   * Resizes internal buffer to given size.
   *
   * <p>A new buffer is allocated because arrays are not resizable.
   *
   * @param size New buffer size.
   */
  @SuppressWarnings("unchecked")
  public void resize(int size) {
    var newBuffer = (T[]) new Object[size];
    m_length = Math.min(m_length, size);
    for (int i = 0; i < m_length; i++) {
      newBuffer[i] = m_data[(m_front + i) % m_data.length];
    }
    m_data = newBuffer;
    m_front = 0;
  }

  /** Sets internal buffer contents to zero. */
  public void clear() {
    m_front = 0;
    m_length = 0;
  }

  /**
   * Get the element at the provided index relative to the start of the buffer.
   *
   * @param index Index into the buffer.
   * @return Element at index starting from front of buffer.
   */
  public T get(int index) {
    return m_data[(m_front + index) % m_data.length];
  }

  /**
   * Increment an index modulo the length of the buffer.
   *
   * @param index Index into the buffer.
   * @return The incremented index.
   */
  private int moduloInc(int index) {
    return (index + 1) % m_data.length;
  }

  /**
   * Decrement an index modulo the length of the buffer.
   *
   * @param index Index into the buffer.
   * @return The decremented index.
   */
  private int moduloDec(int index) {
    if (index == 0) {
      return m_data.length - 1;
    } else {
      return index - 1;
    }
  }
}
