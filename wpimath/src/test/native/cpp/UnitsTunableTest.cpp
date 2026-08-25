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
#include "wpi/units/core.hpp"
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
  UnitsTunableTest() {
    wpi::tunables::TunableRegistry::Reset();
    TunableRegistry::RegisterBackend("", backend);
  }

  ~UnitsTunableTest() { TunableRegistry::Reset(); }

  std::shared_ptr<MockTunableBackend> backend =
      std::make_shared<MockTunableBackend>();
};

struct UnitMemberComplex : public ComplexTunable {
  wpi::units::meters<> distance{1.0};

  std::string_view GetTunableType() const override { return "UnitComplex"; }

  void PublishTunable(TunableTable& table) override {
    table.Publish("distance", this, &UnitMemberComplex::distance);
  }
};

}  // namespace

TEST_CASE_METHOD(UnitsTunableTest,
                 "UnitsTunableTest MemberRemoteTuneMarksDirty",
                 "[wpimath][tunable]") {
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

TEST_CASE_METHOD(UnitsTunableTest, "UnitsTunableTest PublishAndTune",
                 "[wpimath][tunable]") {
  wpi::tunables::Tunable<wpi::units::feet<>> distance{6_ft};
  wpi::tunables::Publish("distance", distance);
  auto distanceUid = backend->GetUid("/distance");
  REQUIRE(distanceUid);
  auto distanceInfo = wpi::tunables::TunableRegistry::GetTunable(*distanceUid);
  REQUIRE(distanceInfo.config);
  CHECK(distance.Get() == 6_ft);
  CHECK(distanceInfo.config->properties.at("unit") == "m");

  backend->SetDouble("/distance", 2.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(distance.Get() == 2_m);

  UnitMemberComplex complex;
  wpi::tunables::Publish("complex", complex);
  auto memberUid = backend->GetUid("/complex/distance");
  REQUIRE(memberUid);
  auto memberInfo = wpi::tunables::TunableRegistry::GetTunable(*memberUid);
  REQUIRE(memberInfo.config);
  CHECK(memberInfo.config->properties.at("unit") == "m");

  backend->SetDouble("/complex/distance", 3.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(complex.distance == 3_m);
}
