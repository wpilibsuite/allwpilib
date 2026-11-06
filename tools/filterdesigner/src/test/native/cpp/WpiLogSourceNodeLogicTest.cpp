// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/WpiLogSourceNodeLogic.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "NameTreeSupport.hpp"
#include "TestAssertions.hpp"
#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/filterdesigner/nodes/NameTree.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {

using wpi::filterdesigner::NameTreeNode;
using wpi::filterdesigner::TimeRange;
using wpi::filterdesigner::WpiLogSourceNodeLogic;

/**
 * Writes a 100 Hz double entry made of @p counts samples per burst separated
 * by @p gaps seconds — the shape of a real match log, where a topic publishes
 * while the robot is enabled and goes quiet in between.
 */
void AppendBursts(wpi::log::DoubleLogEntry& entry, std::vector<int> counts,
                  std::vector<double> gaps) {
  // First sample at 10 ms: a DataLog append with timestamp 0 means "now" and
  // would stamp the record with the wall clock.
  int64_t t = 10'000'000;
  double v = 0.0;
  for (std::size_t b = 0; b < counts.size(); ++b) {
    if (b > 0) {
      t += static_cast<int64_t>(gaps[b - 1] * 1e9);
    }
    for (int i = 0; i < counts[b]; ++i) {
      entry.Append(v, t);
      v += 1.0;
      t += 10'000'000;  // 10 ms
    }
    t -= 10'000'000;
  }
}

class WpiLogSourceNodeLogicTest {
 public:
  wpi::util::Logger msglog;
  std::vector<uint8_t> data;
  wpi::log::DataLogWriter log{
      msglog, std::make_unique<wpi::util::raw_uvector_ostream>(data)};
};

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest FreshLogicIsEmpty",
                 "[filterdesigner]") {
  WpiLogSourceNodeLogic logic;
  CHECK_FALSE(logic.HasFile());
  CHECK(logic.Signal() == nullptr);
  CHECK(logic.LogPath().empty());
  CHECK(logic.SelectedEntry().empty());
  CHECK(logic.LoadError().empty());
}

TEST_CASE_METHOD(
    WpiLogSourceNodeLogicTest,
    "WpiLogSourceNodeLogicTest OpenBufferAndSelectEntryExposesSignal",
    "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/accel/x", 0};
  d.Append(0.5, 1'000'000);
  d.Append(0.75, 2'000'000);
  d.Append(1.0, 3'000'000);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  CHECK(logic.HasFile());
  UNSCOPED_INFO("no entry selected yet");
  CHECK(logic.Signal() == nullptr);

  REQUIRE(logic.SelectEntry("/accel/x"));
  const auto* sig = logic.Signal();
  REQUIRE(sig != nullptr);
  CHECK(sig->name == "/accel/x");
  REQUIRE(sig->values.size() == 3u);
  CHECK_DOUBLE_EQ(sig->values[0], 0.5);
  CHECK_DOUBLE_EQ(sig->values[2], 1.0);
  CHECK(sig->revision > 0u);
}

TEST_CASE_METHOD(
    WpiLogSourceNodeLogicTest,
    "WpiLogSourceNodeLogicTest SelectMissingEntryKeepsPreviousAndReportsError",
    "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "good", 0};
  d.Append(1.0, 1'000);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("good"));
  const auto* before = logic.Signal();
  REQUIRE(before != nullptr);

  CHECK_FALSE(logic.SelectEntry("missing"));
  CHECK_FALSE(logic.LoadError().empty());
  // Previous selection is preserved on a failed pick.
  CHECK(logic.Signal() == before);
  CHECK(logic.SelectedEntry() == "good");
}

TEST_CASE_METHOD(
    WpiLogSourceNodeLogicTest,
    "WpiLogSourceNodeLogicTest RestoreFromPathOpensFileAndPicksEntry",
    "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "persisted", 0};
  d.Append(2.0, 1'000);
  d.Append(4.0, 2'000);
  log.Flush();

  auto tmp = std::filesystem::temp_directory_path() /
             ("filterdesigner_node_test_" +
              std::to_string(
                  std::chrono::steady_clock::now().time_since_epoch().count()) +
              ".wpilog");
  {
    std::ofstream out{tmp, std::ios::binary};
    REQUIRE(out.is_open());
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
  }

  WpiLogSourceNodeLogic logic;
  logic.RestoreFromPath(tmp.string(), "persisted");
  CHECK(logic.HasFile());
  CHECK(logic.SelectedEntry() == "persisted");
  REQUIRE(logic.Signal() != nullptr);
  CHECK(logic.Signal()->values.size() == 2u);
  CHECK(logic.LoadError().empty());

  std::filesystem::remove(tmp);
}

TEST_CASE_METHOD(
    WpiLogSourceNodeLogicTest,
    "WpiLogSourceNodeLogicTest RestoreFromMissingPathLeavesErrorState",
    "[filterdesigner]") {
  WpiLogSourceNodeLogic logic;
  logic.RestoreFromPath("/no/such/file.wpilog", "entry");
  CHECK_FALSE(logic.HasFile());
  CHECK(logic.Signal() == nullptr);
  UNSCOPED_INFO("missing file should produce a re-pick banner, not throw");
  CHECK_FALSE(logic.LoadError().empty());
  UNSCOPED_INFO("path is remembered so the UI can offer to re-pick");
  CHECK(logic.LogPath() == "/no/such/file.wpilog");
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest "
                 "RestoreWithMissingEntryLoadsFileAndSurfacesError",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "still_here", 0};
  d.Append(1.0, 1'000);
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
  UNSCOPED_INFO("log still opens");
  CHECK(logic.HasFile());
  UNSCOPED_INFO("but the named entry is missing");
  CHECK(logic.Signal() == nullptr);
  CHECK_FALSE(logic.LoadError().empty());

  std::filesystem::remove(tmp);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest RevisionAdvancesAcrossSelections",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry a{log, "a", 0};
  wpi::log::DoubleLogEntry b{log, "b", 0};
  a.Append(1.0, 1'000);
  b.Append(2.0, 2'000);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));

  REQUIRE(logic.SelectEntry("a"));
  auto firstRev = logic.Signal()->revision;
  REQUIRE(logic.SelectEntry("b"));
  CHECK(logic.Signal()->revision > firstRev);
}

// --- Time-range selection -------------------------------------------------

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest SelectEntryPublishesTheWholeRecord",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20, 20}, {5.0});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));

  CHECK_NEAR(logic.FullRange().start, 0.01, 1e-6);
  CHECK_NEAR(logic.FullRange().end, 5.39, 1e-6);
  UNSCOPED_INFO("narrowing drops most of the samples, so it is not automatic");
  CHECK(logic.SelectedRange() == logic.FullRange());
  // Which means the published signal is still mostly interpolated across the
  // pause — the state the sampling readout is there to shout about.
  REQUIRE(logic.Signal() != nullptr);
  CHECK(logic.Signal()->quality.filled > 0.9);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest RawSignalKeepsTheSamplesAsLogged",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20, 20}, {5.0});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));

  const auto* raw = logic.RawSignal();
  REQUIRE(raw != nullptr);
  UNSCOPED_INFO("40 samples as logged, not the 540 slots the grid spans");
  CHECK(raw->timestamps.size() == 40u);
  CHECK_FALSE(raw->quality.onGrid);
  CHECK(logic.Signal()->timestamps.size() > raw->timestamps.size());
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest SegmentsFollowTheLoggingPauses",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20, 5, 20}, {5.0, 2.0});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));

  auto segments = logic.Segments();
  REQUIRE(segments.size() == 3u);
  CHECK(segments[0].Count() == 20u);
  CHECK(segments[1].Count() == 5u);
  CHECK(segments[2].Count() == 20u);
}

TEST_CASE_METHOD(
    WpiLogSourceNodeLogicTest,
    "WpiLogSourceNodeLogicTest SelectLongestSegmentPublishesRealSamples",
    "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {10, 40, 10}, {5.0, 5.0});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));
  REQUIRE(logic.SelectLongestSegment());

  const auto* sig = logic.Signal();
  REQUIRE(sig != nullptr);
  CHECK(sig->values.size() == 40u);
  UNSCOPED_INFO("the window has its own grid, inferred from what is in it");
  CHECK(sig->quality.onGrid);
  CHECK_DOUBLE_EQ(sig->quality.filled, 0.0);
  CHECK_NEAR(sig->sampleRate, 100.0, 1e-6);
  // Middle burst: 10 samples plus a 5 s pause ahead of it.
  CHECK_NEAR(logic.SelectedRange().start, 5.10, 1e-6);
}

TEST_CASE_METHOD(
    WpiLogSourceNodeLogicTest,
    "WpiLogSourceNodeLogicTest SelectLongestSegmentNeedsAnInferableRate",
    "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/lonely", 0};
  d.Append(1.0, 1'000'000);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/lonely"));
  UNSCOPED_INFO("one sample infers no period, so there is nothing to segment");
  CHECK(logic.Segments().empty());
  CHECK_FALSE(logic.SelectLongestSegment());
  UNSCOPED_INFO("and the sample is still published");
  REQUIRE(logic.Signal() != nullptr);
  CHECK(logic.Signal()->values.size() == 1u);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest SelectRangeClampsToTheRecord",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20}, {});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));

  REQUIRE(logic.SelectRange(TimeRange{-100.0, 100.0}));
  CHECK(logic.SelectedRange() == logic.FullRange());
  CHECK(logic.Signal()->values.size() == 20u);
}

TEST_CASE_METHOD(
    WpiLogSourceNodeLogicTest,
    "WpiLogSourceNodeLogicTest SelectRangeRejectsWhatSelectsNothing",
    "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20}, {});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));
  REQUIRE(logic.SelectRange(TimeRange{0.05, 0.10}));
  const TimeRange kept = logic.SelectedRange();

  UNSCOPED_INFO("inverted");
  CHECK_FALSE(logic.SelectRange(TimeRange{0.15, 0.05}));
  UNSCOPED_INFO("entirely past the end of the record");
  CHECK_FALSE(logic.SelectRange(TimeRange{50.0, 60.0}));
  CHECK(logic.SelectedRange() == kept);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest SelectRangeWithNoEntryFails",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20}, {});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  CHECK_FALSE(logic.SelectRange(TimeRange{0.0, 0.1}));
  CHECK(logic.Signal() == nullptr);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest SelectFullRangeWidensBackOut",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20, 20}, {5.0});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));
  REQUIRE(logic.SelectLongestSegment());
  REQUIRE(logic.Signal()->values.size() == 20u);

  logic.SelectFullRange();
  CHECK(logic.SelectedRange() == logic.FullRange());
  CHECK(logic.Signal()->quality.filled > 0.9);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest EveryWindowIsCutFromTheRawSamples",
                 "[filterdesigner]") {
  // The trap ResampleToGrid's doc block warns about: windowing a previous
  // window would re-measure that window's interpolant as if it were data, and
  // a widened selection could never recover samples a narrower one excluded.
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20, 20}, {5.0});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));
  auto segments = logic.Segments();
  REQUIRE(segments.size() == 2u);

  REQUIRE(logic.SelectRange(segments[0].Range()));
  REQUIRE(logic.Signal()->values.size() == 20u);
  CHECK_DOUBLE_EQ(logic.Signal()->quality.filled, 0.0);

  UNSCOPED_INFO("moving off the first window must find the second burst");
  REQUIRE(logic.SelectRange(segments[1].Range()));
  REQUIRE(logic.Signal()->values.size() == 20u);
  // Values count up across the whole entry, so these name the samples the
  // second window caught.
  CHECK_DOUBLE_EQ(logic.Signal()->values.front(), 20.0);
  CHECK_DOUBLE_EQ(logic.Signal()->quality.filled, 0.0);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest EachWindowChangeBumpsTheRevision",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20, 20}, {5.0});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));
  // Downstream caches key on this, so a drag committed per frame would churn
  // every one of them; the node commits on mouse release for that reason.
  const auto onLoad = logic.Signal()->revision;
  REQUIRE(logic.SelectLongestSegment());
  const auto onNarrow = logic.Signal()->revision;
  CHECK(onNarrow > onLoad);
  logic.SelectFullRange();
  CHECK(logic.Signal()->revision > onNarrow);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest ClearSelectionDropsTheWindow",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/burst", 0};
  AppendBursts(d, {20, 20}, {5.0});
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/burst"));
  REQUIRE(logic.SelectLongestSegment());

  logic.ClearSelection();
  CHECK(logic.Signal() == nullptr);
  CHECK(logic.RawSignal() == nullptr);
  CHECK(logic.Segments().empty());
  CHECK(logic.SelectedRange() == TimeRange{});
  UNSCOPED_INFO("the log stays open so the entry list still renders");
  CHECK(logic.HasFile());
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest PickingAnotherEntryResetsTheWindow",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry a{log, "/a", 0};
  wpi::log::DoubleLogEntry b{log, "/b", 0};
  AppendBursts(a, {20, 20}, {5.0});
  b.Append(1.0, 1'000'000);
  b.Append(2.0, 11'000'000);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE(logic.SelectEntry("/a"));
  REQUIRE(logic.SelectLongestSegment());

  REQUIRE(logic.SelectEntry("/b"));
  UNSCOPED_INFO("a window into one entry means nothing in another");
  CHECK(logic.SelectedRange() == logic.FullRange());
  CHECK(logic.Signal()->values.size() == 2u);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest EntryTreeIsEmptyWithNoLog",
                 "[filterdesigner]") {
  WpiLogSourceNodeLogic logic;
  CHECK(logic.EntryTree().children.empty());
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest EntryTreeFilesEveryEntryByPath",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry velocity{log, "NT:/Spindexer/VelocityRPM", 0};
  wpi::log::DoubleLogEntry current{log, "NT:/Spindexer/Current", 0};
  wpi::log::StringLogEntry state{log, "NT:/Spindexer/State", 0};
  velocity.Append(1.0, 1'000'000);
  current.Append(2.0, 1'000'000);
  state.Append("idle", 1'000'000);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));

  const NameTreeNode* nt = FindChild(logic.EntryTree(), "NT");
  REQUIRE(nt != nullptr);
  const NameTreeNode* spindexer = FindChild(*nt, "Spindexer");
  REQUIRE(spindexer != nullptr);
  REQUIRE(spindexer->children.size() == 3);

  const NameTreeNode* velocityNode = FindChild(*spindexer, "VelocityRPM");
  REQUIRE(velocityNode != nullptr);
  CHECK(velocityNode->fullPath == "NT:/Spindexer/VelocityRPM");
  CHECK(velocityNode->type == "double");
  CHECK(velocityNode->selectable);

  UNSCOPED_INFO(
      "non-numeric entries stay listed, greyed, as the flat combo "
      "showed them");
  const NameTreeNode* stateNode = FindChild(*spindexer, "State");
  REQUIRE(stateNode != nullptr);
  CHECK_FALSE(stateNode->selectable);
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest EntryTreeIsDroppedOnReset",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/accel/x", 0};
  d.Append(1.0, 1'000'000);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE_FALSE(logic.EntryTree().children.empty());

  logic.Reset();
  CHECK(logic.EntryTree().children.empty());
}

TEST_CASE_METHOD(WpiLogSourceNodeLogicTest,
                 "WpiLogSourceNodeLogicTest EntryTreeIsDroppedWhenAnOpenFails",
                 "[filterdesigner]") {
  wpi::log::DoubleLogEntry d{log, "/accel/x", 0};
  d.Append(1.0, 1'000'000);
  log.Flush();

  WpiLogSourceNodeLogic logic;
  REQUIRE(logic.OpenBuffer(data));
  REQUIRE_FALSE(logic.EntryTree().children.empty());

  UNSCOPED_INFO(
      "a picker still showing the previous log's entries would "
      "offer selections that cannot load");
  CHECK_FALSE(logic.OpenFile("/nonexistent/does-not-exist.wpilog"));
  CHECK(logic.EntryTree().children.empty());
}

}  // namespace
