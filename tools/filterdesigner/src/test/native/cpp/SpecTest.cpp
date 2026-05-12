// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/Spec.hpp"

#include <filesystem>
#include <span>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace {

using wpi::filterdesigner::Family;
using wpi::filterdesigner::KindUsesFamily;
using wpi::filterdesigner::kSpecVersion;
using wpi::filterdesigner::LoadSpecFromFile;
using wpi::filterdesigner::ParseSpec;
using wpi::filterdesigner::SaveSpecToFile;
using wpi::filterdesigner::SerializeSpec;
using wpi::filterdesigner::Stage;
using wpi::filterdesigner::StageKind;

void ExpectStagesEqual(const Stage& got, const Stage& expected) {
  EXPECT_EQ(got.kind, expected.kind);
  // Family is irrelevant for kinds that don't use it (Notch, MovingAverage),
  // since the parser doesn't read it for those.
  if (KindUsesFamily(expected.kind)) {
    EXPECT_EQ(got.family, expected.family);
  }
  EXPECT_EQ(got.order, expected.order);
  EXPECT_EQ(got.taps, expected.taps);
  EXPECT_DOUBLE_EQ(got.f1, expected.f1);
  EXPECT_DOUBLE_EQ(got.f2, expected.f2);
  EXPECT_DOUBLE_EQ(got.q, expected.q);
  EXPECT_DOUBLE_EQ(got.passbandRippleDb, expected.passbandRippleDb);
  EXPECT_DOUBLE_EQ(got.stopbandAttenDb, expected.stopbandAttenDb);
}

// Builds one stage of every kind+family combo the UI can produce, so the
// round-trip test below covers every code path in SerializeSpec and ParseSpec.
std::vector<Stage> BuildSampleStages() {
  std::vector<Stage> input;

  Stage butterLP;
  butterLP.kind = StageKind::LowPass;
  butterLP.family = Family::Butterworth;
  butterLP.order = 4;
  butterLP.f1 = 50.0;
  input.push_back(butterLP);

  Stage cheby1HP;
  cheby1HP.kind = StageKind::HighPass;
  cheby1HP.family = Family::Chebyshev1;
  cheby1HP.order = 3;
  cheby1HP.f1 = 100.0;
  cheby1HP.passbandRippleDb = 0.5;
  input.push_back(cheby1HP);

  Stage cheby2BP;
  cheby2BP.kind = StageKind::BandPass;
  cheby2BP.family = Family::Chebyshev2;
  cheby2BP.order = 4;
  cheby2BP.f1 = 80.0;
  cheby2BP.f2 = 120.0;
  cheby2BP.stopbandAttenDb = 60.0;
  input.push_back(cheby2BP);

  Stage ellipBS;
  ellipBS.kind = StageKind::BandStop;
  ellipBS.family = Family::Elliptic;
  ellipBS.order = 3;
  ellipBS.f1 = 80.0;
  ellipBS.f2 = 120.0;
  ellipBS.passbandRippleDb = 1.5;
  ellipBS.stopbandAttenDb = 50.0;
  input.push_back(ellipBS);

  Stage notch;
  notch.kind = StageKind::Notch;
  notch.f1 = 60.0;
  notch.q = 25.0;
  input.push_back(notch);

  Stage ma;
  ma.kind = StageKind::MovingAverage;
  ma.taps = 7;
  input.push_back(ma);

  return input;
}

TEST(SpecTest, RoundTripsAcrossEveryFamilyAndKind) {
  std::vector<Stage> input = BuildSampleStages();

  std::string text = SerializeSpec(2000.0, std::span{input});
  auto result = ParseSpec(text);
  ASSERT_TRUE(result.spec.has_value()) << result.error;
  EXPECT_DOUBLE_EQ(result.spec->sampleRate, 2000.0);
  ASSERT_EQ(result.spec->stages.size(), input.size());
  for (size_t i = 0; i < input.size(); ++i) {
    SCOPED_TRACE("stage " + std::to_string(i));
    ExpectStagesEqual(result.spec->stages[i], input[i]);
  }
}

TEST(SpecTest, MissingOptionalFieldsFallBackToStageDefaults) {
  // Notch with only "kind" — every other field should hit Stage defaults.
  std::string text = R"({
    "version": 1,
    "sampleRate": 1000.0,
    "stages": [{"kind": "notch"}]
  })";
  auto result = ParseSpec(text);
  ASSERT_TRUE(result.spec.has_value()) << result.error;
  ASSERT_EQ(result.spec->stages.size(), 1u);
  Stage defaults;
  EXPECT_DOUBLE_EQ(result.spec->stages[0].f1, defaults.f1);
  EXPECT_DOUBLE_EQ(result.spec->stages[0].q, defaults.q);
}

TEST(SpecTest, MissingVersionAssumesCurrent) {
  std::string text = R"({
    "sampleRate": 1000.0,
    "stages": [{"kind": "notch", "f1": 60.0, "q": 10.0}]
  })";
  auto result = ParseSpec(text);
  ASSERT_TRUE(result.spec.has_value()) << result.error;
  EXPECT_EQ(result.spec->stages.size(), 1u);
}

TEST(SpecTest, RejectsUnknownNewerVersion) {
  std::string text = R"({
    "version": 999,
    "sampleRate": 1000.0,
    "stages": []
  })";
  auto result = ParseSpec(text);
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_NE(result.error.find("999"), std::string::npos);
}

TEST(SpecTest, RejectsMalformedJson) {
  auto result = ParseSpec("{not json");
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_FALSE(result.error.empty());
}

TEST(SpecTest, RejectsMissingStagesArray) {
  std::string text = R"({"version": 1, "sampleRate": 1000.0})";
  auto result = ParseSpec(text);
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_NE(result.error.find("stages"), std::string::npos);
}

TEST(SpecTest, RejectsMissingSampleRate) {
  std::string text = R"({
    "version": 1,
    "stages": [{"kind": "notch", "f1": 60.0, "q": 10.0}]
  })";
  auto result = ParseSpec(text);
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_NE(result.error.find("sampleRate"), std::string::npos);
}

TEST(SpecTest, RejectsNonNumericSampleRate) {
  std::string text = R"({
    "version": 1,
    "sampleRate": "fast",
    "stages": []
  })";
  auto result = ParseSpec(text);
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_NE(result.error.find("sampleRate"), std::string::npos);
}

TEST(SpecTest, RejectsUnknownKind) {
  std::string text = R"({
    "sampleRate": 1000.0,
    "stages": [{"kind": "hyperpass"}]
  })";
  auto result = ParseSpec(text);
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_NE(result.error.find("hyperpass"), std::string::npos);
}

TEST(SpecTest, RejectsUnknownFamily) {
  std::string text = R"({
    "sampleRate": 1000.0,
    "stages": [{"kind": "lowpass", "family": "newtonian", "f1": 50.0}]
  })";
  auto result = ParseSpec(text);
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_NE(result.error.find("newtonian"), std::string::npos);
}

TEST(SpecTest, RejectsClassicalKindWithoutFamily) {
  // LP/HP/BP/BS need a family field to disambiguate Butterworth vs Chebyshev.
  std::string text = R"({
    "sampleRate": 1000.0,
    "stages": [{"kind": "lowpass", "order": 4, "f1": 50.0}]
  })";
  auto result = ParseSpec(text);
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_NE(result.error.find("family"), std::string::npos);
}

TEST(SpecTest, RejectsNonIntegerOrder) {
  std::string text = R"({
    "sampleRate": 1000.0,
    "stages": [{"kind": "lowpass", "family": "butter", "order": 4.7, "f1": 50.0}]
  })";
  auto result = ParseSpec(text);
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_NE(result.error.find("order"), std::string::npos);
}

TEST(SpecTest, AcceptsIntegerValuedDoubleForOrder) {
  // 4.0 round-trips as 4 in JSON output but readers may see it as a double;
  // that's allowed.
  std::string text = R"({
    "sampleRate": 1000.0,
    "stages": [{"kind": "lowpass", "family": "butter", "order": 4.0, "f1": 50.0}]
  })";
  auto result = ParseSpec(text);
  ASSERT_TRUE(result.spec.has_value()) << result.error;
  EXPECT_EQ(result.spec->stages[0].order, 4);
}

TEST(SpecTest, RejectsWrongTypedField) {
  std::string text = R"({
    "sampleRate": 1000.0,
    "stages": [{"kind": "lowpass", "family": "butter", "order": 4, "f1": "fifty"}]
  })";
  auto result = ParseSpec(text);
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_NE(result.error.find("f1"), std::string::npos);
}

TEST(SpecTest, IgnoresUnknownTopLevelAndStageFields) {
  std::string text = R"({
    "version": 1,
    "sampleRate": 1000.0,
    "untrackedTopField": 42,
    "stages": [
      {
        "kind": "lowpass",
        "family": "butter",
        "order": 4,
        "f1": 50.0,
        "futureField": "ignored"
      }
    ]
  })";
  auto result = ParseSpec(text);
  ASSERT_TRUE(result.spec.has_value()) << result.error;
  EXPECT_EQ(result.spec->stages.size(), 1u);
  EXPECT_EQ(result.spec->stages[0].order, 4);
}

TEST(SpecTest, EmitsCurrentVersionField) {
  std::vector<Stage> empty;
  std::string text = SerializeSpec(1000.0, std::span{empty});
  EXPECT_NE(text.find("\"version\""), std::string::npos);
  EXPECT_NE(text.find(std::to_string(kSpecVersion)), std::string::npos);
}

// File-IO sanity: round-trip through a tmp file and verify the on-disk path
// produces a spec equal to the in-memory one. Also exercises the failure
// branches that DoSave/DoOpen surface in the UI.

class SpecFileIOTest : public ::testing::Test {
 protected:
  void SetUp() override {
    const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
    m_path = std::filesystem::temp_directory_path() /
             (std::string{"filterdesigner_spectest_"} +
              info->test_suite_name() + "_" + info->name() + ".fdsgn");
    std::error_code ec;
    std::filesystem::remove(m_path, ec);
  }
  void TearDown() override {
    std::error_code ec;
    std::filesystem::remove(m_path, ec);
  }

  std::filesystem::path m_path;
};

TEST_F(SpecFileIOTest, SaveThenLoadRoundTrips) {
  std::vector<Stage> input = BuildSampleStages();
  std::string err = SaveSpecToFile(m_path.string(), 2000.0, std::span{input});
  ASSERT_TRUE(err.empty()) << err;
  ASSERT_TRUE(std::filesystem::exists(m_path));

  auto result = LoadSpecFromFile(m_path.string());
  ASSERT_TRUE(result.spec.has_value()) << result.error;
  EXPECT_DOUBLE_EQ(result.spec->sampleRate, 2000.0);
  ASSERT_EQ(result.spec->stages.size(), input.size());
  for (size_t i = 0; i < input.size(); ++i) {
    SCOPED_TRACE("stage " + std::to_string(i));
    ExpectStagesEqual(result.spec->stages[i], input[i]);
  }
}

TEST(SpecFileIOErrorsTest, LoadFromMissingFileSurfacesError) {
  auto result = LoadSpecFromFile(
      (std::filesystem::temp_directory_path() / "definitely_not_there.fdsgn")
          .string());
  EXPECT_FALSE(result.spec.has_value());
  EXPECT_FALSE(result.error.empty());
}

TEST(SpecFileIOErrorsTest, SaveToUnwritableDirectorySurfacesError) {
  std::vector<Stage> empty;
  std::string err =
      SaveSpecToFile("/this/path/should/not/exist/anywhere/spec.fdsgn", 1000.0,
                     std::span{empty});
  EXPECT_FALSE(err.empty());
}

}  // namespace
