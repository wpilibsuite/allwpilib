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
class AddRequirementsRecorder {
 public:
  enum class Overload {
    None,
    Requirements,
    SmallSet,
    Subsystem,
  };

  void AddRequirements(Requirements) { called = Overload::Requirements; }
  void AddRequirements(wpi::util::SmallSet<Subsystem*, 4>) {
    called = Overload::SmallSet;
  }
  void AddRequirements(Subsystem*) { called = Overload::Subsystem; }

  Overload called = Overload::None;
};

TEST_CASE("AddRequirementsTest InitializerListOverloadResolution",
          "[commandsv2][command]") {
  TestSubsystem requirement;

  AddRequirementsRecorder overloadResolver;

  overloadResolver.AddRequirements({&requirement, &requirement});
  CHECK(overloadResolver.called ==
        AddRequirementsRecorder::Overload::Requirements);
}

TEST_CASE("AddRequirementsTest SpanOverloadResolution",
          "[commandsv2][command]") {
  std::span<Subsystem* const> requirementsSpan;

  AddRequirementsRecorder overloadResolver;

  overloadResolver.AddRequirements(requirementsSpan);
  CHECK(overloadResolver.called ==
        AddRequirementsRecorder::Overload::Requirements);
}

TEST_CASE("AddRequirementsTest SmallSetOverloadResolution",
          "[commandsv2][command]") {
  wpi::util::SmallSet<Subsystem*, 4> requirementsSet;

  AddRequirementsRecorder overloadResolver;

  overloadResolver.AddRequirements(requirementsSet);
  CHECK(overloadResolver.called == AddRequirementsRecorder::Overload::SmallSet);
}

TEST_CASE("AddRequirementsTest SubsystemOverloadResolution",
          "[commandsv2][command]") {
  TestSubsystem requirement;

  AddRequirementsRecorder overloadResolver;

  overloadResolver.AddRequirements(&requirement);
  CHECK(overloadResolver.called ==
        AddRequirementsRecorder::Overload::Subsystem);
}

TEST_CASE("AddRequirementsTest InitializerListSemantics",
          "[commandsv2][command]") {
  TestSubsystem requirement1;
  TestSubsystem requirement2;

  RunCommand command([] {});
  command.AddRequirements({&requirement1, &requirement2});
  CHECK(command.HasRequirement(&requirement1));
  CHECK(command.HasRequirement(&requirement2));
  CHECK(command.GetRequirements().size() == 2u);
}

TEST_CASE("AddRequirementsTest InitializerListDuplicatesSemantics",
          "[commandsv2][command]") {
  TestSubsystem requirement;

  RunCommand command([] {});
  command.AddRequirements({&requirement, &requirement});
  CHECK(command.HasRequirement(&requirement));
  CHECK(command.GetRequirements().size() == 1u);
}

TEST_CASE("AddRequirementsTest SpanSemantics", "[commandsv2][command]") {
  TestSubsystem requirement1;
  TestSubsystem requirement2;

  wpi::util::array<Subsystem* const, 2> requirementsArray(&requirement1,
                                                          &requirement2);

  RunCommand command([] {});
  command.AddRequirements(std::span{requirementsArray});
  CHECK(command.HasRequirement(&requirement1));
  CHECK(command.HasRequirement(&requirement2));
  CHECK(command.GetRequirements().size() == 2u);
}

TEST_CASE("AddRequirementsTest SpanDuplicatesSemantics",
          "[commandsv2][command]") {
  TestSubsystem requirement;

  wpi::util::array<Subsystem* const, 2> requirementsArray(&requirement,
                                                          &requirement);

  RunCommand command([] {});
  command.AddRequirements(std::span{requirementsArray});
  CHECK(command.HasRequirement(&requirement));
  CHECK(command.GetRequirements().size() == 1u);
}

TEST_CASE("AddRequirementsTest SmallSetSemantics", "[commandsv2][command]") {
  TestSubsystem requirement1;
  TestSubsystem requirement2;

  wpi::util::SmallSet<Subsystem*, 4> requirementsSet;
  requirementsSet.insert(&requirement1);
  requirementsSet.insert(&requirement2);

  RunCommand command([] {});
  command.AddRequirements(requirementsSet);
  CHECK(command.HasRequirement(&requirement1));
  CHECK(command.HasRequirement(&requirement2));
  CHECK(command.GetRequirements().size() == 2u);
}

TEST_CASE("AddRequirementsTest SubsystemPointerSemantics",
          "[commandsv2][command]") {
  TestSubsystem requirement1;
  TestSubsystem requirement2;

  RunCommand command([] {});
  command.AddRequirements(&requirement1);
  command.AddRequirements(&requirement2);
  CHECK(command.HasRequirement(&requirement1));
  CHECK(command.HasRequirement(&requirement2));
  CHECK(command.GetRequirements().size() == 2u);
}

TEST_CASE("AddRequirementsTest SubsystemPointerDuplicatesSemantics",
          "[commandsv2][command]") {
  TestSubsystem requirement;

  RunCommand command([] {});
  command.AddRequirements(&requirement);
  command.AddRequirements(&requirement);
  CHECK(command.HasRequirement(&requirement));
  CHECK(command.GetRequirements().size() == 1u);
}
