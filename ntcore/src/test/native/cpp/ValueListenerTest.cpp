// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/StringExtras.h>
#include <wpi/Synchronization.h>

#include "TestPrinters.h"
#include "ValueMatcher.h"
#include "gtest/gtest.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::IsNull;
using ::testing::Return;

namespace nt {

// Test only local here; it's more reliable to mock the network
class ValueListenerTest : public ::testing::Test {
 public:
  ValueListenerTest() : m_inst{nt::CreateInstance()} {
    nt::SetNetworkIdentity(m_inst, "server");
  }

  ~ValueListenerTest() override { nt::DestroyInstance(m_inst); }

 protected:
  NT_Inst m_inst;
};

TEST_F(ValueListenerTest, MultiPollSub) {
  auto topic = nt::GetTopic(m_inst, "foo");
  auto pub = nt::Publish(topic, NT_DOUBLE, "double");
  auto sub = nt::Subscribe(topic, NT_DOUBLE, "double");

  auto poller1 = nt::CreateValueListenerPoller(m_inst);
  auto poller2 = nt::CreateValueListenerPoller(m_inst);
  auto poller3 = nt::CreateValueListenerPoller(m_inst);
  auto h1 = nt::AddPolledValueListener(poller1, sub, NT_VALUE_NOTIFY_LOCAL);
  auto h2 = nt::AddPolledValueListener(poller2, sub, NT_VALUE_NOTIFY_LOCAL);
  auto h3 = nt::AddPolledValueListener(poller3, sub, NT_VALUE_NOTIFY_LOCAL);

  nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller1, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  ASSERT_TRUE(wpi::WaitForObject(poller2, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  ASSERT_TRUE(wpi::WaitForObject(poller3, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results1 = nt::ReadValueListenerQueue(poller1);
  auto results2 = nt::ReadValueListenerQueue(poller2);
  auto results3 = nt::ReadValueListenerQueue(poller3);

  ASSERT_EQ(results1.size(), 1u);
  EXPECT_EQ(results1[0].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results1[0].listener, h1);
  EXPECT_EQ(results1[0].subentry, sub);
  EXPECT_EQ(results1[0].topic, topic);
  EXPECT_EQ(results1[0].value, nt::Value::MakeDouble(0.0));

  ASSERT_EQ(results2.size(), 1u);
  EXPECT_EQ(results2[0].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results2[0].listener, h2);
  EXPECT_EQ(results2[0].subentry, sub);
  EXPECT_EQ(results2[0].topic, topic);
  EXPECT_EQ(results2[0].value, nt::Value::MakeDouble(0.0));

  ASSERT_EQ(results3.size(), 1u);
  EXPECT_EQ(results3[0].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results3[0].listener, h3);
  EXPECT_EQ(results3[0].subentry, sub);
  EXPECT_EQ(results3[0].topic, topic);
  EXPECT_EQ(results3[0].value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollMultiSub) {
  auto topic = nt::GetTopic(m_inst, "foo");
  auto pub = nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = nt::Subscribe(topic, NT_DOUBLE, "double");

  auto poller = nt::CreateValueListenerPoller(m_inst);
  auto h1 = nt::AddPolledValueListener(poller, sub1, NT_VALUE_NOTIFY_LOCAL);
  auto h2 = nt::AddPolledValueListener(poller, sub2, NT_VALUE_NOTIFY_LOCAL);

  nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadValueListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results[0].listener, h1);
  EXPECT_EQ(results[0].subentry, sub1);
  EXPECT_EQ(results[0].topic, topic);
  EXPECT_EQ(results[0].value, nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results[1].listener, h2);
  EXPECT_EQ(results[1].subentry, sub2);
  EXPECT_EQ(results[1].topic, topic);
  EXPECT_EQ(results[1].value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollMultiSubTopic) {
  auto topic1 = nt::GetTopic(m_inst, "foo");
  auto topic2 = nt::GetTopic(m_inst, "bar");
  auto pub1 = nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub1 = nt::Subscribe(topic1, NT_DOUBLE, "double");
  auto sub2 = nt::Subscribe(topic2, NT_DOUBLE, "double");

  auto poller = nt::CreateValueListenerPoller(m_inst);
  auto h1 = nt::AddPolledValueListener(poller, sub1, NT_VALUE_NOTIFY_LOCAL);
  auto h2 = nt::AddPolledValueListener(poller, sub2, NT_VALUE_NOTIFY_LOCAL);

  nt::SetDouble(pub1, 0);
  nt::SetDouble(pub2, 1);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadValueListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results[0].listener, h1);
  EXPECT_EQ(results[0].subentry, sub1);
  EXPECT_EQ(results[0].topic, topic1);
  EXPECT_EQ(results[0].value, nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results[1].listener, h2);
  EXPECT_EQ(results[1].subentry, sub2);
  EXPECT_EQ(results[1].topic, topic2);
  EXPECT_EQ(results[1].value, nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, PollSubMultiple) {
  auto topic1 = nt::GetTopic(m_inst, "foo/1");
  auto topic2 = nt::GetTopic(m_inst, "foo/2");
  auto pub1 = nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub = nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = nt::CreateValueListenerPoller(m_inst);
  auto h = nt::AddPolledValueListener(poller, sub, NT_VALUE_NOTIFY_LOCAL);

  nt::SetDouble(pub1, 0);
  nt::SetDouble(pub2, 1);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadValueListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results[0].listener, h);
  EXPECT_EQ(results[0].subentry, sub);
  EXPECT_EQ(results[0].topic, topic1);
  EXPECT_EQ(results[0].value, nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results[1].listener, h);
  EXPECT_EQ(results[1].subentry, sub);
  EXPECT_EQ(results[1].topic, topic2);
  EXPECT_EQ(results[1].value, nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, PollEntry) {
  auto entry = nt::GetEntry(m_inst, "foo");

  auto poller = nt::CreateValueListenerPoller(m_inst);
  auto h = nt::AddPolledValueListener(poller, entry, NT_VALUE_NOTIFY_LOCAL);

  ASSERT_TRUE(nt::SetDouble(entry, 0));

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadValueListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results[0].listener, h);
  EXPECT_EQ(results[0].subentry, entry);
  EXPECT_EQ(results[0].topic, nt::GetTopic(m_inst, "foo"));
  EXPECT_EQ(results[0].value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollImmediate) {
  auto entry = nt::GetEntry(m_inst, "foo");
  ASSERT_TRUE(nt::SetDouble(entry, 0));

  auto poller = nt::CreateValueListenerPoller(m_inst);
  auto h = nt::AddPolledValueListener(
      poller, entry, NT_VALUE_NOTIFY_LOCAL | NT_VALUE_NOTIFY_IMMEDIATE);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadValueListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags, NT_VALUE_NOTIFY_IMMEDIATE);
  EXPECT_EQ(results[0].listener, h);
  EXPECT_EQ(results[0].subentry, entry);
  EXPECT_EQ(results[0].topic, nt::GetTopic(m_inst, "foo"));
  EXPECT_EQ(results[0].value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollImmediateNoValue) {
  auto entry = nt::GetEntry(m_inst, "foo");

  auto poller = nt::CreateValueListenerPoller(m_inst);
  auto h = nt::AddPolledValueListener(
      poller, entry, NT_VALUE_NOTIFY_LOCAL | NT_VALUE_NOTIFY_IMMEDIATE);

  bool timedOut = false;
  ASSERT_FALSE(wpi::WaitForObject(poller, 0.02, &timedOut));
  ASSERT_TRUE(timedOut);
  auto results = nt::ReadValueListenerQueue(poller);
  ASSERT_TRUE(results.empty());

  // now set a value
  ASSERT_TRUE(nt::SetDouble(entry, 0));

  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  results = nt::ReadValueListenerQueue(poller);
  ASSERT_FALSE(timedOut);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags, NT_VALUE_NOTIFY_LOCAL);
  EXPECT_EQ(results[0].listener, h);
  EXPECT_EQ(results[0].subentry, entry);
  EXPECT_EQ(results[0].topic, nt::GetTopic(m_inst, "foo"));
  EXPECT_EQ(results[0].value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollImmediateSubMultiple) {
  auto topic1 = nt::GetTopic(m_inst, "foo/1");
  auto topic2 = nt::GetTopic(m_inst, "foo/2");
  auto pub1 = nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub = nt::SubscribeMultiple(m_inst, {{"foo"}});
  nt::SetDouble(pub1, 0);
  nt::SetDouble(pub2, 1);

  auto poller = nt::CreateValueListenerPoller(m_inst);
  auto h = nt::AddPolledValueListener(
      poller, sub, NT_VALUE_NOTIFY_LOCAL | NT_VALUE_NOTIFY_IMMEDIATE);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadValueListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, NT_VALUE_NOTIFY_IMMEDIATE);
  EXPECT_EQ(results[0].listener, h);
  EXPECT_EQ(results[0].subentry, sub);
  EXPECT_EQ(results[0].topic, topic1);
  EXPECT_EQ(results[0].value, nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, NT_VALUE_NOTIFY_IMMEDIATE);
  EXPECT_EQ(results[1].listener, h);
  EXPECT_EQ(results[1].subentry, sub);
  EXPECT_EQ(results[1].topic, topic2);
  EXPECT_EQ(results[1].value, nt::Value::MakeDouble(1.0));
}

}  // namespace nt
