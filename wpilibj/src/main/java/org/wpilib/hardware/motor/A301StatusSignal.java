// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.motor;

import java.util.Objects;
import java.util.function.Function;

/** A value read from an A301 periodic status frame. */
public final class A301StatusSignal<T> {
  private final T m_value;
  private final int m_status;
  private final long m_timestamp;

  private A301StatusSignal(T value, int status, long timestamp) {
    m_value = value;
    m_status = status;
    m_timestamp = timestamp;
  }

  /**
   * Returns the most recently received value.
   *
   * <p>The value may be stale or may have been produced by a failed read. Call {@link #isValid()}
   * when freshness matters.
   *
   * @return the most recently received value
   */
  public T get() {
    return m_value;
  }

  /**
   * Returns the most recently received value when valid, or a default value otherwise.
   *
   * @param defaultValue the value to return when this signal is invalid
   * @return the received value or {@code defaultValue}
   */
  public T get(T defaultValue) {
    return isValid() ? m_value : defaultValue;
  }

  /**
   * Returns whether the value was read without an error and is recent enough for its configured
   * status-frame period.
   *
   * @return true when the value is valid
   */
  public boolean isValid() {
    return getError() == A301Error.kOk;
  }

  /**
   * Returns the REVLib-compatible error associated with the read.
   *
   * @return the signal error
   */
  public A301Error getError() {
    return A301Error.fromHalStatus(m_status);
  }

  /**
   * Returns the HAL status associated with the read.
   *
   * @return zero on success, or a HAL error code
   */
  public int getStatus() {
    return m_status;
  }

  /**
   * Returns the timestamp of the underlying CAN status frame in microseconds.
   *
   * @return the frame timestamp
   */
  public long getTimestamp() {
    return m_timestamp;
  }

  /**
   * Maps this signal's value while retaining its status and timestamp.
   *
   * @param mapper function that maps the current value
   * @param <R> mapped value type
   * @return a mapped signal with the same status and timestamp
   */
  public <R> A301StatusSignal<R> map(Function<? super T, ? extends R> mapper) {
    return new A301StatusSignal<>(
        Objects.requireNonNull(mapper, "mapper").apply(m_value), m_status, m_timestamp);
  }

  static <T> A301StatusSignal<T> of(T value, int status, long timestamp) {
    return new A301StatusSignal<>(value, status, timestamp);
  }
}
