// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <gtest/gtest.h>

#include "TestPrinters.hpp"
#include "gmock/gmock.h"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ntcore_cpp.hpp"

using ::testing::_;

using MockTableEventListener = testing::MockFunction<void(
    wpi::nt::NetworkTable* table, std::string_view key, const wpi::nt::Event& event)>;
using MockSubTableListener =
    testing::MockFunction<void(wpi::nt::NetworkTable* parent, std::string_view name,
                               std::shared_ptr<wpi::nt::NetworkTable> table)>;

class TableListenerTest : public ::testing::Test {
 public:
  TableListenerTest() : m_inst(wpi::nt::NetworkTableInstance::Create()) {}

  ~TableListenerTest() override { wpi::nt::NetworkTableInstance::Destroy(m_inst); }

  void PublishTopics();

 protected:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::DoublePublisher m_foovalue;
  wpi::nt::DoublePublisher m_barvalue;
  wpi::nt::DoublePublisher m_bazvalue;
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
  EXPECT_CALL(listener, Call(table.get(), std::string_view{"foovalue"}, _));
  PublishTopics();
  EXPECT_TRUE(m_inst.WaitForListenerQueue(1.0));
}

TEST_F(TableListenerTest, AddSubTableListener) {
  auto table = m_inst.GetTable("/foo");
  MockSubTableListener listener;
  table->AddSubTableListener(listener.AsStdFunction());
  EXPECT_CALL(listener, Call(table.get(), std::string_view{"bar"}, _));
  PublishTopics();
  EXPECT_TRUE(m_inst.WaitForListenerQueue(1.0));
}
