// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4521)
#endif

#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandGroupBase.h"
#include "frc2/command/PrintCommand.h"

namespace frc2 {
/**
 * Runs one of a selection of commands, either using a selector and a key to
 * command mapping, or a supplier that returns the command directly at runtime.
 * Does not actually schedule the selected command - rather, the command is run
 * through this command; this ensures that the command will behave as expected
 * if used as part of a CommandGroup.  Requires the requirements of all included
 * commands, again to ensure proper functioning when used in a CommandGroup.  If
 * this is undesired, consider using ScheduleCommand.
 *
 * <p>As this command contains multiple component commands within it, it is
 * technically a command group; the command instances that are passed to it
 * cannot be added to any other groups, or scheduled individually.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their
 * component commands.
 *
 * This class is provided by the NewCommands VendorDep
 */
template <typename Key>
class SelectCommand : public CommandHelper<CommandBase, SelectCommand<Key>> {
 public:
  /**
   * Creates a new selectcommand.
   *
   * @param commands the map of commands to choose from
   * @param selector the selector to determine which command to run
   */
  template <class... Types,
            typename = std::enable_if_t<std::conjunction_v<
                std::is_base_of<Command, std::remove_reference_t<Types>>...>>>
  explicit SelectCommand(std::function<Key()> selector,
                         std::pair<Key, Types>... commands)
      : m_selector{std::move(selector)} {
    std::vector<std::pair<Key, std::unique_ptr<Command>>> foo;

    ((void)foo.emplace_back(commands.first,
                            std::make_unique<std::remove_reference_t<Types>>(
                                std::move(commands.second))),
     ...);

    for (auto&& command : foo) {
      if (!CommandGroupBase::RequireUngrouped(*command.second)) {
        return;
      }
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
    for (auto&& command : commands) {
      if (!CommandGroupBase::RequireUngrouped(*command.second)) {
        return;
      }
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

  /**
   * Creates a new selectcommand.
   *
   * @param toRun a supplier providing the command to run
   * @deprecated Replace with {@link ProxyCommand},
   * composing multiple of them in a {@link ParallelRaceGroup} if needed.
   */
  WPI_DEPRECATED("Replace with ProxyCommand")
  explicit SelectCommand(std::function<Command*()> toRun)
      : m_toRun{std::move(toRun)} {}

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

 protected:
  std::unique_ptr<Command> TransferOwnership() && override {
    return std::make_unique<SelectCommand>(std::move(*this));
  }

 private:
  std::unordered_map<Key, std::unique_ptr<Command>> m_commands;
  std::function<Key()> m_selector;
  std::function<Command*()> m_toRun;
  Command* m_selectedCommand;
  bool m_runsWhenDisabled = true;
  Command::InterruptionBehavior m_interruptBehavior{
      Command::InterruptionBehavior::kCancelIncoming};
};

template <typename T>
void SelectCommand<T>::Initialize() {
  if (m_selector) {
    auto find = m_commands.find(m_selector());
    if (find == m_commands.end()) {
      m_selectedCommand = new PrintCommand(
          "SelectCommand selector value does not correspond to any command!");
      return;
    }
    m_selectedCommand = find->second.get();
  } else {
    m_selectedCommand = m_toRun();
  }
  m_selectedCommand->Initialize();
}

}  // namespace frc2

#ifdef _WIN32
#pragma warning(pop)
#endif
