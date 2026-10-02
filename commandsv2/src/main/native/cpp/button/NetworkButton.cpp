// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/NetworkButton.hpp"

#include <memory>
#include <utility>

using namespace wpi::cmd;

NetworkButton::NetworkButton(wpi::nt::BooleanTopic topic)
    : NetworkButton(topic.Subscribe(false)) {}

NetworkButton::NetworkButton(wpi::nt::BooleanSubscriber sub)
    : Trigger(
          [sub = std::make_shared<wpi::nt::BooleanSubscriber>(std::move(sub))] {
            return sub->GetTopic().GetInstance().IsConnected() && sub->Get();
          }) {}

NetworkButton::NetworkButton(std::shared_ptr<wpi::nt::NetworkTable> table,
                             std::string_view field)
    : NetworkButton(table->GetBooleanTopic(field)) {}

NetworkButton::NetworkButton(std::string_view table, std::string_view field)
    : NetworkButton(wpi::nt::NetworkTableInstance::GetDefault(), table, field) {
}

NetworkButton::NetworkButton(wpi::nt::NetworkTableInstance inst,
                             std::string_view table, std::string_view field)
    : NetworkButton(inst.GetTable(table), field) {}
