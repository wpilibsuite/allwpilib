// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/event/NetworkBooleanEvent.hpp"

#include <memory>
#include <utility>

#include <wpi/ntcore/BooleanTopic.hpp>
#include <wpi/ntcore/NetworkTable.hpp>
#include <wpi/ntcore/NetworkTableInstance.hpp>

using namespace wpi;

NetworkBooleanEvent::NetworkBooleanEvent(EventLoop* loop,
                                         wpi::nt::BooleanTopic topic)
    : NetworkBooleanEvent{loop, topic.Subscribe(false)} {}

NetworkBooleanEvent::NetworkBooleanEvent(EventLoop* loop,
                                         wpi::nt::BooleanSubscriber sub)
    : BooleanEvent{
          loop,
          [sub = std::make_shared<wpi::nt::BooleanSubscriber>(std::move(sub))] {
            return sub->GetTopic().GetInstance().IsConnected() && sub->Get();
          }} {}

NetworkBooleanEvent::NetworkBooleanEvent(
    EventLoop* loop, std::shared_ptr<wpi::nt::NetworkTable> table,
    std::string_view topicName)
    : NetworkBooleanEvent{loop, table->GetBooleanTopic(topicName)} {}

NetworkBooleanEvent::NetworkBooleanEvent(EventLoop* loop,
                                         std::string_view tableName,
                                         std::string_view topicName)
    : NetworkBooleanEvent{loop, wpi::nt::NetworkTableInstance::GetDefault(),
                          tableName, topicName} {}

NetworkBooleanEvent::NetworkBooleanEvent(EventLoop* loop,
                                         wpi::nt::NetworkTableInstance inst,
                                         std::string_view tableName,
                                         std::string_view topicName)
    : NetworkBooleanEvent{loop, inst.GetTable(tableName), topicName} {}
