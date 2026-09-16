// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue;

import java.util.HashMap;
import java.util.IdentityHashMap;
import java.util.Map;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.units.Measure;
import org.wpilib.units.Unit;

/** Utility functions for generated Epilogue telemetry loggers. */
public final class EpilogueTelemetry {
  private static final char[] HEX = "0123456789abcdef".toCharArray();
  private static final Map<Unit, String> s_unitMetadata = new IdentityHashMap<>();
  private static final Map<String, String> s_symbolMetadata = new HashMap<>();

  private EpilogueTelemetry() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Returns JSON-encoded unit metadata for the given unit.
   *
   * <p>The returned string is suitable for use as the {@code unit} telemetry property value.
   *
   * @param unit unit
   * @return JSON-encoded unit metadata
   */
  public static String getUnitMetadata(Unit unit) {
    synchronized (s_unitMetadata) {
      String metadata = s_unitMetadata.get(unit);
      if (metadata != null) {
        return metadata;
      }
      metadata = getUnitMetadata(unit.symbol());
      s_unitMetadata.put(unit, metadata);
      return metadata;
    }
  }

  /**
   * Returns JSON-encoded unit metadata for the given unit symbol.
   *
   * <p>The returned string is suitable for use as the {@code unit} telemetry property value.
   *
   * @param unitSymbol unit symbol
   * @return JSON-encoded unit metadata
   */
  public static String getUnitMetadata(String unitSymbol) {
    synchronized (s_symbolMetadata) {
      String metadata = s_symbolMetadata.get(unitSymbol);
      if (metadata != null) {
        return metadata;
      }
      metadata = jsonString(unitSymbol);
      s_symbolMetadata.put(unitSymbol, metadata);
      return metadata;
    }
  }

  /**
   * Logs a measure with unit metadata.
   *
   * <p>This logs the measure in terms of its base unit.
   *
   * @param table telemetry table
   * @param name entry name
   * @param value measure value
   */
  public static void logMeasure(TelemetryTable table, String name, Measure<?> value) {
    Unit baseUnit = value.unit().getBaseUnit();
    table.setProperty(name, "unit", getUnitMetadata(baseUnit));
    table.log(name, value.baseUnitMagnitude());
  }

  private static String jsonString(String value) {
    StringBuilder out = new StringBuilder(value.length() + 2);
    out.append('"');
    for (int i = 0; i < value.length(); i++) {
      char ch = value.charAt(i);
      switch (ch) {
        case '"' -> out.append("\\\"");
        case '\\' -> out.append("\\\\");
        case '\b' -> out.append("\\b");
        case '\f' -> out.append("\\f");
        case '\n' -> out.append("\\n");
        case '\r' -> out.append("\\r");
        case '\t' -> out.append("\\t");
        default -> {
          if (ch < 0x20) {
            out.append("\\u00").append(HEX[ch >>> 4]).append(HEX[ch & 0xf]);
          } else {
            out.append(ch);
          }
        }
      }
    }
    out.append('"');
    return out.toString();
  }
}
