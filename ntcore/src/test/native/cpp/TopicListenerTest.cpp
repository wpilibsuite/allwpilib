// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "TestPrinters.hpp"
#include "ValueMatcher.hpp"
#include "wpi/nt/ntcore_c.h"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/Synchronization.hpp"

class TopicListenerTest : public ::testing::Test {
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

  ~TopicListenerTest() override {
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
                             wpi::nt::EventFlags::kConnected);
  bool timedOut = false;
  if (!wpi::util::WaitForObject(poller, 1.0, &timedOut)) {
    FAIL() << "client didn't connect to server";
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
  ASSERT_EQ(events.size(), 1u);
  ASSERT_EQ(events[0].listener, handle);
  ASSERT_EQ(events[0].flags, flags);
  auto topicInfo = events[0].GetTopicInfo();
  ASSERT_TRUE(topicInfo);
  ASSERT_EQ(topicInfo->topic, wpi::nt::GetTopic(m_serverInst, topicName));
  ASSERT_EQ(topicInfo->name, topicName);
}

TEST_F(TopicListenerTest, TopicNewLocal) {
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::kPublish);

  PublishTopics(m_serverInst);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::kPublish, "/foo");
}

TEST_F(TopicListenerTest, DISABLED_TopicNewRemote) {
  Connect(10010);
  if (HasFatalFailure()) {
    return;
  }
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::kPublish);

  PublishTopics(m_clientInst);

  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::kPublish, "/foo");
}

TEST_F(TopicListenerTest, TopicPublishImm) {
  PublishTopics(m_serverInst);

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::kPublish | wpi::nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle,
              wpi::nt::EventFlags::kPublish | wpi::nt::EventFlags::kImmediate,
              "/foo");
}

TEST_F(TopicListenerTest, TopicUnpublishPropsImm) {
  PublishTopics(m_serverInst);

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  wpi::nt::AddPolledListener(poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
                             wpi::nt::EventFlags::kUnpublish |
                                 wpi::nt::EventFlags::kProperties |
                                 wpi::nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_FALSE(wpi::util::WaitForObject(poller, 0.02, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  ASSERT_TRUE(events.empty());
}

TEST_F(TopicListenerTest, TopicUnpublishLocal) {
  auto topic = wpi::nt::GetTopic(m_serverInst, "/foo");

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(poller, topic,
                                           wpi::nt::EventFlags::kUnpublish);

  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  wpi::nt::Unpublish(pub);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::kUnpublish, "/foo");
}

TEST_F(TopicListenerTest, DISABLED_TopicUnpublishRemote) {
  Connect(10010);
  if (HasFatalFailure()) {
    return;
  }
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::kUnpublish);

  auto pub = wpi::nt::Publish(wpi::nt::GetTopic(m_clientInst, "/foo"),
                              NT_DOUBLE, "double");
  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  wpi::nt::Unpublish(pub);

  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::kUnpublish, "/foo");
}

TEST_F(TopicListenerTest, TopicPropertiesLocal) {
  auto topic = wpi::nt::GetTopic(m_serverInst, "/foo");

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(poller, topic,
                                           wpi::nt::EventFlags::kProperties);

  wpi::nt::SetTopicProperty(topic, "foo", 5);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::kProperties, "/foo");
}

TEST_F(TopicListenerTest, DISABLED_TopicPropertiesRemote) {
  Connect(10010);
  if (HasFatalFailure()) {
    return;
  }
  // the topic needs to actually exist
  wpi::nt::Publish(wpi::nt::GetTopic(m_serverInst, "/foo"), NT_BOOLEAN,
                   "boolean");

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, wpi::nt::GetTopic(m_serverInst, "/foo"),
      wpi::nt::EventFlags::kProperties);
  wpi::nt::FlushLocal(m_serverInst);

  wpi::nt::SetTopicProperty(wpi::nt::GetTopic(m_clientInst, "/foo"), "foo", 5);
  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::kProperties, "/foo");
}

TEST_F(TopicListenerTest, PrefixPublishLocal) {
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(poller, {{"/foo/"}},
                                           wpi::nt::EventFlags::kPublish);

  PublishTopics(m_serverInst);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::kPublish);
}

TEST_F(TopicListenerTest, DISABLED_PrefixPublishRemote) {
  Connect(10011);
  if (HasFatalFailure()) {
    return;
  }
  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(poller, {{"/foo/"}},
                                           wpi::nt::EventFlags::kPublish);

  PublishTopics(m_clientInst);

  wpi::nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle, wpi::nt::EventFlags::kPublish);
}

TEST_F(TopicListenerTest, PrefixPublishImm) {
  PublishTopics(m_serverInst);

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  auto handle = wpi::nt::AddPolledListener(
      poller, {{"/foo/"}},
      wpi::nt::EventFlags::kPublish | wpi::nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  CheckEvents(events, handle,
              wpi::nt::EventFlags::kPublish | wpi::nt::EventFlags::kImmediate);
}

TEST_F(TopicListenerTest, PrefixUnpublishPropsImm) {
  PublishTopics(m_serverInst);

  auto poller = wpi::nt::CreateListenerPoller(m_serverInst);
  wpi::nt::AddPolledListener(poller, {{"/foo/"}},
                             wpi::nt::EventFlags::kUnpublish |
                                 wpi::nt::EventFlags::kProperties |
                                 wpi::nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_FALSE(wpi::util::WaitForObject(poller, 0.02, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);
  ASSERT_TRUE(events.empty());
}
