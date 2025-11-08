// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import java.util.HashMap;
import java.util.Map;
import us.hebi.quickbuf.ProtoMessage;

/**
 * A backend that logs to an underlying backend, prepending all logged data with a specific prefix.
 * Useful for logging nested data structures.
 */
public class NestedBackend implements EpilogueBackend {
  private final String m_prefix;
  private final EpilogueBackend m_impl;
  private final Map<String, NestedBackend> m_nestedBackends = new HashMap<>();

  // String concatenation can be expensive, especially for deeply nested hierarchies with many
  // logged fields. For example, logging a hypothetical Robot.elevator.io.getHeight() would result
  // in "/Robot/" + "elevator/" + "io/" + "getHeight"; three concatenations and string and byte
  // array allocations that need to be cleaned up by the GC. Caching the results means those
  // allocations only occur once, resulting in no GC (the strings are always referenced in the
  // cache), and minimal time costs (the String object caches its own hash code, so all we do is an
  // O(1) table lookup per concatenation)
  private final Map<String, String> m_prefixedIdentifiers = new HashMap<>();

  /**
   * Creates a new nested backed underneath another backend.
   *
   * @param prefix the prefix to append to all data logged in the nested backend
   * @param impl the backend to log to
   */
  public NestedBackend(String prefix, EpilogueBackend impl) {
    // Add a trailing slash if not already present
    if (prefix.endsWith("/")) {
      this.m_prefix = prefix;
    } else {
      this.m_prefix = prefix + "/";
    }
    this.m_impl = impl;
  }

  /**
   * Fast lookup to avoid redundant `m_prefix + identifier` concatenations. If the identifier has
   * not been seen before, we compute the concatenation and cache the result for later invocations
   * to read. This avoids redundantly recomputing the same concatenations every loop and
   * significantly cuts down on the CPU and memory overhead of the Epilogue library.
   *
   * @param identifier The identifier to prepend with {@link #m_prefix}.
   * @return The concatenated string.
   */
  private String withPrefix(String identifier) {
    // Using computeIfAbsent would result in a new lambda object allocation on every call
    if (m_prefixedIdentifiers.containsKey(identifier)) {
      return m_prefixedIdentifiers.get(identifier);
    }

    String result = m_prefix + identifier;
    m_prefixedIdentifiers.put(identifier, result);
    return result;
  }

  @Override
  public EpilogueBackend getNested(String path) {
    if (!m_nestedBackends.containsKey(path)) {
      var nested = new NestedBackend(path, this);
      m_nestedBackends.put(path, nested);
      return nested;
    }

    return m_nestedBackends.get(path);
  }

  @Override
  public void log(String identifier, int value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, long value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, float value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, double value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, boolean value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, byte[] value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, int[] value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, long[] value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, float[] value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, double[] value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, boolean[] value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, String value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public void log(String identifier, String[] value) {
    m_impl.log(withPrefix(identifier), value);
  }

  @Override
  public <S> void log(String identifier, S value, Struct<S> struct) {
    m_impl.log(withPrefix(identifier), value, struct);
  }

  @Override
  public <S> void log(String identifier, S[] value, Struct<S> struct) {
    m_impl.log(withPrefix(identifier), value, struct);
  }

  @Override
  public <P, M extends ProtoMessage<M>> void log(String identifier, P value, Protobuf<P, M> proto) {
    m_impl.log(m_prefix + identifier, value, proto);
  }
}
