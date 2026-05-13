// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/Topology.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>

#include <ImNodeFlow.h>
#include <gtest/gtest.h>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"

namespace {

using wpi::filterdesigner::FilterDesignerNode;
using wpi::filterdesigner::FindCycle;
using wpi::filterdesigner::FormatCycle;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::HasCycle;

// Test-only nodes with both an int input and an int output, so cycles are
// expressible without involving real wpi::filterdesigner data types
// (Signal*, DesignedFilter*) which would drag in the wider tool dependency
// graph. Each node sets a distinct title so FormatCycle output is readable
// and the per-node display string is verifiable.
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

// Two input pins, one output. Lets a test build "tail D → cycle node A" by
// landing the tail on a free in-pin while the cycle's back-edge occupies the
// other one (ImNodeFlow's InPin only holds one upstream link, so a single-in
// node can't accept both).
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

TEST(TopologyTest, EmptyGraphIsAcyclic) {
  Graph g;
  EXPECT_FALSE(HasCycle(g));
  EXPECT_TRUE(FindCycle(g).empty());
}

TEST(TopologyTest, IsolatedNodesAreAcyclic) {
  Graph g;
  g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  EXPECT_FALSE(HasCycle(g));
}

TEST(TopologyTest, LinearChainIsAcyclic) {
  Graph g;
  auto src = g.AddNode<SourceOnlyNode>(ImVec2{0, 0}, "Src");
  auto mid = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "Mid");
  auto sink = g.AddNode<SinkOnlyNode>(ImVec2{0, 0}, "Sink");
  mid->inPin("in")->createLink(src->outPin("out"));
  sink->inPin("in")->createLink(mid->outPin("out"));
  EXPECT_FALSE(HasCycle(g));
}

TEST(TopologyTest, DiamondDagIsAcyclic) {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  auto c = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "C");
  auto d = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "D");
  b->inPin("in")->createLink(a->outPin("out"));
  c->inPin("in")->createLink(a->outPin("out"));
  d->inPin("in")->createLink(b->outPin("out"));
  // ImNodeFlow's InPin holds at most one upstream Link (createLink replaces
  // any existing one), so D has only B as a real upstream. That still
  // exercises the "two reachable predecessors via different paths" topology
  // (B→D and B comes from A, plus the dangling C→D attempt collapses to a
  // single edge).
  d->inPin("in")->createLink(c->outPin("out"));
  EXPECT_FALSE(HasCycle(g));
}

TEST(TopologyTest, TwoNodeCycleIsDetected) {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  a->inPin("in")->createLink(b->outPin("out"));
  b->inPin("in")->createLink(a->outPin("out"));

  EXPECT_TRUE(HasCycle(g));
  auto path = FindCycle(g);
  ASSERT_GE(path.size(), 3u);
  // Path is closed: first == last.
  EXPECT_EQ(path.front(), path.back());
  // The cycle includes both nodes exactly once (plus the closing repeat).
  EXPECT_EQ(path.size(), 3u);
  EXPECT_TRUE((path[0] == a->GraphId() && path[1] == b->GraphId()) ||
              (path[0] == b->GraphId() && path[1] == a->GraphId()));
}

TEST(TopologyTest, ThreeNodeCycleIsDetected) {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  auto c = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "C");
  b->inPin("in")->createLink(a->outPin("out"));
  c->inPin("in")->createLink(b->outPin("out"));
  a->inPin("in")->createLink(c->outPin("out"));

  EXPECT_TRUE(HasCycle(g));
  auto path = FindCycle(g);
  // Three distinct ids in the cycle, first repeated at end.
  ASSERT_EQ(path.size(), 4u);
  EXPECT_EQ(path.front(), path.back());
}

TEST(TopologyTest, SelfLoopIsDetected) {
  // Smallest non-trivial cycle: a single passthrough wired to itself.
  // Production nodes leave ImNodeFlow's m_allowSelfConnection=false default
  // in place, so users can't draw self-links interactively — but a corrupted
  // saved file could in principle deserialize one, and the topology code
  // should still terminate cleanly. Enable self-connection on the test pin
  // to exercise the path-slicer's degenerate-case behaviour (back-edge
  // target == entry, expected path [A, A]).
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  // allowSameNodeConnections lives on InPin<T>, not the Pin* base returned by
  // inPin(), so down-cast to the concrete pin type. PassthroughNode's "in" is
  // an int pin.
  static_cast<ImFlow::InPin<int>*>(a->inPin("in"))
      ->allowSameNodeConnections(true);
  a->inPin("in")->createLink(a->outPin("out"));

  EXPECT_TRUE(HasCycle(g));
  auto path = FindCycle(g);
  ASSERT_EQ(path.size(), 2u);
  EXPECT_EQ(path[0], a->GraphId());
  EXPECT_EQ(path[1], a->GraphId());
}

TEST(TopologyTest, CycleWithLeadingTailExcludesTail) {
  // Tail S → A, then a cycle A ↔ B (edges A→B and B→A). A is a two-input
  // node so it can hold both the tail-link (S→A on in0) and the back-edge
  // (B→A on in1) at once — single-in-pin nodes can't model this because
  // createLink replaces the upstream.
  //
  // The point of the test: if DFS happens to start at S, it walks S→A→B,
  // sees the back-edge to A, and must slice the path *starting at A* (not S).
  // That exercises the "started" guard inside FindCycleImpl. DFS start order
  // is non-deterministic (ids come from an unordered_map), so we don't rely
  // on it — we just assert the cycle path itself excludes S regardless of
  // entry point, which is the property the slicer guarantees.
  Graph g;
  auto s = g.AddNode<SourceOnlyNode>(ImVec2{0, 0}, "S");
  auto a = g.AddNode<TwoInPassNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  a->inPin("in0")->createLink(s->outPin("out"));  // tail S → A
  b->inPin("in")->createLink(a->outPin("out"));   // cycle leg A → B
  a->inPin("in1")->createLink(b->outPin("out"));  // cycle leg B → A

  EXPECT_TRUE(HasCycle(g));
  auto path = FindCycle(g);
  ASSERT_EQ(path.size(), 3u);
  EXPECT_EQ(path.front(), path.back());
  for (int id : path) {
    EXPECT_NE(id, s->GraphId());
  }
}

TEST(TopologyTest, CycleDetectedAmidAcyclicSubgraph) {
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

  EXPECT_TRUE(HasCycle(g));
  auto path = FindCycle(g);
  ASSERT_FALSE(path.empty());
  // The cycle path should not include the acyclic-subgraph nodes.
  for (int id : path) {
    EXPECT_NE(id, a->GraphId());
    EXPECT_NE(id, b->GraphId());
    EXPECT_NE(id, sink->GraphId());
  }
}

TEST(TopologyTest, FormatCycleUsesTitlesAndIds) {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "Alpha");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "Beta");
  a->inPin("in")->createLink(b->outPin("out"));
  b->inPin("in")->createLink(a->outPin("out"));

  auto path = FindCycle(g);
  std::string s = FormatCycle(g, path);
  // U+2192 in UTF-8 is e2 86 92.
  EXPECT_NE(s.find("\xe2\x86\x92"), std::string::npos);
  EXPECT_NE(s.find("Alpha"), std::string::npos);
  EXPECT_NE(s.find("Beta"), std::string::npos);
  EXPECT_NE(s.find("[" + std::to_string(a->GraphId()) + "]"),
            std::string::npos);
  EXPECT_NE(s.find("[" + std::to_string(b->GraphId()) + "]"),
            std::string::npos);
}

TEST(TopologyTest, FormatCycleEmptyOnEmptyPath) {
  Graph g;
  EXPECT_EQ(FormatCycle(g, {}), "");
}

TEST(TopologyTest, GraphCycleErrorEmptyByDefault) {
  Graph g;
  // RecomputeCycleError is what Graph::Update calls per frame; we drive it
  // here so the test exercises the same code path without needing an ImGui
  // context.
  g.RecomputeCycleError();
  EXPECT_TRUE(g.CycleError().empty());
}

TEST(TopologyTest, GraphCycleErrorPopulatedAfterCycle) {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  a->inPin("in")->createLink(b->outPin("out"));
  b->inPin("in")->createLink(a->outPin("out"));

  g.RecomputeCycleError();
  EXPECT_FALSE(g.CycleError().empty());
  EXPECT_NE(g.CycleError().find("A"), std::string::npos);
  EXPECT_NE(g.CycleError().find("B"), std::string::npos);
}

TEST(TopologyTest, GraphCycleErrorClearedByReset) {
  Graph g;
  auto a = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "A");
  auto b = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "B");
  a->inPin("in")->createLink(b->outPin("out"));
  b->inPin("in")->createLink(a->outPin("out"));
  g.RecomputeCycleError();
  ASSERT_FALSE(g.CycleError().empty());

  g.Reset();
  // Reset drops every node + link; the cached error must follow suit so a
  // post-load graph doesn't show a stale banner before its first
  // RecomputeCycleError tick.
  EXPECT_TRUE(g.CycleError().empty());
}

TEST(TopologyTest, NodeGetGraphPopulatedByAddNode) {
  // Sinks reach back to their owning Graph via GetGraph(); without this
  // wiring the M7 cycle banner can't fire in production. Cover the
  // contract directly here.
  Graph g;
  auto n = g.AddNode<PassthroughNode>(ImVec2{0, 0}, "N");
  EXPECT_EQ(n->GetGraph(), &g);
}

TEST(TopologyTest, NodeGetGraphPopulatedByAddNodeWithId) {
  // The deserializer uses AddNodeWithId, so it also needs to wire the
  // back-pointer. Cover that path too — otherwise loaded sinks would silently
  // miss the cycle banner until the user touched them.
  Graph g;
  auto n = g.AddNodeWithId<PassthroughNode>(ImVec2{0, 0}, 42, "N");
  EXPECT_EQ(n->GetGraph(), &g);
  EXPECT_EQ(n->GraphId(), 42);
}

}  // namespace
