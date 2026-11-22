// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.internal;

import io.avaje.jsonb.Jsonb;
import java.util.HashMap;
import java.util.IdentityHashMap;
import java.util.Map;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.units.Measure;
import org.wpilib.units.Unit;

/** Utility functions for logging unit-bearing values to telemetry. */
public final class UnitTelemetry {
  private static final Jsonb s_json = Jsonb.instance();
  private static final Map<Unit, String> s_unitMetadata = new IdentityHashMap<>();
  private static final Map<String, String> s_symbolMetadata = new HashMap<>();

  private UnitTelemetry() {
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
      metadata = s_json.toJson(unit.symbol());
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
      metadata = s_json.toJson(unitSymbol);
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
  public static void log(TelemetryTable table, String name, Measure<?> value) {
    Unit baseUnit = value.unit().getBaseUnit();
    table.setProperty(name, "unit", getUnitMetadata(baseUnit));
    table.log(name, value.baseUnitMagnitude());
  }

  /**
   * Logs a double value with unit metadata.
   *
   * <p>This logs the value in terms of the given unit's base unit.
   *
   * @param table telemetry table
   * @param name entry name
   * @param value value in the given unit
   * @param unit value unit
   */
  public static void log(TelemetryTable table, String name, double value, Unit unit) {
    table.setProperty(name, "unit", getUnitMetadata(unit.getBaseUnit()));
    table.log(name, unit.toBaseUnits(value));
  }

  /**
   * Logs a double value with unit metadata.
   *
   * @param table telemetry table
   * @param name entry name
   * @param value value in the given unit
   * @param unitSymbol unit symbol
   */
  public static void log(TelemetryTable table, String name, double value, String unitSymbol) {
    table.setProperty(name, "unit", getUnitMetadata(unitSymbol));
    table.log(name, value);
  }

  /**
   * Logs an int value with unit metadata.
   *
   * <p>This logs the value in terms of the given unit's base unit.
   *
   * @param table telemetry table
   * @param name entry name
   * @param value value in the given unit
   * @param unit value unit
   */
  public static void log(TelemetryTable table, String name, int value, Unit unit) {
    log(table, name, (double) value, unit);
  }

  /**
   * Logs a long value with unit metadata.
   *
   * <p>This logs the value in terms of the given unit's base unit.
   *
   * @param table telemetry table
   * @param name entry name
   * @param value value in the given unit
   * @param unit value unit
   */
  public static void log(TelemetryTable table, String name, long value, Unit unit) {
    log(table, name, (double) value, unit);
  }
}
