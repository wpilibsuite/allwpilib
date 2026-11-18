// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.collections;

import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.function.Function;
import java.util.function.ToDoubleFunction;

/** Searching utilities for collections. */
public final class Search {
  private Search() {
    // utility class
  }

  /**
   * Searches a sorted list for a key using binary search, comparing the key against a projection of
   * each element. This is useful when elements are sorted by one of their properties and a bare key
   * of that property's type is all that is available, which {@link
   * java.util.Collections#binarySearch(List, Object, Comparator)} cannot do because it requires a
   * key of the element type.
   *
   * <p>The list must be sorted in ascending order of the projected values according to the
   * comparator. The result is undefined if it is not. If the list contains multiple elements whose
   * projection equals the key, there is no guarantee which one is found.
   *
   * <p>This runs in O(log n) time for a {@link java.util.RandomAccess} list. For other lists, each
   * probe is a {@link List#get(int)} and the search is correspondingly slower.
   *
   * @param <T> The element type of the list.
   * @param <K> The type of the key and of the projected element values.
   * @param list The list to search, sorted by the projected values.
   * @param key The key to search for.
   * @param comparator The comparator that orders projected values.
   * @param projector A function that extracts the value to compare against the key from an element.
   * @return The index of an element whose projection equals the key if one exists. Otherwise,
   *     {@code -(insertion point) - 1}, where the insertion point is the index of the first element
   *     whose projection is greater than the key, or the list size if there is none. The return
   *     value is non-negative if and only if a match is found.
   */
  public static <T, K> int binarySearch(
      List<? extends T> list,
      K key,
      Comparator<? super K> comparator,
      Function<? super T, ? extends K> projector) {
    int low = 0;
    int high = list.size() - 1;

    while (low <= high) {
      int mid = low + (high - low) / 2;
      int cmp = comparator.compare(projector.apply(list.get(mid)), key);

      if (cmp < 0) {
        low = mid + 1;
      } else if (cmp > 0) {
        high = mid - 1;
      } else {
        return mid;
      }
    }

    return -(low + 1);
  }

  /**
   * Searches a sorted list for a {@code double} key using binary search, comparing the key against
   * a projection of each element. This avoids boxing the projected values, unlike {@link
   * #binarySearch(List, Object, Comparator, Function)}.
   *
   * <p>The list must be sorted in ascending order of the projected values according to {@link
   * Double#compare(double, double)}. The result is undefined if it is not. If the list contains
   * multiple elements whose projection equals the key, there is no guarantee which one is found.
   *
   * <p>This runs in O(log n) time for a {@link java.util.RandomAccess} list. For other lists, each
   * probe is a {@link List#get(int)} and the search is correspondingly slower.
   *
   * @param <T> The element type of the list.
   * @param list The list to search, sorted by the projected values.
   * @param key The key to search for.
   * @param projector A function that extracts the value to compare against the key from an element.
   * @return The index of an element whose projection equals the key if one exists. Otherwise,
   *     {@code -(insertion point) - 1}, where the insertion point is the index of the first element
   *     whose projection is greater than the key, or the list size if there is none. The return
   *     value is non-negative if and only if a match is found.
   */
  public static <T> int binarySearch(
      List<? extends T> list, double key, ToDoubleFunction<? super T> projector) {
    int low = 0;
    int high = list.size() - 1;

    while (low <= high) {
      int mid = low + (high - low) / 2;
      int cmp = Double.compare(projector.applyAsDouble(list.get(mid)), key);

      if (cmp < 0) {
        low = mid + 1;
      } else if (cmp > 0) {
        high = mid - 1;
      } else {
        return mid;
      }
    }

    return -(low + 1);
  }

  /**
   * Searches a sorted array for a key using binary search, comparing the key against a projection
   * of each element. See {@link #binarySearch(List, Object, Comparator, Function)} for the full
   * contract.
   *
   * @param <T> The element type of the array.
   * @param <K> The type of the key and of the projected element values.
   * @param array The array to search, sorted by the projected values.
   * @param key The key to search for.
   * @param comparator The comparator that orders projected values.
   * @param projector A function that extracts the value to compare against the key from an element.
   * @return The index of an element whose projection equals the key if one exists. Otherwise,
   *     {@code -(insertion point) - 1}, where the insertion point is the index of the first element
   *     whose projection is greater than the key, or the array length if there is none. The return
   *     value is non-negative if and only if a match is found.
   */
  public static <T, K> int binarySearch(
      T[] array,
      K key,
      Comparator<? super K> comparator,
      Function<? super T, ? extends K> projector) {
    return binarySearch(Arrays.asList(array), key, comparator, projector);
  }

  /**
   * Searches a sorted array for a {@code double} key using binary search, comparing the key against
   * a projection of each element. This avoids boxing the projected values. See {@link
   * #binarySearch(List, double, ToDoubleFunction)} for the full contract.
   *
   * @param <T> The element type of the array.
   * @param array The array to search, sorted by the projected values.
   * @param key The key to search for.
   * @param projector A function that extracts the value to compare against the key from an element.
   * @return The index of an element whose projection equals the key if one exists. Otherwise,
   *     {@code -(insertion point) - 1}, where the insertion point is the index of the first element
   *     whose projection is greater than the key, or the array length if there is none. The return
   *     value is non-negative if and only if a match is found.
   */
  public static <T> int binarySearch(T[] array, double key, ToDoubleFunction<? super T> projector) {
    return binarySearch(Arrays.asList(array), key, projector);
  }
}
