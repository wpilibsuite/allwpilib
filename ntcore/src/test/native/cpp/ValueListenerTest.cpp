// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TestPrinters.hpp"
#undef FAIL
#undef SUCCEED
#include <catch2/catch_test_macros.hpp>

#include "wpi/nt/ntcore_c.h"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/Synchronization.hpp"

namespace wpi::nt {

// Test only local here; it's more reliable to mock the network
class ValueListenerTest {
 public:
  ValueListenerTest() : m_inst{wpi::nt::CreateInstance()} {}

  ~ValueListenerTest() { wpi::nt::DestroyInstance(m_inst); }

 protected:
  NT_Inst m_inst;
};

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest MultiPollSub",
                 "[ntcore][value-listener]") {
  auto topic = wpi::nt::GetTopic(m_inst, "foo");
  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  auto sub = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");

  auto poller1 = wpi::nt::CreateListenerPoller(m_inst);
  auto poller2 = wpi::nt::CreateListenerPoller(m_inst);
  auto poller3 = wpi::nt::CreateListenerPoller(m_inst);
  auto h1 = wpi::nt::AddPolledListener(poller1, sub,
                                       wpi::nt::EventFlags::VALUE_LOCAL);
  auto h2 = wpi::nt::AddPolledListener(poller2, sub,
                                       wpi::nt::EventFlags::VALUE_LOCAL);
  auto h3 = wpi::nt::AddPolledListener(poller3, sub,
                                       wpi::nt::EventFlags::VALUE_LOCAL);

  wpi::nt::SetDouble(pub, 0);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller1, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  REQUIRE(wpi::util::WaitForObject(poller2, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  REQUIRE(wpi::util::WaitForObject(poller3, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results1 = wpi::nt::ReadListenerQueue(poller1);
  auto results2 = wpi::nt::ReadListenerQueue(poller2);
  auto results3 = wpi::nt::ReadListenerQueue(poller3);

  REQUIRE(results1.size() == 1u);
  CHECK(results1[0].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results1[0].listener == h1);
  auto valueData = results1[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub);
  CHECK(valueData->topic == topic);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));

  REQUIRE(results2.size() == 1u);
  CHECK(results2[0].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results2[0].listener == h2);
  valueData = results2[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub);
  CHECK(valueData->topic == topic);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));

  REQUIRE(results3.size() == 1u);
  CHECK(results3[0].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results3[0].listener == h3);
  valueData = results3[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub);
  CHECK(valueData->topic == topic);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));
}

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest PollMultiSub",
                 "[ntcore][value-listener]") {
  auto topic = wpi::nt::GetTopic(m_inst, "foo");
  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h1 = wpi::nt::AddPolledListener(poller, sub1,
                                       wpi::nt::EventFlags::VALUE_LOCAL);
  auto h2 = wpi::nt::AddPolledListener(poller, sub2,
                                       wpi::nt::EventFlags::VALUE_LOCAL);

  wpi::nt::SetDouble(pub, 0);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  REQUIRE(results.size() == 2u);
  CHECK(results[0].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[0].listener == h1);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub1);
  CHECK(valueData->topic == topic);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));

  CHECK(results[1].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[1].listener == h2);
  valueData = results[1].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub2);
  CHECK(valueData->topic == topic);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));
}

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest PollMultiSubTopic",
                 "[ntcore][value-listener]") {
  auto topic1 = wpi::nt::GetTopic(m_inst, "foo");
  auto topic2 = wpi::nt::GetTopic(m_inst, "bar");
  auto pub1 = wpi::nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = wpi::nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub1 = wpi::nt::Subscribe(topic1, NT_DOUBLE, "double");
  auto sub2 = wpi::nt::Subscribe(topic2, NT_DOUBLE, "double");

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h1 = wpi::nt::AddPolledListener(poller, sub1,
                                       wpi::nt::EventFlags::VALUE_LOCAL);
  auto h2 = wpi::nt::AddPolledListener(poller, sub2,
                                       wpi::nt::EventFlags::VALUE_LOCAL);

  wpi::nt::SetDouble(pub1, 0);
  wpi::nt::SetDouble(pub2, 1);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  REQUIRE(results.size() == 2u);
  CHECK(results[0].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[0].listener == h1);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub1);
  CHECK(valueData->topic == topic1);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));

  CHECK(results[1].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[1].listener == h2);
  valueData = results[1].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub2);
  CHECK(valueData->topic == topic2);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(1.0));
}

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest PollSubMultiple",
                 "[ntcore][value-listener]") {
  auto topic1 = wpi::nt::GetTopic(m_inst, "foo/1");
  auto topic2 = wpi::nt::GetTopic(m_inst, "foo/2");
  auto pub1 = wpi::nt::Publish(topic1, NT_DOUBLE, "double");
  auto pub2 = wpi::nt::Publish(topic2, NT_DOUBLE, "double");
  auto sub = wpi::nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h =
      wpi::nt::AddPolledListener(poller, sub, wpi::nt::EventFlags::VALUE_LOCAL);

  wpi::nt::SetDouble(pub1, 0);
  wpi::nt::SetDouble(pub2, 1);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  REQUIRE(results.size() == 2u);
  CHECK(results[0].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[0].listener == h);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub);
  CHECK(valueData->topic == topic1);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));

  CHECK(results[1].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[1].listener == h);
  valueData = results[1].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub);
  CHECK(valueData->topic == topic2);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(1.0));
}

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest PollSubPrefixCreated",
                 "[ntcore][value-listener]") {
  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(poller, {{"foo"}},
                                      wpi::nt::EventFlags::VALUE_LOCAL);

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
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  REQUIRE(results.size() == 2u);
  CHECK(results[0].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[0].listener == h);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->topic == topic1);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));

  CHECK(results[1].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[1].listener == h);
  valueData = results[1].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->topic == topic2);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(1.0));
}

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest PollEntry",
                 "[ntcore][value-listener]") {
  auto entry = wpi::nt::GetEntry(m_inst, "foo");

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(poller, entry,
                                      wpi::nt::EventFlags::VALUE_LOCAL);

  REQUIRE(wpi::nt::SetDouble(entry, 0));

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  REQUIRE(results.size() == 1u);
  CHECK(results[0].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[0].listener == h);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == entry);
  CHECK(valueData->topic == wpi::nt::GetTopic(m_inst, "foo"));
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));
}

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest PollImmediate",
                 "[ntcore][value-listener]") {
  auto entry = wpi::nt::GetEntry(m_inst, "foo");
  REQUIRE(wpi::nt::SetDouble(entry, 0));

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(
      poller, entry,
      wpi::nt::EventFlags::VALUE_LOCAL | wpi::nt::EventFlags::IMMEDIATE);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  REQUIRE(results.size() == 1u);
  CHECK((results[0].flags &
         (wpi::nt::EventFlags::VALUE_LOCAL | wpi::nt::EventFlags::IMMEDIATE)) ==
        (wpi::nt::EventFlags::VALUE_LOCAL | wpi::nt::EventFlags::IMMEDIATE));
  CHECK(results[0].listener == h);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == entry);
  CHECK(valueData->topic == wpi::nt::GetTopic(m_inst, "foo"));
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));
}

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest PollImmediateNoValue",
                 "[ntcore][value-listener]") {
  auto entry = wpi::nt::GetEntry(m_inst, "foo");

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(
      poller, entry,
      wpi::nt::EventFlags::VALUE_LOCAL | wpi::nt::EventFlags::IMMEDIATE);

  bool timedOut = false;
  REQUIRE_FALSE(wpi::util::WaitForObject(poller, 0.02, &timedOut));
  REQUIRE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);
  REQUIRE(results.empty());

  // now set a value
  REQUIRE(wpi::nt::SetDouble(entry, 0));

  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  results = wpi::nt::ReadListenerQueue(poller);
  REQUIRE_FALSE(timedOut);

  REQUIRE(results.size() == 1u);
  CHECK(results[0].flags == wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[0].listener == h);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == entry);
  CHECK(valueData->topic == wpi::nt::GetTopic(m_inst, "foo"));
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));
}

TEST_CASE_METHOD(ValueListenerTest,
                 "ValueListenerTest PollImmediateSubMultiple",
                 "[ntcore][value-listener]") {
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
      wpi::nt::EventFlags::VALUE_LOCAL | wpi::nt::EventFlags::IMMEDIATE);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  REQUIRE(results.size() == 2u);
  CHECK((results[0].flags &
         (wpi::nt::EventFlags::VALUE_LOCAL | wpi::nt::EventFlags::IMMEDIATE)) ==
        (wpi::nt::EventFlags::VALUE_LOCAL | wpi::nt::EventFlags::IMMEDIATE));
  CHECK(results[0].listener == h);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub);
  CHECK(valueData->topic == topic1);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));

  CHECK((results[1].flags &
         (wpi::nt::EventFlags::VALUE_LOCAL | wpi::nt::EventFlags::IMMEDIATE)) ==
        (wpi::nt::EventFlags::VALUE_LOCAL | wpi::nt::EventFlags::IMMEDIATE));
  CHECK(results[1].listener == h);
  valueData = results[1].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub);
  CHECK(valueData->topic == topic2);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(1.0));
}

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest TwoSubOneListener",
                 "[ntcore][value-listener]") {
  auto topic = wpi::nt::GetTopic(m_inst, "foo");
  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub3 = wpi::nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(poller, sub1,
                                      wpi::nt::EventFlags::VALUE_LOCAL);
  (void)sub2;
  (void)sub3;

  wpi::nt::SetDouble(pub, 0);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  REQUIRE(results.size() == 1u);
  CHECK((results[0].flags & wpi::nt::EventFlags::VALUE_LOCAL) ==
        wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[0].listener == h);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub1);
  CHECK(valueData->topic == topic);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));
}

TEST_CASE_METHOD(ValueListenerTest, "ValueListenerTest TwoSubOneMultiListener",
                 "[ntcore][value-listener]") {
  auto topic = wpi::nt::GetTopic(m_inst, "foo");
  auto pub = wpi::nt::Publish(topic, NT_DOUBLE, "double");
  auto sub1 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub2 = wpi::nt::Subscribe(topic, NT_DOUBLE, "double");
  auto sub3 = wpi::nt::SubscribeMultiple(m_inst, {{"foo"}});

  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto h = wpi::nt::AddPolledListener(poller, sub3,
                                      wpi::nt::EventFlags::VALUE_LOCAL);
  (void)sub1;
  (void)sub2;

  wpi::nt::SetDouble(pub, 0);

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  REQUIRE_FALSE(timedOut);
  auto results = wpi::nt::ReadListenerQueue(poller);

  REQUIRE(results.size() == 1u);
  CHECK((results[0].flags & wpi::nt::EventFlags::VALUE_LOCAL) ==
        wpi::nt::EventFlags::VALUE_LOCAL);
  CHECK(results[0].listener == h);
  auto valueData = results[0].GetValueEventData();
  REQUIRE(valueData);
  CHECK(valueData->subentry == sub3);
  CHECK(valueData->topic == topic);
  CHECK(valueData->value == wpi::nt::Value::MakeDouble(0.0));
}

}  // namespace wpi::nt
