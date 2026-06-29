// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/nodes/BiquadStageNodeLogic.hpp"
#include "wpi/util/json.hpp"

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Single-stage filter design node. Designs one biquad cascade from the
 * familiar (kind, family, order, cutoffs) parameters and exposes it on two
 * output pins:
 *
 *   - "filter" (const DesignedFilter*) — the **cumulative** cascade through
 *     this stage in the Signal chain. If the @c in (Signal) pin is wired to
 *     another BiquadStageNode, this stage's biquads are appended to that
 *     upstream stage's cumulative cascade. Mirrors how the Signal pass-
 *     through already composes: `Src → A → B → CodeGen` exports A+B.
 *   - "signal" (const Signal*) — the optional input Signal filtered through
 *     just this stage's cascade. The chain composes naturally because each
 *     stage filters its already-filtered input.
 *
 * Pin shape: in (const Signal*) → out filter, out signal
 *
 * Pure design state lives in @ref BiquadStageNodeLogic; the cumulative
 * cascade is cached on the node because the walk depends on graph
 * topology, which @c BiquadStageNodeLogic deliberately knows nothing about.
 */
class BiquadStageNode final : public FilterDesignerNode {
 public:
  BiquadStageNode();
  ~BiquadStageNode() override;

  std::string_view TypeTag() const override { return "BiquadStage"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const BiquadStageNodeLogic& Logic() const { return *m_logic; }
  BiquadStageNodeLogic& Logic() { return *m_logic; }

  /**
   * Returns the cumulative cascade through this stage, walking the @c in
   * Signal pin upstream and concatenating any BiquadStageNode it finds.
   * Returns nullptr when this stage's own design is invalid; in that case
   * @ref CombinedError carries a human-readable reason. Also returns
   * nullptr (with an error message) on sample-rate mismatch between
   * upstream and this stage, or if the upstream walk exceeds the cycle-
   * guard depth. Defense-in-depth: the Graph-level cycle detector usually
   * stops sinks from pulling before this walk runs, but the depth cap also
   * covers callers that walk upstream directly.
   *
   * Pointer is stable across calls; only contents change.
   */
  const DesignedFilter* CombinedFilter() const;

  /** Error message from the last @ref CombinedFilter call, or empty. */
  const std::string& CombinedError() const { return m_combinedError; }

  /**
   * Returns the upstream BiquadStageNode's @ref CombinedError when @p inPin
   * is wired to one and that node is in an error state, otherwise empty.
   * Sink nodes (CodeGen, Export, Bode, PoleZero) call this when their input
   * filter is null to distinguish "no input wired" from "input wired but the
   * upstream cascade errored" — without it the sinks would render the
   * same misleading "Connect a Filter…" placeholder in both cases.
   */
  static std::string UpstreamErrorFor(ImFlow::Pin* inPin);

 private:
  /** Returns the upstream node feeding `in`, or nullptr if not a stage. */
  const BiquadStageNode* UpstreamStage() const;

  /** Implementation; @p depth is the recursion depth used by the cycle guard.
   */
  const DesignedFilter* CombinedFilterImpl(int depth) const;

  // Held by unique_ptr so the OutPin behaviour lambdas can safely capture a
  // raw pointer — non-copyable, stable address across the node's lifetime.
  std::unique_ptr<BiquadStageNodeLogic> m_logic;

  // Cache for CombinedFilter(). Pointer-stable across calls; cache key is
  // (upstream cumulative pointer, upstream cumulative version, this stage's
  // FilterVersion). Mutable because CombinedFilter is logically const but
  // populates the cache lazily, same pattern as BiquadStageNodeLogic.
  mutable std::optional<DesignedFilter> m_combinedCache;
  mutable const DesignedFilter* m_lastUpstreamFilter = nullptr;
  mutable std::uint64_t m_lastUpstreamVersion = 0;
  mutable std::uint64_t m_lastSelfVersion = 0;
  mutable bool m_haveCombined = false;
  mutable std::uint64_t m_combinedVersion = 0;
  mutable std::string m_combinedError;
};

}  // namespace wpi::filterdesigner
