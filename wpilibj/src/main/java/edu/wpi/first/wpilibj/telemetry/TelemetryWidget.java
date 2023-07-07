package edu.wpi.first.wpilibj.telemetry;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import java.util.function.Function;

public interface TelemetryWidget extends TelemetryMetadata {
  static TelemetryWidget New(String name, Function<ObjectNode, ObjectNode> propertyBuilder) {
    return new TelemetryWidget() {
      @Override
      public JsonNode toJson() {
        return propertyBuilder.apply(JsonNodeFactory.instance.objectNode().put("widget", name));
      }
    };
  }
}
