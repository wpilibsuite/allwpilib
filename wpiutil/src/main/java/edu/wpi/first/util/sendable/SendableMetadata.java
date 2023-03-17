// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable;

import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;

public interface SendableMetadata {
  default ObjectNode getJson() {
    var json = JsonNodeFactory.instance.objectNode();
    apply(json);
    return json;
  }

  void apply(ObjectNode root);
}
