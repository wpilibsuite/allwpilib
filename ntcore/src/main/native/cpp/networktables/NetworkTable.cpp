/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "networktables/NetworkTable.h"

#include <algorithm>

#include <wpi/SmallString.h>
#include <wpi/StringMap.h>
#include <wpi/raw_ostream.h>

#include "networktables/NetworkTableInstance.h"
#include "ntcore.h"
#include "tables/ITableListener.h"

using namespace nt;

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

const char NetworkTable::PATH_SEPARATOR_CHAR = '/';
std::string NetworkTable::s_persistent_filename = "networktables.ini";
bool NetworkTable::s_client = false;
bool NetworkTable::s_enable_ds = true;
bool NetworkTable::s_running = false;
unsigned int NetworkTable::s_port = NT_DEFAULT_PORT;

StringRef NetworkTable::BasenameKey(StringRef key) {
  size_t slash = key.rfind(PATH_SEPARATOR_CHAR);
  if (slash == StringRef::npos) return key;
  return key.substr(slash + 1);
}

std::string NetworkTable::NormalizeKey(const Twine& key,
                                       bool withLeadingSlash) {
  wpi::SmallString<128> buf;
  return NormalizeKey(key, buf, withLeadingSlash);
}

StringRef NetworkTable::NormalizeKey(const Twine& key,
                                     wpi::SmallVectorImpl<char>& buf,
                                     bool withLeadingSlash) {
  buf.clear();
  if (withLeadingSlash) buf.push_back(PATH_SEPARATOR_CHAR);
  // for each path element, add it with a slash following
  wpi::SmallString<128> keyBuf;
  StringRef keyStr = key.toStringRef(keyBuf);
  wpi::SmallVector<StringRef, 16> parts;
  keyStr.split(parts, PATH_SEPARATOR_CHAR, -1, false);
  for (auto i = parts.begin(); i != parts.end(); ++i) {
    buf.append(i->begin(), i->end());
    buf.push_back(PATH_SEPARATOR_CHAR);
  }
  // remove trailing slash if the input key didn't have one
  if (!keyStr.empty() && keyStr.back() != PATH_SEPARATOR_CHAR) buf.pop_back();
  return StringRef(buf.data(), buf.size());
}

std::vector<std::string> NetworkTable::GetHierarchy(const Twine& key) {
  std::vector<std::string> hierarchy;
  hierarchy.emplace_back(1, PATH_SEPARATOR_CHAR);
  // for each path element, add it to the end of what we built previously
  wpi::SmallString<128> keyBuf;
  StringRef keyStr = key.toStringRef(keyBuf);
  wpi::SmallString<128> path;
  wpi::SmallVector<StringRef, 16> parts;
  keyStr.split(parts, PATH_SEPARATOR_CHAR, -1, false);
  if (!parts.empty()) {
    for (auto i = parts.begin(); i != parts.end(); ++i) {
      path += PATH_SEPARATOR_CHAR;
      path += *i;
      hierarchy.emplace_back(path.str());
    }
    // handle trailing slash
    if (keyStr.back() == PATH_SEPARATOR_CHAR) {
      path += PATH_SEPARATOR_CHAR;
      hierarchy.emplace_back(path.str());
    }
  }
  return hierarchy;
}

void NetworkTable::Initialize() {
  if (s_running) Shutdown();
  auto inst = NetworkTableInstance::GetDefault();
  if (s_client) {
    inst.StartClient();
    if (s_enable_ds) inst.StartDSClient(s_port);
  } else {
    inst.StartServer(s_persistent_filename, "", s_port);
  }
  s_running = true;
}

void NetworkTable::Shutdown() {
  if (!s_running) return;
  auto inst = NetworkTableInstance::GetDefault();
  if (s_client) {
    inst.StopDSClient();
    inst.StopClient();
  } else {
    inst.StopServer();
  }
  s_running = false;
}

void NetworkTable::SetClientMode() { s_client = true; }

void NetworkTable::SetServerMode() { s_client = false; }

void NetworkTable::SetTeam(int team) {
  auto inst = NetworkTableInstance::GetDefault();
  inst.SetServerTeam(team, s_port);
  if (s_enable_ds) inst.StartDSClient(s_port);
}

void NetworkTable::SetIPAddress(StringRef address) {
  auto inst = NetworkTableInstance::GetDefault();
  wpi::SmallString<32> addr_copy{address};
  inst.SetServer(addr_copy.c_str(), s_port);

  // Stop the DS client if we're explicitly connecting to localhost
  if (address == "localhost" || address == "127.0.0.1")
    inst.StopDSClient();
  else if (s_enable_ds)
    inst.StartDSClient(s_port);
}

void NetworkTable::SetIPAddress(ArrayRef<std::string> addresses) {
  auto inst = NetworkTableInstance::GetDefault();
  wpi::SmallVector<StringRef, 8> servers;
  for (const auto& ip_address : addresses) servers.emplace_back(ip_address);
  inst.SetServer(servers, s_port);

  // Stop the DS client if we're explicitly connecting to localhost
  if (!addresses.empty() &&
      (addresses[0] == "localhost" || addresses[0] == "127.0.0.1"))
    inst.StopDSClient();
  else if (s_enable_ds)
    inst.StartDSClient(s_port);
}

void NetworkTable::SetPort(unsigned int port) { s_port = port; }

void NetworkTable::SetDSClientEnabled(bool enabled) {
  auto inst = NetworkTableInstance::GetDefault();
  s_enable_ds = enabled;
  if (s_enable_ds)
    inst.StartDSClient(s_port);
  else
    inst.StopDSClient();
}

void NetworkTable::SetPersistentFilename(StringRef filename) {
  s_persistent_filename = filename;
}

void NetworkTable::SetNetworkIdentity(StringRef name) {
  NetworkTableInstance::GetDefault().SetNetworkIdentity(name);
}

void NetworkTable::GlobalDeleteAll() {
  NetworkTableInstance::GetDefault().DeleteAllEntries();
}

void NetworkTable::Flush() { NetworkTableInstance::GetDefault().Flush(); }

void NetworkTable::SetUpdateRate(double interval) {
  NetworkTableInstance::GetDefault().SetUpdateRate(interval);
}

const char* NetworkTable::SavePersistent(StringRef filename) {
  return NetworkTableInstance::GetDefault().SavePersistent(filename);
}

const char* NetworkTable::LoadPersistent(
    StringRef filename,
    std::function<void(size_t line, const char* msg)> warn) {
  return NetworkTableInstance::GetDefault().LoadPersistent(filename, warn);
}

std::shared_ptr<NetworkTable> NetworkTable::GetTable(StringRef key) {
  if (!s_running) Initialize();
  return NetworkTableInstance::GetDefault().GetTable(key);
}

NetworkTable::NetworkTable(NT_Inst inst, const Twine& path, const private_init&)
    : m_inst(inst), m_path(path.str()) {}

NetworkTable::~NetworkTable() {
  for (auto& i : m_listeners) RemoveEntryListener(i.second);
  for (auto i : m_lambdaListeners) RemoveEntryListener(i);
}

NetworkTableInstance NetworkTable::GetInstance() const {
  return NetworkTableInstance{m_inst};
}

NetworkTableEntry NetworkTable::GetEntry(const Twine& key) const {
  wpi::SmallString<128> keyBuf;
  StringRef keyStr = key.toStringRef(keyBuf);
  std::lock_guard<wpi::mutex> lock(m_mutex);
  NT_Entry& entry = m_entries[keyStr];
  if (entry == 0) {
    entry = nt::GetEntry(m_inst, m_path + Twine(PATH_SEPARATOR_CHAR) + keyStr);
  }
  return NetworkTableEntry{entry};
}

NT_EntryListener NetworkTable::AddEntryListener(TableEntryListener listener,
                                                unsigned int flags) const {
  size_t prefix_len = m_path.size() + 1;
  return nt::AddEntryListener(
      m_inst, m_path + Twine(PATH_SEPARATOR_CHAR),
      [=](const EntryNotification& event) {
        StringRef relative_key = event.name.substr(prefix_len);
        if (relative_key.find(PATH_SEPARATOR_CHAR) != StringRef::npos) return;
        listener(const_cast<NetworkTable*>(this), relative_key,
                 NetworkTableEntry{event.entry}, event.value, event.flags);
      },
      flags);
}

NT_EntryListener NetworkTable::AddEntryListener(const Twine& key,
                                                TableEntryListener listener,
                                                unsigned int flags) const {
  size_t prefix_len = m_path.size() + 1;
  auto entry = GetEntry(key);
  return nt::AddEntryListener(entry.GetHandle(),
                              [=](const EntryNotification& event) {
                                listener(const_cast<NetworkTable*>(this),
                                         event.name.substr(prefix_len), entry,
                                         event.value, event.flags);
                              },
                              flags);
}

void NetworkTable::RemoveEntryListener(NT_EntryListener listener) const {
  nt::RemoveEntryListener(listener);
}

void NetworkTable::AddTableListener(ITableListener* listener) {
  AddTableListenerEx(listener, NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
}

void NetworkTable::AddTableListener(ITableListener* listener,
                                    bool immediateNotify) {
  unsigned int flags = NT_NOTIFY_NEW | NT_NOTIFY_UPDATE;
  if (immediateNotify) flags |= NT_NOTIFY_IMMEDIATE;
  AddTableListenerEx(listener, flags);
}

void NetworkTable::AddTableListenerEx(ITableListener* listener,
                                      unsigned int flags) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  wpi::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  size_t prefix_len = path.size();
  NT_EntryListener id = nt::AddEntryListener(
      m_inst, path,
      [=](const EntryNotification& event) {
        StringRef relative_key = event.name.substr(prefix_len);
        if (relative_key.find(PATH_SEPARATOR_CHAR) != StringRef::npos) return;
        listener->ValueChangedEx(this, relative_key, event.value, event.flags);
      },
      flags);
  m_listeners.emplace_back(listener, id);
}

void NetworkTable::AddTableListener(StringRef key, ITableListener* listener,
                                    bool immediateNotify) {
  unsigned int flags = NT_NOTIFY_NEW | NT_NOTIFY_UPDATE;
  if (immediateNotify) flags |= NT_NOTIFY_IMMEDIATE;
  AddTableListenerEx(key, listener, flags);
}

void NetworkTable::AddTableListenerEx(StringRef key, ITableListener* listener,
                                      unsigned int flags) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  size_t prefix_len = m_path.size() + 1;
  auto entry = GetEntry(key);
  NT_EntryListener id = nt::AddEntryListener(
      entry.GetHandle(),
      [=](const EntryNotification& event) {
        listener->ValueChangedEx(this, event.name.substr(prefix_len),
                                 event.value, event.flags);
      },
      flags);
  m_listeners.emplace_back(listener, id);
}

void NetworkTable::AddSubTableListener(ITableListener* listener) {
  AddSubTableListener(listener, false);
}

NT_EntryListener NetworkTable::AddSubTableListener(TableListener listener,
                                                   bool localNotify) {
  size_t prefix_len = m_path.size() + 1;

  // The lambda needs to be copyable, but StringMap is not, so use
  // a shared_ptr to it.
  auto notified_tables = std::make_shared<wpi::StringMap<char>>();

  unsigned int flags = NT_NOTIFY_NEW | NT_NOTIFY_IMMEDIATE;
  if (localNotify) flags |= NT_NOTIFY_LOCAL;
  NT_EntryListener id = nt::AddEntryListener(
      m_inst, m_path + Twine(PATH_SEPARATOR_CHAR),
      [=](const EntryNotification& event) {
        StringRef relative_key = event.name.substr(prefix_len);
        auto end_sub_table = relative_key.find(PATH_SEPARATOR_CHAR);
        if (end_sub_table == StringRef::npos) return;
        StringRef sub_table_key = relative_key.substr(0, end_sub_table);
        if (notified_tables->find(sub_table_key) == notified_tables->end())
          return;
        notified_tables->insert(std::make_pair(sub_table_key, '\0'));
        listener(this, sub_table_key, this->GetSubTable(sub_table_key));
      },
      flags);
  m_lambdaListeners.emplace_back(id);
  return id;
}

void NetworkTable::RemoveTableListener(NT_EntryListener listener) {
  nt::RemoveEntryListener(listener);
  auto matches_begin =
      std::remove(m_lambdaListeners.begin(), m_lambdaListeners.end(), listener);
  m_lambdaListeners.erase(matches_begin, m_lambdaListeners.end());
}

void NetworkTable::AddSubTableListener(ITableListener* listener,
                                       bool localNotify) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  size_t prefix_len = m_path.size() + 1;

  // The lambda needs to be copyable, but StringMap is not, so use
  // a shared_ptr to it.
  auto notified_tables = std::make_shared<wpi::StringMap<char>>();

  unsigned int flags = NT_NOTIFY_NEW | NT_NOTIFY_IMMEDIATE;
  if (localNotify) flags |= NT_NOTIFY_LOCAL;
  NT_EntryListener id = nt::AddEntryListener(
      m_inst, m_path + Twine(PATH_SEPARATOR_CHAR),
      [=](const EntryNotification& event) {
        StringRef relative_key = event.name.substr(prefix_len);
        auto end_sub_table = relative_key.find(PATH_SEPARATOR_CHAR);
        if (end_sub_table == StringRef::npos) return;
        StringRef sub_table_key = relative_key.substr(0, end_sub_table);
        if (notified_tables->find(sub_table_key) == notified_tables->end())
          return;
        notified_tables->insert(std::make_pair(sub_table_key, '\0'));
        listener->ValueChangedEx(this, sub_table_key, nullptr, event.flags);
      },
      flags);
  m_listeners.emplace_back(listener, id);
}

void NetworkTable::RemoveTableListener(ITableListener* listener) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  auto matches_begin =
      std::remove_if(m_listeners.begin(), m_listeners.end(),
                     [=](const Listener& x) { return x.first == listener; });

  for (auto i = matches_begin; i != m_listeners.end(); ++i)
    RemoveEntryListener(i->second);
  m_listeners.erase(matches_begin, m_listeners.end());
}

std::shared_ptr<NetworkTable> NetworkTable::GetSubTable(
    const Twine& key) const {
  return std::make_shared<NetworkTable>(
      m_inst, m_path + Twine(PATH_SEPARATOR_CHAR) + key, private_init{});
}

bool NetworkTable::ContainsKey(const Twine& key) const {
  if (key.isTriviallyEmpty() ||
      (key.isSingleStringRef() && key.getSingleStringRef().empty()))
    return false;
  return GetEntry(key).Exists();
}

bool NetworkTable::ContainsSubTable(const Twine& key) const {
  return !GetEntryInfo(m_inst,
                       m_path + Twine(PATH_SEPARATOR_CHAR) + key +
                           Twine(PATH_SEPARATOR_CHAR),
                       0)
              .empty();
}

std::vector<std::string> NetworkTable::GetKeys(int types) const {
  std::vector<std::string> keys;
  size_t prefix_len = m_path.size() + 1;
  auto infos = GetEntryInfo(m_inst, m_path + Twine(PATH_SEPARATOR_CHAR), types);
  std::lock_guard<wpi::mutex> lock(m_mutex);
  for (auto& info : infos) {
    auto relative_key = StringRef(info.name).substr(prefix_len);
    if (relative_key.find(PATH_SEPARATOR_CHAR) != StringRef::npos) continue;
    keys.push_back(relative_key);
    m_entries[relative_key] = info.entry;
  }
  return keys;
}

std::vector<std::string> NetworkTable::GetSubTables() const {
  std::vector<std::string> keys;
  size_t prefix_len = m_path.size() + 1;
  for (auto& entry :
       GetEntryInfo(m_inst, m_path + Twine(PATH_SEPARATOR_CHAR), 0)) {
    auto relative_key = StringRef(entry.name).substr(prefix_len);
    size_t end_subtable = relative_key.find(PATH_SEPARATOR_CHAR);
    if (end_subtable == StringRef::npos) continue;
    keys.push_back(relative_key.substr(0, end_subtable));
  }
  return keys;
}

void NetworkTable::SetPersistent(StringRef key) {
  GetEntry(key).SetPersistent();
}

void NetworkTable::ClearPersistent(StringRef key) {
  GetEntry(key).ClearPersistent();
}

bool NetworkTable::IsPersistent(StringRef key) const {
  return GetEntry(key).IsPersistent();
}

void NetworkTable::SetFlags(StringRef key, unsigned int flags) {
  GetEntry(key).SetFlags(flags);
}

void NetworkTable::ClearFlags(StringRef key, unsigned int flags) {
  GetEntry(key).ClearFlags(flags);
}

unsigned int NetworkTable::GetFlags(StringRef key) const {
  return GetEntry(key).GetFlags();
}

void NetworkTable::Delete(const Twine& key) { GetEntry(key).Delete(); }

bool NetworkTable::PutNumber(StringRef key, double value) {
  return GetEntry(key).SetDouble(value);
}

bool NetworkTable::SetDefaultNumber(StringRef key, double defaultValue) {
  return GetEntry(key).SetDefaultDouble(defaultValue);
}

double NetworkTable::GetNumber(StringRef key, double defaultValue) const {
  return GetEntry(key).GetDouble(defaultValue);
}

bool NetworkTable::PutString(StringRef key, StringRef value) {
  return GetEntry(key).SetString(value);
}

bool NetworkTable::SetDefaultString(StringRef key, StringRef defaultValue) {
  return GetEntry(key).SetDefaultString(defaultValue);
}

std::string NetworkTable::GetString(StringRef key,
                                    StringRef defaultValue) const {
  return GetEntry(key).GetString(defaultValue);
}

bool NetworkTable::PutBoolean(StringRef key, bool value) {
  return GetEntry(key).SetBoolean(value);
}

bool NetworkTable::SetDefaultBoolean(StringRef key, bool defaultValue) {
  return GetEntry(key).SetDefaultBoolean(defaultValue);
}

bool NetworkTable::GetBoolean(StringRef key, bool defaultValue) const {
  return GetEntry(key).GetBoolean(defaultValue);
}

bool NetworkTable::PutBooleanArray(StringRef key, ArrayRef<int> value) {
  return GetEntry(key).SetBooleanArray(value);
}

bool NetworkTable::SetDefaultBooleanArray(StringRef key,
                                          ArrayRef<int> defaultValue) {
  return GetEntry(key).SetDefaultBooleanArray(defaultValue);
}

std::vector<int> NetworkTable::GetBooleanArray(
    StringRef key, ArrayRef<int> defaultValue) const {
  return GetEntry(key).GetBooleanArray(defaultValue);
}

bool NetworkTable::PutNumberArray(StringRef key, ArrayRef<double> value) {
  return GetEntry(key).SetDoubleArray(value);
}

bool NetworkTable::SetDefaultNumberArray(StringRef key,
                                         ArrayRef<double> defaultValue) {
  return GetEntry(key).SetDefaultDoubleArray(defaultValue);
}

std::vector<double> NetworkTable::GetNumberArray(
    StringRef key, ArrayRef<double> defaultValue) const {
  return GetEntry(key).GetDoubleArray(defaultValue);
}

bool NetworkTable::PutStringArray(StringRef key, ArrayRef<std::string> value) {
  return GetEntry(key).SetStringArray(value);
}

bool NetworkTable::SetDefaultStringArray(StringRef key,
                                         ArrayRef<std::string> defaultValue) {
  return GetEntry(key).SetDefaultStringArray(defaultValue);
}

std::vector<std::string> NetworkTable::GetStringArray(
    StringRef key, ArrayRef<std::string> defaultValue) const {
  return GetEntry(key).GetStringArray(defaultValue);
}

bool NetworkTable::PutRaw(StringRef key, StringRef value) {
  return GetEntry(key).SetRaw(value);
}

bool NetworkTable::SetDefaultRaw(StringRef key, StringRef defaultValue) {
  return GetEntry(key).SetDefaultRaw(defaultValue);
}

std::string NetworkTable::GetRaw(StringRef key, StringRef defaultValue) const {
  return GetEntry(key).GetRaw(defaultValue);
}

bool NetworkTable::PutValue(const Twine& key, std::shared_ptr<Value> value) {
  return GetEntry(key).SetValue(value);
}

bool NetworkTable::SetDefaultValue(const Twine& key,
                                   std::shared_ptr<Value> defaultValue) {
  return GetEntry(key).SetDefaultValue(defaultValue);
}

std::shared_ptr<Value> NetworkTable::GetValue(const Twine& key) const {
  return GetEntry(key).GetValue();
}

StringRef NetworkTable::GetPath() const { return m_path; }

const char* NetworkTable::SaveEntries(const Twine& filename) const {
  return nt::SaveEntries(m_inst, filename, m_path + Twine(PATH_SEPARATOR_CHAR));
}

const char* NetworkTable::LoadEntries(
    const Twine& filename,
    std::function<void(size_t line, const char* msg)> warn) {
  return nt::LoadEntries(m_inst, filename, m_path + Twine(PATH_SEPARATOR_CHAR),
                         warn);
}
