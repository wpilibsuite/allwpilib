// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Downloader.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#else
#include <sys/fcntl.h>
#endif

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <glass/Storage.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <libssh/sftp.h>
#include <portable-file-dialogs.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>

#include "Sftp.h"

Downloader::Downloader(glass::Storage& storage)
    : m_serverTeam{storage.GetString("serverTeam")},
      m_remoteDir{storage.GetString("remoteDir", "/home/lvuser/logs")},
      m_username{storage.GetString("username", "lvuser")},
      m_localDir{storage.GetString("localDir")},
      m_deleteAfter{storage.GetBool("deleteAfter", true)},
      m_thread{[this] { ThreadMain(); }} {}

Downloader::~Downloader() {
  {
    std::scoped_lock lock{m_mutex};
    m_state = kExit;
  }
  m_cv.notify_all();
  m_thread.join();
}

void Downloader::DisplayConnect() {
  // IP or Team Number text box
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
  ImGui::InputText("Team Number / Address", &m_serverTeam);

  // Username/password
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
  ImGui::InputText("Username", &m_username);
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 12);
  ImGui::InputText("Password", &m_password, ImGuiInputTextFlags_Password);

  // Connect button
  if (ImGui::Button("Connect")) {
    m_state = kConnecting;
    m_cv.notify_all();
  }
}

void Downloader::DisplayDisconnectButton() {
  if (ImGui::Button("Disconnect")) {
    m_state = kDisconnecting;
    m_cv.notify_all();
  }
}

void Downloader::DisplayRemoteDirSelector() {
  ImGui::SameLine();
  if (ImGui::Button("Refresh")) {
    m_state = kGetFiles;
    m_cv.notify_all();
  }

  ImGui::SameLine();
  if (ImGui::Button("Deselect All")) {
    for (auto&& download : m_fileList) {
      download.selected = false;
    }
  }

  ImGui::SameLine();
  if (ImGui::Button("Select All")) {
    for (auto&& download : m_fileList) {
      download.selected = true;
    }
  }

  // Remote directory text box
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 20);
  if (ImGui::InputText("Remote Dir", &m_remoteDir,
                       ImGuiInputTextFlags_EnterReturnsTrue)) {
    m_state = kGetFiles;
    m_cv.notify_all();
  }

  // List directories
  for (auto&& dir : m_dirList) {
    if (ImGui::Selectable(dir.c_str())) {
      if (dir == "..") {
        if (wpi::ends_with(m_remoteDir, '/')) {
          m_remoteDir.resize(m_remoteDir.size() - 1);
        }
        m_remoteDir = wpi::rsplit(m_remoteDir, '/').first;
        if (m_remoteDir.empty()) {
          m_remoteDir = "/";
        }
      } else {
        if (!wpi::ends_with(m_remoteDir, '/')) {
          m_remoteDir += '/';
        }
        m_remoteDir += dir;
      }
      m_state = kGetFiles;
      m_cv.notify_all();
    }
  }
}

void Downloader::DisplayLocalDirSelector() {
  // Local directory text / select button
  if (ImGui::Button("Select Download Folder...")) {
    m_localDirSelector =
        std::make_unique<pfd::select_folder>("Select Download Folder");
  }
  ImGui::TextUnformatted(m_localDir.c_str());

  // Delete after download (checkbox)
  ImGui::Checkbox("Delete after download", &m_deleteAfter);

  // Download button
  if (!m_localDir.empty()) {
    if (ImGui::Button("Download")) {
      m_state = kDownload;
      m_cv.notify_all();
    }
  }

  ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(180, 0, 0, 255));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(210, 0, 0, 255));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 0, 0, 255));
  if (ImGui::Button("Delete WITHOUT Downloading")) {
    ImGui::OpenPopup("DeleteConfirm");
  }
  ImGui::PopStyleColor(3);
  if (ImGui::BeginPopup("DeleteConfirm")) {
    ImGui::TextUnformatted("Are you sure? This will NOT download the files");
    if (ImGui::Button("DELETE")) {
      m_state = kDelete;
      m_cv.notify_all();
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

size_t Downloader::DisplayFiles() {
  // List of files (multi-select) (changes to progress bar for downloading)
  size_t fileCount = 0;
  if (ImGui::BeginTable(
          "files", 3,
          ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp)) {
    ImGui::TableSetupColumn("File");
    ImGui::TableSetupColumn("Size");
    ImGui::TableSetupColumn((m_state == kDownload || m_state == kDownloadDone ||
                             m_state == kDelete || m_state == kDeleteDone)
                                ? "Status"
                                : "Selected");
    ImGui::TableHeadersRow();
    for (auto&& file : m_fileList) {
      if ((m_state == kDownload || m_state == kDownloadDone ||
           m_state == kDelete || m_state == kDeleteDone) &&
          !file.selected) {
        continue;
      }

      ++fileCount;

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::TextUnformatted(file.name.c_str());
      ImGui::TableNextColumn();
      auto sizeText = fmt::format("{}", file.size);
      ImGui::TextUnformatted(sizeText.c_str());
      ImGui::TableNextColumn();
      if (m_state == kDownload || m_state == kDownloadDone) {
        if (!file.status.empty()) {
          ImGui::TextUnformatted(file.status.c_str());
        } else {
          ImGui::ProgressBar(file.complete);
        }
      } else if (m_state == kDelete || m_state == kDeleteDone) {
        if (!file.status.empty()) {
          ImGui::TextUnformatted(file.status.c_str());
        }
      } else {
        auto checkboxLabel = fmt::format("##{}", file.name);
        ImGui::Checkbox(checkboxLabel.c_str(), &file.selected);
      }
    }
    ImGui::EndTable();
  }

  return fileCount;
}

void Downloader::Display() {
  if (m_localDirSelector && m_localDirSelector->ready(0)) {
    m_localDir = m_localDirSelector->result();
    m_localDirSelector.reset();
  }

  std::scoped_lock lock{m_mutex};

  if (!m_error.empty()) {
    ImGui::TextUnformatted(m_error.c_str());
  }

  switch (m_state) {
    case kDisconnected:
      DisplayConnect();
      break;
    case kConnecting:
      DisplayDisconnectButton();
      ImGui::Text("Connecting to %s...", m_serverTeam.c_str());
      break;
    case kDisconnecting:
      ImGui::TextUnformatted("Disconnecting...");
      break;
    case kConnected:
    case kGetFiles:
      DisplayDisconnectButton();
      DisplayRemoteDirSelector();
      if (DisplayFiles() > 0) {
        DisplayLocalDirSelector();
      }
      break;
    case kDownload:
    case kDownloadDone:
      DisplayDisconnectButton();
      DisplayFiles();
      if (m_state == kDownloadDone) {
        if (ImGui::Button("Download complete!")) {
          m_state = kGetFiles;
          m_cv.notify_all();
        }
      }
      break;
    case kDelete:
    case kDeleteDone:
      DisplayDisconnectButton();
      DisplayFiles();
      if (m_state == kDeleteDone) {
        if (ImGui::Button("Deletion complete!")) {
          m_state = kGetFiles;
          m_cv.notify_all();
        }
      }
      break;
    default:
      break;
  }
}

void Downloader::ThreadMain() {
  std::unique_ptr<sftp::Session> session;

  static constexpr size_t kBufSize = 32 * 1024;
  std::unique_ptr<uint8_t[]> copyBuf = std::make_unique<uint8_t[]>(kBufSize);

  std::unique_lock lock{m_mutex};
  while (m_state != kExit) {
    State prev = m_state;
    m_cv.wait(lock, [&] { return m_state != prev; });
    m_error.clear();
    try {
      switch (m_state) {
        case kConnecting:
          if (auto team = wpi::parse_integer<unsigned int>(m_serverTeam, 10)) {
            // team number
            session = std::make_unique<sftp::Session>(
                fmt::format("roborio-{}-frc.local", team.value()), 22,
                m_username, m_password);
          } else {
            session = std::make_unique<sftp::Session>(m_serverTeam, 22,
                                                      m_username, m_password);
          }
          lock.unlock();
          try {
            session->Connect();
          } catch (...) {
            lock.lock();
            throw;
          }
          lock.lock();
          // FALLTHROUGH
        case kGetFiles: {
          std::string dir = m_remoteDir;
          std::vector<sftp::Attributes> fileList;
          lock.unlock();
          try {
            fileList = session->ReadDir(dir);
          } catch (sftp::Exception& ex) {
            lock.lock();
            if (ex.err == SSH_FX_OK || ex.err == SSH_FX_CONNECTION_LOST) {
              throw;
            }
            m_error = ex.what();
            m_dirList.clear();
            m_fileList.clear();
            m_state = kConnected;
            break;
          }
          std::sort(
              fileList.begin(), fileList.end(),
              [](const auto& l, const auto& r) { return l.name < r.name; });
          lock.lock();

          m_dirList.clear();
          m_fileList.clear();
          for (auto&& attr : fileList) {
            if (attr.type == SSH_FILEXFER_TYPE_DIRECTORY) {
              if (attr.name != ".") {
                m_dirList.emplace_back(attr.name);
              }
            } else if (attr.type == SSH_FILEXFER_TYPE_REGULAR &&
                       (attr.flags & SSH_FILEXFER_ATTR_SIZE) != 0 &&
                       wpi::ends_with(attr.name, ".wpilog")) {
              m_fileList.emplace_back(attr.name, attr.size);
            }
          }

          m_state = kConnected;
          break;
        }
        case kDisconnecting:
          session.reset();
          m_state = kDisconnected;
          break;
        case kDownload: {
          for (auto&& file : m_fileList) {
            if (m_state != kDownload) {
              // user aborted
              break;
            }
            if (!file.selected) {
              continue;
            }

            auto remoteFilename = fmt::format(
                "{}{}{}", m_remoteDir,
                wpi::ends_with(m_remoteDir, '/') ? "" : "/", file.name);
            auto localFilename = fs::path{m_localDir} / file.name;
            uint64_t fileSize = file.size;

            lock.unlock();

            // open local file
            std::error_code ec;
            fs::file_t of = fs::OpenFileForWrite(localFilename, ec,
                                                 fs::CD_CreateNew, fs::OF_None);
            if (ec) {
              // failed to open
              lock.lock();
              file.status = ec.message();
              continue;
            }
            int ofd = fs::FileToFd(of, ec, fs::OF_None);
            if (ofd == -1 || ec) {
              // failed to convert to fd
              lock.lock();
              file.status = ec.message();
              continue;
            }

            try {
              // open remote file
              sftp::File f = session->Open(remoteFilename, O_RDONLY, 0);

              // copy in chunks
              uint64_t total = 0;
              while (total < fileSize) {
                uint64_t toCopy = (std::min)(fileSize - total,
                                             static_cast<uint64_t>(kBufSize));
                auto copied = f.Read(copyBuf.get(), toCopy);
                if (write(ofd, copyBuf.get(), copied) !=
                    static_cast<int64_t>(copied)) {
                  // error writing
                  close(ofd);
                  fs::remove(localFilename, ec);
                  lock.lock();
                  file.status = "error writing local file";
                  goto err;
                }
                total += copied;
                lock.lock();
                file.complete = static_cast<float>(total) / fileSize;
                lock.unlock();
              }

              // close local file
              close(ofd);
              ofd = -1;

              // delete remote file (if enabled)
              if (m_deleteAfter) {
                f = sftp::File{};
                session->Unlink(remoteFilename);
              }
            } catch (sftp::Exception& ex) {
              if (ofd != -1) {
                // close local file and delete it (due to failure)
                close(ofd);
                fs::remove(localFilename, ec);
              }
              lock.lock();
              file.status = ex.what();
              if (ex.err == SSH_FX_OK || ex.err == SSH_FX_CONNECTION_LOST) {
                throw;
              }
              continue;
            }
            lock.lock();
          err: {}
          }
          if (m_state == kDownload) {
            m_state = kDownloadDone;
          }
          break;
        }
        case kDelete: {
          for (auto&& file : m_fileList) {
            if (m_state != kDelete) {
              // user aborted
              break;
            }
            if (!file.selected) {
              continue;
            }

            auto remoteFilename = fmt::format(
                "{}{}{}", m_remoteDir,
                wpi::ends_with(m_remoteDir, '/') ? "" : "/", file.name);

            lock.unlock();
            try {
              session->Unlink(remoteFilename);
            } catch (sftp::Exception& ex) {
              lock.lock();
              file.status = ex.what();
              if (ex.err == SSH_FX_OK || ex.err == SSH_FX_CONNECTION_LOST) {
                throw;
              }
              continue;
            }
            lock.lock();
            file.status = "Deleted";
          }
          if (m_state == kDelete) {
            m_state = kDeleteDone;
          }
          break;
        }
        default:
          break;
      }
    } catch (sftp::Exception& ex) {
      m_error = ex.what();
      session.reset();
      m_state = kDisconnected;
    }
  }
}
