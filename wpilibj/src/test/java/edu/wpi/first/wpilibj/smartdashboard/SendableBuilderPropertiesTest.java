// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.networktables.BooleanTopic;
import edu.wpi.first.networktables.DoubleTopic;
import edu.wpi.first.networktables.FloatTopic;
import edu.wpi.first.networktables.IntegerTopic;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringTopic;
import edu.wpi.first.networktables.Topic;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

@SuppressWarnings("PMD.RedundantFieldInitializer")
class SendableBuilderPropertiesTest {
  private NetworkTableInstance m_inst;

  @BeforeEach
  void beforeEach() {
    m_inst = NetworkTableInstance.create();
    SmartDashboard.setNetworkTableInstance(m_inst);
  }

  @AfterEach
  void afterEach() {
    SmartDashboard.setNetworkTableInstance(NetworkTableInstance.getDefault());
    m_inst.close();
  }

  @SuppressWarnings({"PMD.SimplifiableTestAssertion", "SimplifiableAssertion"})
  @Test
  void booleanTest() {
    var sendable =
        new Sendable() {
          boolean m_boolean = true;

          @Override
          public void initSendable(SendableBuilder builder) {
            builder
                .addBooleanProperty("boolean", () -> m_boolean, v -> m_boolean = v)
                .withMetadata(node -> node.put("unit", "meters"));
          }
        };

    NetworkTableEntry entry = SmartDashboard.getEntry("Test/boolean");
    Topic topic = new BooleanTopic(entry.getTopic());

    // Shouldn't exist yet.
    assertEquals(false, entry.getBoolean(false));
    assertEquals("null", topic.getProperty("unit"));

    {
      SmartDashboard.putData("Test", sendable);
      SmartDashboard.updateValues();

      // Should be published
      assertEquals(true, entry.getBoolean(false));
      assertEquals("\"meters\"", topic.getProperty("unit"));
    }
    {
      sendable.m_boolean = false;
      SmartDashboard.updateValues();

      // Check getter
      assertEquals(false, entry.getBoolean(true));
    }
    {
      entry.setBoolean(true);
      SmartDashboard.updateValues();

      assertEquals(true, entry.getBoolean(false));
    }
  }

  @Test
  void intTest() {
    var sendable =
        new Sendable() {
          long m_int = 0;

          @Override
          public void initSendable(SendableBuilder builder) {
            builder
                .addIntegerProperty("int", () -> m_int, v -> m_int = v)
                .withMetadata(node -> node.put("unit", "meters"));
          }
        };

    NetworkTableEntry entry = SmartDashboard.getEntry("Test/int");
    Topic topic = new IntegerTopic(entry.getTopic());

    // Shouldn't exist yet.
    assertEquals(-1, entry.getInteger(-1));
    assertEquals("null", topic.getProperty("unit"));

    {
      SmartDashboard.putData("Test", sendable);
      SmartDashboard.updateValues();

      // Should be published
      assertEquals(0, entry.getInteger(-1));
      assertEquals("\"meters\"", topic.getProperty("unit"));
    }
    {
      sendable.m_int = 5;
      SmartDashboard.updateValues();

      // Check getter
      assertEquals(5, entry.getInteger(-1));
    }
    {
      entry.setInteger(-5);
      SmartDashboard.updateValues();

      assertEquals(-5, entry.getInteger(0));
    }
  }

  @Test
  void floatTest() {
    var sendable =
        new Sendable() {
          float m_float = 0.0f;

          @Override
          public void initSendable(SendableBuilder builder) {
            builder
                .addFloatProperty("float", () -> m_float, v -> m_float = v)
                .withMetadata(node -> node.put("unit", "meters"));
          }
        };

    NetworkTableEntry entry = SmartDashboard.getEntry("Test/float");
    Topic topic = new FloatTopic(entry.getTopic());

    // Shouldn't exist yet.
    assertEquals(-1f, entry.getFloat(-1f));
    assertEquals("null", topic.getProperty("unit"));

    {
      SmartDashboard.putData("Test", sendable);
      SmartDashboard.updateValues();

      // Should be published
      assertEquals(0.0f, entry.getFloat(-1f));
      assertEquals("\"meters\"", topic.getProperty("unit"));
    }
    {
      sendable.m_float = 5.0f;
      SmartDashboard.updateValues();

      // Check getter
      assertEquals(5.0f, entry.getFloat(-1f));
    }
    {
      entry.setFloat(-5.0f);
      SmartDashboard.updateValues();

      assertEquals(-5.0f, entry.getFloat(0.0f));
    }
  }

  @Test
  void doubleTest() {
    var sendable =
        new Sendable() {
          double m_double = 0.0;

          @Override
          public void initSendable(SendableBuilder builder) {
            builder
                .addDoubleProperty("double", () -> m_double, v -> m_double = v)
                .withMetadata(node -> node.put("unit", "meters"));
          }
        };

    NetworkTableEntry entry = SmartDashboard.getEntry("Test/double");
    Topic topic = new DoubleTopic(entry.getTopic());

    // Shouldn't exist yet.
    assertEquals(-1.0, entry.getDouble(-1.0));
    assertEquals("null", topic.getProperty("unit"));

    {
      SmartDashboard.putData("Test", sendable);
      SmartDashboard.updateValues();

      // Should be published
      assertEquals(0.0, entry.getDouble(-1.0));
      assertEquals("\"meters\"", topic.getProperty("unit"));
    }
    {
      sendable.m_double = 5.0;
      SmartDashboard.updateValues();

      // Check getter
      assertEquals(5.0, entry.getDouble(-1.0));
    }
    {
      entry.setDouble(-5.0);
      SmartDashboard.updateValues();

      assertEquals(-5.0, entry.getDouble(0.0));
    }
  }

  @Test
  void stringTest() {
    var sendable =
        new Sendable() {
          String m_string = "field-init";

          @Override
          public void initSendable(SendableBuilder builder) {
            builder
                .addStringProperty("string", () -> m_string, v -> m_string = v)
                .withMetadata(node -> node.put("unit", "meters"));
          }
        };

    NetworkTableEntry entry = SmartDashboard.getEntry("Test/string");
    Topic topic = new StringTopic(entry.getTopic());

    // Shouldn't exist yet.
    assertEquals("-", entry.getString("-"));
    assertEquals("null", topic.getProperty("unit"));

    {
      SmartDashboard.putData("Test", sendable);
      SmartDashboard.updateValues();

      // Should be published
      assertEquals("field-init", entry.getString("-"));
      assertEquals("\"meters\"", topic.getProperty("unit"));
    }
    {
      sendable.m_string = "set-by-field";
      SmartDashboard.updateValues();

      // Check getter
      assertEquals("set-by-field", entry.getString("-"));
    }
    {
      entry.setString("set-by-entry");
      SmartDashboard.updateValues();

      assertEquals("set-by-entry", entry.getString("-"));
    }
  }
}
