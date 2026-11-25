// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/Serialize.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ImNodeFlow.h>
#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/util/json.hpp"

namespace {

using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::FilterDesignerNode;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::LoadGraphFromFile;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SaveGraphToFile;
using wpi::filterdesigner::SerializeGraph;

// One source, one sink, both int-typed: the shape of a real pair without the
// ImGui dependencies.

class FakeSourceNode : public FilterDesignerNode {
 public:
  FakeSourceNode() {
    setTitle("Fake Source");
    addOUT<int>("out")->behaviour([this] { return m_value; });
  }
  std::string_view TypeTag() const override { return "FakeSource"; }

  void SerializeParams(wpi::util::json& obj) const override {
    obj["value"] = m_value;
  }
  void DeserializeParams(const wpi::util::json& obj) override {
    if (const auto* v = obj.lookup("value"); v && v->is_number()) {
      m_value = static_cast<int>(v->get_number());
    }
  }

  int m_value = 7;
};

class FakeSinkNode : public FilterDesignerNode {
 public:
  FakeSinkNode() {
    setTitle("Fake Sink");
    addIN<int>("in", 0, ImFlow::ConnectionFilter::SameType());
  }
  std::string_view TypeTag() const override { return "FakeSink"; }
};

// Same shape as FakeSourceNode with an output of another type, for the pin
// filter to reject. Registered per-test rather than in RegisterFakes so the
// fake registry keeps exactly one source everywhere else.
class FakeDoubleSourceNode : public FilterDesignerNode {
 public:
  FakeDoubleSourceNode() {
    setTitle("Fake Double Source");
    addOUT<double>("out")->behaviour([] { return 1.0; });
  }
  std::string_view TypeTag() const override { return "FakeDoubleSource"; }
};

void RegisterDoubleSource(NodeRegistry& reg) {
  NodeRegistry::Entry src;
  src.tag = "FakeDoubleSource";
  src.menuLabel = "Fake Double Source";
  src.menuCategory = "Sources";
  src.outputTypes.emplace_back(typeid(double));
  src.factory = [](Graph& g, const ImVec2& pos) {
    return std::static_pointer_cast<FilterDesignerNode>(
        g.AddNode<FakeDoubleSourceNode>(pos));
  };
  reg.Register(std::move(src));
}

void RegisterFakes(NodeRegistry& reg) {
  NodeRegistry::Entry src;
  src.tag = "FakeSource";
  src.menuLabel = "Fake Source";
  src.menuCategory = "Sources";
  src.outputTypes.emplace_back(typeid(int));
  src.factory = [](Graph& g, const ImVec2& pos) {
    return std::static_pointer_cast<FilterDesignerNode>(
        g.AddNode<FakeSourceNode>(pos));
  };
  reg.Register(std::move(src));

  NodeRegistry::Entry sink;
  sink.tag = "FakeSink";
  sink.menuLabel = "Fake Sink";
  sink.menuCategory = "Sinks";
  sink.inputTypes.emplace_back(typeid(int));
  sink.factory = [](Graph& g, const ImVec2& pos) {
    return std::static_pointer_cast<FilterDesignerNode>(
        g.AddNode<FakeSinkNode>(pos));
  };
  reg.Register(std::move(sink));
}

TEST_CASE("SerializeTest EmptyGraphRoundTrips", "[filterdesigner]") {
  Graph graph;
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  CHECK(result.ok());
  CHECK(restored.Nodes().empty());
  CHECK(restored.Links().empty());
}

TEST_CASE("SerializeTest SourceSinkPairRoundTrips", "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  Graph graph;
  auto src = graph.AddNode<FakeSourceNode>(ImVec2{10.0f, 20.0f});
  src->m_value = 42;
  auto sink = graph.AddNode<FakeSinkNode>(ImVec2{300.0f, 20.0f});
  sink->inPin("in")->createLink(src->outPin("out"));

  int srcId = src->GraphId();
  int sinkId = sink->GraphId();

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(restored.Nodes().size() == 2u);

  auto* restoredSrc =
      dynamic_cast<FakeSourceNode*>(restored.FindNodeById(srcId));
  auto* restoredSink =
      dynamic_cast<FakeSinkNode*>(restored.FindNodeById(sinkId));
  REQUIRE(restoredSrc != nullptr);
  REQUIRE(restoredSink != nullptr);
  UNSCOPED_INFO("per-node params round-trip");
  CHECK(restoredSrc->m_value == 42);

  auto links = restored.Links();
  REQUIRE(links.size() == 1u);
  CHECK(links[0].srcId == srcId);
  CHECK(links[0].dstId == sinkId);
  CHECK(links[0].srcPin == "out");
  CHECK(links[0].dstPin == "in");
}

TEST_CASE("SerializeTest V1FileIsRejectedWithUserFacingMessage",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  Graph graph;
  graph.AddNode<FakeSourceNode>(
      ImVec2{0.0f, 0.0f});  // Should not survive load.

  // v1 was the pre-node-graph format; it must be rejected explicitly.
  std::string v1 = R"({"version": 1, "nodes": [], "links": []})";
  auto result = DeserializeGraph(v1, graph, reg);
  CHECK_FALSE(result.ok());
  UNSCOPED_INFO(
      "error should call out the v1/older format, was: " << result.error);
  CHECK(result.error.find("older format") != std::string::npos);
  // Failed loads must not partially clobber the live graph (the deserializer
  // calls Reset() only after the version check passes).
  CHECK_FALSE(graph.Nodes().empty());
}

TEST_CASE("SerializeTest MissingVersionTreatedAsV1", "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  Graph graph;
  std::string noVersion = R"({"nodes": [], "links": []})";
  auto result = DeserializeGraph(noVersion, graph, reg);
  CHECK_FALSE(result.ok());
}

TEST_CASE("SerializeTest UnknownNodeTypeSkippedWithWarning",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  // FakeSource (id 1) + a node of an unknown type (id 2). Link from 1→2
  // is dropped because endpoint 2 doesn't exist after the skip.
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": [0, 0], "value": 99},
      {"id": 2, "type": "FromTheFuture", "pos": [100, 0]}
    ],
    "links": [
      {"src": {"node": 1, "pin": "out"}, "dst": {"node": 2, "pin": "in"}}
    ]
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  UNSCOPED_INFO("unknown type should warn");
  CHECK_FALSE(result.warnings.empty());

  auto nodes = graph.Nodes();
  REQUIRE(nodes.size() == 1u);
  CHECK(nodes[0]->TypeTag() == "FakeSource");
  CHECK(dynamic_cast<FakeSourceNode*>(nodes[0])->m_value == 99);
  CHECK(graph.Links().empty());
}

TEST_CASE("SerializeTest UnknownPinNameSkippedWithWarning",
          "[filterdesigner]") {
  // A file naming a pin the node doesn't have is skipped with a warning:
  // BaseNode::inPin/outPin would assert, and UB-deref in a release build.
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": [0, 0], "value": 1},
      {"id": 2, "type": "FakeSink", "pos": [100, 0]}
    ],
    "links": [
      {"src": {"node": 1, "pin": "ghost"}, "dst": {"node": 2, "pin": "in"}}
    ]
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  UNSCOPED_INFO("missing pin must surface as a warning");
  CHECK_FALSE(result.warnings.empty());
  // Both nodes still loaded, but the link did not.
  CHECK(graph.Nodes().size() == 2u);
  CHECK(graph.Links().empty());
}

TEST_CASE("SerializeTest DuplicateLinkSkippedWithWarning", "[filterdesigner]") {
  // ImNodeFlow's createLink toggles, so loading the same link twice would
  // leave the input disconnected, and the next save would drop the wire.
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": [0, 0], "value": 1},
      {"id": 2, "type": "FakeSink", "pos": [100, 0]}
    ],
    "links": [
      {"src": {"node": 1, "pin": "out"}, "dst": {"node": 2, "pin": "in"}},
      {"src": {"node": 1, "pin": "out"}, "dst": {"node": 2, "pin": "in"}}
    ]
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  UNSCOPED_INFO("the repeat must surface as a warning");
  CHECK(result.warnings.size() == 1u);
  auto links = graph.Links();
  REQUIRE(links.size() == 1u);
  CHECK(links[0].srcId == 1);
  CHECK(links[0].dstId == 2);
}

TEST_CASE("SerializeTest ConflictingLinkSkippedWithWarning",
          "[filterdesigner]") {
  // An input pin holds one link. ImNodeFlow's createLink silently swaps in a
  // second source, so a file wiring two sources into one input would load
  // with the last one listed and the next save would make that permanent.
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": [0, 0], "value": 1},
      {"id": 2, "type": "FakeSource", "pos": [0, 50], "value": 2},
      {"id": 3, "type": "FakeSink", "pos": [100, 0]}
    ],
    "links": [
      {"src": {"node": 1, "pin": "out"}, "dst": {"node": 3, "pin": "in"}},
      {"src": {"node": 2, "pin": "out"}, "dst": {"node": 3, "pin": "in"}}
    ]
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(result.warnings.size() == 1u);
  CHECK(result.warnings[0].find("Conflicting link") != std::string::npos);
  auto links = graph.Links();
  REQUIRE(links.size() == 1u);
  UNSCOPED_INFO("the first link listed must win");
  CHECK(links[0].srcId == 1);
  CHECK(links[0].dstId == 3);
}

TEST_CASE("SerializeTest NonNumericPosSkippedWithWarning", "[filterdesigner]") {
  // A malformed pos field is skipped with a warning; get_number() on it
  // would throw or abort in a release build.
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": ["x", 0]}
    ],
    "links": []
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  UNSCOPED_INFO("non-numeric pos must surface as a warning");
  CHECK_FALSE(result.warnings.empty());
  UNSCOPED_INFO("bad node must be skipped");
  CHECK(graph.Nodes().empty());
}

TEST_CASE("SerializeTest OutOfFloatRangePosSkippedWithWarning",
          "[filterdesigner]") {
  // ImNodeFlow's positions are floats; 1e100 is a perfectly good JSON number
  // that no float can hold, and narrowing it is undefined behaviour.
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": [1e100, 0]},
      {"id": 2, "type": "FakeSink", "pos": [0, 1e999]}
    ],
    "links": []
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  // 1e999 overflows the JSON reader's double to infinity, which is still a
  // number as far as is_number() is concerned.
  UNSCOPED_INFO("both nodes must be skipped, with a warning each");
  CHECK(result.warnings.size() == 2);
  CHECK(graph.Nodes().empty());
}

TEST_CASE("SerializeTest LinkToUndefinedNodeWarns", "[filterdesigner]") {
  // Node 99 is nowhere in "nodes", so nothing has warned about it yet.
  // Dropping its link quietly would report a clean load and then drop the
  // wiring for good the next time the design is saved.
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": [0, 0]},
      {"id": 2, "type": "FakeSink", "pos": [100, 0]}
    ],
    "links": [
      {"src": {"node": 1, "pin": "out"}, "dst": {"node": 99, "pin": "in"}}
    ]
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(result.warnings.size() == 1);
  UNSCOPED_INFO(result.warnings[0]);
  CHECK(result.warnings[0].find("99") != std::string::npos);
  CHECK(graph.Links().empty());
}

TEST_CASE("SerializeTest LinkToAlreadyReportedNodeWarnsOnce",
          "[filterdesigner]") {
  // Node 2 was skipped with its own warning, so the link it leaves dangling
  // must not produce a second one about the same problem.
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": [0, 0]},
      {"id": 2, "type": "NoSuchType", "pos": [100, 0]}
    ],
    "links": [
      {"src": {"node": 1, "pin": "out"}, "dst": {"node": 2, "pin": "in"}}
    ]
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(result.warnings.size() == 1);
  UNSCOPED_INFO(result.warnings[0]);
  CHECK(result.warnings[0].find("NoSuchType") != std::string::npos);
}

TEST_CASE("SerializeTest LinkRejectedByPinTypeWarns", "[filterdesigner]") {
  // Both endpoints exist and name real pins, but the pins disagree on type,
  // so ImNodeFlow's connection filter drops the link without a word.
  NodeRegistry reg;
  RegisterFakes(reg);
  RegisterDoubleSource(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeDoubleSource", "pos": [0, 0]},
      {"id": 2, "type": "FakeSink", "pos": [100, 0]}
    ],
    "links": [
      {"src": {"node": 1, "pin": "out"}, "dst": {"node": 2, "pin": "in"}}
    ]
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(result.warnings.size() == 1);
  UNSCOPED_INFO(result.warnings[0]);
  CHECK(graph.Links().empty());
  CHECK(graph.Nodes().size() == 2);
}

TEST_CASE("SerializeTest GraphResetCallbackFiresOnDeserialize",
          "[filterdesigner]") {
  // GraphEditor re-attaches the CreationPopup from this hook, the
  // deserializer having rebuilt the ImNodeFlow the old one was bound to.
  NodeRegistry reg;
  RegisterFakes(reg);

  Graph graph;
  int hits = 0;
  graph.SetOnReset([&hits] { ++hits; });

  std::string empty = R"({"version": 2, "nodes": [], "links": []})";
  auto result = DeserializeGraph(empty, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  UNSCOPED_INFO("Reset (via deserialize) must fire the callback");
  CHECK(hits == 1);

  // Direct Reset() also fires it.
  graph.Reset();
  CHECK(hits == 2);
}

TEST_CASE("SerializeTest DuplicateNodeIdSkippedWithWarning",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  // Two nodes claim id 5; the link names it. Only the first may exist, and
  // the link must land on that one rather than whichever the editor's node
  // map visits first.
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 5, "type": "FakeSource", "pos": [0, 0]},
      {"id": 5, "type": "FakeSink", "pos": [50, 0]},
      {"id": 6, "type": "FakeSink", "pos": [100, 0]}
    ],
    "links": [
      {"src": {"node": 5, "pin": "out"}, "dst": {"node": 6, "pin": "in"}}
    ]
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(result.warnings.size() == 1u);
  CHECK(result.warnings[0].find("Duplicate node id 5") != std::string::npos);

  CHECK(graph.Nodes().size() == 2u);
  CHECK(dynamic_cast<FakeSourceNode*>(graph.FindNodeById(5)) != nullptr);
  auto links = graph.Links();
  REQUIRE(links.size() == 1u);
  CHECK(links[0].srcId == 5);
  CHECK(links[0].dstId == 6);
}

TEST_CASE("SerializeTest NewNodesAfterLoadDontCollideWithLoadedIds",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 5, "type": "FakeSource", "pos": [0, 0]},
      {"id": 12, "type": "FakeSink", "pos": [100, 0]}
    ],
    "links": []
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());

  // The next added node should get id > 12, not collide with a loaded one.
  auto fresh = graph.AddNode<FakeSourceNode>(ImVec2{200.0f, 0.0f});
  CHECK(fresh->GraphId() > 12);
}

TEST_CASE("SerializeTest NodeIdNearTheTopOfIntRangeSkippedWithWarning",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  // An id one below INT_MAX fits an int and survives the bump to the next
  // id, but hands AddNode a counter already at the ceiling, and the
  // increment there is undefined. Ids count the nodes of one design, so the
  // loader keeps them under half the range with room to spare.
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 2147483646, "type": "FakeSource", "pos": [0, 0]},
      {"id": 1073741824, "type": "FakeSource", "pos": [0, 0]},
      {"id": 1073741823, "type": "FakeSink", "pos": [100, 0]}
    ],
    "links": []
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(result.warnings.size() == 2u);
  REQUIRE(graph.Nodes().size() == 1u);
  CHECK(graph.FindNodeById(1073741823) != nullptr);
  // The largest id the loader accepts still leaves the counter room to mint
  // the next one.
  auto fresh = graph.AddNode<FakeSourceNode>(ImVec2{200.0f, 0.0f});
  CHECK(fresh->GraphId() == 1073741824);
}

TEST_CASE("SerializeTest NodeIdOutsideIntRangeSkippedWithWarning",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  // Each bad id would either be undefined to cast (the doubles), wrap (the
  // long), or overflow the next-id bump (INT_MAX). Only node 3 may load,
  // and a link naming a bad id is dropped rather than cast.
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1e100, "type": "FakeSource", "pos": [0, 0]},
      {"id": 2.5, "type": "FakeSource", "pos": [0, 0]},
      {"id": -1, "type": "FakeSource", "pos": [0, 0]},
      {"id": 99999999999, "type": "FakeSource", "pos": [0, 0]},
      {"id": 2147483647, "type": "FakeSource", "pos": [0, 0]},
      {"id": 3, "type": "FakeSink", "pos": [100, 0]}
    ],
    "links": [
      {"src": {"node": 1e100, "pin": "out"}, "dst": {"node": 3, "pin": "in"}}
    ]
  })";

  Graph graph;
  auto result = DeserializeGraph(json, graph, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(result.warnings.size() == 6u);
  for (int i = 0; i < 5; ++i) {
    UNSCOPED_INFO("warning " << i << ": " << result.warnings[i]);
    CHECK(result.warnings[i].find("whole number") != std::string::npos);
  }
  CHECK(result.warnings[5].find("endpoint id") != std::string::npos);

  REQUIRE(graph.Nodes().size() == 1u);
  CHECK(graph.FindNodeById(3) != nullptr);
  CHECK(graph.Links().empty());
  // The bad ids never reached the id counter.
  auto fresh = graph.AddNode<FakeSourceNode>(ImVec2{200.0f, 0.0f});
  CHECK(fresh->GraphId() == 4);
}

TEST_CASE("SerializeTest NonIntegralVersionIsRejected", "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);
  Graph graph;
  auto result = DeserializeGraph(
      R"({"version": 1e100, "nodes": [], "links": []})", graph, reg);
  CHECK_FALSE(result.ok());
  CHECK(result.error.find("Unsupported") != std::string::npos);
}

TEST_CASE("SerializeTest LoadedNodesComeBackIntoView", "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  // Saved after panning a long way from the origin. A load rebuilds the
  // editor at scroll zero, so left where they are these are off-screen with
  // nothing to bring them back.
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": [5000, 4000]},
      {"id": 2, "type": "FakeSink", "pos": [5200, 4090]}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  CHECK(result.warnings.empty());

  auto* src = restored.FindNodeById(1);
  auto* sink = restored.FindNodeById(2);
  REQUIRE(src != nullptr);
  REQUIRE(sink != nullptr);
  UNSCOPED_INFO("the top-left node lands just inside the canvas corner");
  CHECK_FLOAT_EQ(src->getPos().x, 40.0f);
  CHECK_FLOAT_EQ(src->getPos().y, 40.0f);
  UNSCOPED_INFO("and the layout moves with it, unchanged");
  CHECK_FLOAT_EQ(sink->getPos().x, 240.0f);
  CHECK_FLOAT_EQ(sink->getPos().y, 130.0f);
}

TEST_CASE("SerializeTest RecenteringADesignTwiceMovesItOnce",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  Graph graph;
  graph.AddNode<FakeSourceNode>(ImVec2{900.0f, 700.0f});
  graph.AddNode<FakeSinkNode>(ImVec2{1100.0f, 700.0f});

  Graph once;
  REQUIRE(DeserializeGraph(SerializeGraph(graph), once, reg).ok());
  Graph twice;
  REQUIRE(DeserializeGraph(SerializeGraph(once), twice, reg).ok());

  // Save, load, save, load has to settle: a shift applied to an already
  // recentered design is a shift of nothing.
  REQUIRE(once.Nodes().size() == twice.Nodes().size());
  for (FilterDesignerNode* node : once.Nodes()) {
    auto* other = twice.FindNodeById(node->GraphId());
    REQUIRE(other != nullptr);
    UNSCOPED_INFO("node " << node->GraphId());
    CHECK_FLOAT_EQ(other->getPos().x, node->getPos().x);
    CHECK_FLOAT_EQ(other->getPos().y, node->getPos().y);
  }
}

TEST_CASE("SerializeTest NodesTooFarApartToRecenterAreLeftWhereTheyAre",
          "[filterdesigner]") {
  NodeRegistry reg;
  RegisterFakes(reg);

  // Each coordinate is a fine float on its own, but no single shift brings
  // both inside the float range. Moving only the one that fits would take the
  // graph apart, so neither moves.
  std::string json = R"({
    "version": 2,
    "nodes": [
      {"id": 1, "type": "FakeSource", "pos": [-3e38, 0]},
      {"id": 2, "type": "FakeSink", "pos": [3e38, 0]}
    ],
    "links": []
  })";

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  UNSCOPED_INFO(result.error);
  REQUIRE(result.ok());
  REQUIRE(restored.Nodes().size() == 2u);
  auto* src = restored.FindNodeById(1);
  auto* sink = restored.FindNodeById(2);
  REQUIRE(src != nullptr);
  REQUIRE(sink != nullptr);
  CHECK_FLOAT_EQ(src->getPos().x, -3e38f);
  CHECK_FLOAT_EQ(sink->getPos().x, 3e38f);
  REQUIRE(result.warnings.size() == 1u);
  UNSCOPED_INFO(result.warnings[0]);
  CHECK(result.warnings[0].find("too far apart") != std::string::npos);
}

TEST_CASE("SerializeTest SaveWritesTheWholeFileBeforeReportingSuccess",
          "[filterdesigner]") {
  // The stream is only known to be good once it has been closed: the last
  // buffered chunk is flushed by the destructor, and nothing can see that
  // fail. An empty return here has to mean the bytes are all on disk.
  NodeRegistry reg;
  RegisterFakes(reg);

  Graph graph;
  auto src = graph.AddNode<FakeSourceNode>(ImVec2{10.0f, 20.0f});
  src->m_value = 42;
  auto sink = graph.AddNode<FakeSinkNode>(ImVec2{300.0f, 20.0f});
  sink->inPin("in")->createLink(src->outPin("out"));

  auto tmp = std::filesystem::temp_directory_path() /
             ("filterdesigner_test_" +
              std::to_string(
                  std::chrono::steady_clock::now().time_since_epoch().count()) +
              ".fdsgn");

  CHECK(SaveGraphToFile(tmp.string(), graph).empty());

  const std::string expected = SerializeGraph(graph);
  std::ifstream in{tmp, std::ios::binary};
  REQUIRE(in.is_open());
  const std::string written{std::istreambuf_iterator<char>{in},
                            std::istreambuf_iterator<char>{}};
  in.close();
  UNSCOPED_INFO("a truncated write must not report a clean save");
  CHECK(written.size() == expected.size());
  CHECK(written == expected);

  Graph restored;
  auto result = LoadGraphFromFile(tmp.string(), restored, reg);
  UNSCOPED_INFO(result.error);
  CHECK(result.ok());
  CHECK(restored.Nodes().size() == 2u);

  std::filesystem::remove(tmp);
}

TEST_CASE("SerializeTest SaveToAnUnopenablePathReports", "[filterdesigner]") {
  Graph graph;
  auto bad = std::filesystem::temp_directory_path() /
             "filterdesigner_no_such_dir" / "nested" / "x.fdsgn";
  const std::string error = SaveGraphToFile(bad.string(), graph);
  CHECK_FALSE(error.empty());
}

}  // namespace
