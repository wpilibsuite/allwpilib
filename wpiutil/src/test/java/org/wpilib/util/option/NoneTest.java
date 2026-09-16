// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.option;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertThrowsExactly;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.NoSuchElementException;
import org.junit.jupiter.api.Test;

class NoneTest {
  @Test
  void orElseReturnsFallback() {
    var none = None.none();
    assertEquals("foo", none.orElse("foo"));
  }

  @Test
  void orElseThrowsOnNull() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.orElse(null));
    assertTrue(
        err.getMessage().contains("Parameter fallbackValue in method Option.orElse was null"));
  }

  @Test
  void orElseGetReturnsFallback() {
    var none = None.none();
    assertEquals("foo", none.orElseGet(() -> "foo"));
  }

  @Test
  void orElseGetThrowsOnNullSupplier() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.orElseGet(null));
    assertTrue(
        err.getMessage()
            .contains("Parameter fallbackSupplier in method Option.orElseGet was null"));
  }

  @Test
  void orElseGetThrowsOnNullSupplierResult() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.orElseGet(() -> null));
    assertTrue(err.getMessage().contains("The supplier passed to Option.orElseGet returned null"));
  }

  @Test
  void orElseThrowThrows() {
    var none = None.none();
    var err = assertThrowsExactly(NoSuchElementException.class, none::orElseThrow);
    assertEquals("This option has no value", err.getMessage());
  }

  @Test
  void orElseThrowSupplierThrows() {
    var none = None.none();
    var err =
        assertThrowsExactly(
            NoSuchElementException.class,
            () -> none.orElseThrow(() -> new NoSuchElementException("boom!")));
    assertEquals("boom!", err.getMessage());
  }

  @Test
  void orElseThrowsSupplierThrowsOnNullSupplier() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.orElseThrow(null));
    assertTrue(
        err.getMessage()
            .contains("Parameter exceptionSupplier in method Option.orElseThrow was null"));
  }

  @Test
  void orElseThrowsSupplierThrowsOnNullSupplierResult() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.orElseThrow(() -> null));
    assertEquals("The supplier passed to Option.orElseThrow returned null", err.getMessage());
  }

  @Test
  void orReturnsOther() {
    var none = None.<String>none();
    var other = Some.of("foo");
    assertSame(other, none.or(() -> other));
    assertSame(other, other.or(() -> none));
  }

  @Test
  void orThrowsWithNullSupplier() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.or(null));
    assertTrue(err.getMessage().contains("Parameter supplier in method Option.or was null"));
  }

  @Test
  void orThrowsWithNullSupplierResult() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.or(() -> null));
    assertTrue(err.getMessage().contains("The supplier passed to Option.or returned null"));
  }

  @Test
  void filterOnTrueReturnsNone() {
    var none = None.none();
    var result = none.filter(_ -> true);
    assertSame(none, result);
  }

  @Test
  void filterOnFalseReturnsNone() {
    var none = None.none();
    var result = none.filter(_ -> false);
    assertSame(none, result);
  }

  @Test
  void filterThrowsOnNullPredicate() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.filter(null));
    assertTrue(err.getMessage().contains("Parameter predicate in method Option.filter was null"));
  }

  @Test
  void mapReturnsNone() {
    var none = None.none();
    var result = none.map(x -> "foo");
    assertSame(none, result);
  }

  @Test
  void mapThrowsOnNullFunction() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.map(null));
    assertTrue(err.getMessage().contains("Parameter mapper in method Option.map was null"));
  }

  @Test
  void mapDoesNotThrowOnNullFunctionResult() {
    var none = None.none();
    // mapper function should not be evaluated
    var result = assertDoesNotThrow(() -> none.map(x -> null));
    assertSame(none, result);
  }

  @Test
  void flatMapReturnsNone() {
    var none = None.none();
    assertSame(none, none.flatMap(_ -> Some.of("foo")));
  }

  @Test
  void flatMapThrowsOnNullFunction() {
    var none = None.none();
    var err = assertThrowsExactly(NullPointerException.class, () -> none.flatMap(null));
    assertTrue(err.getMessage().contains("Parameter mapper in method Option.flatMap was null"));
  }

  @Test
  void flatMapDoesNotThrowOnNullFunctionResult() {
    var none = None.none();
    var result = assertDoesNotThrow(() -> none.flatMap(x -> null));
    assertSame(none, result);
  }

  @Test
  void stream() {
    var none = None.none();
    var stream = none.stream();
    assertEquals(0, stream.count());
  }
}
