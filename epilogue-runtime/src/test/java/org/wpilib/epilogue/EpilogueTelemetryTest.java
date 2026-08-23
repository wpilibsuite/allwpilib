// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.telemetry.MockTelemetryBackend;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.units.Units;

class EpilogueTelemetryTest {
  @Test
  void logsMeasureAsBaseUnitMagnitudeWithUnitMetadata() {
    MockTelemetryBackend backend = new MockTelemetryBackend();
    TelemetryTable table = new TelemetryTable(backend);

    EpilogueTelemetry.logMeasure(table, "distance", Units.Feet.of(1.0));

    var property = (MockTelemetryBackend.SetPropertyValue) backend.getActions().get(0).value();
    assertEquals("/distance", backend.getActions().get(0).path());
    assertEquals("unit", property.key());
    assertEquals("\"m\"", property.value());
    assertEquals(
        Units.Feet.of(1.0).baseUnitMagnitude(),
        backend.getLastValue("/distance", Double.class),
        1e-9);
  }

  @Test
  void unitMetadataIsJsonStringEscaped() {
    assertEquals(
        "\"quoted\\\"\\\\\\nunit\"", EpilogueTelemetry.getUnitMetadata("quoted\"\\\nunit"));
  }
}
