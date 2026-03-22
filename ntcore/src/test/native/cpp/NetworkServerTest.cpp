// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "networktables/IntegerTopic.h"
#include "networktables/NetworkTableInstance.h"

// Valid persistent JSON containing a single persistent integer topic.
static constexpr const char* kPersistentJson = R"([
  {
    "name": "/test/persistent_value",
    "type": "int",
    "value": 42,
    "properties": {"persistent": true}
  }
])";

class NetworkServerPersistentTest : public ::testing::Test {
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

  ~NetworkServerPersistentTest() override {
    std::error_code ec;
    std::filesystem::remove_all(m_tempDir, ec);
  }

 protected:
  // Write content to a file.
  static void WriteFile(const std::string& path, const std::string& content) {
    std::ofstream os{path};
    ASSERT_TRUE(os.is_open()) << "Failed to create file: " << path;
    os << content;
  }

  // Wait for the server to finish initializing.  Returns true if a topic with
  // the given name was seen before the timeout expired.
  bool WaitForTopic(
      nt::NetworkTableInstance& inst, std::string_view name,
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
TEST_F(NetworkServerPersistentTest,
       LoadPersistentRestoresFromBackupWhenOriginalMissing) {
  // Set up "interrupted" state: only .bck file exists, no original.
  std::string backupFile = m_persistFile + ".bck";
  WriteFile(backupFile, kPersistentJson);
  ASSERT_TRUE(std::filesystem::exists(backupFile));
  ASSERT_FALSE(std::filesystem::exists(m_persistFile));

  // Start a server that references the (missing) persistent file.
  // Subscribe BEFORE starting the server so the server's local client has a
  // matching subscriber when persistent topics are announced.
  auto inst = nt::NetworkTableInstance::Create();
  nt::IntegerSubscriber sub =
      inst.GetIntegerTopic("/test/persistent_value").Subscribe(0);
  inst.StartServer(m_persistFile, "127.0.0.1");

  // Wait for the persistent topic to appear.
  EXPECT_TRUE(WaitForTopic(inst, "/test/persistent_value"))
      << "LoadPersistent did not restore from the .bck backup file";

  // Also verify the value is correct.
  EXPECT_EQ(sub.Get(), 42);

  // The .bck should have been renamed to the original filename.
  EXPECT_TRUE(std::filesystem::exists(m_persistFile));

  inst.StopServer();
  nt::NetworkTableInstance::Destroy(inst);
}

// Verify that LoadPersistent works normally when the original persistent file
// is present (no interruption scenario).
TEST_F(NetworkServerPersistentTest, LoadPersistentNormalLoad) {
  // Write the persistent file directly (no backup).
  WriteFile(m_persistFile, kPersistentJson);
  ASSERT_TRUE(std::filesystem::exists(m_persistFile));

  auto inst = nt::NetworkTableInstance::Create();
  nt::IntegerSubscriber sub =
      inst.GetIntegerTopic("/test/persistent_value").Subscribe(0);
  inst.StartServer(m_persistFile, "127.0.0.1");

  EXPECT_TRUE(WaitForTopic(inst, "/test/persistent_value"))
      << "LoadPersistent did not load the persistent file";

  EXPECT_EQ(sub.Get(), 42);

  inst.StopServer();
  nt::NetworkTableInstance::Destroy(inst);
}

// Verify that when both the original file and .bck exist, the original file
// takes precedence (the backup is not used).
TEST_F(NetworkServerPersistentTest, LoadPersistentPrefersOriginalOverBackup) {
  // Original file with value 100.
  static constexpr const char* kOriginalJson = R"([
  {
    "name": "/test/persistent_value",
    "type": "int",
    "value": 100,
    "properties": {"persistent": true}
  }
])";

  // Backup file with a different value (42).
  WriteFile(m_persistFile, kOriginalJson);
  WriteFile(m_persistFile + ".bck", kPersistentJson);
  ASSERT_TRUE(std::filesystem::exists(m_persistFile));
  ASSERT_TRUE(std::filesystem::exists(m_persistFile + ".bck"));

  auto inst = nt::NetworkTableInstance::Create();
  nt::IntegerSubscriber sub =
      inst.GetIntegerTopic("/test/persistent_value").Subscribe(0);
  inst.StartServer(m_persistFile, "127.0.0.1");

  EXPECT_TRUE(WaitForTopic(inst, "/test/persistent_value"))
      << "LoadPersistent did not load any persistent file";

  // The value should come from the original (100), not the backup (42).
  EXPECT_EQ(sub.Get(), 100);

  inst.StopServer();
  nt::NetworkTableInstance::Destroy(inst);
}

// Verify that LoadPersistent handles a missing persistent file and no backup
// gracefully (no crash, no topics loaded).
TEST_F(NetworkServerPersistentTest, LoadPersistentNoFile) {
  ASSERT_FALSE(std::filesystem::exists(m_persistFile));
  ASSERT_FALSE(std::filesystem::exists(m_persistFile + ".bck"));

  auto inst = nt::NetworkTableInstance::Create();
  inst.StartServer(m_persistFile, "127.0.0.1");

  // Give the server time to initialize.
  std::this_thread::sleep_for(std::chrono::milliseconds{500});

  // No persistent topics should exist.
  auto infos = inst.GetTopicInfo("/test/persistent_value");
  EXPECT_TRUE(infos.empty());

  inst.StopServer();
  nt::NetworkTableInstance::Destroy(inst);
}
