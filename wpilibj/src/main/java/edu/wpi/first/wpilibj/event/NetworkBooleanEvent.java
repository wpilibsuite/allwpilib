// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.networktables.BooleanSubscriber;
import edu.wpi.first.networktables.BooleanTopic;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;

/** This class provides an easy way to link NetworkTables boolean topics to callback actions. */
public class NetworkBooleanEvent extends BooleanEvent {
  /**
   * Creates a new event with the given boolean topic determining whether it is active.
   *
   * @param loop the loop that polls this event
   * @param topic The boolean topic that contains the value
   */
  public NetworkBooleanEvent(EventLoop loop, BooleanTopic topic) {
    this(loop, topic.subscribe(false));
  }

  /**
   * Creates a new event with the given boolean subscriber determining whether it is active.
   *
   * @param loop the loop that polls this event
   * @param sub The boolean subscriber that provides the value
   */
  public NetworkBooleanEvent(EventLoop loop, BooleanSubscriber sub) {
    super(loop, () -> sub.getTopic().getInstance().isConnected() && sub.get());
    requireNonNullParam(sub, "sub", "NetworkBooleanEvent");
  }

  /**
   * Creates a new event with the given boolean topic determining whether it is active.
   *
   * @param loop the loop that polls this event
   * @param table The NetworkTable that contains the topic
   * @param topicName The topic name within the table that contains the value
   */
  public NetworkBooleanEvent(EventLoop loop, NetworkTable table, String topicName) {
    this(loop, table.getBooleanTopic(topicName));
  }

  /**
   * Creates a new event with the given boolean topic determining whether it is active.
   *
   * @param loop the loop that polls this event
   * @param tableName The NetworkTable name that contains the topic
   * @param topicName The topic name within the table that contains the value
   */
  public NetworkBooleanEvent(EventLoop loop, String tableName, String topicName) {
    this(loop, NetworkTableInstance.getDefault(), tableName, topicName);
  }

  /**
   * Creates a new event with the given boolean topic determining whether it is active.
   *
   * @param loop the loop that polls this event
   * @param inst The NetworkTable instance to use
   * @param tableName The NetworkTable that contains the topic
   * @param topicName The topic name within the table that contains the value
   */
  public NetworkBooleanEvent(
      EventLoop loop, NetworkTableInstance inst, String tableName, String topicName) {
    this(loop, inst.getTable(tableName), topicName);
  }
}
