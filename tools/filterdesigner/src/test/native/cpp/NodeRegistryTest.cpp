// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/graph/NodeRegistry.hpp"

#include <memory>
#include <string>
#include <typeindex>
#include <utility>

#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("NodeRegistryTest RegisterAndFindByTag", "[filterdesigner]") {
  NodeRegistry reg;
  reg.Register(MakeEntry("Alpha"));
  reg.Register(MakeEntry("Beta"));

  CHECK(reg.FindByTag("Alpha") != nullptr);
  CHECK(reg.FindByTag("Beta") != nullptr);
  CHECK(reg.FindByTag("Gamma") == nullptr);
  CHECK(reg.All().size() == 2u);
}

TEST_CASE("NodeRegistryTest ReRegisterReplacesPreviousEntry",
          "[filterdesigner]") {
  NodeRegistry reg;
  auto first = MakeEntry("Alpha");
  first.menuLabel = "First";
  reg.Register(std::move(first));

  auto second = MakeEntry("Alpha");
  second.menuLabel = "Second";
  reg.Register(std::move(second));

  REQUIRE(reg.All().size() == 1u);
  CHECK(reg.FindByTag("Alpha")->menuLabel == "Second");
}

TEST_CASE("NodeRegistryTest FactoryProducesNodeOnGraph", "[filterdesigner]") {
  NodeRegistry reg;
  reg.Register(MakeEntry("Alpha"));

  Graph graph;
  auto node = reg.FindByTag("Alpha")->factory(graph, ImVec2{10.0f, 20.0f});
  REQUIRE(node != nullptr);
  CHECK(node->TypeTag() == "Alpha");
  UNSCOPED_INFO("Graph::AddNode should assign a graph-local id");
  CHECK(node->GraphId() > 0);
  CHECK(graph.Nodes().size() == 1u);
}

}  // namespace
