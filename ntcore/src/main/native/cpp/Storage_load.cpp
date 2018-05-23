/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cctype>
#include <string>

#include <wpi/Base64.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/raw_istream.h>

#include "IDispatcher.h"
#include "IEntryNotifier.h"
#include "Storage.h"

using namespace nt;

namespace {

class LoadPersistentImpl {
 public:
  typedef std::pair<std::string, std::shared_ptr<Value>> Entry;
  typedef std::function<void(size_t line, const char* msg)> WarnFunc;

  LoadPersistentImpl(wpi::raw_istream& is, WarnFunc warn)
      : m_is(is), m_warn(warn) {}

  bool Load(StringRef prefix, std::vector<Entry>* entries);

 private:
  bool ReadLine();
  bool ReadHeader();
  NT_Type ReadType();
  wpi::StringRef ReadName(wpi::SmallVectorImpl<char>& buf);
  std::shared_ptr<Value> ReadValue(NT_Type type);
  std::shared_ptr<Value> ReadBooleanValue();
  std::shared_ptr<Value> ReadDoubleValue();
  std::shared_ptr<Value> ReadStringValue();
  std::shared_ptr<Value> ReadRawValue();
  std::shared_ptr<Value> ReadBooleanArrayValue();
  std::shared_ptr<Value> ReadDoubleArrayValue();
  std::shared_ptr<Value> ReadStringArrayValue();

  void Warn(const char* msg) {
    if (m_warn) m_warn(m_line_num, msg);
  }

  wpi::raw_istream& m_is;
  WarnFunc m_warn;

  wpi::StringRef m_line;
  wpi::SmallString<128> m_line_buf;
  size_t m_line_num = 0;

  std::vector<int> m_buf_boolean_array;
  std::vector<double> m_buf_double_array;
  std::vector<std::string> m_buf_string_array;
};

}  // namespace

/* Extracts an escaped string token.  Does not unescape the string.
 * If a string cannot be matched, an empty string is returned.
 * If the string is unterminated, an empty tail string is returned.
 * The returned token includes the starting and trailing quotes (unless the
 * string is unterminated).
 * Returns a pair containing the extracted token (if any) and the remaining
 * tail string.
 */
static std::pair<wpi::StringRef, wpi::StringRef> ReadStringToken(
    wpi::StringRef source) {
  // Match opening quote
  if (source.empty() || source.front() != '"')
    return std::make_pair(wpi::StringRef(), source);

  // Scan for ending double quote, checking for escaped as we go.
  size_t size = source.size();
  size_t pos;
  for (pos = 1; pos < size; ++pos) {
    if (source[pos] == '"' && source[pos - 1] != '\\') {
      ++pos;  // we want to include the trailing quote in the result
      break;
    }
  }
  return std::make_pair(source.slice(0, pos), source.substr(pos));
}

static int fromxdigit(char ch) {
  if (ch >= 'a' && ch <= 'f')
    return (ch - 'a' + 10);
  else if (ch >= 'A' && ch <= 'F')
    return (ch - 'A' + 10);
  else
    return ch - '0';
}

static wpi::StringRef UnescapeString(wpi::StringRef source,
                                     wpi::SmallVectorImpl<char>& buf) {
  assert(source.size() >= 2 && source.front() == '"' && source.back() == '"');
  buf.clear();
  buf.reserve(source.size() - 2);
  for (auto s = source.begin() + 1, end = source.end() - 1; s != end; ++s) {
    if (*s != '\\') {
      buf.push_back(*s);
      continue;
    }
    switch (*++s) {
      case '\\':
      case '"':
        buf.push_back(s[-1]);
        break;
      case 't':
        buf.push_back('\t');
        break;
      case 'n':
        buf.push_back('\n');
        break;
      case 'x': {
        if (!isxdigit(*(s + 1))) {
          buf.push_back('x');  // treat it like a unknown escape
          break;
        }
        int ch = fromxdigit(*++s);
        if (std::isxdigit(*(s + 1))) {
          ch <<= 4;
          ch |= fromxdigit(*++s);
        }
        buf.push_back(static_cast<char>(ch));
        break;
      }
      default:
        buf.push_back(s[-1]);
        break;
    }
  }
  return wpi::StringRef{buf.data(), buf.size()};
}

bool LoadPersistentImpl::Load(StringRef prefix, std::vector<Entry>* entries) {
  if (!ReadHeader()) return false;  // header

  while (ReadLine()) {
    // type
    NT_Type type = ReadType();
    if (type == NT_UNASSIGNED) {
      Warn("unrecognized type");
      continue;
    }

    // name
    wpi::SmallString<128> buf;
    wpi::StringRef name = ReadName(buf);
    if (name.empty() || !name.startswith(prefix)) continue;

    // =
    m_line = m_line.ltrim(" \t");
    if (m_line.empty() || m_line.front() != '=') {
      Warn("expected = after name");
      continue;
    }
    m_line = m_line.drop_front().ltrim(" \t");

    // value
    auto value = ReadValue(type);

    // move to entries
    if (value) entries->emplace_back(name, std::move(value));
  }
  return true;
}

bool LoadPersistentImpl::ReadLine() {
  // ignore blank lines and lines that start with ; or # (comments)
  while (!m_is.has_error()) {
    ++m_line_num;
    m_line = m_is.getline(m_line_buf, INT_MAX).trim();
    if (!m_line.empty() && m_line.front() != ';' && m_line.front() != '#')
      return true;
  }
  return false;
}

bool LoadPersistentImpl::ReadHeader() {
  // header
  if (!ReadLine() || m_line != "[NetworkTables Storage 3.0]") {
    Warn("header line mismatch, ignoring rest of file");
    return false;
  }
  return true;
}

NT_Type LoadPersistentImpl::ReadType() {
  wpi::StringRef tok;
  std::tie(tok, m_line) = m_line.split(' ');
  if (tok == "boolean") {
    return NT_BOOLEAN;
  } else if (tok == "double") {
    return NT_DOUBLE;
  } else if (tok == "string") {
    return NT_STRING;
  } else if (tok == "raw") {
    return NT_RAW;
  } else if (tok == "array") {
    wpi::StringRef array_tok;
    std::tie(array_tok, m_line) = m_line.split(' ');
    if (array_tok == "boolean")
      return NT_BOOLEAN_ARRAY;
    else if (array_tok == "double")
      return NT_DOUBLE_ARRAY;
    else if (array_tok == "string")
      return NT_STRING_ARRAY;
  }
  return NT_UNASSIGNED;
}

wpi::StringRef LoadPersistentImpl::ReadName(wpi::SmallVectorImpl<char>& buf) {
  wpi::StringRef tok;
  std::tie(tok, m_line) = ReadStringToken(m_line);
  if (tok.empty()) {
    Warn("missing name");
    return wpi::StringRef{};
  }
  if (tok.back() != '"') {
    Warn("unterminated name string");
    return wpi::StringRef{};
  }
  return UnescapeString(tok, buf);
}

std::shared_ptr<Value> LoadPersistentImpl::ReadValue(NT_Type type) {
  switch (type) {
    case NT_BOOLEAN:
      return ReadBooleanValue();
    case NT_DOUBLE:
      return ReadDoubleValue();
    case NT_STRING:
      return ReadStringValue();
    case NT_RAW:
      return ReadRawValue();
    case NT_BOOLEAN_ARRAY:
      return ReadBooleanArrayValue();
    case NT_DOUBLE_ARRAY:
      return ReadDoubleArrayValue();
    case NT_STRING_ARRAY:
      return ReadStringArrayValue();
    default:
      return nullptr;
  }
}

std::shared_ptr<Value> LoadPersistentImpl::ReadBooleanValue() {
  // only true or false is accepted
  if (m_line == "true") return Value::MakeBoolean(true);
  if (m_line == "false") return Value::MakeBoolean(false);
  Warn("unrecognized boolean value, not 'true' or 'false'");
  return nullptr;
}

std::shared_ptr<Value> LoadPersistentImpl::ReadDoubleValue() {
  // need to convert to null-terminated string for std::strtod()
  wpi::SmallString<64> buf = m_line;
  char* end;
  double v = std::strtod(buf.c_str(), &end);
  if (*end != '\0') {
    Warn("invalid double value");
    return nullptr;
  }
  return Value::MakeDouble(v);
}

std::shared_ptr<Value> LoadPersistentImpl::ReadStringValue() {
  wpi::StringRef tok;
  std::tie(tok, m_line) = ReadStringToken(m_line);
  if (tok.empty()) {
    Warn("missing string value");
    return nullptr;
  }
  if (tok.back() != '"') {
    Warn("unterminated string value");
    return nullptr;
  }
  wpi::SmallString<128> buf;
  return Value::MakeString(UnescapeString(tok, buf));
}

std::shared_ptr<Value> LoadPersistentImpl::ReadRawValue() {
  wpi::SmallString<128> buf;
  size_t nr;
  return Value::MakeRaw(wpi::Base64Decode(m_line, &nr, buf));
}

std::shared_ptr<Value> LoadPersistentImpl::ReadBooleanArrayValue() {
  m_buf_boolean_array.clear();
  while (!m_line.empty()) {
    wpi::StringRef tok;
    std::tie(tok, m_line) = m_line.split(',');
    tok = tok.trim(" \t");
    if (tok == "true") {
      m_buf_boolean_array.push_back(1);
    } else if (tok == "false") {
      m_buf_boolean_array.push_back(0);
    } else {
      Warn("unrecognized boolean value, not 'true' or 'false'");
      return nullptr;
    }
  }
  return Value::MakeBooleanArray(std::move(m_buf_boolean_array));
}

std::shared_ptr<Value> LoadPersistentImpl::ReadDoubleArrayValue() {
  m_buf_double_array.clear();
  while (!m_line.empty()) {
    wpi::StringRef tok;
    std::tie(tok, m_line) = m_line.split(',');
    tok = tok.trim(" \t");
    // need to convert to null-terminated string for std::strtod()
    wpi::SmallString<64> buf = tok;
    char* end;
    double v = std::strtod(buf.c_str(), &end);
    if (*end != '\0') {
      Warn("invalid double value");
      return nullptr;
    }
    m_buf_double_array.push_back(v);
  }

  return Value::MakeDoubleArray(std::move(m_buf_double_array));
}

std::shared_ptr<Value> LoadPersistentImpl::ReadStringArrayValue() {
  m_buf_string_array.clear();
  while (!m_line.empty()) {
    wpi::StringRef tok;
    std::tie(tok, m_line) = ReadStringToken(m_line);
    if (tok.empty()) {
      Warn("missing string value");
      return nullptr;
    }
    if (tok.back() != '"') {
      Warn("unterminated string value");
      return nullptr;
    }

    wpi::SmallString<128> buf;
    m_buf_string_array.push_back(UnescapeString(tok, buf));

    m_line = m_line.ltrim(" \t");
    if (m_line.empty()) break;
    if (m_line.front() != ',') {
      Warn("expected comma between strings");
      return nullptr;
    }
    m_line = m_line.drop_front().ltrim(" \t");
  }

  return Value::MakeStringArray(std::move(m_buf_string_array));
}

bool Storage::LoadEntries(
    wpi::raw_istream& is, const Twine& prefix, bool persistent,
    std::function<void(size_t line, const char* msg)> warn) {
  wpi::SmallString<128> prefixBuf;
  StringRef prefixStr = prefix.toStringRef(prefixBuf);

  // entries to add
  std::vector<LoadPersistentImpl::Entry> entries;

  // load file
  if (!LoadPersistentImpl(is, warn).Load(prefixStr, &entries)) return false;

  // copy values into storage as quickly as possible so lock isn't held
  std::vector<std::shared_ptr<Message>> msgs;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  for (auto& i : entries) {
    Entry* entry = GetOrNew(i.first);
    auto old_value = entry->value;
    entry->value = i.second;
    bool was_persist = entry->IsPersistent();
    if (!was_persist && persistent) entry->flags |= NT_PERSISTENT;

    // if we're the server, assign an id if it doesn't have one
    if (m_server && entry->id == 0xffff) {
      unsigned int id = m_idmap.size();
      entry->id = id;
      m_idmap.push_back(entry);
    }

    // notify (for local listeners)
    if (m_notifier.local_notifiers()) {
      if (!old_value) {
        m_notifier.NotifyEntry(entry->local_id, i.first, i.second,
                               NT_NOTIFY_NEW | NT_NOTIFY_LOCAL);
      } else if (*old_value != *i.second) {
        unsigned int notify_flags = NT_NOTIFY_UPDATE | NT_NOTIFY_LOCAL;
        if (!was_persist && persistent) notify_flags |= NT_NOTIFY_FLAGS;
        m_notifier.NotifyEntry(entry->local_id, i.first, i.second,
                               notify_flags);
      } else if (!was_persist && persistent) {
        m_notifier.NotifyEntry(entry->local_id, i.first, i.second,
                               NT_NOTIFY_FLAGS | NT_NOTIFY_LOCAL);
      }
    }

    if (!m_dispatcher) continue;  // shortcut
    ++entry->seq_num;

    // put on update queue
    if (!old_value || old_value->type() != i.second->type()) {
      msgs.emplace_back(Message::EntryAssign(
          i.first, entry->id, entry->seq_num.value(), i.second, entry->flags));
    } else if (entry->id != 0xffff) {
      // don't send an update if we don't have an assigned id yet
      if (*old_value != *i.second)
        msgs.emplace_back(
            Message::EntryUpdate(entry->id, entry->seq_num.value(), i.second));
      if (!was_persist)
        msgs.emplace_back(Message::FlagsUpdate(entry->id, entry->flags));
    }
  }

  if (m_dispatcher) {
    auto dispatcher = m_dispatcher;
    lock.unlock();
    for (auto& msg : msgs)
      dispatcher->QueueOutgoing(std::move(msg), nullptr, nullptr);
  }

  return true;
}

const char* Storage::LoadPersistent(
    const Twine& filename,
    std::function<void(size_t line, const char* msg)> warn) {
  std::error_code ec;
  wpi::raw_fd_istream is(filename, ec);
  if (ec.value() != 0) return "could not open file";
  if (!LoadEntries(is, "", true, warn)) return "error reading file";
  return nullptr;
}

const char* Storage::LoadEntries(
    const Twine& filename, const Twine& prefix,
    std::function<void(size_t line, const char* msg)> warn) {
  std::error_code ec;
  wpi::raw_fd_istream is(filename, ec);
  if (ec.value() != 0) return "could not open file";
  if (!LoadEntries(is, prefix, false, warn)) return "error reading file";
  return nullptr;
}
