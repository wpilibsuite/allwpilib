// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.container;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.lang.ref.WeakReference;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.SequencedSet;
import java.util.Set;
import org.junit.jupiter.api.Test;

@SuppressWarnings({"PMD.UnusedAssignment", "PMD.DoNotCallGarbageCollectionExplicitly"})
class WeakLinkedHashSetTest {
  private record TestValue(int id) {}

  @Test
  void behavesLikeLinkedHashSetForBasicSetOperations() {
    final Set<TestValue> expected = new LinkedHashSet<>();
    final Set<TestValue> actual = new WeakLinkedHashSet<>();

    final var a = new TestValue(1);
    final var b = new TestValue(2);
    final var c = new TestValue(3);
    final var d = new TestValue(4);

    assertEquals(expected.add(a), actual.add(a));
    assertEquals(expected.add(b), actual.add(b));
    assertEquals(expected.add(c), actual.add(c));
    assertEquals(expected.add(b), actual.add(b));

    assertEquals(expected.contains(a), actual.contains(a));
    assertEquals(expected.contains(d), actual.contains(d));

    assertEquals(expected.remove(b), actual.remove(b));
    assertEquals(expected.remove(b), actual.remove(b));

    assertEquals(expected.add(d), actual.add(d));

    assertEquals(expected.size(), actual.size());
    assertEquals(List.copyOf(expected), List.copyOf(actual));
  }

  @Test
  void reversedViewIterationMatchesLinkedHashSet() {
    final SequencedSet<TestValue> expected = new LinkedHashSet<>();
    final SequencedSet<TestValue> actual = new WeakLinkedHashSet<>();

    final var a = new TestValue(1);
    final var b = new TestValue(2);
    final var c = new TestValue(3);

    expected.add(a);
    expected.add(b);
    expected.add(c);

    actual.add(a);
    actual.add(b);
    actual.add(c);

    assertEquals(List.copyOf(expected.reversed()), List.copyOf(actual.reversed()));
  }

  @Test
  void positionalInsertionsMatchLinkedHashSet() {
    final SequencedSet<TestValue> expected = new LinkedHashSet<>();
    final SequencedSet<TestValue> actual = new WeakLinkedHashSet<>();

    final var a = new TestValue(1);
    final var b = new TestValue(2);
    final var c = new TestValue(3);
    final var d = new TestValue(4);

    expected.add(a);
    expected.add(b);
    actual.add(a);
    actual.add(b);

    expected.addFirst(c);
    expected.addLast(d);
    expected.addFirst(a);

    actual.addFirst(c);
    actual.addLast(d);
    actual.addFirst(a);

    assertEquals(List.copyOf(expected), List.copyOf(actual));
    assertEquals(List.copyOf(expected.reversed()), List.copyOf(actual.reversed()));
  }

  @Test
  void reversedViewAddMatchesLinkedHashSetAndDuplicateReturnValue() {
    final SequencedSet<TestValue> expected = new LinkedHashSet<>();
    final SequencedSet<TestValue> actual = new WeakLinkedHashSet<>();

    final var a = new TestValue(1);
    final var b = new TestValue(2);
    final var c = new TestValue(3);

    expected.add(a);
    expected.add(b);
    actual.add(a);
    actual.add(b);

    final SequencedSet<TestValue> expectedReversed = expected.reversed();
    final SequencedSet<TestValue> actualReversed = actual.reversed();

    assertEquals(expectedReversed.add(c), actualReversed.add(c));
    assertEquals(expectedReversed.add(c), actualReversed.add(c));

    assertEquals(List.copyOf(expected), List.copyOf(actual));
    assertEquals(List.copyOf(expectedReversed), List.copyOf(actualReversed));
  }

  @Test
  void nullArgumentsThrowInMainView() {
    final var set = new WeakLinkedHashSet<TestValue>();

    assertThrows(NullPointerException.class, () -> set.add(null));
    assertThrows(NullPointerException.class, () -> set.addFirst(null));
    assertThrows(NullPointerException.class, () -> set.addLast(null));
    assertThrows(NullPointerException.class, () -> set.contains(null));
    assertThrows(NullPointerException.class, () -> set.remove(null));
  }

  @Test
  void nullArgumentsThrowInReversedView() {
    final SequencedSet<TestValue> set = new WeakLinkedHashSet<>();
    final SequencedSet<TestValue> reversed = set.reversed();

    assertThrows(NullPointerException.class, () -> reversed.add(null));
    assertThrows(NullPointerException.class, () -> reversed.addFirst(null));
    assertThrows(NullPointerException.class, () -> reversed.addLast(null));
    assertThrows(NullPointerException.class, () -> reversed.contains(null));
    assertThrows(NullPointerException.class, () -> reversed.remove(null));
  }

  @Test
  void iteratorRemoveAfterHasNextRemovesPreviouslyReturnedElement() {
    final var set = new WeakLinkedHashSet<TestValue>();
    final var a = new TestValue(1);
    final var b = new TestValue(2);
    final var c = new TestValue(3);
    set.add(a);
    set.add(b);
    set.add(c);

    final Iterator<TestValue> iterator = set.iterator();
    assertEquals(a, iterator.next());
    assertTrue(iterator.hasNext());

    iterator.remove();

    assertEquals(List.of(b, c), List.copyOf(set));
  }

  @Test
  void iteratorRemoveBeforeNextThrows() {
    final var set = new WeakLinkedHashSet<TestValue>();
    set.add(new TestValue(1));

    final Iterator<TestValue> iterator = set.iterator();
    assertTrue(iterator.hasNext());
    assertThrows(IllegalStateException.class, iterator::remove);
  }

  @Test
  void garbageCollectedEntriesArePurged() {
    final var set = new WeakLinkedHashSet<TestValue>();

    var value = new TestValue(10);
    final var ref = new WeakReference<>(value);

    set.add(value);
    value = null; // clear the only reference to this object

    assertTrue(waitForCollection(ref));
    assertNull(ref.get());

    assertEquals(0, set.size());
    assertFalse(set.iterator().hasNext());
  }

  @Test
  void collectedEntriesDoNotRemoveLiveEntries() {
    final var set = new WeakLinkedHashSet<TestValue>();

    final var kept = new TestValue(1);
    var dropped = new TestValue(2);
    final var droppedRef = new WeakReference<>(dropped);

    set.add(kept);
    set.add(dropped);
    dropped = null; // clear the only reference to this object

    assertTrue(waitForCollection(droppedRef));
    assertNull(droppedRef.get());

    assertEquals(1, set.size());
    assertTrue(set.contains(new TestValue(1)));
    assertFalse(set.contains(new TestValue(2)));
    assertEquals(List.of(new TestValue(1)), List.copyOf(set));
  }

  private static boolean waitForCollection(WeakReference<?> reference) {
    // Allocate in a loop to increase garbage collection pressure and force the GC to clean up
    // our WeakReferences
    for (int i = 0; i < 200; i++) {
      if (reference.get() == null) {
        return true;
      }

      System.gc();
      byte[] pressure = new byte[1024 * 1024];
      pressure[0] = 1;

      try {
        Thread.sleep(5);
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
        break;
      }

      // Ensure JIT doesn't decide to elide the allocation
      assertEquals(1, pressure[0]);
    }

    return reference.get() == null;
  }
}
