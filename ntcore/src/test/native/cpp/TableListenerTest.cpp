// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include "TestPrinters.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "networktables/DoubleTopic.h"
#include "networktables/NetworkTableInstance.h"
#include "ntcore_cpp.h"

using ::testing::_;

using MockTableEventListener = testing::MockFunction<void(
    nt::NetworkTable* table, std::string_view key, const nt::Event& event)>;
using MockSubTableListener =
    testing::MockFunction<void(nt::NetworkTable* parent, std::string_view name,
                               std::shared_ptr<nt::NetworkTable> table)>;

class TableListenerTest : public ::testing::Test {
 public:
  TableListenerTest() : m_inst(nt::NetworkTableInstance::Create()) {}

  ~TableListenerTest() override { nt::NetworkTableInstance::Destroy(m_inst); }

  void PublishTopics();

 protected:
  nt::NetworkTableInstance m_inst;
  nt::DoublePublisher m_foovalue;
  nt::DoublePublisher m_barvalue;
  nt::DoublePublisher m_bazvalue;
};

void TableListenerTest::PublishTopics() {
  m_foovalue = m_inst.GetDoubleTopic("/foo/foovalue").Publish();
  m_barvalue = m_inst.GetDoubleTopic("/foo/bar/barvalue").Publish();
  m_bazvalue = m_inst.GetDoubleTopic("/baz/bazvalue").Publish();
}

TEST_F(TableListenerTest, AddListener) {
  auto table = m_inst.GetTable("/foo");
  MockTableEventListener listener;
  table->AddListener(NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE,
                     listener.AsStdFunction());
  EXPECT_CALL(listener, Call(table.get(), "foovalue", _));
  PublishTopics();
  EXPECT_TRUE(m_inst.WaitForListenerQueue(1.0));
}

TEST_F(TableListenerTest, AddSubTableListener) {
  auto table = m_inst.GetTable("/foo");
  MockSubTableListener listener;
  table->AddSubTableListener(listener.AsStdFunction());
  EXPECT_CALL(listener, Call(table.get(), "bar", _));
  PublishTopics();
  EXPECT_TRUE(m_inst.WaitForListenerQueue(1.0));
}
