// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import edu.wpi.first.util.struct.Struct;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * A backend implementation that delegates to other backends. Helpful for simultaneous logging to
 * multiple data stores at once.
 */
public class MultiBackend implements EpilogueBackend {
  private final List<EpilogueBackend> m_backends;
  private final Map<String, NestedBackend> m_nestedBackends = new HashMap<>();

  // Use EpilogueBackend.multi(...) instead of instantiation directly
  MultiBackend(EpilogueBackend... backends) {
    this.m_backends = List.of(backends);
  }

  @Override
  public EpilogueBackend getNested(String path) {
    return m_nestedBackends.computeIfAbsent(path, k -> new NestedBackend(k, this));
  }

  @Override
  public void log(String identifier, int value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, long value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, float value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, double value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, boolean value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, byte[] value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, int[] value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, long[] value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, float[] value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, double[] value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, boolean[] value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, String value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, String[] value) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value);
    }
  }

  @Override
  public <S> void log(String identifier, S value, Struct<S> struct) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value, struct);
    }
  }

  @Override
  public <S> void log(String identifier, S[] value, Struct<S> struct) {
    for (EpilogueBackend backend : m_backends) {
      backend.log(identifier, value, struct);
    }
  }
}
