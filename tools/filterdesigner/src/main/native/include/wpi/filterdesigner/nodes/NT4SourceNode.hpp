// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "wpi/filterdesigner/graph/FilterDesignerNode.hpp"
#include "wpi/filterdesigner/nodes/NT4SourceNodeLogic.hpp"
#include "wpi/filterdesigner/nodes/NameTree.hpp"
#include "wpi/nt/MultiSubscriber.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/NetworkTableListener.hpp"
#include "wpi/util/json.hpp"

namespace wpi::nt {
class GenericSubscriber;
}  // namespace wpi::nt

namespace wpi::filterdesigner {

class NodeRegistry;

/**
 * Source node that subscribes to a numeric NetworkTables 4 topic and exposes
 * a rolling window of recent samples on its "out" pin as a
 * @c const Signal*. This class owns the NT subscription; config and
 * ring-buffer state live in @ref NT4SourceNodeLogic so tests can run without
 * ntcore.
 *
 * Pin shape: — → out (const Signal*)
 *
 * The wrapped @c NetworkTableInstance is created on the first Connect, so
 * constructing the node allocates no ntcore state.
 */
class NT4SourceNode final : public FilterDesignerNode {
 public:
  NT4SourceNode();
  ~NT4SourceNode() override;

  std::string_view TypeTag() const override { return "NT4Source"; }
  void SerializeParams(wpi::util::json& obj) const override;
  void DeserializeParams(const wpi::util::json& obj) override;

  void draw() override;

  static void Register(NodeRegistry& registry);

  const NT4SourceNodeLogic& Logic() const { return *m_logic; }
  NT4SourceNodeLogic& Logic() { return *m_logic; }

  /** True once the wrapped @c NetworkTableInstance has been allocated. */
  bool IsInstanceCreated() const { return static_cast<bool>(m_inst); }

 private:
  void StartClient();
  void StopClient();
  void Subscribe(std::string_view topicName);
  void Unsubscribe();
  void RefreshTopics();

  struct TopicEntry {
    std::string name;
    std::string type;
  };

  /** Rebuilds @ref m_topicTree from the flat @ref m_topics list. */
  void RebuildTopicTree();

  // Held by unique_ptr so the OutPin behaviour + drain lambdas can capture a
  // raw pointer with stable address.
  std::unique_ptr<NT4SourceNodeLogic> m_logic;

  wpi::nt::NetworkTableInstance m_inst;
  bool m_clientStarted = false;

  std::vector<TopicEntry> m_topics;
  NameTreeNode m_topicTree;
  std::string m_topicSearch;

  // Subscriber for the selected topic. Held by unique_ptr so we can drop it
  // cleanly during Unsubscribe / StopClient without leaving a dangling handle
  // on the instance we're about to Destroy.
  std::unique_ptr<wpi::nt::GenericSubscriber> m_sub;

  // Discovery subscription. NT4 servers won't announce topics until at least
  // one client has subscribed to a matching prefix, so a wildcard subscription
  // ({"", "$"}) is the standard trick to populate the topic combo. Mirrors
  // what Glass / OutlineViewer do.
  wpi::nt::MultiSubscriber m_topicSub;
  wpi::nt::NetworkTableListenerPoller m_topicPoller;
  NT_Listener m_topicListener = 0;

  // StopClient blocks while the connection thread cleans up — run it off the
  // UI thread. m_stopInProgress gates the Connect button without joining.
  std::thread m_stopThread;
  std::atomic<bool> m_stopInProgress{false};
};

}  // namespace wpi::filterdesigner
