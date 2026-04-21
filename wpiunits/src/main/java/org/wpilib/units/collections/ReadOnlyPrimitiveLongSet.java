// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.units.collections;

import java.util.Arrays;
import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.stream.LongStream;

/** A read-only set of unique primitive {@code long} values. */
public class ReadOnlyPrimitiveLongSet implements Iterable<Long> {
  private final long[] m_values;

  /**
   * Creates a new set from the given values. These values do not have to be unique.
   *
   * @param values the values that belong to the set.
   */
  @SuppressWarnings({"PMD.ForLoopCanBeForeach", "ForLoopReplaceableByForEach"})
  public ReadOnlyPrimitiveLongSet(long... values) {
    // initial size is the upper limit
    long[] uniqueValues = new long[values.length];
    int numUniqueValues = 0;
    boolean seenZero = false;

    // copy the set of unique values to our array
    // using indexed for-loops to avoid allocations
    copyLoop:
    for (int i = 0; i < values.length; i++) {
      long value = values[i];
      if (value == 0 && !seenZero) {
        // special case to support zero
        seenZero = true;
      } else {
        for (int j = 0; j < uniqueValues.length; j++) {
          long uniqueValue = uniqueValues[j];
          if (uniqueValue == value) {
            continue copyLoop;
          }
        }
      }
      uniqueValues[numUniqueValues] = value;
      numUniqueValues++;
    }

    if (numUniqueValues == values.length) {
      // all input values were unique, no need to truncate
      m_values = uniqueValues;
    } else {
      // truncate the array to remove trailing empty space
      m_values = Arrays.copyOf(uniqueValues, numUniqueValues);
    }
  }

  /**
   * Checks if the set contains a particular value.
   *
   * @param value the value to check for
   * @return true if the value is in the set, false if not
   */
  public boolean contains(long value) {
    for (long mValue : m_values) {
      if (mValue == value) {
        return true;
      }
    }
    return false;
  }

  /**
   * Retrieves the number of elements in the set.
   *
   * @return the number of elements in the set
   */
  public int size() {
    return m_values.length;
  }

  /**
   * Checks if the set is empty, i.e. contains no values.
   *
   * @return true if there are no values in the set, false otherwise.
   */
  public boolean isEmpty() {
    return size() == 0;
  }

  /**
   * Creates a stream of primitive long values for the set.
   *
   * @return a sequential Stream over the elements in this collection
   * @see Set#stream()
   */
  public LongStream stream() {
    return Arrays.stream(m_values);
  }

  /**
   * Creates a new array that contains all of the values in the set.
   *
   * @return an array containing all the values in the set
   */
  public long[] toArray() {
    return Arrays.copyOf(m_values, m_values.length);
  }

  @Override
  public Iterator<Long> iterator() {
    return new Iterator<>() {
      private int m_index = 0;

      @Override
      public boolean hasNext() {
        return m_index < ReadOnlyPrimitiveLongSet.this.size();
      }

      @Override
      public Long next() {
        if (!hasNext()) {
          throw new NoSuchElementException();
        }

        return ReadOnlyPrimitiveLongSet.this.m_values[m_index++];
      }
    };
  }
}
