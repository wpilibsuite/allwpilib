#include "networktables/NetworkTable.h"

#include <algorithm>

#include "llvm/raw_ostream.h"
#include "llvm/SmallString.h"
#include "llvm/StringMap.h"
#include "tables/ITableListener.h"
#include "tables/TableKeyNotDefinedException.h"
#include "ntcore.h"

using llvm::StringRef;

const char NetworkTable::PATH_SEPARATOR_CHAR = '/';
std::string NetworkTable::s_persistent_filename = "networktables.ini";
bool NetworkTable::s_client = false;
bool NetworkTable::s_enable_ds = true;
bool NetworkTable::s_running = false;
unsigned int NetworkTable::s_port = NT_DEFAULT_PORT;

void NetworkTable::Initialize() {
  if (s_running) Shutdown();
  if (s_client) {
    nt::StartClient();
    if (s_enable_ds) nt::StartDSClient(s_port);
  } else
    nt::StartServer(s_persistent_filename, "", s_port);
  s_running = true;
}

void NetworkTable::Shutdown() {
  if (!s_running) return;
  if (s_client) {
    nt::StopDSClient();
    nt::StopClient();
  } else
    nt::StopServer();
  s_running = false;
}

void NetworkTable::SetClientMode() { s_client = true; }

void NetworkTable::SetServerMode() { s_client = false; }

void NetworkTable::SetTeam(int team) {
  std::pair<StringRef, unsigned int> servers[5];

  // 10.te.am.2
  llvm::SmallString<32> fixed;
  {
    llvm::raw_svector_ostream oss{fixed};
    oss << "10." << static_cast<int>(team / 100) << '.'
        << static_cast<int>(team % 100) << ".2";
    servers[0] = std::make_pair(oss.str(), s_port);
  }

  // 172.22.11.2
  servers[1] = std::make_pair("172.22.11.2", s_port);

  // roboRIO-<team>-FRC.local
  llvm::SmallString<32> mdns;
  {
    llvm::raw_svector_ostream oss{mdns};
    oss << "roboRIO-" << team << "-FRC.local";
    servers[2] = std::make_pair(oss.str(), s_port);
  }

  // roboRIO-<team>-FRC.lan
  llvm::SmallString<32> mdns_lan;
  {
    llvm::raw_svector_ostream oss{mdns_lan};
    oss << "roboRIO-" << team << "-FRC.lan";
    servers[3] = std::make_pair(oss.str(), s_port);
  }

  // roboRIO-<team>-FRC.frc-field.local
  llvm::SmallString<64> field_local;
  {
    llvm::raw_svector_ostream oss{field_local};
    oss << "roboRIO-" << team << "-FRC.frc-field.local";
    servers[4] = std::make_pair(oss.str(), s_port);
  }

  nt::SetServer(servers);
}

void NetworkTable::SetIPAddress(StringRef address) {
  llvm::SmallString<32> addr_copy{address};
  nt::SetServer(addr_copy.c_str(), s_port);

  // Stop the DS client if we're explicitly connecting to localhost
  if (address == "localhost" || address == "127.0.0.1")
    nt::StopDSClient();
  else if (s_enable_ds)
    nt::StartDSClient(s_port);
}

void NetworkTable::SetIPAddress(llvm::ArrayRef<std::string> addresses) {
  llvm::SmallVector<std::pair<StringRef, unsigned int>, 8> servers;
  for (const auto& ip_address : addresses)
    servers.emplace_back(std::make_pair(ip_address, s_port));
  nt::SetServer(servers);

  // Stop the DS client if we're explicitly connecting to localhost
  if (!addresses.empty() &&
      (addresses[0] == "localhost" || addresses[0] == "127.0.0.1"))
    nt::StopDSClient();
  else if (s_enable_ds)
    nt::StartDSClient(s_port);
}

void NetworkTable::SetPort(unsigned int port) { s_port = port; }

void NetworkTable::SetDSClientEnabled(bool enabled) {
  s_enable_ds = enabled;
  if (s_enable_ds)
    nt::StartDSClient(s_port);
  else
    nt::StopDSClient();
}

void NetworkTable::SetPersistentFilename(StringRef filename) {
  s_persistent_filename = filename;
}

void NetworkTable::SetNetworkIdentity(StringRef name) {
  nt::SetNetworkIdentity(name);
}

void NetworkTable::GlobalDeleteAll() { nt::DeleteAllEntries(); }

void NetworkTable::Flush() { nt::Flush(); }

void NetworkTable::SetUpdateRate(double interval) {
  nt::SetUpdateRate(interval);
}

const char* NetworkTable::SavePersistent(llvm::StringRef filename) {
  return nt::SavePersistent(filename);
}

const char* NetworkTable::LoadPersistent(
    llvm::StringRef filename,
    std::function<void(size_t line, const char* msg)> warn) {
  return nt::LoadPersistent(filename, warn);
}

std::shared_ptr<NetworkTable> NetworkTable::GetTable(StringRef key) {
  if (!s_running) Initialize();
  if (key.empty() || key[0] == PATH_SEPARATOR_CHAR) {
    return std::make_shared<NetworkTable>(key, private_init());
  } else {
    llvm::SmallString<128> path;
    path += PATH_SEPARATOR_CHAR;
    path += key;
    return std::make_shared<NetworkTable>(path, private_init());
  }
}

NetworkTable::NetworkTable(StringRef path, const private_init&)
    : m_path(path) {}

NetworkTable::~NetworkTable() {
  for (auto& i : m_listeners) nt::RemoveEntryListener(i.second);
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
  std::lock_guard<std::mutex> lock(m_mutex);
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  std::size_t prefix_len = path.size();
  unsigned int id = nt::AddEntryListener(
      path,
      [=](unsigned int /*uid*/, StringRef name,
          std::shared_ptr<nt::Value> value, unsigned int flags_) {
        StringRef relative_key = name.substr(prefix_len);
        if (relative_key.find(PATH_SEPARATOR_CHAR) != StringRef::npos) return;
        listener->ValueChangedEx(this, relative_key, value, flags_);
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
  std::lock_guard<std::mutex> lock(m_mutex);
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  std::size_t prefix_len = path.size();
  path += key;
  unsigned int id = nt::AddEntryListener(
      path,
      [=](unsigned int /*uid*/, StringRef name,
          std::shared_ptr<nt::Value> value, unsigned int flags_) {
        if (name != path) return;
        listener->ValueChangedEx(this, name.substr(prefix_len), value, flags_);
      },
      flags);
  m_listeners.emplace_back(listener, id);
}

void NetworkTable::AddSubTableListener(ITableListener* listener) {
  AddSubTableListener(listener, false);
}

void NetworkTable::AddSubTableListener(ITableListener* listener,
                                       bool localNotify) {
  std::lock_guard<std::mutex> lock(m_mutex);
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  std::size_t prefix_len = path.size();

  // The lambda needs to be copyable, but StringMap is not, so use
  // a shared_ptr to it.
  auto notified_tables = std::make_shared<llvm::StringMap<char>>();

  unsigned int flags = NT_NOTIFY_NEW | NT_NOTIFY_IMMEDIATE;
  if (localNotify) flags |= NT_NOTIFY_LOCAL;
  unsigned int id = nt::AddEntryListener(
      path,
      [=](unsigned int /*uid*/, StringRef name,
          std::shared_ptr<nt::Value> /*value*/, unsigned int flags_) mutable {
        StringRef relative_key = name.substr(prefix_len);
        auto end_sub_table = relative_key.find(PATH_SEPARATOR_CHAR);
        if (end_sub_table == StringRef::npos) return;
        StringRef sub_table_key = relative_key.substr(0, end_sub_table);
        if (notified_tables->find(sub_table_key) == notified_tables->end())
          return;
        notified_tables->insert(std::make_pair(sub_table_key, '\0'));
        listener->ValueChangedEx(this, sub_table_key, nullptr, flags_);
      },
      flags);
  m_listeners.emplace_back(listener, id);
}

void NetworkTable::RemoveTableListener(ITableListener* listener) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto matches_begin =
      std::remove_if(m_listeners.begin(), m_listeners.end(),
                     [=](const Listener& x) { return x.first == listener; });

  for (auto i = matches_begin; i != m_listeners.end(); ++i)
    nt::RemoveEntryListener(i->second);
  m_listeners.erase(matches_begin, m_listeners.end());
}

std::shared_ptr<ITable> NetworkTable::GetSubTable(StringRef key) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return std::make_shared<NetworkTable>(path, private_init());
}

bool NetworkTable::ContainsKey(StringRef key) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::GetEntryValue(path) != nullptr;
}

bool NetworkTable::ContainsSubTable(StringRef key) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  path += PATH_SEPARATOR_CHAR;
  return !nt::GetEntryInfo(path, 0).empty();
}

std::vector<std::string> NetworkTable::GetKeys(int types) const {
  std::vector<std::string> keys;
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  for (auto& entry : nt::GetEntryInfo(path, types)) {
    auto relative_key = StringRef(entry.name).substr(path.size());
    if (relative_key.find(PATH_SEPARATOR_CHAR) != StringRef::npos) continue;
    keys.push_back(relative_key);
  }
  return keys;
}

std::vector<std::string> NetworkTable::GetSubTables() const {
  std::vector<std::string> keys;
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  for (auto& entry : nt::GetEntryInfo(path, 0)) {
    auto relative_key = StringRef(entry.name).substr(path.size());
    std::size_t end_subtable = relative_key.find(PATH_SEPARATOR_CHAR);
    if (end_subtable == StringRef::npos) continue;
    keys.push_back(relative_key.substr(0, end_subtable));
  }
  return keys;
}

void NetworkTable::SetPersistent(StringRef key) {
  SetFlags(key, NT_PERSISTENT);
}

void NetworkTable::ClearPersistent(StringRef key) {
  ClearFlags(key, NT_PERSISTENT);
}

bool NetworkTable::IsPersistent(StringRef key) const {
  return (GetFlags(key) & NT_PERSISTENT) != 0;
}

void NetworkTable::SetFlags(StringRef key, unsigned int flags) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  nt::SetEntryFlags(path, nt::GetEntryFlags(path) | flags);
}

void NetworkTable::ClearFlags(StringRef key, unsigned int flags) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  nt::SetEntryFlags(path, nt::GetEntryFlags(path) & ~flags);
}

unsigned int NetworkTable::GetFlags(StringRef key) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::GetEntryFlags(path);
}

void NetworkTable::Delete(StringRef key) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  nt::DeleteEntry(path);
}

bool NetworkTable::PutNumber(StringRef key, double value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetEntryValue(path, nt::Value::MakeDouble(value));
}

bool NetworkTable::SetDefaultNumber(StringRef key, double defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetDefaultEntryValue(path, nt::Value::MakeDouble(defaultValue));
}

double NetworkTable::GetNumber(StringRef key) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_DOUBLE)
    throw TableKeyNotDefinedException(path);
  return value->GetDouble();
}

double NetworkTable::GetNumber(StringRef key, double defaultValue) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_DOUBLE) return defaultValue;
  return value->GetDouble();
}

bool NetworkTable::PutString(StringRef key, StringRef value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetEntryValue(path, nt::Value::MakeString(value));
}

bool NetworkTable::SetDefaultString(StringRef key, StringRef defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetDefaultEntryValue(path, nt::Value::MakeString(defaultValue));
}

std::string NetworkTable::GetString(StringRef key) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_STRING)
    throw TableKeyNotDefinedException(path);
  return value->GetString();
}

std::string NetworkTable::GetString(StringRef key,
                                    StringRef defaultValue) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_STRING) return defaultValue;
  return value->GetString();
}

bool NetworkTable::PutBoolean(StringRef key, bool value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetEntryValue(path, nt::Value::MakeBoolean(value));
}

bool NetworkTable::SetDefaultBoolean(StringRef key, bool defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetDefaultEntryValue(path, nt::Value::MakeBoolean(defaultValue));
}

bool NetworkTable::GetBoolean(StringRef key) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_BOOLEAN)
    throw TableKeyNotDefinedException(path);
  return value->GetBoolean();
}

bool NetworkTable::GetBoolean(StringRef key, bool defaultValue) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_BOOLEAN) return defaultValue;
  return value->GetBoolean();
}

bool NetworkTable::PutBooleanArray(llvm::StringRef key,
                                   llvm::ArrayRef<int> value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetEntryValue(path, nt::Value::MakeBooleanArray(value));
}

bool NetworkTable::SetDefaultBooleanArray(StringRef key,
                                          llvm::ArrayRef<int> defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetDefaultEntryValue(path,
                                  nt::Value::MakeBooleanArray(defaultValue));
}

std::vector<int> NetworkTable::GetBooleanArray(
    llvm::StringRef key, llvm::ArrayRef<int> defaultValue) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_BOOLEAN_ARRAY) return defaultValue;
  return value->GetBooleanArray();
}

bool NetworkTable::PutNumberArray(llvm::StringRef key,
                                  llvm::ArrayRef<double> value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetEntryValue(path, nt::Value::MakeDoubleArray(value));
}

bool NetworkTable::SetDefaultNumberArray(StringRef key,
                                         llvm::ArrayRef<double> defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetDefaultEntryValue(path,
                                  nt::Value::MakeDoubleArray(defaultValue));
}

std::vector<double> NetworkTable::GetNumberArray(
    llvm::StringRef key, llvm::ArrayRef<double> defaultValue) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_DOUBLE_ARRAY) return defaultValue;
  return value->GetDoubleArray();
}

bool NetworkTable::PutStringArray(llvm::StringRef key,
                                  llvm::ArrayRef<std::string> value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetEntryValue(path, nt::Value::MakeStringArray(value));
}

bool NetworkTable::SetDefaultStringArray(
    StringRef key, llvm::ArrayRef<std::string> defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetDefaultEntryValue(path,
                                  nt::Value::MakeStringArray(defaultValue));
}

std::vector<std::string> NetworkTable::GetStringArray(
    llvm::StringRef key, llvm::ArrayRef<std::string> defaultValue) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_STRING_ARRAY) return defaultValue;
  return value->GetStringArray();
}

bool NetworkTable::PutRaw(llvm::StringRef key, llvm::StringRef value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetEntryValue(path, nt::Value::MakeRaw(value));
}

bool NetworkTable::SetDefaultRaw(StringRef key, StringRef defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetDefaultEntryValue(path, nt::Value::MakeRaw(defaultValue));
}

std::string NetworkTable::GetRaw(llvm::StringRef key,
                                 llvm::StringRef defaultValue) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_RAW) return defaultValue;
  return value->GetRaw();
}

bool NetworkTable::PutValue(StringRef key, std::shared_ptr<nt::Value> value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetEntryValue(path, value);
}

bool NetworkTable::SetDefaultValue(StringRef key,
                                   std::shared_ptr<nt::Value> defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::SetDefaultEntryValue(path, defaultValue);
}

std::shared_ptr<nt::Value> NetworkTable::GetValue(StringRef key) const {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::GetEntryValue(path);
}
