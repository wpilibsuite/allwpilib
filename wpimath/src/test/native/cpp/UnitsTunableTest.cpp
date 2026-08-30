// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <memory>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/tunables/ComplexTunable.hpp"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableBackend.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/TunableTable.hpp"
#include "wpi/tunables/Tunables.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/tunable.hpp"

using namespace wpi;
using namespace wpi::tunables;

namespace {
class RecordingDirtyBackend : public TunableBackend {
 public:
  bool Publish(std::string_view, uint32_t, detail::TunableBase&,
               const TunableConfig*, detail::TunableTypeValue) override {
    return true;
  }

  void MarkDirty(uint32_t uid) override { dirtyUids.emplace_back(uid); }

  void Remove(std::string_view) override {}

  std::vector<PublishedTunable> RemovePrefix(std::string_view) override {
    return {};
  }

  void UnregisterTunable(uint32_t) override {}

  void Update() override {}

  std::vector<uint32_t> dirtyUids;
};

struct UnitsTunableTest {
  UnitsTunableTest() { TunableRegistry::RegisterBackend("", backend); }

  ~UnitsTunableTest() { TunableRegistry::Reset(); }

  std::shared_ptr<MockTunableBackend> backend =
      std::make_shared<MockTunableBackend>();
};

struct UnitMemberComplex : public ComplexTunable {
  wpi::units::meter_t distance{1.0};

  void PublishTunable(TunableTable& table) override {
    table.Publish("distance", this, &UnitMemberComplex::distance);
  }
};
}  // namespace

TEST_CASE_METHOD(UnitsTunableTest,
                 "UnitsTunableTest MemberRemoteTuneMarksDirty", "[tunable]") {
  auto recordingBackend = std::make_shared<RecordingDirtyBackend>();
  TunableRegistry::RegisterBackend("/recording", recordingBackend);

  UnitMemberComplex complex;
  Publish("unit", complex);
  auto uid = backend->GetUid("/unit/distance");
  REQUIRE(uid);

  backend->SetDouble("/unit/distance", 2.0);
  TunableRegistry::Update();

  CHECK(complex.distance.to<double>() == 2.0);
  CHECK(std::find(recordingBackend->dirtyUids.begin(),
                  recordingBackend->dirtyUids.end(),
                  *uid) != recordingBackend->dirtyUids.end());
}
