/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

import java.util.Vector;

/**
 * A vector that is sorted.
 */
public class SortedVector extends Vector {

  /**
   * Interface used to determine the order to place sorted objects.
   */
  public interface Comparator {

    /**
     * Compare the given two objects.
     *
     * <p>Should return -1, 0, or 1 if the first object is less than, equal to, or greater than the
     * second, respectively.
     *
     * @param object1 First object to compare
     * @param object2 Second object to compare
     * @return -1, 0, or 1.
     */
    int compare(Object object1, Object object2);
  }

  private final Comparator m_comparator;

  /**
   * Create a new sorted vector and use the given comparator to determine order.
   *
   * @param comparator The comparator to use to determine what order to place the elements in this
   *                   vector.
   */
  public SortedVector(Comparator comparator) {
    m_comparator = comparator;
  }

  /**
   * Adds an element in the Vector, sorted from greatest to least.
   *
   * @param element The element to add to the Vector
   */
  public void addElement(Object element) {
    int highBound = size();
    int lowBound = 0;
    while (highBound - lowBound > 0) {
      int index = (highBound + lowBound) / 2;
      int result = m_comparator.compare(element, elementAt(index));
      if (result < 0) {
        lowBound = index + 1;
      } else if (result > 0) {
        highBound = index;
      } else {
        lowBound = index;
        highBound = index;
      }
    }
    insertElementAt(element, lowBound);
  }

  /**
   * Sort the vector.
   */
  public void sort() {
    Object[] array = new Object[size()];
    copyInto(array);
    removeAllElements();
    for (int i = 0; i < array.length; i++) {
      addElement(array[i]);
    }
  }
}
