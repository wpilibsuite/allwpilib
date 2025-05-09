// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <utility>

#include <gtest/gtest.h>

#include "CommandTestBase.h"
#include "frc2/command/Commands.h"
#include "make_vector.h"

namespace frc2 {

inline namespace single {
template <typename T>
class SingleCompositionRunsWhenDisabledTest : public CommandTestBase {};

TYPED_TEST_SUITE_P(SingleCompositionRunsWhenDisabledTest);

TYPED_TEST_P(SingleCompositionRunsWhenDisabledTest, True) {
  auto param = true;
  TypeParam command = TypeParam(cmd::Idle().IgnoringDisable(param).Unwrap());
  EXPECT_EQ(param, command.RunsWhenDisabled());
}

TYPED_TEST_P(SingleCompositionRunsWhenDisabledTest, False) {
  auto param = false;
  TypeParam command = TypeParam(cmd::Idle().IgnoringDisable(param).Unwrap());
  EXPECT_EQ(param, command.RunsWhenDisabled());
}

REGISTER_TYPED_TEST_SUITE_P(SingleCompositionRunsWhenDisabledTest, True, False);

template <typename T>
class SingleCompositionInterruptibilityTest : public CommandTestBase {};

TYPED_TEST_SUITE_P(SingleCompositionInterruptibilityTest);

TYPED_TEST_P(SingleCompositionInterruptibilityTest, CancelSelf) {
  auto param = Command::InterruptionBehavior::CANCEL_SELF;
  TypeParam command =
      TypeParam(cmd::Idle().WithInterruptBehavior(param).Unwrap());
  EXPECT_EQ(param, command.GetInterruptionBehavior());
}

TYPED_TEST_P(SingleCompositionInterruptibilityTest, CancelIncoming) {
  auto param = Command::InterruptionBehavior::CANCEL_INCOMING;
  TypeParam command =
      TypeParam(cmd::Idle().WithInterruptBehavior(param).Unwrap());
  EXPECT_EQ(param, command.GetInterruptionBehavior());
}

REGISTER_TYPED_TEST_SUITE_P(SingleCompositionInterruptibilityTest, CancelSelf,
                            CancelIncoming);

#define INSTANTIATE_SINGLE_COMMAND_COMPOSITION_TEST_SUITE(Suite,               \
                                                          CompositionType)     \
  INSTANTIATE_TYPED_TEST_SUITE_P(Suite, SingleCompositionInterruptibilityTest, \
                                 CompositionType);                             \
  INSTANTIATE_TYPED_TEST_SUITE_P(Suite, SingleCompositionRunsWhenDisabledTest, \
                                 CompositionType)
}  // namespace single

inline namespace multi {
template <typename T>
class MultiCompositionRunsWhenDisabledTest : public CommandTestBase {};

TYPED_TEST_SUITE_P(MultiCompositionRunsWhenDisabledTest);

TYPED_TEST_P(MultiCompositionRunsWhenDisabledTest, OneTrue) {
  auto param = true;
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(
      cmd::impl::MakeVector(cmd::Idle().IgnoringDisable(param))));
  EXPECT_EQ(param, command.RunsWhenDisabled());
}

TYPED_TEST_P(MultiCompositionRunsWhenDisabledTest, OneFalse) {
  auto param = false;
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(
      cmd::impl::MakeVector(cmd::Idle().IgnoringDisable(param))));
  EXPECT_EQ(param, command.RunsWhenDisabled());
}

TYPED_TEST_P(MultiCompositionRunsWhenDisabledTest, AllTrue) {
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(cmd::impl::MakeVector(
      cmd::Idle().IgnoringDisable(true), cmd::Idle().IgnoringDisable(true),
      cmd::Idle().IgnoringDisable(true))));
  EXPECT_EQ(true, command.RunsWhenDisabled());
}

TYPED_TEST_P(MultiCompositionRunsWhenDisabledTest, AllFalse) {
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(cmd::impl::MakeVector(
      cmd::Idle().IgnoringDisable(false), cmd::Idle().IgnoringDisable(false),
      cmd::Idle().IgnoringDisable(false))));
  EXPECT_EQ(false, command.RunsWhenDisabled());
}

TYPED_TEST_P(MultiCompositionRunsWhenDisabledTest, TwoTrueOneFalse) {
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(cmd::impl::MakeVector(
      cmd::Idle().IgnoringDisable(true), cmd::Idle().IgnoringDisable(true),
      cmd::Idle().IgnoringDisable(false))));
  EXPECT_EQ(false, command.RunsWhenDisabled());
}

TYPED_TEST_P(MultiCompositionRunsWhenDisabledTest, TwoFalseOneTrue) {
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(cmd::impl::MakeVector(
      cmd::Idle().IgnoringDisable(false), cmd::Idle().IgnoringDisable(false),
      cmd::Idle().IgnoringDisable(true))));
  EXPECT_EQ(false, command.RunsWhenDisabled());
}

REGISTER_TYPED_TEST_SUITE_P(MultiCompositionRunsWhenDisabledTest, OneTrue,
                            OneFalse, AllTrue, AllFalse, TwoTrueOneFalse,
                            TwoFalseOneTrue);

template <typename T>
class MultiCompositionInterruptibilityTest
    : public SingleCompositionInterruptibilityTest<T> {};

TYPED_TEST_SUITE_P(MultiCompositionInterruptibilityTest);

TYPED_TEST_P(MultiCompositionInterruptibilityTest, AllCancelSelf) {
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(
      cmd::impl::MakeVector(cmd::Idle().WithInterruptBehavior(
                                Command::InterruptionBehavior::CANCEL_SELF),
                            cmd::Idle().WithInterruptBehavior(
                                Command::InterruptionBehavior::CANCEL_SELF),
                            cmd::Idle().WithInterruptBehavior(
                                Command::InterruptionBehavior::CANCEL_SELF))));
  EXPECT_EQ(Command::InterruptionBehavior::CANCEL_SELF,
            command.GetInterruptionBehavior());
}

TYPED_TEST_P(MultiCompositionInterruptibilityTest, AllCancelIncoming) {
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(cmd::impl::MakeVector(
      cmd::Idle().WithInterruptBehavior(
          Command::InterruptionBehavior::CANCEL_INCOMING),
      cmd::Idle().WithInterruptBehavior(
          Command::InterruptionBehavior::CANCEL_INCOMING),
      cmd::Idle().WithInterruptBehavior(
          Command::InterruptionBehavior::CANCEL_INCOMING))));
  EXPECT_EQ(Command::InterruptionBehavior::CANCEL_INCOMING,
            command.GetInterruptionBehavior());
}

TYPED_TEST_P(MultiCompositionInterruptibilityTest, TwoCancelSelfOneIncoming) {
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(cmd::impl::MakeVector(
      cmd::Idle().WithInterruptBehavior(
          Command::InterruptionBehavior::CANCEL_SELF),
      cmd::Idle().WithInterruptBehavior(
          Command::InterruptionBehavior::CANCEL_SELF),
      cmd::Idle().WithInterruptBehavior(
          Command::InterruptionBehavior::CANCEL_INCOMING))));
  EXPECT_EQ(Command::InterruptionBehavior::CANCEL_SELF,
            command.GetInterruptionBehavior());
}

TYPED_TEST_P(MultiCompositionInterruptibilityTest, TwoCancelIncomingOneSelf) {
  TypeParam command = TypeParam(CommandPtr::UnwrapVector(
      cmd::impl::MakeVector(cmd::Idle().WithInterruptBehavior(
                                Command::InterruptionBehavior::CANCEL_INCOMING),
                            cmd::Idle().WithInterruptBehavior(
                                Command::InterruptionBehavior::CANCEL_INCOMING),
                            cmd::Idle().WithInterruptBehavior(
                                Command::InterruptionBehavior::CANCEL_SELF))));
  EXPECT_EQ(Command::InterruptionBehavior::CANCEL_SELF,
            command.GetInterruptionBehavior());
}

REGISTER_TYPED_TEST_SUITE_P(MultiCompositionInterruptibilityTest, AllCancelSelf,
                            AllCancelIncoming, TwoCancelSelfOneIncoming,
                            TwoCancelIncomingOneSelf);

#define INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(Suite,               \
                                                         CompositionType)     \
  INSTANTIATE_TYPED_TEST_SUITE_P(Suite, MultiCompositionInterruptibilityTest, \
                                 CompositionType);                            \
  INSTANTIATE_TYPED_TEST_SUITE_P(Suite, MultiCompositionRunsWhenDisabledTest, \
                                 CompositionType)
}  // namespace multi
}  // namespace frc2
