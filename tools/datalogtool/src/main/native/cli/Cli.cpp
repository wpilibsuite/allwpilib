// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Cli.hpp"

#include <fcntl.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "CsvExport.hpp"
#include "Sftp.hpp"
#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/argparse.hpp"
#include "wpi/util/fmt/raw_ostream.hpp"
#include "wpi/util/fs.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/raw_ostream.hpp"

const char* GetWPILibVersion();

namespace {
constexpr std::string_view HELP =
    R"(Usage: datalogtool-cli <command> [options] [files...]

Commands:
  export      Export local .wpilog files to CSV (list or table)
  entries     List local log entries, types, and metadata as CSV
  list        List remote directories and .wpilog files as CSV
  download    Download selected remote logs
  delete      Delete selected remote logs without downloading

Use <command> --help for options. --version prints the WPILib version.
Existing output files are never overwritten.
Exit status: 0 success, 1 operation failed, 2 invalid arguments.
)";

using wpi::util::ArgumentParser;

void AddSelection(ArgumentParser& parser) {
  parser.add_argument("--entry").append().help(
      "Include an exact entry name (repeatable; default: all entries)");
  parser.add_argument("--entry-prefix")
      .append()
      .help("Include names starting with this prefix (repeatable)");
  parser.add_argument("--exclude")
      .append()
      .help("Exclude an exact entry name (repeatable)");
  parser.add_argument("--exclude-prefix")
      .append()
      .help("Exclude names starting with this prefix (repeatable)");
}

dlt::EntryFilter MakeSelection(const ArgumentParser& parser) {
  auto names = parser.get<std::vector<std::string>>("--entry");
  auto prefixes = parser.get<std::vector<std::string>>("--entry-prefix");
  auto excluded = parser.get<std::vector<std::string>>("--exclude");
  auto excludedPrefixes =
      parser.get<std::vector<std::string>>("--exclude-prefix");
  return [=](std::string_view name) {
    auto matches = [&](const auto& entries, const auto& starts) {
      return std::ranges::find(entries, name) != entries.end() ||
             std::ranges::any_of(starts, [&](auto&& prefix) {
               return name.starts_with(prefix);
             });
    };
    return ((names.empty() && prefixes.empty()) || matches(names, prefixes)) &&
           !matches(excluded, excludedPrefixes);
  };
}

void CheckDirectory(const std::string& directory) {
  if (!fs::is_directory(directory)) {
    throw std::runtime_error{
        std::format("not an existing directory: {}", directory)};
  }
}

// Create exclusively, check both write and close errors, and remove incomplete
// output. A failed output must never cause a remote log to be deleted.
template <typename Write>
void WriteNewFile(const fs::path& path, Write write) {
  std::error_code ec;
  wpi::util::raw_fd_ostream os{path.string(), ec, fs::CD_CreateNew};
  if (ec) {
    throw std::runtime_error{ec.message()};
  }
  try {
    write(os);
  } catch (...) {
    os.close();
    os.clear_error();
    fs::remove(path, ec);
    throw;
  }
  os.close();
  if (os.has_error()) {
    auto message = os.error().message();
    os.clear_error();
    fs::remove(path, ec);
    throw std::runtime_error{message};
  }
}

int Local(const ArgumentParser& parser, std::span<const std::string> files,
          bool exportCsv, int64_t fuzziness, wpi::util::raw_ostream& out,
          wpi::util::raw_ostream& err) {
  auto selected = MakeSelection(parser);
  std::string outputDir;
  auto style = dlt::CsvStyle::LIST;
  if (exportCsv) {
    outputDir = parser.get<std::string>("--output-dir");
    CheckDirectory(outputDir);
    if (parser.get<std::string>("--style") == "table") {
      style = dlt::CsvStyle::TABLE;
    }
  } else {
    out << "File,Name,Type,Metadata\n";
  }
  int result = 0;
  for (auto&& filename : files) {
    try {
      auto buffer = wpi::util::MemoryBuffer::GetFile(filename);
      if (!buffer) {
        throw std::runtime_error{buffer.error().message()};
      }
      wpi::log::DataLogReader reader{std::move(*buffer)};
      if (!reader.IsValid()) {
        throw std::runtime_error{"not a valid datalog file"};
      }
      if (exportCsv) {
        auto path = fs::path{outputDir} /
                    fs::path{filename}.filename().replace_extension("csv");
        WriteNewFile(path, [&](auto& os) {
          dlt::ExportCsv(reader, os, style, fuzziness, selected);
        });
        wpi::util::print(err, "Exported {} -> {}\n", filename, path.string());
      } else {
        size_t records = 0;
        std::set<std::string_view> names;
        // Emit each distinct start definition, retaining conflicting types and
        // metadata instead of silently hiding them across files or restarts.
        std::set<std::array<std::string_view, 3>> entries;
        for (auto&& record : reader) {
          ++records;
          wpi::log::StartRecordData data;
          if (record.IsStart() && record.GetStartData(&data)) {
            names.insert(data.name);
            if (selected(data.name)) {
              entries.insert({data.name, data.type, data.metadata});
            }
          }
        }
        for (auto&& entry : entries) {
          dlt::WriteCsvString(out, filename);
          for (auto&& field : entry) {
            out << ',';
            dlt::WriteCsvString(out, field);
          }
          out << '\n';
        }
        wpi::util::print(err, "{}: {} records, {} entries\n", filename, records,
                         names.size());
      }
    } catch (const std::exception& ex) {
      wpi::util::print(err, "{}: {}\n", filename, ex.what());
      result = 1;
    }
  }
  return result;
}

bool IsSafeFilename(std::string_view name) {
  return !name.empty() && name != "." && name != ".." &&
         name.find_first_of("/\\:") == std::string_view::npos &&
         name.find('\0') == std::string_view::npos;
}

int Remote(const ArgumentParser& parser, std::span<const std::string> names,
           std::string_view command, wpi::util::raw_ostream& out,
           wpi::util::raw_ostream& err) {
  std::string outputDir;
  if (command == "download") {
    outputDir = parser.get<std::string>("--output-dir");
    CheckDirectory(outputDir);
  }
  if (ssh_init() != SSH_OK) {
    throw std::runtime_error{"could not initialize SSH"};
  }
  struct SshCleanup {
    ~SshCleanup() { ssh_finalize(); }
  } cleanup;
  auto host = parser.get<std::string>("--host");
  if (wpi::util::parse_integer<unsigned int>(host, 10)) {
    host = "robot.local";  // Same team-number resolution as the GUI.
  }
  sftp::Session session{host, parser.get<int>("--port"),
                        parser.get<std::string>("--username"),
                        parser.get<std::string>("--password")};
  session.Connect();
  auto remoteDir = parser.get<std::string>("--remote-dir");
  if (!remoteDir.ends_with('/')) {
    remoteDir += '/';
  }
  auto files = session.ReadDir(remoteDir);
  std::ranges::sort(files, {}, &sftp::Attributes::name);
  if (command == "list") {
    out << "Name,Type,Size\n";
  }
  std::set<std::string> requested;
  if (command != "list") {
    requested.insert(names.begin(), names.end());
  }
  int result = 0;
  for (auto&& file : files) {
    if (file.name == "." || file.name == "..") {
      continue;
    }
    bool directory = file.type == SSH_FILEXFER_TYPE_DIRECTORY;
    bool log = file.type == SSH_FILEXFER_TYPE_REGULAR &&
               (file.flags & SSH_FILEXFER_ATTR_SIZE) != 0 &&
               file.name.ends_with(".wpilog");
    if (command == "list") {
      if (directory || log) {
        dlt::WriteCsvString(out, file.name);
        wpi::util::print(out, ",{},{}\n", directory ? "directory" : "file",
                         directory ? 0 : file.size);
      }
      continue;
    }
    if (!log ||
        (!requested.contains(file.name) && !parser.get<bool>("--all"))) {
      continue;
    }
    requested.erase(file.name);
    try {
      if (!IsSafeFilename(file.name)) {
        throw std::runtime_error{"unsafe remote filename"};
      }
      auto remotePath = remoteDir + file.name;
      if (command == "download") {
        auto localPath = fs::path{outputDir} / file.name;
        WriteNewFile(localPath, [&](auto& os) {
          auto input = session.Open(remotePath, O_RDONLY, 0);
          std::array<char, 32768> buffer;
          uint64_t remaining = file.size;
          while (remaining != 0) {
            auto copied = input.Read(
                buffer.data(), (std::min)(remaining, uint64_t{buffer.size()}));
            if (copied == 0) {
              throw std::runtime_error{"remote file ended before listed size"};
            }
            os.write(buffer.data(), copied);
            os.flush();
            if (os.has_error()) {
              throw std::runtime_error{os.error().message()};
            }
            remaining -= copied;
          }
          if (input.Read(buffer.data(), 1) != 0) {
            throw std::runtime_error{"remote file grew during download; retry"};
          }
        });
        wpi::util::print(err, "Downloaded {}\n", file.name);
        if (!parser.get<bool>("--delete-after")) {
          continue;
        }
      }
      session.Unlink(remotePath);
      wpi::util::print(err, "Deleted {}\n", file.name);
    } catch (const std::exception& ex) {
      wpi::util::print(err, "{}: {}\n", file.name, ex.what());
      result = 1;
    }
  }
  for (auto&& name : requested) {
    wpi::util::print(err, "{}: remote log not found\n", name);
    result = 1;
  }
  return result;
}
}  // namespace

int dlt::RunCli(std::span<const std::string> args, wpi::util::raw_ostream& out,
                wpi::util::raw_ostream& err) {
  if (args.empty() ||
      (args.size() == 1 && (args[0] == "--help" || args[0] == "-h"))) {
    out << HELP;
    return 0;
  }
  if (args.size() == 1 && (args[0] == "--version" || args[0] == "-v")) {
    out << GetWPILibVersion() << '\n';
    return 0;
  }
  const auto& command = args[0];
  bool local = command == "export" || command == "entries";
  if (!local && command != "list" && command != "download" &&
      command != "delete") {
    wpi::util::print(err, "Unknown command: {}\n{}", command, HELP);
    return 2;
  }
  ArgumentParser parser{std::format("datalogtool-cli {}", command),
                        GetWPILibVersion(), wpi::util::default_arguments::none};
  parser.add_argument("-h", "--help").flag().help("Show command help");
  if (local) {
    AddSelection(parser);
    if (command == "export") {
      parser.add_argument("-o", "--output-dir")
          .help("Existing CSV output directory (required)");
      parser.add_argument("--style")
          .default_value(std::string{"list"})
          .choices("list", "table")
          .help("CSV layout");
      parser.add_argument("--timestamp-fuzziness")
          .default_value(0.0)
          .scan<'g', double>()
          .help("Maximum table row timestamp span in milliseconds");
    }
  } else {
    parser.add_argument("-H", "--host")
        .help("Team number or server address (required)");
    parser.add_argument("--port").default_value(22).scan<'i', int>();
    parser.add_argument("--username").default_value(std::string{"systemcore"});
    parser.add_argument("--password").default_value(std::string{"systemcore"});
    parser.add_argument("--remote-dir")
        .default_value(std::string{"/home/systemcore/logs"});
    if (command != "list") {
      parser.add_argument("--all").flag().help(
          "Select all remote .wpilog files");
    }
    if (command == "download") {
      parser.add_argument("-o", "--output-dir")
          .help("Existing download directory (required)");
      parser.add_argument("--delete-after")
          .flag()
          .help("Delete each remote log only after its download succeeds");
    } else if (command == "delete") {
      parser.add_argument("--yes").flag().help(
          "Confirm deletion without download");
    }
  }
  if (command != "list") {
    parser.add_argument("files")
        .nargs(wpi::util::nargs_pattern::any)
        .help(local ? "Local .wpilog files" : "Exact remote .wpilog filenames");
  }
  int64_t fuzziness = 0;
  std::vector<std::string> files;
  try {
    std::vector<std::string> parseArgs{
        std::format("datalogtool-cli {}", command)};
    // The bundled argument parser does not implement the end-of-options marker.
    auto endOptions = std::find(args.begin() + 1, args.end(), "--");
    parseArgs.insert(parseArgs.end(), args.begin() + 1, endOptions);
    parser.parse_args(parseArgs);
    if (parser.get<bool>("--help")) {
      out << parser.help().str();
      return 0;
    }
    if (command != "list") {
      files = parser.get<std::vector<std::string>>("files");
    }
    if (endOptions != args.end()) {
      files.insert(files.end(), endOptions + 1, args.end());
    }
    auto require = [](bool condition, std::string_view message) {
      if (!condition) {
        throw std::runtime_error{std::string{message}};
      }
    };
    require(command != "list" || files.empty(),
            "list does not accept filenames");
    if (command == "export" || command == "download") {
      require(parser.present<std::string>("--output-dir").has_value(),
              "--output-dir is required");
    }
    if (local) {
      require(!files.empty(), "at least one input file is required");
      if (command == "export") {
        auto ms = parser.get<double>("--timestamp-fuzziness");
        constexpr double MAX_MS =
            std::numeric_limits<int64_t>::max() / 1'000'000;
        require(
            std::isfinite(ms) && ms >= 0 && ms <= MAX_MS,
            "timestamp fuzziness must be finite, nonnegative, and in range");
        require(!parser.is_used("--timestamp-fuzziness") ||
                    parser.get<std::string>("--style") == "table",
                "--timestamp-fuzziness requires --style table");
        fuzziness = static_cast<int64_t>(std::round(ms * 1'000'000.0));
      }
    } else {
      auto host = parser.present<std::string>("--host");
      require(host && !host->empty(), "--host is required");
      auto port = parser.get<int>("--port");
      require(port > 0 && port <= 65535, "port must be between 1 and 65535");
      require(!parser.get<std::string>("--remote-dir").empty(),
              "remote directory must not be empty");
      if (command != "list") {
        require(files.empty() == parser.get<bool>("--all"),
                "specify remote filenames or --all, but not both");
        for (auto&& name : files) {
          require(IsSafeFilename(name) && name.ends_with(".wpilog"),
                  "remote filenames must be .wpilog basenames, without a path");
        }
      }
      if (command == "delete") {
        require(parser.get<bool>("--yes"),
                "deleting remote logs requires --yes");
      }
    }
  } catch (const std::exception& ex) {
    wpi::util::print(err, "{}\nUse 'datalogtool-cli {} --help' for options.\n",
                     ex.what(), command);
    return 2;
  }
  try {
    return local
               ? Local(parser, files, command == "export", fuzziness, out, err)
               : Remote(parser, files, command, out, err);
  } catch (const std::exception& ex) {
    wpi::util::print(err, "{}\n", ex.what());
    return 1;
  }
}
