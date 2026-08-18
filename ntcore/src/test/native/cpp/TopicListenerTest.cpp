// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

#include "TestPrinters.hpp"
#undef FAIL
#undef SUCCEED
#include <catch2/catch_test_macros.hpp>

#include "wpi/nt/ntcore_c.h"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/Synchronization.hpp"
#include "wpi/util/json.hpp"

class TopicListenerTest {
 public:
  TopicListenerTest()
      : m_serverInst(wpi::nt::CreateInstance()),
        m_clientInst(wpi::nt::CreateInstance()) {
#if 0
    wpi::nt::AddLogger(m_serverInst, 0, UINT_MAX, [](auto& event) {
      if (auto msg = event.GetLogMessage()) {
        std::fprintf(stderr, "SERVER: %s\n", msg->message.c_str());
      }
    });
    wpi::nt::AddLogger(m_clientInst, 0, UINT_MAX, [](auto& event) {
      if (auto msg = event.GetLogMessage()) {
        std::fprintf(stderr, "CLIENT: %s\n", msg.message.c_str());
      }
    });
#endif
  }

  ~TopicListenerTest() {
    wpi::nt::DestroyInstance(m_serverInst);
    wpi::nt::DestroyInstance(m_clientInst);
  }

  void Connect(unsigned int port);
  static void PublishTopics(NT_Inst inst);
  void CheckEvents(const std::vector<wpi::nt::Event>& events,
                   NT_Listener handle, unsigned int flags,
                   std::string_view topicName = "/foo/bar");

 protected:
  NT_Inst m_serverInst;
  NT_Inst m_clientInst;
};

void TopicListenerTest::Connect(unsigned int port) {
  wpi::nt::StartServer(m_serverInst, "topiclistenertest.json", "127.0.0.1", "",
                       port);
  wpi::nt::StartClient(m_clientInst, "client");
  wpi::nt::SetServer(m_clientInst, "127.0.0.1", port);

  // Use connection listener to ensure we've connected
  NT_ListenerPoller poller = wpi::nt::CreateListenerPoller(m_clientInst);
  wpi::nt::AddPolledListener(poller, m_clientInst,
                             wpi::nt::EventFlags::CONNECTED);
  bool timedOut = false;
  if (!wpi::util::WaitForObject(poller, 1.0, &timedOut)) {
    FAIL("client didn't connect to server");
  }
}

void TopicListenerTest::PublishTopics(NT_Inst inst) {
  wpi::nt::Publish(wpi::nt::GetTopic(inst, "/foo/bar"), NT_DOUBLE, "double");
  wpi::nt::Publish(wpi::nt::GetTopic(inst, "/foo"), NT_DOUBLE, "double");
  wpi::nt::Publish(wpi::nt::GetTopic(inst, "/baz"), NT_DOUBLE, "double");
}

void TopicListenerTest::CheckEvents(const std::vector<wpi::nt::Event>& events,
                                    NT_Listener handle, unsigned int flags,
                                    std::string_view topicName) {
  REQUIRE(events.size() == 1u);
  REQUIRE(events[0].listener == handle);
  REQUIRE(events[0].flags == flags);
  auto topicInfo = events[0].GetTopicInfo();
  REQUIRE(topicInfo);
  REQUIRE(topicInfo->topic == wpi::nt::GetTopic(m_serverInst, topicName));
  REQUIRE(topicInfo->name == topicName);
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest TopicNewLocal",
                 "[ntcore][topic-listener]") {
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::PUBLISH);

  PublishTopics(m_serverInst);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::PUBLISH, "/foo");
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest TopicNewRemote",
                 "[ntcore][topic-listener][.]") {
  Connect(10010);
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::PUBLISH);

  PublishTopics(m_clientInst);

  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::PUBLISH, "/foo");
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest TopicPublishImm",
                 "[ntcore][topic-listener]") {
  PublishTopics(m_serverInst);

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::PUBLISH | wpi::nt::EventFlags::IMMEDIATE);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle,
              wpi::nt::EventFlags::PUBLISH | wpi::nt::EventFlags::IMMEDIATE,
              "/foo");
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest TopicUnpublishPropsImm",
                 "[ntcore][topic-listener]") {
  PublishTopics(m_serverInst);

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  wpi::nt::AddPolledListener(poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
                             wpi::nt::EventFlags::UNPUBLISH |
                                 wpi::nt::EventFlags::PROPERTIES |
                                 wpi::nt::EventFlags::IMMEDIATE);

  bool timedOut = false;
  REQUIRE_FALSE(wpi::util::WaitForObject(poller, 0.02, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  REQUIRE(events.empty());
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest TopicUnpublishLocal",
                 "[ntcore][topic-listener]") {
  auto topic = wpi::nt::GetTopic(m_serverInst, "/foo");

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle =
      wpi::nt::AddPolledListener(poller, topic, wpi::nt::EventFlags::UNPUBLISH);

  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  wpi::nt::Unpublish(pub);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::UNPUBLISH, "/foo");
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest TopicUnpublishRemote",
                 "[ntcore][topic-listener][.]") {
  Connect(10010);
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::UNPUBLISH);

  auto pub = wpi::nt::Publish(wpi::nt::GetTopic(m_clientInst, "/foo"),
                              NT_DOUBLE, "double");
  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  wpi::nt::Unpublish(pub);

  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::UNPUBLISH, "/foo");
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest TopicPropertiesLocal",
                 "[ntcore][topic-listener]") {
  auto topic = wpi::nt::GetTopic(m_serverInst, "/foo");

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(poller, topic,
                                           wpi::nt::EventFlags::PROPERTIES);

  wpi::nt::SetTopicProperty(topic, "foo", wpi::util::json{5});

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::PROPERTIES, "/foo");
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest TopicPropertiesRemote",
                 "[ntcore][topic-listener][.]") {
  Connect(10010);
  // the topic needs to actually exist
  wpi::nt::Publish(wpi::nt::GetTopic(m_serverInst, "/foo"), NT_BOOLEAN,
                   "boolean");

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::PROPERTIES);
  wpi::nt::FlushLocal(m_serverInst);

  wpi::nt::SetTopicProperty(wpi::nt::GetTopic(m_clientInst, "/foo"), "foo",
                            wpi::util::json{5});
  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::PROPERTIES, "/foo");
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest PrefixPublishLocal",
                 "[ntcore][topic-listener]") {
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(poller, {{"/foo/"}},
                                           wpi::nt::EventFlags::PUBLISH);

  PublishTopics(m_serverInst);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::PUBLISH);
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest PrefixPublishRemote",
                 "[ntcore][topic-listener][.]") {
  Connect(10011);
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(poller, {{"/foo/"}},
                                           wpi::nt::EventFlags::PUBLISH);

  PublishTopics(m_clientInst);

  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::PUBLISH);
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest PrefixPublishImm",
                 "[ntcore][topic-listener]") {
  PublishTopics(m_serverInst);

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, {{"/foo/"}},
      wpi::nt::EventFlags::PUBLISH | wpi::nt::EventFlags::IMMEDIATE);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle,
              wpi::nt::EventFlags::PUBLISH | wpi::nt::EventFlags::IMMEDIATE);
}

TEST_CASE_METHOD(TopicListenerTest, "TopicListenerTest PrefixUnpublishPropsImm",
                 "[ntcore][topic-listener]") {
  PublishTopics(m_serverInst);

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  wpi::nt::AddPolledListener(poller, {{"/foo/"}},
                             wpi::nt::EventFlags::UNPUBLISH |
                                 wpi::nt::EventFlags::PROPERTIES |
                                 wpi::nt::EventFlags::IMMEDIATE);

  bool timedOut = false;
  REQUIRE_FALSE(wpi::util::WaitForObject(poller, 0.02, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  REQUIRE(events.empty());
}
