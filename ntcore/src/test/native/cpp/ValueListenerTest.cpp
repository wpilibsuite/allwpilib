// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "TestPrinters.hpp"
#include "ValueMatcher.hpp"
#include "wpi/nt/ntcore_c.h"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/Synchronization.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::IsNull;
using ::testing::Return;

namespace wpi::nt {

// Test only local here; it's more reliable to mock the network
class ValueListenerTest : public ::testing::Test {
 public:
  ValueListenerTest() : m_inst{wpi::nt::CreateInstance()} {}

  ~ValueListenerTest() override { wpi::nt::DestroyInstance(m_inst); }

 protected:
  NT_Inst m_inst;
};

TEST_F(ValueListenerTest, MultiPollSub) {
  auto topic = wpi::nt::GetTopic(m_inst, "foo");
  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  auto sub = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");

  auto poller1 = wpi::nt::CreateListenerPoller(m_inst);
  auto poller2 = wpi::nt::CreateListenerPoller(m_inst);
  auto poller3 = wpi::nt::CreateListenerPoller(m_inst);
  auto h1 = wpi::nt::AddPolledListener(poller1, sub,
                                       wpi::nt::EventFlags::kValueLocal);
  auto h2 = wpi::nt::AddPolledListener(poller2, sub,
                                       wpi::nt::EventFlags::kValueLocal);
  auto h3 = wpi::nt::AddPolledListener(poller3, sub,
                                       wpi::nt::EventFlags::kValueLocal);

  wpi::nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller1, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  ASSERT_TRUE(wpi::util::WaitForObject(poller2, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  ASSERT_TRUE(wpi::util::WaitForObject(poller3, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results1 = wpi::nt::ReadListenerQueue(poller1);
  auto results2 = wpi::nt::ReadListenerQueue(poller2);
  auto results3 = wpi::nt::ReadListenerQueue(poller3);

  ASSERT_EQ(results1.size(), 1u);
  EXPECT_EQ(results1[0].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results1[0].listener, h1);
  auto valueData = results1[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));

  ASSERT_EQ(results2.size(), 1u);
  EXPECT_EQ(results2[0].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results2[0].listener, h2);
  valueData = results2[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));

  ASSERT_EQ(results3.size(), 1u);
  EXPECT_EQ(results3[0].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results3[0].listener, h3);
  valueData = results3[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollMultiSub) {
  auto topic = wpi::nt::GetTopic(m_inst, "foo");
  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h1 = wpi::nt::AddPolledListener(poller, sub1,
                                       wpi::nt::EventFlags::kValueLocal);
  auto h2 = wpi::nt::AddPolledListener(poller, sub2,
                                       wpi::nt::EventFlags::kValueLocal);

  wpi::nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h1);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub1);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[1].listener, h2);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub2);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollMultiSubTopic) {
  auto topic1 = wpi::nt::GetTopic(m_inst, "foo");
  auto topic2 = wpi::nt::GetTopic(m_inst, "bar");
  auto pub1 = wpi::nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = wpi::nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub1 = wpi::nt::Subscribe(topic1, NT_DOUBLE, "double");
  auto sub2 = wpi::nt::Subscribe(topic2, NT_DOUBLE, "double");

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h1 = wpi::nt::AddPolledListener(poller, sub1,
                                       wpi::nt::EventFlags::kValueLocal);
  auto h2 = wpi::nt::AddPolledListener(poller, sub2,
                                       wpi::nt::EventFlags::kValueLocal);

  wpi::nt::SetDouble(pub1, 0);
  wpi::nt::SetDouble(pub2, 1);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h1);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub1);
  EXPECT_EQ(valueData->topic, topic1);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[1].listener, h2);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub2);
  EXPECT_EQ(valueData->topic, topic2);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, PollSubMultiple) {
  auto topic1 = wpi::nt::GetTopic(m_inst, "foo/1");
  auto topic2 = wpi::nt::GetTopic(m_inst, "foo/2");
  auto pub1 = wpi::nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = wpi::nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub = wpi::nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h =
      wpi::nt::AddPolledListener(poller, sub, wpi::nt::EventFlags::kValueLocal);

  wpi::nt::SetDouble(pub1, 0);
  wpi::nt::SetDouble(pub2, 1);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic1);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[1].listener, h);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic2);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, PollSubPrefixCreated) {
  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(poller, {{"foo"}},
                                      wpi::nt::EventFlags::kValueLocal);

  auto topic1 = wpi::nt::GetTopic(m_inst, "foo/1");
  auto topic2 = wpi::nt::GetTopic(m_inst, "foo/2");
  auto topic3 = wpi::nt::GetTopic(m_inst, "bar/3");
  auto pub1 = wpi::nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = wpi::nt::Publish(topic2, NT_DOUBLE, "double");
  auto pub3 = wpi::nt::Publish(topic3, NT_DOUBLE, "double");

  wpi::nt::SetDouble(pub1, 0);
  wpi::nt::SetDouble(pub2, 1);
  wpi::nt::SetDouble(pub3, 1);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->topic, topic1);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[1].listener, h);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->topic, topic2);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, PollEntry) {
  auto entry = wpi::nt::GetEntry(m_inst, "foo");

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(poller, entry,
                                      wpi::nt::EventFlags::kValueLocal);

  ASSERT_TRUE(wpi::nt::SetDouble(entry, 0));

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, entry);
  EXPECT_EQ(valueData->topic, wpi::nt::GetTopic(m_inst, "foo"));
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollImmediate) {
  auto entry = wpi::nt::GetEntry(m_inst, "foo");
  ASSERT_TRUE(wpi::nt::SetDouble(entry, 0));

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(
      poller, entry,
      wpi::nt::EventFlags::kValueLocal | wpi::nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags & (wpi::nt::EventFlags::kValueLocal |
                                wpi::nt::EventFlags::kImmediate),
            wpi::nt::EventFlags::kValueLocal | wpi::nt::EventFlags::kImmediate);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, entry);
  EXPECT_EQ(valueData->topic, wpi::nt::GetTopic(m_inst, "foo"));
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollImmediateNoValue) {
  auto entry = wpi::nt::GetEntry(m_inst, "foo");

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(
      poller, entry,
      wpi::nt::EventFlags::kValueLocal | wpi::nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_FALSE(wpi::util::WaitForObject(poller, 0.02, &timedOut));
  ASSERT_TRUE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);
  ASSERT_TRUE(results.empty());

  // now set a value
  ASSERT_TRUE(wpi::nt::SetDouble(entry, 0));

  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  results = wpi::nt::ReadListenerQueue(poller);
  ASSERT_FALSE(timedOut);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags, wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, entry);
  EXPECT_EQ(valueData->topic, wpi::nt::GetTopic(m_inst, "foo"));
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, PollImmediateSubMultiple) {
  auto topic1 = wpi::nt::GetTopic(m_inst, "foo/1");
  auto topic2 = wpi::nt::GetTopic(m_inst, "foo/2");
  auto pub1 = wpi::nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = wpi::nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub = wpi::nt::SubscribeMultiple(m_inst, {{"foo"}});
  wpi::nt::SetDouble(pub1, 0);
  wpi::nt::SetDouble(pub2, 1);

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(
      poller, sub,
      wpi::nt::EventFlags::kValueLocal | wpi::nt::EventFlags::kImmediate);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0].flags & (wpi::nt::EventFlags::kValueLocal |
                                wpi::nt::EventFlags::kImmediate),
            wpi::nt::EventFlags::kValueLocal | wpi::nt::EventFlags::kImmediate);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic1);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));

  EXPECT_EQ(results[1].flags & (wpi::nt::EventFlags::kValueLocal |
                                wpi::nt::EventFlags::kImmediate),
            wpi::nt::EventFlags::kValueLocal | wpi::nt::EventFlags::kImmediate);
  EXPECT_EQ(results[1].listener, h);
  valueData = results[1].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub);
  EXPECT_EQ(valueData->topic, topic2);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(1.0));
}

TEST_F(ValueListenerTest, TwoSubOneListener) {
  auto topic = wpi::nt::GetTopic(m_inst, "foo");
  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub3 = wpi::nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(poller, sub1,
                                      wpi::nt::EventFlags::kValueLocal);
  (void)sub2;
  (void)sub3;

  wpi::nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags & wpi::nt::EventFlags::kValueLocal,
            wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub1);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));
}

TEST_F(ValueListenerTest, TwoSubOneMultiListener) {
  auto topic = wpi::nt::GetTopic(m_inst, "foo");
  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub3 = wpi::nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(poller, sub3,
                                      wpi::nt::EventFlags::kValueLocal);
  (void)sub1;
  (void)sub2;

  wpi::nt::SetDouble(pub, 0);

  bool timedOut = false;
  ASSERT_TRUE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  ASSERT_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].flags & wpi::nt::EventFlags::kValueLocal,
            wpi::nt::EventFlags::kValueLocal);
  EXPECT_EQ(results[0].listener, h);
  auto valueData = results[0].GetValueEventData();
  ASSERT_TRUE(valueData);
  EXPECT_EQ(valueData->subentry, sub3);
  EXPECT_EQ(valueData->topic, topic);
  EXPECT_EQ(valueData->value, wpi::nt::Value::MakeDouble(0.0));
}

}  // namespace wpi::nt
