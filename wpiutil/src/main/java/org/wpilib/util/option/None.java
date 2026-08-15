// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.option;

import static java.util.Objects.requireNonNull;
import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.NoSuchElementException;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.function.Supplier;
import java.util.stream.Stream;

/**
 * An option type that does not contain a value.
 *
 * @param <T> The type of the optional value
 */
public record None<T>() implements Option<T> {
  // This type has no state and is immutable, so all instances are functionally identical.
  // Using a singleton cuts down on unnecessary object allocation.
  @SuppressWarnings("rawtypes")
  private static final None NONE = new None();

  @SuppressWarnings("unchecked")
  public static <T> None<T> none() {
    return (None<T>) NONE;
  }

  @Override
  public T orElse(T fallbackValue) {
    requireNonNullParam(fallbackValue, "fallbackValue", "Option.orElse");
    return fallbackValue;
  }

  @Override
  public T orElseGet(Supplier<? extends T> fallbackSupplier) {
    requireNonNullParam(fallbackSupplier, "fallbackSupplier", "Option.orElseGet");

    var fallbackValue = fallbackSupplier.get();
    requireNonNull(fallbackValue, "The supplier passed to Option.orElseGet returned null");
    return fallbackValue;
  }

  @Override
  public T orElseThrow() {
    throw new NoSuchElementException("This option has no value");
  }

  @Override
  public <X extends Throwable> T orElseThrow(Supplier<? extends X> exceptionSupplier) throws X {
    requireNonNullParam(exceptionSupplier, "exceptionSupplier", "Option.orElseThrow");

    var exception = exceptionSupplier.get();
    requireNonNull(exception, "The supplier passed to Option.orElseThrow returned null");
    throw exception;
  }

  @Override
  @SuppressWarnings("unchecked")
  public Option<T> or(Supplier<? extends Option<? extends T>> supplier) {
    requireNonNullParam(supplier, "supplier", "Option.or");

    var other = supplier.get();
    requireNonNull(other, "The supplier passed to Option.or returned null");
    return (Option<T>) other;
  }

  @Override
  public Option<T> filter(Predicate<? super T> predicate) {
    requireNonNullParam(predicate, "predicate", "Option.filter");

    return none();
  }

  @Override
  public <R> Option<R> map(Function<? super T, ? extends R> mapper) {
    requireNonNullParam(mapper, "mapper", "Option.map");

    return none();
  }

  @Override
  public <R> Option<R> flatMap(Function<? super T, ? extends Option<? extends R>> mapper) {
    requireNonNullParam(mapper, "mapper", "Option.flatMap");

    return none();
  }

  @Override
  public Stream<T> stream() {
    return Stream.empty();
  }
}
