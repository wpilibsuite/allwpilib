package edu.wpi.first.wpilibj.telemetry;

import com.fasterxml.jackson.databind.JsonNode;

public interface TelemetryMetadata {
  JsonNode toJson();
}
