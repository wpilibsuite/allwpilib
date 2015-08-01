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
#include "Log.h"
#include "NetworkConnection.h"

using namespace nt;

ATOMIC_STATIC_INIT(Storage)

Storage::Storage() {}

Storage::~Storage() {}

void Storage::SetOutgoing(QueueOutgoingFunc queue_outgoing, bool server) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_queue_outgoing = queue_outgoing;
  m_server = server;
}

void Storage::ClearOutgoing() {
  m_queue_outgoing = nullptr;
}

NT_Type Storage::GetEntryType(unsigned int id) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (id >= m_idmap.size()) return NT_UNASSIGNED;
  Entry* entry = m_idmap[id];
  if (!entry || !entry->value) return NT_UNASSIGNED;
  return entry->value->type();
}

void Storage::ProcessIncoming(std::shared_ptr<Message> msg,
                              NetworkConnection* conn) {
  std::unique_lock<std::mutex> lock(m_mutex);
  switch (msg->type()) {
    case Message::kKeepAlive:
      break;  // ignore
    case Message::kClientHello:
    case Message::kProtoUnsup:
    case Message::kServerHelloDone:
    case Message::kServerHello:
    case Message::kClientHelloDone:
      // shouldn't get these, but ignore if we do
      break;
    case Message::kEntryAssign: {
      unsigned int id = msg->id();
      StringRef name = msg->str();
      Entry* entry;
      bool may_need_update = false;
      if (m_server) {
        // if we're a server, id=0xffff requests are requests for an id
        // to be assigned, and we need to send the new assignment back to
        // the sender as well as all other connections.
        if (id == 0xffff) {
          // see if it was already assigned; ignore if so.
          if (m_entries.count(name) != 0) return;

          // create it locally
          id = m_idmap.size();
          auto& new_entry = m_entries[name];
          if (!new_entry) new_entry.reset(new Entry(name));
          entry = new_entry.get();
          entry->value = msg->value();
          entry->flags = msg->flags();
          entry->id = id;
          m_idmap.push_back(entry);

          // send the assignment to everyone (including the originator)
          if (m_queue_outgoing) {
            auto queue_outgoing = m_queue_outgoing;
            auto outmsg = Message::EntryAssign(
                name, id, entry->seq_num.value(), msg->value(), msg->flags());
            lock.unlock();
            queue_outgoing(outmsg, nullptr, nullptr);
          }
          return;
        }
        if (id >= m_idmap.size() || !m_idmap[id]) {
          // ignore arbitrary entry assignments
          // this can happen due to e.g. assignment to deleted entry
          lock.unlock();
          DEBUG("server: received assignment to unknown entry");
          return;
        }
        entry = m_idmap[id];
      } else {
        // clients simply accept new assignments
        if (id == 0xffff) {
          lock.unlock();
          DEBUG("client: received entry assignment request?");
          return;
        }
        if (id >= m_idmap.size()) m_idmap.resize(id+1);
        entry = m_idmap[id];
        if (!entry) {
          // create local
          auto& new_entry = m_entries[name];
          if (!new_entry) {
            // didn't exist at all (rather than just being a response to a
            // id assignment request)
            new_entry.reset(new Entry(name));
            new_entry->value = msg->value();
            new_entry->flags = msg->flags();
          } else
            may_need_update = true;  // we may need to send an update message
          entry = new_entry.get();
          entry->id = id;
          m_idmap[id] = entry;
          return;
        }
      }

      // common client and server handling

      // already exists; ignore if sequence number not higher than local
      SequenceNumber seq_num(msg->seq_num_uid());
      if (seq_num <= entry->seq_num) {
        if (may_need_update) {
          auto queue_outgoing = m_queue_outgoing;
          auto msg = Message::EntryUpdate(entry->id, entry->seq_num.value(),
                                          entry->value);
          lock.unlock();
          queue_outgoing(msg, nullptr, nullptr);
        }
        return;
      }

      // sanity check: name should match id
      if (msg->str() != entry->name) {
        lock.unlock();
        DEBUG("entry assignment for same id with different name?");
        return;
      }

      // update local
      entry->value = msg->value();
      entry->seq_num = seq_num;

      // don't update flags from a <3.0 remote (not part of message)
      if (conn->proto_rev() >= 0x0300) entry->flags = msg->flags();

      // broadcast to all other connections (note for client there won't
      // be any other connections, so don't bother)
      if (m_server && m_queue_outgoing) {
        auto queue_outgoing = m_queue_outgoing;
        auto outmsg =
            Message::EntryAssign(entry->name, id, msg->seq_num_uid(),
                                 msg->value(), entry->flags);
        lock.unlock();
        queue_outgoing(outmsg, nullptr, conn);
      }
      break;
    }
    case Message::kEntryUpdate: {
      unsigned int id = msg->id();
      if (id >= m_idmap.size() || !m_idmap[id]) {
        // ignore arbitrary entry updates;
        // this can happen due to deleted entries
        lock.unlock();
        DEBUG("received update to unknown entry");
        return;
      }
      Entry* entry = m_idmap[id];

      // ignore if sequence number not higher than local
      SequenceNumber seq_num(msg->seq_num_uid());
      if (seq_num <= entry->seq_num) return;

      // update local
      entry->value = msg->value();
      entry->seq_num = seq_num;

      // broadcast to all other connections (note for client there won't
      // be any other connections, so don't bother)
      if (m_server && m_queue_outgoing) {
        auto queue_outgoing = m_queue_outgoing;
        lock.unlock();
        queue_outgoing(msg, nullptr, conn);
      }
      break;
    }
    case Message::kFlagsUpdate: {
      unsigned int id = msg->id();
      if (id >= m_idmap.size() || !m_idmap[id]) {
        // ignore arbitrary entry updates;
        // this can happen due to deleted entries
        lock.unlock();
        DEBUG("received flags update to unknown entry");
        return;
      }
      Entry* entry = m_idmap[id];

      // update local
      entry->flags = msg->flags();

      // broadcast to all other connections (note for client there won't
      // be any other connections, so don't bother)
      if (m_server && m_queue_outgoing) {
        auto queue_outgoing = m_queue_outgoing;
        lock.unlock();
        queue_outgoing(msg, nullptr, conn);
      }
      break;
    }
    case Message::kEntryDelete: {
      unsigned int id = msg->id();
      if (id >= m_idmap.size() || !m_idmap[id]) {
        // ignore arbitrary entry updates;
        // this can happen due to deleted entries
        lock.unlock();
        DEBUG("received delete to unknown entry");
        return;
      }
      Entry* entry = m_idmap[id];

      // update local
      m_entries.erase(entry->name);  // erase from map
      m_idmap[id] = nullptr;  // delete it from idmap too

      // broadcast to all other connections (note for client there won't
      // be any other connections, so don't bother)
      if (m_server && m_queue_outgoing) {
        auto queue_outgoing = m_queue_outgoing;
        lock.unlock();
        queue_outgoing(msg, nullptr, conn);
      }
      break;
    }
    case Message::kClearEntries: {
      // update local
      m_entries.clear();
      m_idmap.resize(0);

      // broadcast to all other connections (note for client there won't
      // be any other connections, so don't bother)
      if (m_server && m_queue_outgoing) {
        auto queue_outgoing = m_queue_outgoing;
        lock.unlock();
        queue_outgoing(msg, nullptr, conn);
      }
      break;
    }
    case Message::kExecuteRpc:
    case Message::kRpcResponse:
      // TODO
      break;
    default:
      break;
  }
}

void Storage::GetInitialAssignments(
    NetworkConnection& conn, std::vector<std::shared_ptr<Message>>* msgs) {
  std::lock_guard<std::mutex> lock(m_mutex);
  conn.set_state(NetworkConnection::kSynchronized);
  for (auto& i : m_entries) {
    Entry* entry = i.getValue().get();
    msgs->emplace_back(Message::EntryAssign(i.getKey(), entry->id,
                                            entry->seq_num.value(),
                                            entry->value, entry->flags));
  }
}

void Storage::ApplyInitialAssignments(
    NetworkConnection& conn, llvm::ArrayRef<std::shared_ptr<Message>> msgs,
    bool new_server, std::vector<std::shared_ptr<Message>>* out_msgs) {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_server) return;  // should not do this on server

  conn.set_state(NetworkConnection::kSynchronized);

  std::vector<std::shared_ptr<Message>> update_msgs;

  // clear existing id's
  for (auto& i : m_entries) i.getValue()->id = 0xffff;

  // clear existing idmap
  m_idmap.resize(0);

  // apply assignments
  for (auto& msg : msgs) {
    if (!msg->Is(Message::kEntryAssign)) {
      DEBUG("client: received non-entry assignment request?");
      continue;
    }

    unsigned int id = msg->id();
    if (id == 0xffff) {
      DEBUG("client: received entry assignment request?");
      continue;
    }

    SequenceNumber seq_num(msg->seq_num_uid());
    StringRef name = msg->str();

    auto& entry = m_entries[name];
    if (!entry) {
      // doesn't currently exist
      entry.reset(new Entry(name));
      entry->value = msg->value();
      entry->flags = msg->flags();
      entry->seq_num = seq_num;
    } else {
      // if reconnect and sequence number not higher than local, then we
      // don't update the local value and instead send it back to the server
      // as an update message
      if (!new_server && seq_num <= entry->seq_num) {
        update_msgs.emplace_back(Message::EntryUpdate(
            entry->id, entry->seq_num.value(), entry->value));
      } else {
        entry->value = msg->value();
        entry->seq_num = seq_num;
        // don't update flags from a <3.0 remote (not part of message)
        if (conn.proto_rev() >= 0x0300) entry->flags = msg->flags();
      }
    }

    // set id and save to idmap
    entry->id = id;
    if (id >= m_idmap.size()) m_idmap.resize(id+1);
    m_idmap[id] = entry.get();
  }

  // generate assign messages for unassigned local entries
  for (auto& i : m_entries) {
    Entry* entry = i.getValue().get();
    if (entry->id != 0xffff) continue;
    out_msgs->emplace_back(Message::EntryAssign(entry->name, entry->id,
                                                entry->seq_num.value(),
                                                entry->value, entry->flags));
  }
  auto queue_outgoing = m_queue_outgoing;
  lock.unlock();
  for (auto& msg : update_msgs) queue_outgoing(msg, nullptr, nullptr);
}

std::shared_ptr<Value> Storage::GetEntryValue(StringRef name) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  return i == m_entries.end() ? nullptr : i->getValue()->value;
}

bool Storage::SetEntryValue(StringRef name, std::shared_ptr<Value> value) {
  if (name.empty()) return true;
  if (!value) return true;
  std::unique_lock<std::mutex> lock(m_mutex);
  auto& new_entry = m_entries[name];
  if (!new_entry) new_entry.reset(new Entry(name));
  Entry* entry = new_entry.get();
  auto old_value = entry->value;
  if (old_value && old_value->type() != value->type())
    return false;  // error on type mismatch
  entry->value = value;

  // if we're the server, assign an id if it doesn't have one
  if (m_server && entry->id == 0xffff) {
    unsigned int id = m_idmap.size();
    entry->id = id;
    m_idmap.push_back(entry);
  }

  // generate message
  if (!m_queue_outgoing) return true;
  auto queue_outgoing = m_queue_outgoing;
  if (!old_value) {
    auto msg = Message::EntryAssign(name, entry->id, entry->seq_num.value(),
                                    value, entry->flags);
    lock.unlock();
    queue_outgoing(msg, nullptr, nullptr);
  } else if (*old_value != *value) {
    ++entry->seq_num;
    // don't send an update if we don't have an assigned id yet
    if (entry->id != 0xffff) {
      auto msg =
          Message::EntryUpdate(entry->id, entry->seq_num.value(), value);
      lock.unlock();
      queue_outgoing(msg, nullptr, nullptr);
    }
  }
  return true;
}

void Storage::SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value) {
  if (name.empty()) return;
  if (!value) return;
  std::unique_lock<std::mutex> lock(m_mutex);
  auto& new_entry = m_entries[name];
  if (!new_entry) new_entry.reset(new Entry(name));
  Entry* entry = new_entry.get();
  auto old_value = entry->value;
  entry->value = value;
  if (old_value && *old_value == *value) return;

  // if we're the server, assign an id if it doesn't have one
  if (m_server && entry->id == 0xffff) {
    unsigned int id = m_idmap.size();
    entry->id = id;
    m_idmap.push_back(entry);
  }

  // generate message
  if (!m_queue_outgoing) return;
  auto queue_outgoing = m_queue_outgoing;
  if (!old_value || old_value->type() != value->type()) {
    ++entry->seq_num;
    auto msg = Message::EntryAssign(name, entry->id, entry->seq_num.value(),
                                    value, entry->flags);
    lock.unlock();
    queue_outgoing(msg, nullptr, nullptr);
  } else {
    ++entry->seq_num;
    // don't send an update if we don't have an assigned id yet
    if (entry->id != 0xffff) {
      auto msg =
          Message::EntryUpdate(entry->id, entry->seq_num.value(), value);
      lock.unlock();
      queue_outgoing(msg, nullptr, nullptr);
    }
  }
}

void Storage::SetEntryFlags(StringRef name, unsigned int flags) {
  if (name.empty()) return;
  std::unique_lock<std::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  if (i == m_entries.end()) return;
  Entry* entry = i->getValue().get();
  if (entry->flags == flags) return;
  entry->flags = flags;

  // generate message
  if (!m_queue_outgoing) return;
  auto queue_outgoing = m_queue_outgoing;
  unsigned int id = entry->id;
  // don't send an update if we don't have an assigned id yet
  if (id != 0xffff) {
    lock.unlock();
    queue_outgoing(Message::FlagsUpdate(id, flags), nullptr, nullptr);
  }
}

unsigned int Storage::GetEntryFlags(StringRef name) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  return i == m_entries.end() ? 0 : i->getValue()->flags;
}

void Storage::DeleteEntry(StringRef name) {
  std::unique_lock<std::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  if (i == m_entries.end()) return;
  Entry* entry = i->getValue().get();
  unsigned int id = entry->id;
  m_entries.erase(i);  // erase from map
  if (id < m_idmap.size()) m_idmap[id] = nullptr; 

  if (!entry->value) return;

  // if it had a value, generate message
  // don't send an update if we don't have an assigned id yet
  if (id != 0xffff) {
    if (!m_queue_outgoing) return;
    auto queue_outgoing = m_queue_outgoing;
    lock.unlock();
    queue_outgoing(Message::EntryDelete(id), nullptr, nullptr);
  }
}

void Storage::DeleteAllEntries() {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_entries.empty()) return;
  m_entries.clear();
  m_idmap.resize(0);

  // generate message
  if (!m_queue_outgoing) return;
  auto queue_outgoing = m_queue_outgoing;
  lock.unlock();
  queue_outgoing(Message::ClearEntries(), nullptr, nullptr);
}

std::vector<EntryInfo> Storage::GetEntryInfo(StringRef prefix,
                                             unsigned int types) {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::vector<EntryInfo> infos;
  for (auto& i : m_entries) {
    if (!i.getKey().startswith(prefix)) continue;
    Entry* entry = i.getValue().get();
    auto value = entry->value;
    if (!value) continue;
    if (types != 0 && (types & value->type()) == 0) continue;
    EntryInfo info;
    info.name = i.getKey();
    info.type = value->type();
    info.flags = entry->flags;
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
      Entry* entry = i.getValue().get();
      // only write persistent-flagged values
      if (!entry->IsPersistent()) continue;
      entries.push_back(std::make_pair(i.getKey(), entry->value));
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
    std::vector<std::shared_ptr<Message>> msgs;
    std::unique_lock<std::mutex> lock(m_mutex);
    for (auto& i : entries) {
      auto& new_entry = m_entries[i.first];
      if (!new_entry) new_entry.reset(new Entry(i.first));
      Entry* entry = new_entry.get();
      auto old_value = entry->value;
      entry->value = i.second;
      bool was_persist = entry->IsPersistent();
      if (!was_persist) entry->flags |= NT_PERSISTENT;

      // if we're the server, assign an id if it doesn't have one
      if (m_server && entry->id == 0xffff) {
        unsigned int id = m_idmap.size();
        entry->id = id;
        m_idmap.push_back(entry);
      }

      if (!m_queue_outgoing) continue;  // shortcut
      ++entry->seq_num;

      // put on update queue
      if (!old_value || old_value->type() != i.second->type())
        msgs.emplace_back(Message::EntryAssign(i.first, entry->id,
                                               entry->seq_num.value(),
                                               i.second, entry->flags));
      else if (entry->id != 0xffff) {
        // don't send an update if we don't have an assigned id yet
        if (*old_value != *i.second)
          msgs.emplace_back(Message::EntryUpdate(
              entry->id, entry->seq_num.value(), i.second));
        if (!was_persist)
          msgs.emplace_back(Message::FlagsUpdate(entry->id, entry->flags));
      }
    }

    if (m_queue_outgoing) {
      auto queue_outgoing = m_queue_outgoing;
      lock.unlock();
      for (auto& msg : msgs) queue_outgoing(std::move(msg), nullptr, nullptr);
    }
  }

  return true;
}
