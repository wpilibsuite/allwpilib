// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <ImNodeFlow.h>
#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/filterdesigner/graph/Serialize.hpp"
#include "wpi/filterdesigner/nodes/TimePlotNode.hpp"
#include "wpi/filterdesigner/nodes/WpiLogSourceNode.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {

using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;
using wpi::filterdesigner::TimePlotNode;
using wpi::filterdesigner::TimeRange;
using wpi::filterdesigner::WpiLogSourceNode;

/**
 * Writes a two-burst 100 Hz log to a temp file and hands back the path. Two
 * bursts because a window is only interesting when there is something outside
 * it to leave out.
 */
class WpiLogSourceNodeSerializeTest {
 public:
  WpiLogSourceNodeSerializeTest() {
    wpi::log::DoubleLogEntry entry{log, "/burst", 0};
    // First sample at 10 ms: a DataLog append with timestamp 0 means "now".
    int64_t t = 10'000'000;
    double v = 0.0;
    for (int burst = 0; burst < 2; ++burst) {
      for (int i = 0; i < 20; ++i) {
        entry.Append(v, t);
        v += 1.0;
        t += 10'000'000;
      }
      t += 5'000'000'000 - 10'000'000;
    }
    log.Flush();

    path = std::filesystem::temp_directory_path() /
           ("filterdesigner_serialize_test_" +
            std::to_string(
                std::chrono::steady_clock::now().time_since_epoch().count()) +
            ".wpilog");
    std::ofstream out{path, std::ios::binary};
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
  }

  ~WpiLogSourceNodeSerializeTest() { std::filesystem::remove(path); }

  wpi::util::Logger msglog;
  std::vector<uint8_t> data;
  wpi::log::DataLogWriter log{
      msglog, std::make_unique<wpi::util::raw_uvector_ostream>(data)};
  std::filesystem::path path;
};

void RegisterAll(NodeRegistry& reg) {
  WpiLogSourceNode::Register(reg);
  TimePlotNode::Register(reg);
}

TEST_CASE_METHOD(WpiLogSourceNodeSerializeTest,
                 "WpiLogSourceNodeSerializeTest SelectedWindowRoundTrips",
                 "[filterdesigner]") {
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto node = graph.AddNode<WpiLogSourceNode>(ImVec2{10.0f, 20.0f});
  REQUIRE(node->Logic().OpenFile(path.string()));
  REQUIRE(node->Logic().SelectEntry("/burst"));
  REQUIRE(node->Logic().SelectLongestSegment());
  const TimeRange window = node->Logic().SelectedRange();
  const int id = node->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<WpiLogSourceNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().SelectedEntry() == "/burst");
  CHECK_NEAR(loaded->Logic().SelectedRange().start, window.start, 1e-9);
  CHECK_NEAR(loaded->Logic().SelectedRange().end, window.end, 1e-9);
  UNSCOPED_INFO("and the reopened graph publishes that window, not the record");
  REQUIRE(loaded->Logic().Signal() != nullptr);
  CHECK(loaded->Logic().Signal()->values.size() == 20u);
  CHECK_DOUBLE_EQ(loaded->Logic().Signal()->quality.filled, 0.0);
}

TEST_CASE_METHOD(WpiLogSourceNodeSerializeTest,
                 "WpiLogSourceNodeSerializeTest FileWithoutARangeOpensWhole",
                 "[filterdesigner]") {
  // What a .fdsgn written before time ranges existed looks like.
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto node = graph.AddNode<WpiLogSourceNode>(ImVec2{0.0f, 0.0f});
  REQUIRE(node->Logic().OpenFile(path.string()));
  REQUIRE(node->Logic().SelectEntry("/burst"));
  const int id = node->GraphId();

  std::string json = SerializeGraph(graph);
  const auto cut = json.find("\"rangeStart\"");
  REQUIRE(cut != std::string::npos);
  json.replace(cut, std::string{"\"rangeStart\""}.size(), "\"unusedStart\"");

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<WpiLogSourceNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().SelectedRange() == loaded->Logic().FullRange());
}

TEST_CASE_METHOD(WpiLogSourceNodeSerializeTest,
                 "WpiLogSourceNodeSerializeTest UnusableRangeFallsBackToWhole",
                 "[filterdesigner]") {
  // The log was replaced with one covering a different stretch of time. A
  // window into the old record is meaningless against the new one, and must
  // not leave the node publishing nothing.
  NodeRegistry reg;
  RegisterAll(reg);
  Graph graph;
  auto node = graph.AddNode<WpiLogSourceNode>(ImVec2{0.0f, 0.0f});
  REQUIRE(node->Logic().OpenFile(path.string()));
  REQUIRE(node->Logic().SelectEntry("/burst"));
  const int id = node->GraphId();

  std::string json = SerializeGraph(graph);
  const auto cut = json.find("\"rangeStart\":");
  REQUIRE(cut != std::string::npos);
  const auto comma = json.find(',', cut);
  REQUIRE(comma != std::string::npos);
  json.replace(cut, comma - cut, "\"rangeStart\":900.0");

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  auto* loaded = dynamic_cast<WpiLogSourceNode*>(restored.FindNodeById(id));
  REQUIRE(loaded != nullptr);
  CHECK(loaded->Logic().SelectedRange() == loaded->Logic().FullRange());
  REQUIRE(loaded->Logic().Signal() != nullptr);
  CHECK(loaded->Logic().Signal()->values.size() > 0u);
}

}  // namespace
