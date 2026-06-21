// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "CommandTestBase.hpp"
#include "make_vector.hpp"
#include "wpi/commands2/Commands.hpp"

namespace wpi::cmd {

inline namespace single {
template <typename T>
void CheckSingleCompositionRunsWhenDisabled(bool param) {
  T command = T(cmd::Idle().IgnoringDisable(param).Unwrap());
  CHECK(command.RunsWhenDisabled() == param);
}

template <typename T>
void CheckSingleCompositionInterruptibility(
    Command::InterruptionBehavior param) {
  T command = T(cmd::Idle().WithInterruptBehavior(param).Unwrap());
  CHECK(command.GetInterruptionBehavior() == param);
}

#define INSTANTIATE_SINGLE_COMMAND_COMPOSITION_TEST_SUITE(Suite,           \
                                                          CompositionType) \
  TEST_CASE(#Suite " SingleCompositionInterruptibility CancelSelf",        \
            "[commandsv2][command][composition]") {                        \
    CommandTestBase testBase;                                              \
    CheckSingleCompositionInterruptibility<CompositionType>(               \
        Command::InterruptionBehavior::kCancelSelf);                       \
  }                                                                        \
  TEST_CASE(#Suite " SingleCompositionInterruptibility CancelIncoming",    \
            "[commandsv2][command][composition]") {                        \
    CommandTestBase testBase;                                              \
    CheckSingleCompositionInterruptibility<CompositionType>(               \
        Command::InterruptionBehavior::kCancelIncoming);                   \
  }                                                                        \
  TEST_CASE(#Suite " SingleCompositionRunsWhenDisabled True",              \
            "[commandsv2][command][composition]") {                        \
    CommandTestBase testBase;                                              \
    CheckSingleCompositionRunsWhenDisabled<CompositionType>(true);         \
  }                                                                        \
  TEST_CASE(#Suite " SingleCompositionRunsWhenDisabled False",             \
            "[commandsv2][command][composition]") {                        \
    CommandTestBase testBase;                                              \
    CheckSingleCompositionRunsWhenDisabled<CompositionType>(false);        \
  }
}  // namespace single

inline namespace multi {
template <typename T>
void CheckMultiCompositionRunsWhenDisabledOne(bool param) {
  T command = T(CommandPtr::UnwrapVector(
      cmd::impl::MakeVector(cmd::Idle().IgnoringDisable(param))));
  CHECK(command.RunsWhenDisabled() == param);
}

template <typename T>
void CheckMultiCompositionRunsWhenDisabledAll(bool param) {
  T command = T(CommandPtr::UnwrapVector(cmd::impl::MakeVector(
      cmd::Idle().IgnoringDisable(param), cmd::Idle().IgnoringDisable(param),
      cmd::Idle().IgnoringDisable(param))));
  CHECK(command.RunsWhenDisabled() == param);
}

template <typename T>
void CheckMultiCompositionRunsWhenDisabledMixed(bool first, bool second,
                                                bool third) {
  T command = T(CommandPtr::UnwrapVector(cmd::impl::MakeVector(
      cmd::Idle().IgnoringDisable(first), cmd::Idle().IgnoringDisable(second),
      cmd::Idle().IgnoringDisable(third))));
  CHECK_FALSE(command.RunsWhenDisabled());
}

template <typename T>
void CheckMultiCompositionInterruptibility(
    Command::InterruptionBehavior first, Command::InterruptionBehavior second,
    Command::InterruptionBehavior third,
    Command::InterruptionBehavior expected) {
  T command = T(CommandPtr::UnwrapVector(
      cmd::impl::MakeVector(cmd::Idle().WithInterruptBehavior(first),
                            cmd::Idle().WithInterruptBehavior(second),
                            cmd::Idle().WithInterruptBehavior(third))));
  CHECK(command.GetInterruptionBehavior() == expected);
}

#define INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(Suite,               \
                                                         CompositionType)     \
  TEST_CASE(#Suite " MultiCompositionInterruptibility AllCancelSelf",         \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionInterruptibility<CompositionType>(                   \
        Command::InterruptionBehavior::kCancelSelf,                           \
        Command::InterruptionBehavior::kCancelSelf,                           \
        Command::InterruptionBehavior::kCancelSelf,                           \
        Command::InterruptionBehavior::kCancelSelf);                          \
  }                                                                           \
  TEST_CASE(#Suite " MultiCompositionInterruptibility AllCancelIncoming",     \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionInterruptibility<CompositionType>(                   \
        Command::InterruptionBehavior::kCancelIncoming,                       \
        Command::InterruptionBehavior::kCancelIncoming,                       \
        Command::InterruptionBehavior::kCancelIncoming,                       \
        Command::InterruptionBehavior::kCancelIncoming);                      \
  }                                                                           \
  TEST_CASE(#Suite                                                            \
            " MultiCompositionInterruptibility TwoCancelSelfOneIncoming",     \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionInterruptibility<CompositionType>(                   \
        Command::InterruptionBehavior::kCancelSelf,                           \
        Command::InterruptionBehavior::kCancelSelf,                           \
        Command::InterruptionBehavior::kCancelIncoming,                       \
        Command::InterruptionBehavior::kCancelSelf);                          \
  }                                                                           \
  TEST_CASE(#Suite                                                            \
            " MultiCompositionInterruptibility TwoCancelIncomingOneSelf",     \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionInterruptibility<CompositionType>(                   \
        Command::InterruptionBehavior::kCancelIncoming,                       \
        Command::InterruptionBehavior::kCancelIncoming,                       \
        Command::InterruptionBehavior::kCancelSelf,                           \
        Command::InterruptionBehavior::kCancelSelf);                          \
  }                                                                           \
  TEST_CASE(#Suite " MultiCompositionRunsWhenDisabled OneTrue",               \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionRunsWhenDisabledOne<CompositionType>(true);          \
  }                                                                           \
  TEST_CASE(#Suite " MultiCompositionRunsWhenDisabled OneFalse",              \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionRunsWhenDisabledOne<CompositionType>(false);         \
  }                                                                           \
  TEST_CASE(#Suite " MultiCompositionRunsWhenDisabled AllTrue",               \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionRunsWhenDisabledAll<CompositionType>(true);          \
  }                                                                           \
  TEST_CASE(#Suite " MultiCompositionRunsWhenDisabled AllFalse",              \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionRunsWhenDisabledAll<CompositionType>(false);         \
  }                                                                           \
  TEST_CASE(#Suite " MultiCompositionRunsWhenDisabled TwoTrueOneFalse",       \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionRunsWhenDisabledMixed<CompositionType>(true, true,   \
                                                                false);       \
  }                                                                           \
  TEST_CASE(#Suite " MultiCompositionRunsWhenDisabled TwoFalseOneTrue",       \
            "[commandsv2][command][composition]") {                           \
    CommandTestBase testBase;                                                 \
    CheckMultiCompositionRunsWhenDisabledMixed<CompositionType>(false, false, \
                                                                true);        \
  }
}  // namespace multi
}  // namespace wpi::cmd
