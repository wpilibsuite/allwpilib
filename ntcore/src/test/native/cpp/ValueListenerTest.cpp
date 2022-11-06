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
  ValueListenerTest() : m_inst{nt::CreateInstance()} {}

  ~ValueListenerTest() override { nt::DestroyInstance(m_inst); }

 protected:
  NT_Inst m_inst;
};

TEST_F(ValueListenerTest, MultiPollSub) {
  auto topic = nt::GetTopic(m_inst, "foo");
  auto pub = nt::Publish(topic, NT_DOUBLE, "double");
  auto sub = nt::Subscribe(topic, NT_DOUBLE, "double");

  auto poller1 = nt::CreateListenerPoller(m_inst);
  auto poller2 = nt::CreateListenerPoller(m_inst);
  auto poller3 = nt::CreateListenerPoller(m_inst);
  auto h1 = nt::AddPolledListener(poller1, sub, nt::EventFlags::kValueLocal);
  auto h2 = nt::AddPolledListener(poller2, sub, nt::EventFlags::kValueLocal);
  auto h3 = nt::AddPolledListener(poller3, sub, nt::EventFlags::kValueLocal);

  nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller1, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  ASSERT_TRUE(wpi::WaitForObject(poller2, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  ASSERT_TRUE(wpi::WaitForObject(poller3, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results1 = nt::ReadListenerQueue(poller1);
  auto results2 = nt::ReadListenerQueue(poller2);
  auto results3 = nt::ReadListenerQueue(poller3);

  ASSERT_EQ(results1.size(), 1u);
  EXPECT_EQ(results1[0].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results1[0].listener, h1);
  auto valueData = results1[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));

  ASSERT_EQ(results2.size(), 1u);
  EXPECT_EQ(results2[0].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results2[0].listener, h2);
  valueData = results2[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));

  ASSERT_EQ(results3.size(), 1u);
  EXPECT_EQ(results3[0].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results3[0].listener, h3);
  valueData = results3[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollMultiSub) {
  auto topic = nt::GetTopic(m_inst, "foo");
  auto pub = nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = nt::Subscribe(topic, NT_DOUBLE, "double");

  auto poller = nt::CreateListenerPoller(m_inst);
  auto h1 = nt::AddPolledListener(poller, sub1, nt::EventFlags::kValueLocal);
  auto h2 = nt::AddPolledListener(poller, sub2, nt::EventFlags::kValueLocal);

  nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h1);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub1);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[1].listener, h2);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub2);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollMultiSubTopic) {
  auto topic1 = nt::GetTopic(m_inst, "foo");
  auto topic2 = nt::GetTopic(m_inst, "bar");
  auto pub1 = nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub1 = nt::Subscribe(topic1, NT_DOUBLE, "double");
  auto sub2 = nt::Subscribe(topic2, NT_DOUBLE, "double");

  auto poller = nt::CreateListenerPoller(m_inst);
  auto h1 = nt::AddPolledListener(poller, sub1, nt::EventFlags::kValueLocal);
  auto h2 = nt::AddPolledListener(poller, sub2, nt::EventFlags::kValueLocal);

  nt::SetDouble(pub1, 0);
  nt::SetDouble(pub2, 1);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h1);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub1);
  EXPECT_EQ(valueData->topic, topic1);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[1].listener, h2);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub2);
  EXPECT_EQ(valueData->topic, topic2);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, PollSubMultiple) {
  auto topic1 = nt::GetTopic(m_inst, "foo/1");
  auto topic2 = nt::GetTopic(m_inst, "foo/2");
  auto pub1 = nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub = nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = nt::CreateListenerPoller(m_inst);
  auto h = nt::AddPolledListener(poller, sub, nt::EventFlags::kValueLocal);

  nt::SetDouble(pub1, 0);
  nt::SetDouble(pub2, 1);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic1);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[1].listener, h);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic2);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, PollSubPrefixCreated) {
  auto poller = nt::CreateListenerPoller(m_inst);
  auto h =
      nt::AddPolledListener(poller, {{"foo"}}, nt::EventFlags::kValueLocal);

  auto topic1 = nt::GetTopic(m_inst, "foo/1");
  auto topic2 = nt::GetTopic(m_inst, "foo/2");
  auto topic3 = nt::GetTopic(m_inst, "bar/3");
  auto pub1 = nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = nt::Publish(topic2, NT_DOUBLE, "double");
  auto pub3 = nt::Publish(topic3, NT_DOUBLE, "double");

  nt::SetDouble(pub1, 0);
  nt::SetDouble(pub2, 1);
  nt::SetDouble(pub3, 1);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->topic, topic1);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[1].listener, h);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->topic, topic2);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, PollEntry) {
  auto entry = nt::GetEntry(m_inst, "foo");

  auto poller = nt::CreateListenerPoller(m_inst);
  auto h = nt::AddPolledListener(poller, entry, nt::EventFlags::kValueLocal);

  ASSERT_TRUE(nt::SetDouble(entry, 0));

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, entry);
  EXPECT_EQ(valueData->topic, nt::GetTopic(m_inst, "foo"));
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollImmediate) {
  auto entry = nt::GetEntry(m_inst, "foo");
  ASSERT_TRUE(nt::SetDouble(entry, 0));

  auto poller = nt::CreateListenerPoller(m_inst);
  auto h = nt::AddPolledListener(
      poller, entry, nt::EventFlags::kValueLocal | nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags &
                (nt::EventFlags::kValueLocal | nt::EventFlags::kImmediate),
            nt::EventFlags::kValueLocal | nt::EventFlags::kImmediate);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, entry);
  EXPECT_EQ(valueData->topic, nt::GetTopic(m_inst, "foo"));
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollImmediateNoValue) {
  auto entry = nt::GetEntry(m_inst, "foo");

  auto poller = nt::CreateListenerPoller(m_inst);
  auto h = nt::AddPolledListener(
      poller, entry, nt::EventFlags::kValueLocal | nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_FALSE(wpi::WaitForObject(poller, 0.02, &timedOut));
  ASSERT_TRUE(timedOut);
  auto results = nt::ReadListenerQueue(poller);
  ASSERT_TRUE(results.empty());

  // now set a value
  ASSERT_TRUE(nt::SetDouble(entry, 0));

  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  results = nt::ReadListenerQueue(poller);
  ASSERT_FALSE(timedOut);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags, nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, entry);
  EXPECT_EQ(valueData->topic, nt::GetTopic(m_inst, "foo"));
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollImmediateSubMultiple) {
  auto topic1 = nt::GetTopic(m_inst, "foo/1");
  auto topic2 = nt::GetTopic(m_inst, "foo/2");
  auto pub1 = nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub = nt::SubscribeMultiple(m_inst, {{"foo"}});
  nt::SetDouble(pub1, 0);
  nt::SetDouble(pub2, 1);

  auto poller = nt::CreateListenerPoller(m_inst);
  auto h = nt::AddPolledListener(
      poller, sub, nt::EventFlags::kValueLocal | nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags &
                (nt::EventFlags::kValueLocal | nt::EventFlags::kImmediate),
            nt::EventFlags::kValueLocal | nt::EventFlags::kImmediate);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic1);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags &
                (nt::EventFlags::kValueLocal | nt::EventFlags::kImmediate),
            nt::EventFlags::kValueLocal | nt::EventFlags::kImmediate);
  EXPECT_EQ(results[1].listener, h);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic2);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, TwoSubOneListener) {
  auto topic = nt::GetTopic(m_inst, "foo");
  auto pub = nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub3 = nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = nt::CreateListenerPoller(m_inst);
  auto h = nt::AddPolledListener(poller, sub1, nt::EventFlags::kValueLocal);
  (void)sub2;
  (void)sub3;

  nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags & nt::EventFlags::kValueLocal,
            nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub1);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, TwoSubOneMultiListener) {
  auto topic = nt::GetTopic(m_inst, "foo");
  auto pub = nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub3 = nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = nt::CreateListenerPoller(m_inst);
  auto h = nt::AddPolledListener(poller, sub3, nt::EventFlags::kValueLocal);
  (void)sub1;
  (void)sub2;

  nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags & nt::EventFlags::kValueLocal,
            nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub3);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, nt::Value::MakeDouble(0.0));
}

}  // namespace nt
