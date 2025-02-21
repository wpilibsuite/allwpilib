#include "wpi/datalog/export/DataLogExportUtils.h"
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include "fmt/format.h"
#include "wpi/MemoryBuffer.h"
#include "wpi/datalog/DataLogReader.h"
#include "wpi/datalog/DataLogReaderThread.h"
#include "wpi/mutex.h"
#include <wpi/fs.h>

static wpi::mutex entriesMutex;
static std::map<std::string, std::unique_ptr<wpi::log::fileexport::Entry>,
                std::less<>>
    entries;

namespace wpi::log::fileexport {
InputFile::InputFile(std::unique_ptr<wpi::log::DataLogReaderThread> datalog_)
    : filename{datalog_->GetBufferIdentifier()},
      stem{fs::path{filename}.stem().string()},
      datalog{std::move(datalog_)} {
  datalog->sigEntryAdded.connect([this](const wpi::log::StartRecordData& srd) {
    // add this entry to the map
    std::scoped_lock lock{entriesMutex};
    auto it = entries.find(srd.name);
    if (it == entries.end()) {
      // this entry isnt in the map, so lets add it
      it = entries.emplace(srd.name, std::make_unique<Entry>(srd)).first;
      // if this is a gui make it rebuild the entry tree somehow lmfao
      // maybe accept an optional callback???
      // TODO: RebuildEntryTree
    } else {
      // this entry IS already known, so lets make sure the start records match
      if (it->second->type != srd.type) {
        it->second->typeConflict = true;
      }
      if (it->second->metadata != srd.metadata) {
        it->second->metadataConflict = true;
      }
      it->second->inputFiles.emplace(this);
    }
  });
}

InputFile::~InputFile() {
  if (!datalog) {
    return;
  }
  std::scoped_lock lock{entriesMutex};
  bool changed = false;
  for (auto it = entries.begin(); it != entries.end();) {
    it->second->inputFiles.erase(this);
    if (it->second->inputFiles.empty()) {
      it = entries.erase(it);
      changed = true;
    } else {
      ++it;
    }
  }
  if (changed) {
    // TODO: rebuildentrytree
  }
}

std::unique_ptr<InputFile> LoadDataLog(std::string_view filename) {
  auto fileBuffer = wpi::MemoryBuffer::GetFile(filename);
  if (!fileBuffer) {
    return std::make_unique<InputFile>(
        filename, fmt::format("Could not open file: {}", fileBuffer.error()));
  }

  wpi::log::DataLogReader reader{std::move(*fileBuffer)};
  if (!reader.IsValid()) {
    return std::make_unique<InputFile>(filename, "Not a valid datalog file");
  }

  return std::make_unique<InputFile>(
      std::make_unique<wpi::log::DataLogReaderThread>(std::move(reader)));
}
}  // namespace wpi::log::fileexport
