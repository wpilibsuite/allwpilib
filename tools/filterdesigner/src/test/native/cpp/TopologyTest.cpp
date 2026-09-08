// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/Topology.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>

#include <ImNodeFlow.h>
#include <catch2/catch_test_macros.hpp>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"

namespace {

using wpi::filterdesigner::FilterDesignerNode;
using wpi::filterdesigner::FindCycle;
using wpi::filterdesigner::FormatCycle;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::HasCycle;

// Int in and int out, so a cycle is expressible without the real wire types.
// Distinct titles make FormatCycle's output verifiable.
class PassthroughNode : public FilterDesignerNode {
 public:
  explicit PassthroughNode(std::string_view title) {
    setTitle(std::string{title});
    addIN<int>("in", 0, ImFlow::ConnectionFilter::SameType());
    addOUT<int>("out")->behaviour([this] { return getInVal<int>("in"); });
  }
  std::string_view TypeTag() const override { return "Passthrough"; }
};

class SourceOnlyNode : public FilterDesignerNode {
 public:
  explicit SourceOnlyNode(std::string_view title) {
    setTitle(std::string{title});
    addOUT<int>("out")->behaviour([] { return 0; });
  }
  std::string_view TypeTag() const override { return "Source"; }
};

class SinkOnlyNode : public FilterDesignerNode {
 public:
  explicit SinkOnlyNode(std::string_view title) {
    setTitle(std::string{title});
    addIN<int>("in", 0, ImFlow::ConnectionFilter::SameType());
  }
  std::string_view TypeTag() const override { return "Sink"; }
};

// Two in-pins, so a node can hold a tail link and a cycle's back-edge at once:
// an InPin holds only one upstream link.
class TwoInPassNode : public FilterDesignerNode {
 public:
  explicit TwoInPassNode(std::string_view title) {
    setTitle(std::string{title});
    addIN<int>("in0", 0, ImFlow::ConnectionFilter::SameType());
    addIN<int>("in1", 0, ImFlow::ConnectionFilter::SameType());
    addOUT<int>("out")->behaviour([this] { return getInVal<int>("in0"); });
  }
  std::string_view TypeTag() const override { return "TwoInPass"; }
};

TEST_CASE("TopologyTest EmptyGraphIsAcyclic", "[filterdesigner]") {
  Graph g;
  CHECK_FALSE(HasCycle(g));
  CHECK(FindCycle(g).empty());
}

TEST_CASE("TopologyTest IsolatedNodesAreAcyclic", "[filterdesigner]") {
  Graph g;
  g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  CHECK_FALSE(HasCycle(g));
}

TEST_CASE("TopologyTest LinearChainIsAcyclic", "[filterdesigner]") {
  Graph g;
  auto src = g.AddNode<SourceOnlyNode>(ImVec2{0, 0}, "Src");
  auto mid = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "Mid");
  auto sink = g.AddNode<SinkOnlyNode>(ImVec2{0, 0}, "Sink");
  mid->inPin("in")->createLink(src->outPin("out"));
  sink->inPin("in")->createLink(mid->outPin("out"));
  CHECK_FALSE(HasCycle(g));
}

TEST_CASE("TopologyTest DiamondDagIsAcyclic", "[filterdesigner]") {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  auto c = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "C");
  auto d = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "D");
  b->inPin("in")->createLink(a->outPin("out"));
  c->inPin("in")->createLink(a->outPin("out"));
  d->inPin("in")->createLink(b->outPin("out"));
  // createLink replaces any existing upstream, so D keeps only B — still two
  // predecessors reachable by different paths, since B comes from A.
  d->inPin("in")->createLink(c->outPin("out"));
  CHECK_FALSE(HasCycle(g));
}

TEST_CASE("TopologyTest TwoNodeCycleIsDetected", "[filterdesigner]") {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  a->inPin("in")->createLink(b->outPin("out"));
  b->inPin("in")->createLink(a->outPin("out"));

  CHECK(HasCycle(g));
  auto path = FindCycle(g);
  REQUIRE(path.size() >= 3u);
  // Path is closed: first == last.
  CHECK(path.front() == path.back());
  // The cycle includes both nodes exactly once (plus the closing repeat).
  CHECK(path.size() == 3u);
  CHECK(((path[0] == a->GraphId() && path[1] == b->GraphId()) ||
         (path[0] == b->GraphId() && path[1] == a->GraphId())));
}

TEST_CASE("TopologyTest ThreeNodeCycleIsDetected", "[filterdesigner]") {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  auto c = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "C");
  b->inPin("in")->createLink(a->outPin("out"));
  c->inPin("in")->createLink(b->outPin("out"));
  a->inPin("in")->createLink(c->outPin("out"));

  CHECK(HasCycle(g));
  auto path = FindCycle(g);
  // Three distinct ids in the cycle, first repeated at end.
  REQUIRE(path.size() == 4u);
  CHECK(path.front() == path.back());
}

TEST_CASE("TopologyTest SelfLoopIsDetected", "[filterdesigner]") {
  // The smallest cycle: a passthrough wired to itself. Production nodes keep
  // ImNodeFlow's m_allowSelfConnection default, so only a corrupted file can
  // produce one, and the slicer should still terminate — back-edge target ==
  // entry, path [A, A].
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  // allowSameNodeConnections lives on InPin<T>, not the Pin* base returned by
  // inPin(), so down-cast to the concrete pin type. PassthroughNode's "in" is
  // an int pin.
  static_cast<ImFlow::InPin<int>*>(a->inPin("in"))
      ->allowSameNodeConnections(true);
  a->inPin("in")->createLink(a->outPin("out"));

  CHECK(HasCycle(g));
  auto path = FindCycle(g);
  REQUIRE(path.size() == 2u);
  CHECK(path[0] == a->GraphId());
  CHECK(path[1] == a->GraphId());
}

TEST_CASE("TopologyTest CycleWithLeadingTailExcludesTail", "[filterdesigner]") {
  // Tail S → A, then a cycle A ↔ B. A DFS starting at S walks S→A→B, sees the
  // back-edge to A, and must slice the path from A — the "started" guard in
  // FindCycleImpl. Start order is non-deterministic (ids come from an
  // unordered_map), so the assertion is only that S is not in the cycle.
  Graph g;
  auto s = g.AddNode<SourceOnlyNode>(ImVec2{0, 0}, "S");
  auto a = g.AddNode<TwoInPassNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  a->inPin("in0")->createLink(s->outPin("out"));  // tail S → A
  b->inPin("in")->createLink(a->outPin("out"));   // cycle leg A → B
  a->inPin("in1")->createLink(b->outPin("out"));  // cycle leg B → A

  CHECK(HasCycle(g));
  auto path = FindCycle(g);
  REQUIRE(path.size() == 3u);
  CHECK(path.front() == path.back());
  for (int id : path) {
    CHECK(id != s->GraphId());
  }
}

TEST_CASE("TopologyTest CycleDetectedAmidAcyclicSubgraph", "[filterdesigner]") {
  // One subgraph with a clean A → B → Sink; a separate disconnected cycle
  // C ↔ D. FindCycle should still surface the cycle.
  Graph g;
  auto a = g.AddNode<SourceOnlyNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  auto sink = g.AddNode<SinkOnlyNode>(ImVec2{0, 0}, "Sink");
  b->inPin("in")->createLink(a->outPin("out"));
  sink->inPin("in")->createLink(b->outPin("out"));

  auto c = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "C");
  auto d = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "D");
  c->inPin("in")->createLink(d->outPin("out"));
  d->inPin("in")->createLink(c->outPin("out"));

  CHECK(HasCycle(g));
  auto path = FindCycle(g);
  REQUIRE_FALSE(path.empty());
  // The cycle path should not include the acyclic-subgraph nodes.
  for (int id : path) {
    CHECK(id != a->GraphId());
    CHECK(id != b->GraphId());
    CHECK(id != sink->GraphId());
  }
}

TEST_CASE("TopologyTest FormatCycleUsesTitlesAndIds", "[filterdesigner]") {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "Alpha");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "Beta");
  a->inPin("in")->createLink(b->outPin("out"));
  b->inPin("in")->createLink(a->outPin("out"));

  auto path = FindCycle(g);
  std::string s = FormatCycle(g, path);
  // U+2192 in UTF-8 is e2 86 92.
  CHECK(s.find("\xe2\x86\x92") != std::string::npos);
  CHECK(s.find("Alpha") != std::string::npos);
  CHECK(s.find("Beta") != std::string::npos);
  CHECK(s.find("[" + std::to_string(a->GraphId()) + "]") != std::string::npos);
  CHECK(s.find("[" + std::to_string(b->GraphId()) + "]") != std::string::npos);
}

TEST_CASE("TopologyTest FormatCycleEmptyOnEmptyPath", "[filterdesigner]") {
  Graph g;
  CHECK(FormatCycle(g, {}) == "");
}

TEST_CASE("TopologyTest GraphCycleErrorEmptyByDefault", "[filterdesigner]") {
  Graph g;
  // What Graph::Update calls per frame, driven directly so the test needs no
  // ImGui context.
  g.RecomputeCycleError();
  CHECK(g.CycleError().empty());
}

TEST_CASE("TopologyTest GraphCycleErrorPopulatedAfterCycle",
          "[filterdesigner]") {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  a->inPin("in")->createLink(b->outPin("out"));
  b->inPin("in")->createLink(a->outPin("out"));

  g.RecomputeCycleError();
  CHECK_FALSE(g.CycleError().empty());
  CHECK(g.CycleError().find("A") != std::string::npos);
  CHECK(g.CycleError().find("B") != std::string::npos);
}

TEST_CASE("TopologyTest GraphCycleErrorClearedByReset", "[filterdesigner]") {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  a->inPin("in")->createLink(b->outPin("out"));
  b->inPin("in")->createLink(a->outPin("out"));
  g.RecomputeCycleError();
  REQUIRE_FALSE(g.CycleError().empty());

  g.Reset();
  // Reset drops every node and link, so the cached error has to go with them
  // or a freshly-loaded graph shows a stale banner.
  CHECK(g.CycleError().empty());
}

TEST_CASE("TopologyTest NodeGetGraphPopulatedByAddNode", "[filterdesigner]") {
  // Sinks reach their owning Graph through GetGraph() to draw the banner.
  Graph g;
  auto n = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "N");
  CHECK(n->GetGraph() == &g);
}

TEST_CASE("TopologyTest NodeGetGraphPopulatedByAddNodeWithId",
          "[filterdesigner]") {
  // AddNodeWithId is the deserializer's path, and has to wire the
  // back-pointer too.
  Graph g;
  auto n = g.AddNodeWithId<PassthroughNode>(ImVec2{0, 0}, 42, "N");
  CHECK(n->GetGraph() == &g);
  CHECK(n->GraphId() == 42);
}

}  // namespace
