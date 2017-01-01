/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.BeforeClass;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class CircularBufferTest {
  private double[] m_values = {751.848, 766.366, 342.657, 234.252, 716.126,
      132.344, 445.697, 22.727, 421.125, 799.913};
  private double[] m_pushFrontOut = {799.913, 421.125, 22.727, 445.697, 132.344,
      716.126, 234.252, 342.657};
  private double[] m_pushBackOut = {342.657, 234.252, 716.126, 132.344, 445.697,
      22.727, 421.125, 799.913};

  @BeforeClass
  public static void before() {
    UnitTestUtility.setupMockBase();
  }

  @Test
  public void pushFrontTest() {
    CircularBuffer queue = new CircularBuffer(8);

    for (double value : m_values) {
      queue.pushFront(value);
    }

    for (int i = 0; i < m_pushFrontOut.length; i++) {
      assertEquals(m_pushFrontOut[i], queue.get(i), 0.00005);
    }
  }

  @Test
  public void pushBackTest() {
    CircularBuffer queue = new CircularBuffer(8);

    for (double value : m_values) {
      queue.pushBack(value);
    }

    for (int i = 0; i < m_pushBackOut.length; i++) {
      assertEquals(m_pushBackOut[i], queue.get(i), 0.00005);
    }
  }

  @Test
  public void pushPopTest() {
    CircularBuffer queue = new CircularBuffer(3);

    // Insert three elements into the buffer
    queue.pushBack(1.0);
    queue.pushBack(2.0);
    queue.pushBack(3.0);

    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);
    assertEquals(3.0, queue.get(2), 0.00005);

    /*
     * The buffer is full now, so pushing subsequent elements will overwrite the
     * front-most elements.
     */

    queue.pushBack(4.0); // Overwrite 1 with 4

    // The buffer now contains 2, 3, and 4
    assertEquals(2.0, queue.get(0), 0.00005);
    assertEquals(3.0, queue.get(1), 0.00005);
    assertEquals(4.0, queue.get(2), 0.00005);

    queue.pushBack(5.0); // Overwrite 2 with 5

    // The buffer now contains 3, 4, and 5
    assertEquals(3.0, queue.get(0), 0.00005);
    assertEquals(4.0, queue.get(1), 0.00005);
    assertEquals(5.0, queue.get(2), 0.00005);

    assertEquals(5.0, queue.popBack(), 0.00005); // 5 is removed

    // The buffer now contains 3 and 4
    assertEquals(3.0, queue.get(0), 0.00005);
    assertEquals(4.0, queue.get(1), 0.00005);

    assertEquals(3.0, queue.popFront(), 0.00005); // 3 is removed

    // Leaving only one element with value == 4
    assertEquals(4.0, queue.get(0), 0.00005);
  }

  @Test
  public void resetTest() {
    CircularBuffer queue = new CircularBuffer(5);

    for (int i = 0; i < 6; i++) {
      queue.pushBack(i);
    }

    queue.reset();

    for (int i = 0; i < 5; i++) {
      assertEquals(0.0, queue.get(i), 0.00005);
    }
  }

  @Test
  public void resizeTest() {
    CircularBuffer queue = new CircularBuffer(5);

    /* Buffer contains {1, 2, 3, _, _}
     *                  ^ front
     */
    queue.pushBack(1.0);
    queue.pushBack(2.0);
    queue.pushBack(3.0);

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.reset();

    /* Buffer contains {_, 1, 2, 3, _}
     *                     ^ front
     */
    queue.pushBack(0.0);
    queue.pushBack(1.0);
    queue.pushBack(2.0);
    queue.pushBack(3.0);
    queue.popFront();

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.reset();

    /* Buffer contains {_, _, 1, 2, 3}
     *                        ^ front
     */
    queue.pushBack(0.0);
    queue.pushBack(0.0);
    queue.pushBack(1.0);
    queue.pushBack(2.0);
    queue.pushBack(3.0);
    queue.popFront();
    queue.popFront();

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.reset();

    /* Buffer contains {3, _, _, 1, 2}
     *                           ^ front
     */
    queue.pushBack(3.0);
    queue.pushFront(2.0);
    queue.pushFront(1.0);

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.reset();

    /* Buffer contains {2, 3, _, _, 1}
     *                              ^ front
     */
    queue.pushBack(2.0);
    queue.pushBack(3.0);
    queue.pushFront(1.0);

    queue.resize(2);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    queue.resize(5);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);

    // Test pushBack() after resize
    queue.pushBack(3.0);
    assertEquals(1.0, queue.get(0), 0.00005);
    assertEquals(2.0, queue.get(1), 0.00005);
    assertEquals(3.0, queue.get(2), 0.00005);

    // Test pushFront() after resize
    queue.pushFront(4.0);
    assertEquals(4.0, queue.get(0), 0.00005);
    assertEquals(1.0, queue.get(1), 0.00005);
    assertEquals(2.0, queue.get(2), 0.00005);
    assertEquals(3.0, queue.get(3), 0.00005);
  }
}
