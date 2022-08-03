// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

namespace glass {
class Storage;
}  // namespace glass

namespace pfd {
class select_folder;
}  // namespace pfd

class Downloader {
 public:
  explicit Downloader(glass::Storage& storage);
  ~Downloader();

  void Display();

 private:
  void DisplayConnect();
  void DisplayDisconnectButton();
  void DisplayRemoteDirSelector();
  void DisplayLocalDirSelector();
  size_t DisplayFiles();

  void ThreadMain();

  wpi::mutex m_mutex;
  enum State {
    kDisconnected,
    kConnecting,
    kConnected,
    kDisconnecting,
    kGetFiles,
    kDownload,
    kDownloadDone,
    kExit
  } m_state = kDisconnected;
  std::condition_variable m_cv;

  std::string& m_serverTeam;
  std::string& m_remoteDir;
  std::string& m_username;
  std::string m_password;

  std::string& m_localDir;
  std::unique_ptr<pfd::select_folder> m_localDirSelector;

  bool& m_deleteAfter;

  std::vector<std::string> m_dirList;
  struct DownloadState {
    DownloadState(std::string_view name, uint64_t size)
        : name{name}, size{size} {}

    std::string name;
    uint64_t size;
    bool enabled = true;
    float complete = 0.0;
    std::string status;
  };
  std::vector<DownloadState> m_downloadList;

  std::string m_error;

  std::thread m_thread;
};
