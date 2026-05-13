// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <ImNodeFlow.h>

#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class Graph;

/**
 * Base class for every node in the Filter Designer graph.
 *
 * Adds three things on top of ImFlow::BaseNode:
 *   1. A stable, graph-local integer id. ImNodeFlow's own UID is derived from
 *      the node pointer, so it can't be persisted; this id is assigned by
 *      Graph::AddNode and survives save/load.
 *   2. A string type tag + per-node serialize/deserialize hooks so the
 *      registry+serializer can reconstitute concrete node types from JSON
 *      without dynamic_cast chains.
 *   3. A back-pointer to the owning @ref Graph. Sinks read it to query
 *      graph-level state like the per-frame cycle indicator (M7) without
 *      threading a Graph reference through every node ctor.
 */
class FilterDesignerNode : public ImFlow::BaseNode {
 public:
  FilterDesignerNode() = default;
  ~FilterDesignerNode() override = default;

  FilterDesignerNode(const FilterDesignerNode&) = delete;
  FilterDesignerNode& operator=(const FilterDesignerNode&) = delete;

  /** Identifier used in .fdsgn v2 files. Stable per-node-type string. */
  virtual std::string_view TypeTag() const = 0;

  /**
   * Emit node-specific fields into @p obj (already a JSON object containing
   * id/type/pos — node implementations only add their own keys).
   */
  virtual void SerializeParams(wpi::util::json& obj) const { (void)obj; }

  /**
   * Restore node-specific fields from @p obj. Called by the deserializer
   * right after construction; node defaults apply for absent fields.
   */
  virtual void DeserializeParams(const wpi::util::json& obj) { (void)obj; }

  /** Graph-local id used by the serializer. Assigned by Graph::AddNode. */
  int GraphId() const { return m_graphId; }
  void SetGraphId(int id) { m_graphId = id; }

  /**
   * Owning @ref Graph, or nullptr for a node that was constructed outside the
   * Graph::AddNode path (test fixtures only — production nodes always go
   * through AddNode/AddNodeWithId). Lifetime-safe: the Graph outlives every
   * node it owns.
   */
  Graph* GetGraph() const { return m_graph; }
  void SetGraph(Graph* graph) { m_graph = graph; }

 private:
  int m_graphId = 0;
  Graph* m_graph = nullptr;
};

}  // namespace wpi::filterdesigner
