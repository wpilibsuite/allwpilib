// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/WpiLogSourceNodeLogic.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {

using wpi::filterdesigner::WpiLogSourceNodeLogic;

class WpiLogSourceNodeLogicTest : public ::testing::Test {
 public:
  wpi::util::Logger msglog;
  std::vector<uint8_t> data;
  wpi::log::DataLogWriter log{
      msglog, std::make_unique<wpi::util::raw_uvector_ostream>(data)};
};

TEST_F(WpiLogSourceNodeLogicTest, FreshLogicIsEmpty) {
  WpiLogSourceNodeLogic logic;
  EXPECT_FALSE(logic.HasFile());
  EXPECT_EQ(logic.Signal(), nullptr);
  EXPECT_TRUE(logic.LogPath().empty());
  EXPECT_TRUE(logic.SelectedEntry().empty());
  EXPECT_TRUE(logic.LoadError().empty());
}

TEST_F(WpiLogSourceNodeLogicTest, OpenBufferAndSelectEntryExposesSignal) {
  wpi::log::DoubleLogEntry d{log, "/accel/x", 0};
  d.Append(0.5, 1000);
  d.Append(0.75, 2000);
  d.Append(1.0, 3000);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  ASSERT_TRUE(logic.OpenBuffer(data));
  EXPECT_TRUE(logic.HasFile());
  EXPECT_EQ(logic.Signal(), nullptr) << "no entry selected yet";

  ASSERT_TRUE(logic.SelectEntry("/accel/x"));
  const auto* sig = logic.Signal();
  ASSERT_NE(sig, nullptr);
  EXPECT_EQ(sig->name, "/accel/x");
  ASSERT_EQ(sig->values.size(), 3u);
  EXPECT_DOUBLE_EQ(sig->values[0], 0.5);
  EXPECT_DOUBLE_EQ(sig->values[2], 1.0);
  EXPECT_GT(sig->revision, 0u);
}

TEST_F(WpiLogSourceNodeLogicTest,
       SelectMissingEntryKeepsPreviousAndReportsError) {
  wpi::log::DoubleLogEntry d{log, "good", 0};
  d.Append(1.0, 1);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  ASSERT_TRUE(logic.OpenBuffer(data));
  ASSERT_TRUE(logic.SelectEntry("good"));
  const auto* before = logic.Signal();
  ASSERT_NE(before, nullptr);

  EXPECT_FALSE(logic.SelectEntry("missing"));
  EXPECT_FALSE(logic.LoadError().empty());
  // Previous selection is preserved on a failed pick.
  EXPECT_EQ(logic.Signal(), before);
  EXPECT_EQ(logic.SelectedEntry(), "good");
}

TEST_F(WpiLogSourceNodeLogicTest, RestoreFromPathOpensFileAndPicksEntry) {
  wpi::log::DoubleLogEntry d{log, "persisted", 0};
  d.Append(2.0, 1);
  d.Append(4.0, 2);
  log.Flush();

  auto tmp = std::filesystem::temp_directory_path() /
             ("filterdesigner_node_test_" +
              std::to_string(
                  std::chrono::steady_clock::now().time_since_epoch().count()) +
              ".wpilog");
  {
    std::ofstream out{tmp, std::ios::binary};
    ASSERT_TRUE(out.is_open());
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
  }

  WpiLogSourceNodeLogic logic;
  logic.RestoreFromPath(tmp.string(), "persisted");
  EXPECT_TRUE(logic.HasFile());
  EXPECT_EQ(logic.SelectedEntry(), "persisted");
  ASSERT_NE(logic.Signal(), nullptr);
  EXPECT_EQ(logic.Signal()->values.size(), 2u);
  EXPECT_TRUE(logic.LoadError().empty());

  std::filesystem::remove(tmp);
}

TEST_F(WpiLogSourceNodeLogicTest, RestoreFromMissingPathLeavesErrorState) {
  WpiLogSourceNodeLogic logic;
  logic.RestoreFromPath("/no/such/file.wpilog", "entry");
  EXPECT_FALSE(logic.HasFile());
  EXPECT_EQ(logic.Signal(), nullptr);
  EXPECT_FALSE(logic.LoadError().empty())
      << "missing file should produce a re-pick banner, not throw";
  EXPECT_EQ(logic.LogPath(), "/no/such/file.wpilog")
      << "path is remembered so the UI can offer to re-pick";
}

TEST_F(WpiLogSourceNodeLogicTest,
       RestoreWithMissingEntryLoadsFileAndSurfacesError) {
  wpi::log::DoubleLogEntry d{log, "still_here", 0};
  d.Append(1.0, 1);
  log.Flush();

  auto tmp = std::filesystem::temp_directory_path() /
             ("filterdesigner_node_test_" +
              std::to_string(
                  std::chrono::steady_clock::now().time_since_epoch().count()) +
              "_b.wpilog");
  {
    std::ofstream out{tmp, std::ios::binary};
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
  }

  WpiLogSourceNodeLogic logic;
  logic.RestoreFromPath(tmp.string(), "gone");
  EXPECT_TRUE(logic.HasFile()) << "log still opens";
  EXPECT_EQ(logic.Signal(), nullptr) << "but the named entry is missing";
  EXPECT_FALSE(logic.LoadError().empty());

  std::filesystem::remove(tmp);
}

TEST_F(WpiLogSourceNodeLogicTest, RevisionAdvancesAcrossSelections) {
  wpi::log::DoubleLogEntry a{log, "a", 0};
  wpi::log::DoubleLogEntry b{log, "b", 0};
  a.Append(1.0, 1);
  b.Append(2.0, 2);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  ASSERT_TRUE(logic.OpenBuffer(data));

  ASSERT_TRUE(logic.SelectEntry("a"));
  auto firstRev = logic.Signal()->revision;
  ASSERT_TRUE(logic.SelectEntry("b"));
  EXPECT_GT(logic.Signal()->revision, firstRev);
}

}  // namespace
