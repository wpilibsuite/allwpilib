/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Storage.h"

#include <string>
#include <tuple>

#include "llvm/StringExtras.h"
#include "Base64.h"

using namespace nt;

std::unique_ptr<Storage> Storage::m_instance;

Storage::Storage() {}

Storage::~Storage() {}

std::shared_ptr<StorageEntry> Storage::FindEntry(StringRef name) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  return i == m_entries.end() ? nullptr : i->getValue();
}

std::shared_ptr<StorageEntry> Storage::GetEntry(StringRef name) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto& entry = m_entries[name];
  if (!entry) entry = std::make_shared<StorageEntry>();
  return entry;
}

std::shared_ptr<Value> Storage::GetEntryValue(StringRef name) const {
  auto entry = FindEntry(name);
  if (!entry) return nullptr;
  return entry->value();
}

bool Storage::SetEntryValue(StringRef name, std::shared_ptr<Value> value) {
  if (name.empty()) return true;
  if (!value) return true;
  auto entry = GetEntry(name);
  auto old_value = entry->value();
  if (old_value && old_value->type() != value->type())
    return false;  // error on type mismatch
  entry->set_value(value);
  // put on update queue
  if (!old_value)
    m_updates.push(Update{entry, Update::kAssign});
  else if (*old_value != *value)
    m_updates.push(Update{entry, Update::kValueUpdate});
  return true;
}

void Storage::SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value) {
  if (name.empty()) return;
  if (!value) return;
  auto entry = GetEntry(name);
  auto old_value = entry->value();
  entry->set_value(value);
  if (!old_value || *old_value != *value) {
    // put on update queue
    if (!old_value || old_value->type() != value->type())
      m_updates.push(Update{entry, Update::kAssign});
    else
      m_updates.push(Update{entry, Update::kValueUpdate});
  }
}

void Storage::SetEntryFlags(StringRef name, unsigned int flags) {
  if (name.empty()) return;
  auto entry = FindEntry(name);
  if (!entry) return;
  if (entry->flags() != flags) {
    entry->set_flags(flags);
    m_updates.push(Update{entry, Update::kFlagsUpdate});  // put on update queue
  }
}

unsigned int Storage::GetEntryFlags(StringRef name) const {
  auto entry = FindEntry(name);
  if (!entry) return 0;
  return entry->flags();
}

void Storage::DeleteEntry(StringRef name) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  if (i == m_entries.end()) return;
  auto entry = i->getValue();
  m_entries.erase(i);  // erase from map
  // if it had a value, put on update queue
  if (entry->value()) m_updates.push(Update{entry, Update::kDelete});
}

void Storage::DeleteAllEntries() {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_entries.empty()) return;
  m_entries.clear();
  m_updates.push(Update{nullptr, Update::kDeleteAll});  // put on update queue
}

std::vector<EntryInfo> Storage::GetEntryInfo(StringRef prefix,
                                             unsigned int types) {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::vector<EntryInfo> infos;
  for (auto& i : m_entries) {
    if (!i.getKey().startswith(prefix)) continue;
    auto entry = i.getValue();
    auto value = entry->value();
    if (!value) continue;
    if (types != 0 && (types & value->type()) == 0) continue;
    EntryInfo info;
    info.name = i.getKey();
    info.type = value->type();
    info.flags = entry->flags();
    info.last_change = value->last_change();
    infos.push_back(std::move(info));
  }
  return infos;
}

/* Escapes and writes a string, including start and end double quotes */
static void WriteString(std::ostream& os, llvm::StringRef str) {
  os << '"';
  for (auto c : str) {
    switch (c) {
      case '\\':
        os << "\\\\";
        break;
      case '\t':
        os << "\\t";
        break;
      case '\n':
        os << "\\n";
        break;
      case '"':
        os << "\\\"";
        break;
      default:
        if (std::isprint(c)) {
          os << c;
          break;
        }

        // Write out the escaped representation.
        os << "\\x";
        os << llvm::hexdigit((c >> 4) & 0xF);
        os << llvm::hexdigit((c >> 0) & 0xF);
    }
  }
  os << '"';
}

void Storage::SavePersistent(std::ostream& os) const {
  // copy values out of storage as quickly as possible so lock isn't held
  std::vector<std::pair<std::string, std::shared_ptr<Value>>> entries;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    entries.reserve(m_entries.size());
    for (auto& i : m_entries) {
      auto entry = i.getValue();
      // only write persistent-flagged values
      if (!entry->IsPersistent()) continue;
      entries.push_back(std::make_pair(i.getKey(), entry->value()));
    }
  }

  // sort in name order
  std::sort(entries.begin(), entries.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });

  std::string base64_encoded;

  // header
  os << "[NetworkTables Storage 3.0]\n";

  for (auto& i : entries) {
    // type
    auto v = i.second;
    if (!v) continue;
    switch (v->type()) {
      case NT_BOOLEAN:
        os << "boolean ";
        break;
      case NT_DOUBLE:
        os << "double ";
        break;
      case NT_STRING:
        os << "string ";
        break;
      case NT_RAW:
        os << "raw ";
        break;
      case NT_BOOLEAN_ARRAY:
        os << "array boolean ";
        break;
      case NT_DOUBLE_ARRAY:
        os << "array double ";
        break;
      case NT_STRING_ARRAY:
        os << "array string ";
        break;
      default:
        continue;
    }

    // name
    WriteString(os, i.first);

    // =
    os << '=';

    // value
    switch (v->type()) {
      case NT_BOOLEAN:
        os << (v->GetBoolean() ? "true" : "false");
        break;
      case NT_DOUBLE:
        os << v->GetDouble();
        break;
      case NT_STRING:
        WriteString(os, v->GetString());
        break;
      case NT_RAW:
        Base64Encode(v->GetRaw(), &base64_encoded);
        os << base64_encoded;
        break;
      case NT_BOOLEAN_ARRAY: {
        bool first = true;
        for (auto elem : v->GetBooleanArray()) {
          if (!first) os << ',';
          first = false;
          os << (elem ? "true" : "false");
        }
        break;
      }
      case NT_DOUBLE_ARRAY: {
        bool first = true;
        for (auto elem : v->GetDoubleArray()) {
          if (!first) os << ',';
          first = false;
          os << elem;
        }
        break;
      }
      case NT_STRING_ARRAY: {
        bool first = true;
        for (auto& elem : v->GetStringArray()) {
          if (!first) os << ',';
          first = false;
          WriteString(os, elem);
        }
        break;
      }
      default:
        break;
    }

    // eol
    os << '\n';
  }
}

/* Extracts an escaped string token.  Does not unescape the string.
 * If a string cannot be matched, an empty string is returned.
 * If the string is unterminated, an empty tail string is returned.
 * The returned token includes the starting and trailing quotes (unless the
 * string is unterminated).
 * Returns a pair containing the extracted token (if any) and the remaining
 * tail string.
 */
static std::pair<llvm::StringRef, llvm::StringRef> ReadStringToken(
    llvm::StringRef source) {
  // Match opening quote
  if (source.empty() || source.front() != '"')
    return std::make_pair(llvm::StringRef(), source);

  // Scan for ending double quote, checking for escaped as we go.
  std::size_t size = source.size();
  std::size_t pos;
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

static void UnescapeString(llvm::StringRef source, std::string* dest) {
  assert(source.size() >= 2 && source.front() == '"' && source.back() == '"');
  dest->clear();
  dest->reserve(source.size() - 2);
  for (auto s = source.begin() + 1, end = source.end() - 1; s != end; ++s) {
    if (*s != '\\') {
      dest->push_back(*s);
      continue;
    }
    switch (*++s) {
      case '\\':
      case '"':
        dest->push_back(s[-1]);
        break;
      case 't':
        dest->push_back('\t');
        break;
      case 'n':
        dest->push_back('\n');
        break;
      case 'x': {
        if (!isxdigit(*(s+1))) {
          dest->push_back('x');  // treat it like a unknown escape
          break;
        }
        int ch = fromxdigit(*++s);
        if (isxdigit(*(s+1))) {
          ch <<= 4;
          ch |= fromxdigit(*++s);
        }
        dest->push_back(static_cast<char>(ch));
        break;
      }
      default:
        dest->push_back(s[-1]);
        break;
    }
  }
}

bool Storage::LoadPersistent(
    std::istream& is,
    std::function<void(std::size_t line, const char* msg)> warn) {
  std::string line_str;
  std::size_t line_num = 1;

  // entries to add
  std::vector<std::pair<std::string, std::shared_ptr<Value>>> entries;

  // declare these outside the loop to reduce reallocs
  std::string name, str;
  std::vector<int> boolean_array;
  std::vector<double> double_array;
  std::vector<std::string> string_array;

  // ignore blank lines and lines that start with ; or # (comments)
  while (std::getline(is, line_str)) {
    llvm::StringRef line = llvm::StringRef(line_str).trim();
    if (!line.empty() && line.front() != ';' && line.front() != '#')
      break;
  }

  // header
  if (line_str != "[NetworkTables Storage 3.0]") {
    if (warn) warn(line_num, "header line mismatch, ignoring rest of file");
    return false;
  }

  while (std::getline(is, line_str)) {
    llvm::StringRef line = llvm::StringRef(line_str).trim();
    ++line_num;

    // ignore blank lines and lines that start with ; or # (comments)
    if (line.empty() || line.front() == ';' || line.front() == '#')
      continue;

    // type
    llvm::StringRef type_tok;
    std::tie(type_tok, line) = line.split(' ');
    NT_Type type = NT_UNASSIGNED;
    if (type_tok == "boolean") type = NT_BOOLEAN;
    else if (type_tok == "double") type = NT_DOUBLE;
    else if (type_tok == "string") type = NT_STRING;
    else if (type_tok == "raw") type = NT_RAW;
    else if (type_tok == "array") {
      llvm::StringRef array_tok;
      std::tie(array_tok, line) = line.split(' ');
      if (array_tok == "boolean") type = NT_BOOLEAN_ARRAY;
      else if (array_tok == "double") type = NT_DOUBLE_ARRAY;
      else if (array_tok == "string") type = NT_STRING_ARRAY;
    }
    if (type == NT_UNASSIGNED) {
      if (warn) warn(line_num, "unrecognized type");
      continue;
    }

    // name
    llvm::StringRef name_tok;
    std::tie(name_tok, line) = ReadStringToken(line);
    if (name_tok.empty()) {
      if (warn) warn(line_num, "missing name");
      continue;
    }
    if (name_tok.back() != '"') {
      if (warn) warn(line_num, "unterminated name string");
      continue;
    }
    UnescapeString(name_tok, &name);

    // =
    line = line.ltrim(" \t");
    if (line.empty() || line.front() != '=') {
      if (warn) warn(line_num, "expected = after name");
      continue;
    }
    line = line.drop_front().ltrim(" \t");

    // value
    std::shared_ptr<Value> value;
    switch (type) {
      case NT_BOOLEAN:
        // only true or false is accepted
        if (line == "true")
          value = Value::MakeBoolean(true);
        else if (line == "false")
          value = Value::MakeBoolean(false);
        else {
          if (warn)
            warn(line_num, "unrecognized boolean value, not 'true' or 'false'");
          goto next_line;
        }
        break;
      case NT_DOUBLE: {
        // need to convert to null-terminated string for strtod()
        str.clear();
        str += line;
        char* end;
        double v = std::strtod(str.c_str(), &end);
        if (*end != '\0') {
          if (warn) warn(line_num, "invalid double value");
          goto next_line;
        }
        value = Value::MakeDouble(v);
        break;
      }
      case NT_STRING: {
        llvm::StringRef str_tok;
        std::tie(str_tok, line) = ReadStringToken(line);
        if (str_tok.empty()) {
          if (warn) warn(line_num, "missing string value");
          goto next_line;
        }
        if (str_tok.back() != '"') {
          if (warn) warn(line_num, "unterminated string value");
          goto next_line;
        }
        UnescapeString(str_tok, &str);
        value = Value::MakeString(std::move(str));
        break;
      }
      case NT_RAW:
        Base64Decode(line, &str);
        value = Value::MakeRaw(std::move(str));
        break;
      case NT_BOOLEAN_ARRAY: {
        llvm::StringRef elem_tok;
        boolean_array.clear();
        while (!line.empty()) {
          std::tie(elem_tok, line) = line.split(',');
          elem_tok = elem_tok.trim(" \t");
          if (elem_tok == "true")
            boolean_array.push_back(1);
          else if (elem_tok == "false")
            boolean_array.push_back(0);
          else {
            if (warn)
              warn(line_num,
                   "unrecognized boolean value, not 'true' or 'false'");
            goto next_line;
          }
        }

        value = Value::MakeBooleanArray(std::move(boolean_array));
        break;
      }
      case NT_DOUBLE_ARRAY: {
        llvm::StringRef elem_tok;
        double_array.clear();
        while (!line.empty()) {
          std::tie(elem_tok, line) = line.split(',');
          elem_tok = elem_tok.trim(" \t");
          // need to convert to null-terminated string for strtod()
          str.clear();
          str += elem_tok;
          char* end;
          double v = std::strtod(str.c_str(), &end);
          if (*end != '\0') {
            if (warn) warn(line_num, "invalid double value");
            goto next_line;
          }
          double_array.push_back(v);
        }

        value = Value::MakeDoubleArray(std::move(double_array));
        break;
      }
      case NT_STRING_ARRAY: {
        llvm::StringRef elem_tok;
        string_array.clear();
        while (!line.empty()) {
          std::tie(elem_tok, line) = ReadStringToken(line);
          if (elem_tok.empty()) {
            if (warn) warn(line_num, "missing string value");
            goto next_line;
          }
          if (elem_tok.back() != '"') {
            if (warn) warn(line_num, "unterminated string value");
            goto next_line;
          }

          UnescapeString(elem_tok, &str);
          string_array.push_back(std::move(str));

          line = line.ltrim(" \t");
          if (line.empty()) break;
          if (line.front() != ',') {
            if (warn) warn(line_num, "expected comma between strings");
            goto next_line;
          }
          line = line.drop_front().ltrim(" \t");
        }

        value = Value::MakeStringArray(std::move(string_array));
        break;
      }
      default:
        break;
    }
    if (!name.empty() && value)
      entries.push_back(std::make_pair(std::move(name), std::move(value)));
next_line:
    ;
  }

  // copy values into storage as quickly as possible so lock isn't held
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& i : entries) {
      auto& entry = m_entries[i.first];
      if (!entry) entry = std::make_shared<StorageEntry>();
      auto old_value = entry->value();
      entry->set_value(i.second);
      bool was_persist = entry->IsPersistent();
      if (!was_persist) entry->set_flags(entry->flags() | NT_PERSISTENT);

      // put on update queue
      if (!old_value || old_value->type() != i.second->type())
        m_updates.push(Update{entry, Update::kAssign});
      else {
        if (*old_value != *i.second)
          m_updates.push(Update{entry, Update::kValueUpdate});
        if (!was_persist)
          m_updates.push(Update{entry, Update::kFlagsUpdate});
      }
    }
  }

  return true;
}
