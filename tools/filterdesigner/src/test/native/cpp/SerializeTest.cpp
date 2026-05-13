// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/Serialize.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ImNodeFlow.h>
#include <gtest/gtest.h>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"
#include "wpi/filterdesigner/graph/NodeRegistry.hpp"
#include "wpi/util/json.hpp"

namespace {

using wpi::filterdesigner::DeserializeGraph;
using wpi::filterdesigner::FilterDesignerNode;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::NodeRegistry;
using wpi::filterdesigner::SerializeGraph;

// Two test-only node types: one source (one Signal-typed output), one sink
// (one Signal-typed input). Mirrors the real M2 pair without dragging in
// ImGui dependencies (wpilog loading, ImPlot rendering).

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

TEST(SerializeTest, EmptyGraphRoundTrips) {
  Graph graph;
  NodeRegistry reg;
  RegisterFakes(reg);

  std::string json = SerializeGraph(graph);

  Graph restored;
  auto result = DeserializeGraph(json, restored, reg);
  EXPECT_TRUE(result.ok()) << result.error;
  EXPECT_TRUE(restored.Nodes().empty());
  EXPECT_TRUE(restored.Links().empty());
}

TEST(SerializeTest, SourceSinkPairRoundTrips) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  ASSERT_EQ(restored.Nodes().size(), 2u);

  auto* restoredSrc =
      dynamic_cast<FakeSourceNode*>(restored.FindNodeById(srcId));
  auto* restoredSink =
      dynamic_cast<FakeSinkNode*>(restored.FindNodeById(sinkId));
  ASSERT_NE(restoredSrc, nullptr);
  ASSERT_NE(restoredSink, nullptr);
  EXPECT_EQ(restoredSrc->m_value, 42) << "per-node params round-trip";

  auto links = restored.Links();
  ASSERT_EQ(links.size(), 1u);
  EXPECT_EQ(links[0].srcId, srcId);
  EXPECT_EQ(links[0].dstId, sinkId);
  EXPECT_EQ(links[0].srcPin, "out");
  EXPECT_EQ(links[0].dstPin, "in");
}

TEST(SerializeTest, V1FileIsRejectedWithUserFacingMessage) {
  NodeRegistry reg;
  RegisterFakes(reg);

  Graph graph;
  graph.AddNode<FakeSourceNode>(
      ImVec2{0.0f, 0.0f});  // Should not survive load.

  // The M1 spike emitted v1; v1 must be rejected explicitly.
  std::string v1 = R"({"version": 1, "nodes": [], "links": []})";
  auto result = DeserializeGraph(v1, graph, reg);
  EXPECT_FALSE(result.ok());
  EXPECT_NE(result.error.find("linear-chain"), std::string::npos)
      << "error should reference the linear-chain tool, was: " << result.error;
  // Failed loads must not partially clobber the live graph (the deserializer
  // calls Reset() only after the version check passes).
  EXPECT_FALSE(graph.Nodes().empty());
}

TEST(SerializeTest, MissingVersionTreatedAsV1) {
  NodeRegistry reg;
  RegisterFakes(reg);

  Graph graph;
  std::string noVersion = R"({"nodes": [], "links": []})";
  auto result = DeserializeGraph(noVersion, graph, reg);
  EXPECT_FALSE(result.ok());
}

TEST(SerializeTest, UnknownNodeTypeSkippedWithWarning) {
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
  ASSERT_TRUE(result.ok()) << result.error;
  EXPECT_FALSE(result.warnings.empty()) << "unknown type should warn";

  auto nodes = graph.Nodes();
  ASSERT_EQ(nodes.size(), 1u);
  EXPECT_EQ(nodes[0]->TypeTag(), "FakeSource");
  EXPECT_EQ(dynamic_cast<FakeSourceNode*>(nodes[0])->m_value, 99);
  EXPECT_TRUE(graph.Links().empty());
}

TEST(SerializeTest, UnknownPinNameSkippedWithWarning) {
  // Pre-fix, BaseNode::inPin/outPin asserted on miss + dereffed end() in
  // release builds — so a stale or hand-edited file referencing a pin name
  // that doesn't exist on the node would UB-deref instead of warning. Now
  // the load path matches by name via getIns()/getOuts() and skips
  // gracefully.
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
  ASSERT_TRUE(result.ok()) << result.error;
  EXPECT_FALSE(result.warnings.empty())
      << "missing pin must surface as a warning";
  // Both nodes still loaded, but the link did not.
  EXPECT_EQ(graph.Nodes().size(), 2u);
  EXPECT_TRUE(graph.Links().empty());
}

TEST(SerializeTest, NonNumericPosSkippedWithWarning) {
  // Pre-fix, pos[0].get_number() would throw/abort in release builds on a
  // malformed pos field. Now the loader validates types and skips the
  // offending node with a warning.
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
  ASSERT_TRUE(result.ok()) << result.error;
  EXPECT_FALSE(result.warnings.empty())
      << "non-numeric pos must surface as a warning";
  EXPECT_TRUE(graph.Nodes().empty()) << "bad node must be skipped";
}

TEST(SerializeTest, GraphResetCallbackFiresOnDeserialize) {
  // GraphEditor relies on the OnReset hook to re-attach the CreationPopup
  // after the deserializer rebuilds the underlying ImNodeFlow. Pin that
  // contract here so a future refactor of Graph::Reset can't silently drop
  // the call without a failing test.
  NodeRegistry reg;
  RegisterFakes(reg);

  Graph graph;
  int hits = 0;
  graph.SetOnReset([&hits] { ++hits; });

  std::string empty = R"({"version": 2, "nodes": [], "links": []})";
  auto result = DeserializeGraph(empty, graph, reg);
  ASSERT_TRUE(result.ok()) << result.error;
  EXPECT_EQ(hits, 1) << "Reset (via deserialize) must fire the callback";

  // Direct Reset() also fires it.
  graph.Reset();
  EXPECT_EQ(hits, 2);
}

TEST(SerializeTest, NewNodesAfterLoadDontCollideWithLoadedIds) {
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
  ASSERT_TRUE(result.ok()) << result.error;

  // The next added node should get id > 12, not collide with a loaded one.
  auto fresh = graph.AddNode<FakeSourceNode>(ImVec2{200.0f, 0.0f});
  EXPECT_GT(fresh->GraphId(), 12);
}

}  // namespace
