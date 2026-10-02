// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;

class UsageReportingTest {
  private String m_resource;
  private String m_data;

  @AfterEach
  void resetReportUsageImpl() {
    UsageReporting.setReportUsageImpl(null);
  }

  @Test
  void defaultNoOp() {
    UsageReporting.reportUsage("Resource", "Data");
  }

  @Test
  void reportUsageCallsImpl() {
    UsageReporting.setReportUsageImpl(
        (resource, data) -> {
          m_resource = resource;
          m_data = data;
        });

    UsageReporting.reportUsage("Resource", "Data");

    assertEquals("Resource", m_resource);
    assertEquals("Data", m_data);
  }

  @Test
  void reportUsageInstanceFormatsResource() {
    UsageReporting.setReportUsageImpl(
        (resource, data) -> {
          m_resource = resource;
          m_data = data;
        });

    UsageReporting.reportUsage("Resource", 3, "Data");

    assertEquals("Resource[3]", m_resource);
    assertEquals("Data", m_data);
  }
}
