// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/NodeRegistry.hpp"

#include <memory>
#include <string>
#include <typeindex>
#include <utility>

#include <gtest/gtest.h>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/graph/Graph.hpp"

namespace {

using wpi::filterdesigner::FilterDesignerNode;
using wpi::filterdesigner::Graph;
using wpi::filterdesigner::NodeRegistry;

class FakeNode : public FilterDesignerNode {
 public:
  std::string_view TypeTag() const override { return m_tag; }
  std::string m_tag;
};

NodeRegistry::Entry MakeEntry(const std::string& tag) {
  NodeRegistry::Entry entry;
  entry.tag = tag;
  entry.menuLabel = tag;
  entry.factory = [tag](Graph& g, const ImVec2& pos) {
    auto node = g.AddNode<FakeNode>(pos);
    node->m_tag = tag;
    return std::static_pointer_cast<FilterDesignerNode>(node);
  };
  return entry;
}

TEST(NodeRegistryTest, RegisterAndFindByTag) {
  NodeRegistry reg;
  reg.Register(MakeEntry("Alpha"));
  reg.Register(MakeEntry("Beta"));

  EXPECT_NE(reg.FindByTag("Alpha"), nullptr);
  EXPECT_NE(reg.FindByTag("Beta"), nullptr);
  EXPECT_EQ(reg.FindByTag("Gamma"), nullptr);
  EXPECT_EQ(reg.All().size(), 2u);
}

TEST(NodeRegistryTest, ReRegisterReplacesPreviousEntry) {
  NodeRegistry reg;
  auto first = MakeEntry("Alpha");
  first.menuLabel = "First";
  reg.Register(std::move(first));

  auto second = MakeEntry("Alpha");
  second.menuLabel = "Second";
  reg.Register(std::move(second));

  ASSERT_EQ(reg.All().size(), 1u);
  EXPECT_EQ(reg.FindByTag("Alpha")->menuLabel, "Second");
}

TEST(NodeRegistryTest, FactoryProducesNodeOnGraph) {
  NodeRegistry reg;
  reg.Register(MakeEntry("Alpha"));

  Graph graph;
  auto node = reg.FindByTag("Alpha")->factory(graph, ImVec2{10.0f, 20.0f});
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->TypeTag(), "Alpha");
  EXPECT_GT(node->GraphId(), 0)
      << "Graph::AddNode should assign a graph-local id";
  EXPECT_EQ(graph.Nodes().size(), 1u);
}

}  // namespace
