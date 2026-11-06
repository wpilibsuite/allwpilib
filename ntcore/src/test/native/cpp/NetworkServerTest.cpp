// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/net/HttpUtil.hpp"
#include "wpi/net/TCPConnector.h"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/util/Logger.hpp"

// Valid persistent JSON containing a single persistent integer topic.
static constexpr const char* PERSISTENT_JSON = R"([
  {
    "name": "/test/persistent_value",
    "type": "int",
    "value": 42,
    "properties": {"persistent": true}
  }
])";

static constexpr unsigned int RESTORE_BACKUP_PORT = 10040;
static constexpr unsigned int NORMAL_LOAD_PORT = 10041;
static constexpr unsigned int ORIGINAL_OVER_BACKUP_PORT = 10042;
static constexpr unsigned int NO_FILE_PORT = 10043;
static constexpr unsigned int WEB_VIEWER_PORT = 10044;

class NetworkServerWebTest {
 public:
  NetworkServerWebTest() {
    m_inst.StartServer("", "127.0.0.1", "", WEB_VIEWER_PORT);
  }

  ~NetworkServerWebTest() { wpi::nt::NetworkTableInstance::Destroy(m_inst); }

 protected:
  std::unique_ptr<wpi::net::HttpConnection> Get(std::string_view path) {
    wpi::util::Logger logger;
    std::unique_ptr<wpi::net::NetworkStream> stream;
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{3};
    do {
      stream = wpi::net::TCPConnector::connect("127.0.0.1", WEB_VIEWER_PORT,
                                               logger, 1);
      if (stream) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds{10});
    } while (std::chrono::steady_clock::now() < deadline);
    REQUIRE(stream);
    auto conn =
        std::make_unique<wpi::net::HttpConnection>(std::move(stream), 3);
    wpi::net::HttpRequest request;
    request.host = "127.0.0.1";
    request.port = WEB_VIEWER_PORT;
    request.path = path.substr(1);
    std::string warning;
    bool success = conn->Handshake(request, &warning);
    INFO(warning);
    REQUIRE(success);
    return conn;
  }

  wpi::nt::NetworkTableInstance m_inst =
      wpi::nt::NetworkTableInstance::Create();
};

TEST_CASE_METHOD(NetworkServerWebTest, "NetworkServerWebTest OutlineViewer",
                 "[ntcore][network-server]") {
  auto conn = Get("/");
  CHECK(std::string_view{conn->contentType} == "text/html; charset=utf-8");
  auto length = std::stoul(std::string{conn->contentLength});
  std::string body(length, '\0');
  conn->is.read(body.data(), body.size());
  REQUIRE_FALSE(conn->is.has_error());
  CHECK(body.starts_with("<!doctype html>"));
  CHECK(body.find("NetworkTables Outline Viewer") != std::string::npos);
  CHECK(body.find("v4.1.networktables.first.wpi.edu") != std::string::npos);
  CHECK(body.find("</html>") != std::string::npos);

  auto persistent = Get("/nt/persistent.json");
  CHECK(std::string_view{persistent->contentType} == "application/json");
}

class NetworkServerPersistentTest {
 public:
  NetworkServerPersistentTest() {
    // Create a unique temp directory for each test
    m_tempDir =
        std::filesystem::temp_directory_path() /
        ("ntcore_test_" +
         std::to_string(
             std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(m_tempDir);
    m_persistFile = (m_tempDir / "test_persistent.json").string();
  }

  ~NetworkServerPersistentTest() {
    std::error_code ec;
    std::filesystem::remove_all(m_tempDir, ec);
  }

 protected:
  // Write content to a file.
  static void WriteFile(const std::string& path, const std::string& content) {
    std::ofstream os{path};
    UNSCOPED_INFO("Failed to create file: " << path);
    REQUIRE(os.is_open());
    os << content;
  }

  // Wait for the server to finish initializing.  Returns true if a topic with
  // the given name was seen before the timeout expired.
  bool WaitForTopic(
      wpi::nt::NetworkTableInstance& inst, std::string_view name,
      std::chrono::milliseconds timeout = std::chrono::milliseconds{3000}) {
    auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
      auto infos = inst.GetTopicInfo(name);
      if (!infos.empty()) {
        return true;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds{50});
    }
    return false;
  }

  std::filesystem::path m_tempDir;
  std::string m_persistFile;
};

// Verify that LoadPersistent restores from the .bck backup file when the
// original persistent file is missing.  This simulates SavePersistent being
// interrupted after renaming the original file to .bck but before the
// temporary file has been renamed to the original filename.
TEST_CASE_METHOD(NetworkServerPersistentTest,
                 "NetworkServerPersistentTest "
                 "LoadPersistentRestoresFromBackupWhenOriginalMissing",
                 "[ntcore][network-server]") {
  // Set up "interrupted" state: only .bck file exists, no original.
  std::string backupFile = m_persistFile + ".bck";
  WriteFile(backupFile, PERSISTENT_JSON);
  REQUIRE(std::filesystem::exists(backupFile));
  REQUIRE_FALSE(std::filesystem::exists(m_persistFile));

  // Start a server that references the (missing) persistent file.
  // Subscribe BEFORE starting the server so the server's local client has a
  // matching subscriber when persistent topics are announced.
  auto inst = wpi::nt::NetworkTableInstance::Create();
  wpi::nt::IntegerSubscriber sub =
      inst.GetIntegerTopic("/test/persistent_value").Subscribe(0);
  inst.StartServer(m_persistFile, "127.0.0.1", "", RESTORE_BACKUP_PORT);

  // Wait for the persistent topic to appear.
  UNSCOPED_INFO("LoadPersistent did not restore from the .bck backup file");
  CHECK(WaitForTopic(inst, "/test/persistent_value"));

  // Also verify the value is correct.
  CHECK(sub.Get() == 42);

  // The .bck should have been renamed to the original filename.
  CHECK(std::filesystem::exists(m_persistFile));

  inst.StopServer();
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

// Verify that LoadPersistent works normally when the original persistent file
// is present (no interruption scenario).
TEST_CASE_METHOD(NetworkServerPersistentTest,
                 "NetworkServerPersistentTest LoadPersistentNormalLoad",
                 "[ntcore][network-server]") {
  // Write the persistent file directly (no backup).
  WriteFile(m_persistFile, PERSISTENT_JSON);
  REQUIRE(std::filesystem::exists(m_persistFile));

  auto inst = wpi::nt::NetworkTableInstance::Create();
  wpi::nt::IntegerSubscriber sub =
      inst.GetIntegerTopic("/test/persistent_value").Subscribe(0);
  inst.StartServer(m_persistFile, "127.0.0.1", "", NORMAL_LOAD_PORT);

  UNSCOPED_INFO("LoadPersistent did not load the persistent file");
  CHECK(WaitForTopic(inst, "/test/persistent_value"));

  CHECK(sub.Get() == 42);

  inst.StopServer();
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

// Verify that when both the original file and .bck exist, the original file
// takes precedence (the backup is not used).
TEST_CASE_METHOD(
    NetworkServerPersistentTest,
    "NetworkServerPersistentTest LoadPersistentPrefersOriginalOverBackup",
    "[ntcore][network-server]") {
  // Original file with value 100.
  static constexpr const char* ORIGINAL_JSON = R"([
  {
    "name": "/test/persistent_value",
    "type": "int",
    "value": 100,
    "properties": {"persistent": true}
  }
])";

  // Backup file with a different value (42).
  WriteFile(m_persistFile, ORIGINAL_JSON);
  WriteFile(m_persistFile + ".bck", PERSISTENT_JSON);
  REQUIRE(std::filesystem::exists(m_persistFile));
  REQUIRE(std::filesystem::exists(m_persistFile + ".bck"));

  auto inst = wpi::nt::NetworkTableInstance::Create();
  wpi::nt::IntegerSubscriber sub =
      inst.GetIntegerTopic("/test/persistent_value").Subscribe(0);
  inst.StartServer(m_persistFile, "127.0.0.1", "", ORIGINAL_OVER_BACKUP_PORT);

  UNSCOPED_INFO("LoadPersistent did not load any persistent file");
  CHECK(WaitForTopic(inst, "/test/persistent_value"));

  // The value should come from the original (100), not the backup (42).
  CHECK(sub.Get() == 100);

  inst.StopServer();
  wpi::nt::NetworkTableInstance::Destroy(inst);
}

// Verify that LoadPersistent handles a missing persistent file and no backup
// gracefully (no crash, no topics loaded).
TEST_CASE_METHOD(NetworkServerPersistentTest,
                 "NetworkServerPersistentTest LoadPersistentNoFile",
                 "[ntcore][network-server]") {
  REQUIRE_FALSE(std::filesystem::exists(m_persistFile));
  REQUIRE_FALSE(std::filesystem::exists(m_persistFile + ".bck"));

  auto inst = wpi::nt::NetworkTableInstance::Create();
  inst.StartServer(m_persistFile, "127.0.0.1", "", NO_FILE_PORT);

  // Give the server time to initialize.
  std::this_thread::sleep_for(std::chrono::milliseconds{500});

  // No persistent topics should exist.
  auto infos = inst.GetTopicInfo("/test/persistent_value");
  CHECK(infos.empty());

  inst.StopServer();
  wpi::nt::NetworkTableInstance::Destroy(inst);
}
