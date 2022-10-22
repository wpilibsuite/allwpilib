// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <thread>

#include <wpi/Synchronization.h>
#include <wpi/json.h>

#include "TestPrinters.h"
#include "ValueMatcher.h"
#include "gtest/gtest.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

class TopicListenerTest : public ::testing::Test {
 public:
  TopicListenerTest()
      : m_serverInst(nt::CreateInstance()), m_clientInst(nt::CreateInstance()) {
#if 0
    nt::AddLogger(server_inst,
                  [](const nt::LogMessage& msg) {
                    std::fprintf(stderr, "SERVER: %s\n", msg.message.c_str());
                  },
                  0, UINT_MAX);
    nt::AddLogger(client_inst,
                  [](const nt::LogMessage& msg) {
                    std::fprintf(stderr, "CLIENT: %s\n", msg.message.c_str());
                  },
                  0, UINT_MAX);
#endif
  }

  ~TopicListenerTest() override {
    nt::DestroyInstance(m_serverInst);
    nt::DestroyInstance(m_clientInst);
  }

  void Connect(unsigned int port);
  static void PublishTopics(NT_Inst inst);
  void CheckEvents(const std::vector<nt::TopicNotification>& events,
                   NT_TopicListener handle, unsigned int flags,
                   std::string_view topicName = "/foo/bar");

 protected:
  NT_Inst m_serverInst;
  NT_Inst m_clientInst;
};

void TopicListenerTest::Connect(unsigned int port) {
  nt::StartServer(m_serverInst, "topiclistenertest.json", "127.0.0.1", 0, port);
  nt::StartClient4(m_clientInst, "client");
  nt::SetServer(m_clientInst, "127.0.0.1", port);

  // Use connection listener to ensure we've connected
  NT_ConnectionListenerPoller poller =
      nt::CreateConnectionListenerPoller(m_clientInst);
  nt::AddPolledConnectionListener(poller, false);
  bool timedOut = false;
  if (!wpi::WaitForObject(poller, 1.0, &timedOut)) {
    FAIL() << "client didn't connect to server";
  }
}

void TopicListenerTest::PublishTopics(NT_Inst inst) {
  nt::Publish(nt::GetTopic(inst, "/foo/bar"), NT_DOUBLE, "double");
  nt::Publish(nt::GetTopic(inst, "/foo"), NT_DOUBLE, "double");
  nt::Publish(nt::GetTopic(inst, "/baz"), NT_DOUBLE, "double");
}

void TopicListenerTest::CheckEvents(
    const std::vector<nt::TopicNotification>& events, NT_TopicListener handle,
    unsigned int flags, std::string_view topicName) {
  ASSERT_EQ(events.size(), 1u);
  ASSERT_EQ(events[0].listener, handle);
  ASSERT_EQ(events[0].info.topic, nt::GetTopic(m_serverInst, topicName));
  ASSERT_EQ(events[0].info.name, topicName);
  ASSERT_EQ(events[0].flags, flags);
}

TEST_F(TopicListenerTest, TopicNewLocal) {
  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle = nt::AddPolledTopicListener(
      poller, nt::GetTopic(m_serverInst, "/foo"), NT_TOPIC_NOTIFY_PUBLISH);

  PublishTopics(m_serverInst);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle, NT_TOPIC_NOTIFY_PUBLISH, "/foo");
}

TEST_F(TopicListenerTest, DISABLED_TopicNewRemote) {
  Connect(10010);
  if (HasFatalFailure()) {
    return;
  }
  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle = nt::AddPolledTopicListener(
      poller, nt::GetTopic(m_serverInst, "/foo"), NT_TOPIC_NOTIFY_PUBLISH);

  PublishTopics(m_clientInst);

  nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle, NT_TOPIC_NOTIFY_PUBLISH, "/foo");
}

TEST_F(TopicListenerTest, TopicPublishImm) {
  PublishTopics(m_serverInst);

  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle = nt::AddPolledTopicListener(
      poller, nt::GetTopic(m_serverInst, "/foo"),
      NT_TOPIC_NOTIFY_PUBLISH | NT_TOPIC_NOTIFY_IMMEDIATE);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle,
              NT_TOPIC_NOTIFY_PUBLISH | NT_TOPIC_NOTIFY_IMMEDIATE, "/foo");
}

TEST_F(TopicListenerTest, TopicUnpublishPropsImm) {
  PublishTopics(m_serverInst);

  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  nt::AddPolledTopicListener(poller, nt::GetTopic(m_serverInst, "/foo"),
                             NT_TOPIC_NOTIFY_UNPUBLISH |
                                 NT_TOPIC_NOTIFY_PROPERTIES |
                                 NT_TOPIC_NOTIFY_IMMEDIATE);

  bool timedOut = false;
  ASSERT_FALSE(wpi::WaitForObject(poller, 0.02, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  ASSERT_TRUE(events.empty());
}

TEST_F(TopicListenerTest, TopicUnpublishLocal) {
  auto topic = nt::GetTopic(m_serverInst, "/foo");

  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle =
      nt::AddPolledTopicListener(poller, topic, NT_TOPIC_NOTIFY_UNPUBLISH);

  auto pub = nt::Publish(topic, NT_DOUBLE, "double");
  nt::Unpublish(pub);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle, NT_TOPIC_NOTIFY_UNPUBLISH, "/foo");
}

TEST_F(TopicListenerTest, DISABLED_TopicUnpublishRemote) {
  Connect(10010);
  if (HasFatalFailure()) {
    return;
  }
  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle = nt::AddPolledTopicListener(
      poller, nt::GetTopic(m_serverInst, "/foo"), NT_TOPIC_NOTIFY_UNPUBLISH);

  auto pub =
      nt::Publish(nt::GetTopic(m_clientInst, "/foo"), NT_DOUBLE, "double");
  nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  nt::Unpublish(pub);

  nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle, NT_TOPIC_NOTIFY_UNPUBLISH, "/foo");
}

TEST_F(TopicListenerTest, TopicPropertiesLocal) {
  auto topic = nt::GetTopic(m_serverInst, "/foo");

  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle =
      nt::AddPolledTopicListener(poller, topic, NT_TOPIC_NOTIFY_PROPERTIES);

  nt::SetTopicProperty(topic, "foo", 5);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle, NT_TOPIC_NOTIFY_PROPERTIES, "/foo");
}

TEST_F(TopicListenerTest, DISABLED_TopicPropertiesRemote) {
  Connect(10010);
  if (HasFatalFailure()) {
    return;
  }
  // the topic needs to actually exist
  nt::Publish(nt::GetTopic(m_serverInst, "/foo"), NT_BOOLEAN, "boolean");

  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle = nt::AddPolledTopicListener(
      poller, nt::GetTopic(m_serverInst, "/foo"), NT_TOPIC_NOTIFY_PROPERTIES);
  nt::FlushLocal(m_serverInst);

  nt::SetTopicProperty(nt::GetTopic(m_clientInst, "/foo"), "foo", 5);
  nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle, NT_TOPIC_NOTIFY_PROPERTIES, "/foo");
}

TEST_F(TopicListenerTest, PrefixPublishLocal) {
  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle =
      nt::AddPolledTopicListener(poller, {{"/foo/"}}, NT_TOPIC_NOTIFY_PUBLISH);

  PublishTopics(m_serverInst);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle, NT_TOPIC_NOTIFY_PUBLISH);
}

TEST_F(TopicListenerTest, DISABLED_PrefixPublishRemote) {
  Connect(10011);
  if (HasFatalFailure()) {
    return;
  }
  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle =
      nt::AddPolledTopicListener(poller, {{"/foo/"}}, NT_TOPIC_NOTIFY_PUBLISH);

  PublishTopics(m_clientInst);

  nt::Flush(m_clientInst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle, NT_TOPIC_NOTIFY_PUBLISH);
}

TEST_F(TopicListenerTest, PrefixPublishImm) {
  PublishTopics(m_serverInst);

  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  auto handle = nt::AddPolledTopicListener(
      poller, {{"/foo/"}}, NT_TOPIC_NOTIFY_PUBLISH | NT_TOPIC_NOTIFY_IMMEDIATE);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  CheckEvents(events, handle,
              NT_TOPIC_NOTIFY_PUBLISH | NT_TOPIC_NOTIFY_IMMEDIATE);
}

TEST_F(TopicListenerTest, PrefixUnpublishPropsImm) {
  PublishTopics(m_serverInst);

  auto poller = nt::CreateTopicListenerPoller(m_serverInst);
  nt::AddPolledTopicListener(poller, {{"/foo/"}},
                             NT_TOPIC_NOTIFY_UNPUBLISH |
                                 NT_TOPIC_NOTIFY_PROPERTIES |
                                 NT_TOPIC_NOTIFY_IMMEDIATE);

  bool timedOut = false;
  ASSERT_FALSE(wpi::WaitForObject(poller, 0.02, &timedOut));
  auto events = nt::ReadTopicListenerQueue(poller);
  ASSERT_TRUE(events.empty());
}
