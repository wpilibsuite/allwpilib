/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.Arrays;
import java.util.stream.IntStream;

/**
 * A class that implements a moving-window majority vote filter for booleans.
 * Useful for reducing boolean inconsistency, such as inconsistent limit switch readings or
 * checking whether a quickly-changing measurement is in a certain tolerance.
 */
public class BooleanMajorityFilter {
  private final boolean[] m_values;
  private int m_index;
  private final double m_percentage;

  /**
   * Creates a new BooleanMajorityFilter.
   *
   * @param size The number of votes in the moving window, must be positive
   * @param percentage The percentage of true values in order for filtered value to be true
   * @throws IllegalArgumentException if size is smaller than 1 or percentage is out of range
   */
  public BooleanMajorityFilter(int size, double percentage) {
    if (size < 1) {
      throw new IllegalArgumentException("Must have a size of above zero");
    }
    if (percentage > 1.0 || percentage < 0.0) {
      throw new IllegalArgumentException("True percentage must be between 0 and 1");
    }
    m_percentage = percentage;
    //values in rolling window, ordered by time in a cycle
    m_values = new boolean[size];
    Arrays.fill(m_values, false);
    //rotating index
    m_index = 0;
  }

  /**
   * Creates a new BooleanMajorityFilter.
   *
   * @param size The number of votes in the moving window, must be positive
   * @throws IllegalArgumentException if size is smaller than 1
   */
  public BooleanMajorityFilter(int size) {
    this(size, 0.5);
  }

  /**
   * Calculates the moving-window majority for the next value of the input stream.
   *
   * @param next The next input value.
   * @return The majority vote of the moving window, updated to include the next value.
   If tied, returns false.
   *
   */
  public boolean calculate(boolean next) {
    //insert value in current index, rotated to the array size
    m_values[m_index % m_values.length] = next;

    //advance index
    m_index++;

    // if array isn't full, how many items do we have
    int itemCount = Math.min(m_values.length, m_index);

    //count how many 'true' values we haves
    long trueCount = IntStream.range(0, itemCount)
            .filter(i -> m_values[i]).count();

    //if more than half of the values are true, then majority is true
    return trueCount > (itemCount * m_percentage);
  }


  /**
   * Resets the filter, clearing the window of all elements.
   */
  public void reset() {
    m_index = 0;
    Arrays.fill(m_values, false);
  }
}
