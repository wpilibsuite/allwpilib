// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "wpi/filterdesigner/io/NT4Source.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/nt/MultiSubscriber.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/NetworkTableListener.hpp"
#include "wpi/nt/Topic.hpp"

namespace wpi::filterdesigner {

/**
 * ImGui view for a live NetworkTables data source. Owns its own
 * NetworkTableInstance so this tool doesn't fight with other consumers of
 * the default instance.
 *
 * The view exposes the current topic's recent samples as a pointer-stable
 * @ref Signal (see @ref SelectedSignal), matching the same contract the
 * WPILOG loader offers.
 */
class NT4SourceView {
 public:
  NT4SourceView();
  ~NT4SourceView();

  NT4SourceView(const NT4SourceView&) = delete;
  NT4SourceView& operator=(const NT4SourceView&) = delete;

  /** Draw the view. Must be inside an ImGui window. */
  void Display();

  /**
   * Latest buffered samples. Pointer is stable for this view's lifetime;
   * returns nullptr when no topic is subscribed or no samples have arrived
   * yet on the current subscription.
   */
  const Signal* SelectedSignal() const;

 private:
  enum class ServerMode { Team, Host };

  void StartClient();
  void StopClient();
  void Subscribe(const std::string& topicName);
  void Unsubscribe();
  void RefreshTopics();

  wpi::nt::NetworkTableInstance m_inst;
  bool m_clientStarted = false;

  // Server settings.
  ServerMode m_serverMode = ServerMode::Host;
  std::string m_host = "127.0.0.1";
  int m_team = 0;
  int m_port = NT_DEFAULT_PORT;

  // Topic selection.
  struct TopicEntry {
    std::string name;
    std::string type;
  };
  std::vector<TopicEntry> m_topics;
  std::string m_selectedTopic;

  // Subscriber is held as a unique_ptr so we can tear down in StopClient
  // without leaving a dangling handle on the instance we're about to stop.
  std::unique_ptr<wpi::nt::GenericSubscriber> m_sub;

  // Discovery subscription. NT4 servers don't announce topics to a client
  // unless a subscriber matches them, so without this the topic dropdown
  // stays empty no matter what's published. Mirrors the {"", "$"} prefix
  // pair Glass / OutlineViewer use; topicsOnly keeps value traffic off this
  // sub since m_sub already handles values for the selected topic.
  wpi::nt::MultiSubscriber m_topicSub;
  wpi::nt::NetworkTableListenerPoller m_topicPoller;
  NT_Listener m_topicListener = 0;

  // StopClient blocks while the connection thread cleans up an in-flight
  // attempt — can take seconds. Run it off the UI thread. The atomic flag
  // lets Display gate the Connect button without joining (which would
  // re-block the UI). The thread is joined in the destructor or once the
  // flag clears and the user reconnects.
  std::thread m_stopThread;
  std::atomic<bool> m_stopInProgress{false};

  NT4Source m_source;
};

}  // namespace wpi::filterdesigner
