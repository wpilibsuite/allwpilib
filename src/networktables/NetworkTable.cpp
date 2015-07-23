#include "networktables/NetworkTable.h"

#include <algorithm>

#include "llvm/SmallString.h"
#include "tables/ITableListener.h"
#include "ntcore.h"

using llvm::StringRef;

const char NetworkTable::PATH_SEPARATOR_CHAR = '/';
std::string NetworkTable::s_ip_address;
bool NetworkTable::s_client = false;
bool NetworkTable::s_running = false;

void NetworkTable::Initialize() {
  if (s_client)
    nt::StartClient(s_ip_address.c_str(), NT_DEFAULT_PORT);
  else
    nt::StartServer("", "", NT_DEFAULT_PORT);
  s_running = true;
}

void NetworkTable::Shutdown() {
  if (s_client)
    nt::StopClient();
  else
    nt::StopServer();
  s_running = false;
}

void NetworkTable::SetClientMode() { s_client = true; }

void NetworkTable::SetServerMode() { s_client = false; }

void NetworkTable::SetTeam(int team) {
  char tmp[30];
  sprintf(tmp, "%d.%d.%d.%d\n", 10, team / 100, team % 100, 2);
  SetIPAddress(tmp);
}

void NetworkTable::SetIPAddress(StringRef address) {
  s_ip_address = address;
}

std::shared_ptr<NetworkTable> NetworkTable::GetTable(StringRef key) {
  if (!s_running) Initialize();
  llvm::SmallString<128> path;
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return std::make_shared<NetworkTable>(path, private_init());
}

NetworkTable::NetworkTable(StringRef path, const private_init&)
    : m_path(path) {}

NetworkTable::~NetworkTable() {
  for (auto& i : m_listeners)
    nt::RemoveEntryListener(i.second);
}

void NetworkTable::AddTableListener(ITableListener* listener) {
  AddTableListener(listener, false);
}

void NetworkTable::AddTableListener(ITableListener* listener,
                                    bool immediateNotify) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  unsigned int id = nt::AddEntryListener(
      path,
      [=](unsigned int uid, StringRef name, std::shared_ptr<nt::Value> value,
          bool is_new) { listener->ValueChanged(this, name, value, is_new); },
      immediateNotify);
  m_listeners.emplace_back(listener, id);
}

void NetworkTable::AddTableListener(StringRef key,
                                    ITableListener* listener,
                                    bool immediateNotify) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  unsigned int id = nt::AddEntryListener(
      path,
      [=](unsigned int uid, StringRef name, std::shared_ptr<nt::Value> value,
          bool is_new) { listener->ValueChanged(this, name, value, is_new); },
      immediateNotify);
  m_listeners.emplace_back(listener, id);
}

void NetworkTable::RemoveTableListener(ITableListener* listener) {
  auto matches_begin =
      std::remove_if(m_listeners.begin(), m_listeners.end(),
                     [=](const auto& x) { return x.first == listener; });

  for (auto i = matches_begin; i != m_listeners.end(); ++i)
    nt::RemoveEntryListener(i->second);
  m_listeners.erase(matches_begin, m_listeners.end());
}

std::shared_ptr<ITable> NetworkTable::GetSubTable(StringRef key) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return std::make_shared<NetworkTable>(path, private_init());
}

bool NetworkTable::ContainsKey(StringRef key) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return !nt::GetEntryValue(path);
}

bool NetworkTable::ContainsSubTable(StringRef key) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  path += PATH_SEPARATOR_CHAR;
  return !nt::GetEntryInfo(path, 0).empty();
}

void NetworkTable::PutNumber(StringRef key, double value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  nt::SetEntryValue(path, nt::Value::MakeDouble(value));
}

double NetworkTable::GetNumber(StringRef key, double defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_DOUBLE)
    return defaultValue;
  return value->GetDouble();
}

void NetworkTable::PutString(StringRef key, StringRef value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  nt::SetEntryValue(path, nt::Value::MakeString(value));
}

std::string NetworkTable::GetString(StringRef key, StringRef defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_STRING)
    return defaultValue;
  return value->GetString();
}

void NetworkTable::PutBoolean(StringRef key, bool value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  nt::SetEntryValue(path, nt::Value::MakeBoolean(value));
}

bool NetworkTable::GetBoolean(StringRef key, bool defaultValue) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  auto value = nt::GetEntryValue(path);
  if (!value || value->type() != NT_BOOLEAN)
    return defaultValue;
  return value->GetBoolean();
}

void NetworkTable::PutValue(StringRef key, std::shared_ptr<nt::Value> value) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  nt::SetEntryValue(path, value);
}

std::shared_ptr<nt::Value> NetworkTable::GetValue(StringRef key) {
  llvm::SmallString<128> path(m_path);
  path += PATH_SEPARATOR_CHAR;
  path += key;
  return nt::GetEntryValue(path);
}
