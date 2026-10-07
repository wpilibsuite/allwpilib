// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.collections;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Comparator;
import java.util.List;
import org.junit.jupiter.api.Test;

class SearchTest {
  private record Item(String name, double value) {}

  private static final List<Item> ITEMS =
      List.of(new Item("a", 1.0), new Item("b", 2.5), new Item("c", 4.0), new Item("d", 10.0));

  @Test
  void testFindsExactMatchWithComparator() {
    for (int i = 0; i < ITEMS.size(); i++) {
      assertEquals(
          i,
          Search.binarySearch(ITEMS, ITEMS.get(i).value(), Comparator.naturalOrder(), Item::value));
    }
  }

  @Test
  void testInsertionPointWithComparator() {
    // Before the first element
    assertEquals(-1, Search.binarySearch(ITEMS, 0.0, Comparator.naturalOrder(), Item::value));
    // Between elements 0 and 1
    assertEquals(-2, Search.binarySearch(ITEMS, 2.0, Comparator.naturalOrder(), Item::value));
    // Between elements 2 and 3
    assertEquals(-4, Search.binarySearch(ITEMS, 5.0, Comparator.naturalOrder(), Item::value));
    // After the last element
    assertEquals(-5, Search.binarySearch(ITEMS, 11.0, Comparator.naturalOrder(), Item::value));
  }

  @Test
  void testComparatorIsUsed() {
    // Sorted descending, so the reversed comparator is required to find elements
    var descending = List.of(new Item("d", 10.0), new Item("c", 4.0), new Item("b", 2.5));
    assertEquals(1, Search.binarySearch(descending, 4.0, Comparator.reverseOrder(), Item::value));
    assertEquals(-1, Search.binarySearch(descending, 20.0, Comparator.reverseOrder(), Item::value));
    assertEquals(-4, Search.binarySearch(descending, 1.0, Comparator.reverseOrder(), Item::value));
  }

  @Test
  void testNonNumericKey() {
    assertEquals(2, Search.binarySearch(ITEMS, "c", Comparator.naturalOrder(), Item::name));
    assertEquals(-3, Search.binarySearch(ITEMS, "bb", Comparator.naturalOrder(), Item::name));
  }

  @Test
  void testFindsExactMatchWithDouble() {
    for (int i = 0; i < ITEMS.size(); i++) {
      assertEquals(i, Search.binarySearch(ITEMS, ITEMS.get(i).value(), Item::value));
    }
  }

  @Test
  void testInsertionPointWithDouble() {
    assertEquals(-1, Search.binarySearch(ITEMS, 0.0, Item::value));
    assertEquals(-2, Search.binarySearch(ITEMS, 2.0, Item::value));
    assertEquals(-4, Search.binarySearch(ITEMS, 5.0, Item::value));
    assertEquals(-5, Search.binarySearch(ITEMS, 11.0, Item::value));
  }

  @Test
  void testEmptyList() {
    assertEquals(-1, Search.binarySearch(List.<Item>of(), 1.0, Item::value));
    assertEquals(
        -1, Search.binarySearch(List.<Item>of(), 1.0, Comparator.naturalOrder(), Item::value));
  }

  @Test
  void testSingleElement() {
    var single = List.of(new Item("a", 3.0));
    assertEquals(0, Search.binarySearch(single, 3.0, Item::value));
    assertEquals(-1, Search.binarySearch(single, 2.0, Item::value));
    assertEquals(-2, Search.binarySearch(single, 4.0, Item::value));
  }

  @Test
  void testDuplicateKeysFindOneOfThem() {
    var duplicates =
        List.of(new Item("a", 1.0), new Item("b", 2.0), new Item("c", 2.0), new Item("d", 3.0));
    int index = Search.binarySearch(duplicates, 2.0, Item::value);
    assertTrue(index == 1 || index == 2, "Expected an index of a matching element, got " + index);
  }

  @Test
  void testMatchesCollectionsBinarySearchForPlainValues() {
    var values = List.of(-5.0, -1.0, 0.0, 0.5, 3.0, 7.0, 7.5, 100.0);
    for (double key : new double[] {-10.0, -5.0, -2.0, 0.0, 0.25, 7.0, 50.0, 100.0, 200.0}) {
      assertEquals(
          java.util.Collections.binarySearch(values, key),
          Search.binarySearch(values, key, Double::doubleValue));
    }
  }

  @Test
  void testNegativeZeroAndZeroAreDistinctLikeDoubleCompare() {
    // Double.compare treats -0.0 < 0.0, matching Arrays.binarySearch(double[], double)
    var values = List.of(-0.0, 0.0);
    assertEquals(0, Search.binarySearch(values, -0.0, Double::doubleValue));
    assertEquals(1, Search.binarySearch(values, 0.0, Double::doubleValue));
  }
}
