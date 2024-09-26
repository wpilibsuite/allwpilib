// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/event/NetworkBooleanEvent.h"

#include <memory>
#include <utility>

#include <networktables/BooleanTopic.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>

using namespace frc;

NetworkBooleanEvent::NetworkBooleanEvent(EventLoop* loop,
                                         nt::BooleanTopic topic)
    : NetworkBooleanEvent{loop, topic.Subscribe(false)} {}

NetworkBooleanEvent::NetworkBooleanEvent(EventLoop* loop,
                                         nt::BooleanSubscriber sub)
    : BooleanEvent{
          loop,
          [sub = std::make_shared<nt::BooleanSubscriber>(std::move(sub))] {
            return sub->GetTopic().GetInstance().IsConnected() && sub->Get();
          }} {}

NetworkBooleanEvent::NetworkBooleanEvent(
    EventLoop* loop, std::shared_ptr<nt::NetworkTable> table,
    std::string_view topicName)
    : NetworkBooleanEvent{loop, table->GetBooleanTopic(topicName)} {}

NetworkBooleanEvent::NetworkBooleanEvent(EventLoop* loop,
                                         std::string_view tableName,
                                         std::string_view topicName)
    : NetworkBooleanEvent{loop, nt::NetworkTableInstance::GetDefault(),
                          tableName, topicName} {}

NetworkBooleanEvent::NetworkBooleanEvent(EventLoop* loop,
                                         nt::NetworkTableInstance inst,
                                         std::string_view tableName,
                                         std::string_view topicName)
    : NetworkBooleanEvent{loop, inst.GetTable(tableName), topicName} {}
