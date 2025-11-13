// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/RunCommand.hpp"
#include "wpi/util/array.hpp"

using namespace wpi::cmd;

// Class to verify the overload resolution of Command::AddRequirements. This
// does not derive from Command because AddRequirements is non-virtual,
// preventing overriding anyways.
class MockAddRequirements {
 public:
  MOCK_METHOD(void, AddRequirements, (Requirements), ());
  MOCK_METHOD(void, AddRequirements, ((wpi::util::SmallSet<Subsystem*, 4>)),
              ());
  MOCK_METHOD(void, AddRequirements, (Subsystem*), ());
};

TEST(AddRequirementsTest, InitializerListOverloadResolution) {
  TestSubsystem requirement;

  MockAddRequirements overloadResolver;

  EXPECT_CALL(overloadResolver, AddRequirements(testing::An<Requirements>()));

  overloadResolver.AddRequirements({&requirement, &requirement});
}

TEST(AddRequirementsTest, SpanOverloadResolution) {
  std::span<Subsystem* const> requirementsSpan;

  MockAddRequirements overloadResolver;

  EXPECT_CALL(overloadResolver, AddRequirements(testing::An<Requirements>()));

  overloadResolver.AddRequirements(requirementsSpan);
}

TEST(AddRequirementsTest, SmallSetOverloadResolution) {
  wpi::util::SmallSet<Subsystem*, 4> requirementsSet;

  MockAddRequirements overloadResolver;

  EXPECT_CALL(
      overloadResolver,
      AddRequirements(testing::An<wpi::util::SmallSet<Subsystem*, 4>>()));

  overloadResolver.AddRequirements(requirementsSet);
}

TEST(AddRequirementsTest, SubsystemOverloadResolution) {
  TestSubsystem requirement;

  MockAddRequirements overloadResolver;

  EXPECT_CALL(overloadResolver, AddRequirements(testing::An<Subsystem*>()));

  overloadResolver.AddRequirements(&requirement);
}

TEST(AddRequirementsTest, InitializerListSemantics) {
  TestSubsystem requirement1;
  TestSubsystem requirement2;

  RunCommand command([] {});
  command.AddRequirements({&requirement1, &requirement2});
  EXPECT_TRUE(command.HasRequirement(&requirement1));
  EXPECT_TRUE(command.HasRequirement(&requirement2));
  EXPECT_EQ(command.GetRequirements().size(), 2u);
}

TEST(AddRequirementsTest, InitializerListDuplicatesSemantics) {
  TestSubsystem requirement;

  RunCommand command([] {});
  command.AddRequirements({&requirement, &requirement});
  EXPECT_TRUE(command.HasRequirement(&requirement));
  EXPECT_EQ(command.GetRequirements().size(), 1u);
}

TEST(AddRequirementsTest, SpanSemantics) {
  TestSubsystem requirement1;
  TestSubsystem requirement2;

  wpi::util::array<Subsystem* const, 2> requirementsArray(&requirement1,
                                                          &requirement2);

  RunCommand command([] {});
  command.AddRequirements(std::span{requirementsArray});
  EXPECT_TRUE(command.HasRequirement(&requirement1));
  EXPECT_TRUE(command.HasRequirement(&requirement2));
  EXPECT_EQ(command.GetRequirements().size(), 2u);
}

TEST(AddRequirementsTest, SpanDuplicatesSemantics) {
  TestSubsystem requirement;

  wpi::util::array<Subsystem* const, 2> requirementsArray(&requirement,
                                                          &requirement);

  RunCommand command([] {});
  command.AddRequirements(std::span{requirementsArray});
  EXPECT_TRUE(command.HasRequirement(&requirement));
  EXPECT_EQ(command.GetRequirements().size(), 1u);
}

TEST(AddRequirementsTest, SmallSetSemantics) {
  TestSubsystem requirement1;
  TestSubsystem requirement2;

  wpi::util::SmallSet<Subsystem*, 4> requirementsSet;
  requirementsSet.insert(&requirement1);
  requirementsSet.insert(&requirement2);

  RunCommand command([] {});
  command.AddRequirements(requirementsSet);
  EXPECT_TRUE(command.HasRequirement(&requirement1));
  EXPECT_TRUE(command.HasRequirement(&requirement2));
  EXPECT_EQ(command.GetRequirements().size(), 2u);
}

TEST(AddRequirementsTest, SubsystemPointerSemantics) {
  TestSubsystem requirement1;
  TestSubsystem requirement2;

  RunCommand command([] {});
  command.AddRequirements(&requirement1);
  command.AddRequirements(&requirement2);
  EXPECT_TRUE(command.HasRequirement(&requirement1));
  EXPECT_TRUE(command.HasRequirement(&requirement2));
  EXPECT_EQ(command.GetRequirements().size(), 2u);
}

TEST(AddRequirementsTest, SubsystemPointerDuplicatesSemantics) {
  TestSubsystem requirement;

  RunCommand command([] {});
  command.AddRequirements(&requirement);
  command.AddRequirements(&requirement);
  EXPECT_TRUE(command.HasRequirement(&requirement));
  EXPECT_EQ(command.GetRequirements().size(), 1u);
}
