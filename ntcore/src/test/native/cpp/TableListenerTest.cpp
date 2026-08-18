// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestPrinters.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ntcore_cpp.hpp"

class TableListenerTest {
 public:
  TableListenerTest() : m_inst(wpi::nt::NetworkTableInstance::Create()) {}

  ~TableListenerTest() { wpi::nt::NetworkTableInstance::Destroy(m_inst); }

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

TEST_CASE_METHOD(TableListenerTest, "TableListenerTest AddListener",
                 "[ntcore][table-listener]") {
  auto table = m_inst.GetTable("/foo");
  struct ListenerCall {
    wpi::nt::NetworkTable* table;
    std::string key;
  };
  std::vector<ListenerCall> listenerCalls;
  table->AddListener(NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE,
                     [&](wpi::nt::NetworkTable* callbackTable,
                         std::string_view key, const wpi::nt::Event&) {
                       listenerCalls.emplace_back(callbackTable,
                                                  std::string{key});
                     });
  PublishTopics();
  CHECK(m_inst.WaitForListenerQueue(1.0));
  REQUIRE(listenerCalls.size() == 1u);
  CHECK(listenerCalls[0].table == table.get());
  CHECK(listenerCalls[0].key == "foovalue");
}

TEST_CASE_METHOD(TableListenerTest, "TableListenerTest AddSubTableListener",
                 "[ntcore][table-listener]") {
  auto table = m_inst.GetTable("/foo");
  struct ListenerCall {
    wpi::nt::NetworkTable* parent;
    std::string name;
    std::shared_ptr<wpi::nt::NetworkTable> table;
  };
  std::vector<ListenerCall> listenerCalls;
  table->AddSubTableListener(
      [&](wpi::nt::NetworkTable* parent, std::string_view name,
          std::shared_ptr<wpi::nt::NetworkTable> callbackTable) {
        listenerCalls.emplace_back(parent, std::string{name}, callbackTable);
      });
  PublishTopics();
  CHECK(m_inst.WaitForListenerQueue(1.0));
  REQUIRE(listenerCalls.size() == 1u);
  CHECK(listenerCalls[0].parent == table.get());
  CHECK(listenerCalls[0].name == "bar");
}
