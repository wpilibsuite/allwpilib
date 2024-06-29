// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4521)
#endif

#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <wpi/sendable/SendableBuilder.h>

#include "frc2/command/Command.h"
#include "frc2/command/PrintCommand.h"

namespace frc2 {
/**
 * A command composition that runs one of a selection of commands using a
 * selector and a key to command mapping.
 *
 * <p>The rules for command compositions apply: command instances that are
 * passed to it are owned by the composition and cannot be added to any other
 * composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 *
 * This class is provided by the NewCommands VendorDep
 */
template <typename Key>
class SelectCommand : public CommandHelper<Command, SelectCommand<Key>> {
 public:
  /**
   * Creates a new SelectCommand.
   *
   * @param commands the map of commands to choose from
   * @param selector the selector to determine which command to run
   */
  template <std::derived_from<Command>... Commands>
  explicit SelectCommand(std::function<Key()> selector,
                         std::pair<Key, Commands>... commands)
      : m_selector{std::move(selector)} {
    std::vector<std::pair<Key, std::unique_ptr<Command>>> foo;

    ((void)foo.emplace_back(
         commands.first,
         std::make_unique<std::decay_t<Commands>>(std::move(commands.second))),
     ...);

    m_defaultCommand.SetComposed(true);
    for (auto&& command : foo) {
      CommandScheduler::GetInstance().RequireUngroupedAndUnscheduled(
          command.second.get());
      command.second.get()->SetComposed(true);
    }

    for (auto&& command : foo) {
      this->AddRequirements(command.second->GetRequirements());
      m_runsWhenDisabled &= command.second->RunsWhenDisabled();
      if (command.second->GetInterruptionBehavior() ==
          Command::InterruptionBehavior::kCancelSelf) {
        m_interruptBehavior = Command::InterruptionBehavior::kCancelSelf;
      }
      m_commands.emplace(std::move(command.first), std::move(command.second));
    }
  }

  SelectCommand(
      std::function<Key()> selector,
      std::vector<std::pair<Key, std::unique_ptr<Command>>>&& commands)
      : m_selector{std::move(selector)} {
    m_defaultCommand.SetComposed(true);
    for (auto&& command : commands) {
      CommandScheduler::GetInstance().RequireUngroupedAndUnscheduled(
          command.second.get());
      command.second.get()->SetComposed(true);
    }

    for (auto&& command : commands) {
      this->AddRequirements(command.second->GetRequirements());
      m_runsWhenDisabled &= command.second->RunsWhenDisabled();
      if (command.second->GetInterruptionBehavior() ==
          Command::InterruptionBehavior::kCancelSelf) {
        m_interruptBehavior = Command::InterruptionBehavior::kCancelSelf;
      }
      m_commands.emplace(std::move(command.first), std::move(command.second));
    }
  }

  // No copy constructors for command groups
  SelectCommand(const SelectCommand& other) = delete;

  // Prevent template expansion from emulating copy ctor
  SelectCommand(SelectCommand&) = delete;

  SelectCommand(SelectCommand&& other) = default;

  void Initialize() override;

  void Execute() override { m_selectedCommand->Execute(); }

  void End(bool interrupted) override {
    return m_selectedCommand->End(interrupted);
  }

  bool IsFinished() override { return m_selectedCommand->IsFinished(); }

  bool RunsWhenDisabled() const override { return m_runsWhenDisabled; }

  Command::InterruptionBehavior GetInterruptionBehavior() const override {
    return m_interruptBehavior;
  }

  void InitSendable(wpi::SendableBuilder& builder) override {
    Command::InitSendable(builder);

    builder.AddStringProperty(
        "selected",
        [this] {
          if (m_selectedCommand) {
            return m_selectedCommand->GetName();
          } else {
            return std::string{"null"};
          }
        },
        nullptr);
  }

 private:
  std::unordered_map<Key, std::unique_ptr<Command>> m_commands;
  std::function<Key()> m_selector;
  Command* m_selectedCommand;
  bool m_runsWhenDisabled = true;
  Command::InterruptionBehavior m_interruptBehavior{
      Command::InterruptionBehavior::kCancelIncoming};

  PrintCommand m_defaultCommand{
      "SelectCommand selector value does not correspond to any command!"};
};

template <typename T>
void SelectCommand<T>::Initialize() {
  auto find = m_commands.find(m_selector());
  if (find == m_commands.end()) {
    m_selectedCommand = &m_defaultCommand;
  } else {
    m_selectedCommand = find->second.get();
  }
  m_selectedCommand->Initialize();
}

}  // namespace frc2

#ifdef _WIN32
#pragma warning(pop)
#endif
