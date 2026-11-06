// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

struct DependencyValue {
  int value;
};

struct FixtureValue {
  DependencyValue dependency;
};

/** Returns the dependency's value, retaining its type in DWARF. */
int ReadDependency(const DependencyValue* value);

/** Calls the transitive dependency from the published library. */
int ReadFixture(const FixtureValue* value);
