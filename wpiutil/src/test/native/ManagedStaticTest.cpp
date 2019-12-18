/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/ManagedStatic.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"

static int refCount = 0;

struct StaticTestClass {
  StaticTestClass() { refCount++; }
  ~StaticTestClass() { refCount--; }

  void Func() {}
};

namespace wpi {
TEST(ManagedStaticTest, LazyDoesNotInitialize) {
  {
    refCount = 0;
    wpi::ManagedStatic<StaticTestClass> managedStatic;
    ASSERT_EQ(refCount, 0);
  }
  ASSERT_EQ(refCount, 0);
  wpi_shutdown();
}

TEST(ManagedStaticTest, LazyInitDoesntDestruct) {
  {
    refCount = 0;
    wpi::ManagedStatic<StaticTestClass> managedStatic;
    ASSERT_EQ(refCount, 0);
    managedStatic->Func();
    ASSERT_EQ(refCount, 1);
  }
  ASSERT_EQ(refCount, 1);
  wpi_shutdown();
  ASSERT_EQ(refCount, 0);
}

TEST(ManagedStaticTest, EagerInit) {
  {
    refCount = 0;
    StaticTestClass* test = new StaticTestClass{};
    ASSERT_EQ(refCount, 1);
    wpi::ManagedStatic<StaticTestClass> managedStatic(
        test, [](void* val) { delete static_cast<StaticTestClass*>(val); });
    ASSERT_EQ(refCount, 1);
    managedStatic->Func();
    ASSERT_EQ(refCount, 1);
  }
  ASSERT_EQ(refCount, 1);
  wpi_shutdown();
  ASSERT_EQ(refCount, 0);
}
}  // namespace wpi
