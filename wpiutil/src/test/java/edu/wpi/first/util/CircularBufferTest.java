// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class CircularBufferTest {
  private final double[] m_values = {
    751.848, 766.366, 342.657, 234.252, 716.126, 132.344, 445.697, 22.727, 421.125, 799.913
  };
  private final double[] m_addFirstOut = {
    799.913, 421.125, 22.727, 445.697, 132.344, 716.126, 234.252, 342.657
  };
  private final double[] m_addLastOut = {
    342.657, 234.252, 716.126, 132.344, 445.697, 22.727, 421.125, 799.913
  };

  @Test
  void addFirstTest() {
    CircularBuffer queue = new CircularBuffer(8);

    for (double value : m_values) {
      queue.addFirst(value);
    }

    for (int i = 0; i < m_addFirstOut.length; i++) {
      assertEquals(m_addFirstOut[i], queue.get(i), 0.00005);
    }
  }

  @Test
  void addLastTest() {
    CircularBuffer queue = new CircularBuffer(8);

    for (double value : m_values) {
      queue.addLast(value);
    }

    for (int i = 0; i < m_addLastOut.length; i++) {
      assertEquals(m_addLastOut[i], queue.get(i), 0.00005);
    }
  }

  @Test
  void pushPopTest() {
    CircularBuffer queue = new CircularBuffer(3);

    // Insert three elements into the buffer
    queue.addLast(1.0);
    queue.addLast(2.0);
    queue.addLast(3.0);

    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);
    assertEquals(3.0, queue.get(2), 0.00005);

    /*
     * The buffer is full now, so pushing subsequent elements will overwrite the
     * front-most elements.
     */

    queue.addLast(4.0); // Overwrite 1 with 4

    // The buffer now contains 2, 3, and 4
    assertEquals(2.0, queue.get(0), 0.00005);
    assertEquals(3.0, queue.get(1), 0.00005);
    assertEquals(4.0, queue.get(2), 0.00005);

    queue.addLast(5.0); // Overwrite 2 with 5

    // The buffer now contains 3, 4, and 5
    assertEquals(3.0, queue.get(0), 0.00005);
    assertEquals(4.0, queue.get(1), 0.00005);
    assertEquals(5.0, queue.get(2), 0.00005);

    assertEquals(5.0, queue.removeLast(), 0.00005); // 5 is removed

    // The buffer now contains 3 and 4
    assertEquals(3.0, queue.get(0), 0.00005);
    assertEquals(4.0, queue.get(1), 0.00005);

    assertEquals(3.0, queue.removeFirst(), 0.00005); // 3 is removed

    // Leaving only one element with value == 4
    assertEquals(4.0, queue.get(0), 0.00005);
  }

  @Test
  void resetTest() {
    CircularBuffer queue = new CircularBuffer(5);

    for (int i = 0; i < 6; i++) {
      queue.addLast(i);
    }

    queue.clear();

    for (int i = 0; i < 5; i++) {
      assertEquals(0.0, queue.get(i), 0.00005);
    }
  }

  @Test
  void resizeTest() {
    CircularBuffer queue = new CircularBuffer(5);

    /* Buffer contains {1, 2, 3, _, _}
     *                  ^ front
     */
    queue.addLast(1.0);
    queue.addLast(2.0);
    queue.addLast(3.0);

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.clear();

    /* Buffer contains {_, 1, 2, 3, _}
     *                     ^ front
     */
    queue.addLast(0.0);
    queue.addLast(1.0);
    queue.addLast(2.0);
    queue.addLast(3.0);
    queue.removeFirst();

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.clear();

    /* Buffer contains {_, _, 1, 2, 3}
     *                        ^ front
     */
    queue.addLast(0.0);
    queue.addLast(0.0);
    queue.addLast(1.0);
    queue.addLast(2.0);
    queue.addLast(3.0);
    queue.removeFirst();
    queue.removeFirst();

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.clear();

    /* Buffer contains {3, _, _, 1, 2}
     *                           ^ front
     */
    queue.addLast(3.0);
    queue.addFirst(2.0);
    queue.addFirst(1.0);

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.clear();

    /* Buffer contains {2, 3, _, _, 1}
     *                              ^ front
     */
    queue.addLast(2.0);
    queue.addLast(3.0);
    queue.addFirst(1.0);

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    // Test addLast() after resize
    queue.addLast(3.0);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);
    assertEquals(3.0, queue.get(2), 0.00005);

    // Test addFirst() after resize
    queue.addFirst(4.0);
    assertEquals(4.0, queue.get(0), 0.00005);
    assertEquals(1.0, queue.get(1), 0.00005);
    assertEquals(2.0, queue.get(2), 0.00005);
    assertEquals(3.0, queue.get(3), 0.00005);
  }
}
