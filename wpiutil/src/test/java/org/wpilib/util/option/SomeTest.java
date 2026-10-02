// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.option;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertThrowsExactly;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.Test;

class SomeTest {
  @Test
  void ofRejectsNulls() {
    var err = assertThrowsExactly(NullPointerException.class, () -> Some.of(null));
    assertTrue(err.getMessage().contains("Parameter value in method Some.of was null"));
  }

  @Test
  void constructorRejectsNulls() {
    var err = assertThrowsExactly(NullPointerException.class, () -> new Some<>(null));
    assertTrue(err.getMessage().contains("Parameter value in method Some was null"));
  }

  @Test
  void ofAcceptsNonNullValues() {
    var some = Some.of("value");
    assertEquals("value", some.value());
  }

  @Test
  void orElseReturnsValue() {
    var some = Some.of("foo");
    assertEquals("foo", some.orElse("bar"));
  }

  @Test
  void orElseThrowsOnNullFallback() {
    var some = Some.of("foo");
    var err = assertThrowsExactly(NullPointerException.class, () -> some.orElse(null));
    assertTrue(
        err.getMessage().contains("Parameter fallbackValue in method Option.orElse was null"));
  }

  @Test
  void orElseGet() {
    var some = Some.of("foo");
    assertEquals("foo", some.orElseGet(() -> "bar"));
  }

  @Test
  void orElseGetThrowsOnNullFallback() {
    var some = Some.of("foo");
    var err = assertThrowsExactly(NullPointerException.class, () -> some.orElseGet(null));
    assertTrue(
        err.getMessage()
            .contains("Parameter fallbackSupplier in method Option.orElseGet was null"));
  }

  @Test
  void orElseThrowDoesNotThrow() {
    var some = Some.of("foo");
    assertEquals("foo", some.orElseThrow());
  }

  @Test
  void orElseThrowSupplierDoesNotThrow() {
    var some = Some.of("foo");
    assertEquals("foo", some.orElseThrow(NullPointerException::new));
  }

  @Test
  void orElseThrowSupplierThrowsOnNullSupplier() {
    var some = Some.of("foo");
    var err = assertThrowsExactly(NullPointerException.class, () -> some.orElseThrow(null));
    assertTrue(
        err.getMessage()
            .contains("Parameter exceptionSupplier in method Option.orElseThrow was null"));
  }

  @Test
  void orReturnsSelf() {
    var some = Some.of("foo");
    var other = Some.of("bar");
    assertSame(some, some.or(() -> other));
    assertSame(other, other.or(() -> some));
  }

  @Test
  void filterOnTrueReturnsSelf() {
    var some = Some.of("foo");
    assertSame(some, some.filter(_ -> true));
  }

  @Test
  void filterOnFalseReturnsNone() {
    var some = Some.of("foo");
    assertSame(None.none(), some.filter(_ -> false));
  }

  @Test
  void filterThrowsOnNullPredicate() {
    var some = Some.of("foo");
    var err = assertThrowsExactly(NullPointerException.class, () -> some.filter(null));
    assertTrue(err.getMessage().contains("Parameter predicate in method Option.filter was null"));
  }

  @Test
  void mapReturningNullGivesNone() {
    var some = Some.of("foo");
    var result = some.map(_ -> null);
    assertSame(None.none(), result);
  }

  @Test
  void mapReturnsNewOption() {
    var some = Some.of("foo");
    var result = some.map(value -> value.repeat(2));
    assertEquals(Some.of("foofoo"), result);
  }

  @Test
  void mapThrowsOnNullFunction() {
    var some = Some.of("foo");
    var err = assertThrowsExactly(NullPointerException.class, () -> some.map(null));
    assertTrue(err.getMessage().contains("Parameter mapper in method Option.map was null"));
  }

  @Test
  void flatMap() {
    var some = Some.of("foo");
    var result = some.flatMap(x -> Some.of(x + "bar"));
    assertEquals(Some.of("foobar"), result);
  }

  @Test
  void flatMapThrowsOnNullFunction() {
    var some = Some.of("foo");
    var err = assertThrowsExactly(NullPointerException.class, () -> some.flatMap(null));
    assertTrue(err.getMessage().contains("Parameter mapper in method Option.flatMap was null"));
  }

  @Test
  void flatMapThrowsOnNullMapperOutput() {
    var some = Some.of("foo");
    var err = assertThrowsExactly(NullPointerException.class, () -> some.flatMap(_ -> null));
    assertTrue(
        err.getMessage().contains("The mapping function passed to Option.flatMap returned null"));
  }

  @Test
  void stream() {
    var some = Some.of("foo");
    var result = some.stream().toList();
    assertEquals(List.of("foo"), result);
  }

  @Test
  void value() {
    var some = Some.of("foo");
    assertEquals("foo", some.value());
  }
}
