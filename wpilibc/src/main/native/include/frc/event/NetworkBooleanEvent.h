// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>

#include "BooleanEvent.h"

namespace nt {
class BooleanSubscriber;
class BooleanTopic;
class NetworkTable;
class NetworkTableInstance;
}  // namespace nt

namespace frc {
/**
 * A Button that uses a NetworkTable boolean field.
 *
 * This class is provided by the NewCommands VendorDep
 */
class NetworkBooleanEvent : public BooleanEvent {
 public:
  /**
   * Creates a new event with the given boolean topic determining whether it is
   * active.
   *
   * @param loop the loop that polls this event
   * @param topic The boolean topic that contains the value
   */
  NetworkBooleanEvent(EventLoop* loop, nt::BooleanTopic topic);

  /**
   * Creates a new event with the given boolean subscriber determining whether
   * it is active.
   *
   * @param loop the loop that polls this event
   * @param sub The boolean subscriber that provides the value
   */
  NetworkBooleanEvent(EventLoop* loop, nt::BooleanSubscriber sub);

  /**
   * Creates a new event with the given boolean topic determining whether it is
   * active.
   *
   * @param loop the loop that polls this event
   * @param table The NetworkTable that contains the topic
   * @param topicName The topic name within the table that contains the value
   */
  NetworkBooleanEvent(EventLoop* loop, std::shared_ptr<nt::NetworkTable> table,
                      std::string_view topicName);

  /**
   * Creates a new event with the given boolean topic determining whether it is
   * active.
   *
   * @param loop the loop that polls this event
   * @param tableName The NetworkTable name that contains the topic
   * @param topicName The topic name within the table that contains the value
   */
  NetworkBooleanEvent(EventLoop* loop, std::string_view tableName,
                      std::string_view topicName);

  /**
   * Creates a new event with the given boolean topic determining whether it is
   * active.
   *
   * @param loop the loop that polls this event
   * @param inst The NetworkTable instance to use
   * @param tableName The NetworkTable that contains the topic
   * @param topicName The topic name within the table that contains the value
   */
  NetworkBooleanEvent(EventLoop* loop, nt::NetworkTableInstance inst,
                      std::string_view tableName, std::string_view topicName);
};
}  // namespace frc
