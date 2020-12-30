// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cameraserver/CameraServer.h"

#include <atomic>
#include <vector>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/DenseMap.h>
#include <wpi/ManagedStatic.h>
#include <wpi/SmallString.h>
#include <wpi/StringMap.h>
#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>

#include "cameraserver/CameraServerShared.h"
#include "ntcore_cpp.h"

using namespace frc;

static constexpr char const* kPublishName = "/CameraPublisher";

struct CameraServer::Impl {
  Impl();
  std::shared_ptr<nt::NetworkTable> GetSourceTable(CS_Source source);
  std::vector<std::string> GetSinkStreamValues(CS_Sink sink);
  std::vector<std::string> GetSourceStreamValues(CS_Source source);
  void UpdateStreamValues();

  wpi::mutex m_mutex;
  std::atomic<int> m_defaultUsbDevice{0};
  std::string m_primarySourceName;
  wpi::StringMap<cs::VideoSource> m_sources;
  wpi::StringMap<cs::VideoSink> m_sinks;
  wpi::DenseMap<CS_Sink, CS_Source> m_fixedSources;
  wpi::DenseMap<CS_Source, std::shared_ptr<nt::NetworkTable>> m_tables;
  std::shared_ptr<nt::NetworkTable> m_publishTable{
      nt::NetworkTableInstance::GetDefault().GetTable(kPublishName)};
  cs::VideoListener m_videoListener;
  int m_tableListener;
  int m_nextPort;
  std::vector<std::string> m_addresses;
};

CameraServer* CameraServer::GetInstance() {
  struct Creator {
    static void* call() { return new CameraServer{}; }
  };
  struct Deleter {
    static void call(void* ptr) { delete static_cast<CameraServer*>(ptr); }
  };
  static wpi::ManagedStatic<CameraServer, Creator, Deleter> instance;
  return &(*instance);
}

static wpi::StringRef MakeSourceValue(CS_Source source,
                                      wpi::SmallVectorImpl<char>& buf) {
  CS_Status status = 0;
  buf.clear();
  switch (cs::GetSourceKind(source, &status)) {
    case CS_SOURCE_USB: {
      wpi::StringRef prefix{"usb:"};
      buf.append(prefix.begin(), prefix.end());
      auto path = cs::GetUsbCameraPath(source, &status);
      buf.append(path.begin(), path.end());
      break;
    }
    case CS_SOURCE_HTTP: {
      wpi::StringRef prefix{"ip:"};
      buf.append(prefix.begin(), prefix.end());
      auto urls = cs::GetHttpCameraUrls(source, &status);
      if (!urls.empty()) {
        buf.append(urls[0].begin(), urls[0].end());
      }
      break;
    }
    case CS_SOURCE_CV:
      return "cv:";
    default:
      return "unknown:";
  }

  return wpi::StringRef{buf.begin(), buf.size()};
}

static std::string MakeStreamValue(const wpi::Twine& address, int port) {
  return ("mjpg:http://" + address + wpi::Twine(':') + wpi::Twine(port) +
          "/?action=stream")
      .str();
}

std::shared_ptr<nt::NetworkTable> CameraServer::Impl::GetSourceTable(
    CS_Source source) {
  std::scoped_lock lock(m_mutex);
  return m_tables.lookup(source);
}

std::vector<std::string> CameraServer::Impl::GetSinkStreamValues(CS_Sink sink) {
  CS_Status status = 0;

  // Ignore all but MjpegServer
  if (cs::GetSinkKind(sink, &status) != CS_SINK_MJPEG) {
    return {};
  }

  // Get port
  int port = cs::GetMjpegServerPort(sink, &status);

  // Generate values
  std::vector<std::string> values;
  auto listenAddress = cs::GetMjpegServerListenAddress(sink, &status);
  if (!listenAddress.empty()) {
    // If a listen address is specified, only use that
    values.emplace_back(MakeStreamValue(listenAddress, port));
  } else {
    // Otherwise generate for hostname and all interface addresses
    values.emplace_back(MakeStreamValue(cs::GetHostname() + ".local", port));

    for (const auto& addr : m_addresses) {
      if (addr == "127.0.0.1") {
        continue;  // ignore localhost
      }
      values.emplace_back(MakeStreamValue(addr, port));
    }
  }

  return values;
}

std::vector<std::string> CameraServer::Impl::GetSourceStreamValues(
    CS_Source source) {
  CS_Status status = 0;

  // Ignore all but HttpCamera
  if (cs::GetSourceKind(source, &status) != CS_SOURCE_HTTP) {
    return {};
  }

  // Generate values
  auto values = cs::GetHttpCameraUrls(source, &status);
  for (auto& value : values) {
    value = "mjpg:" + value;
  }

#ifdef __FRC_ROBORIO__
  // Look to see if we have a passthrough server for this source
  // Only do this on the roboRIO
  for (const auto& i : m_sinks) {
    CS_Sink sink = i.second.GetHandle();
    CS_Source sinkSource = cs::GetSinkSource(sink, &status);
    if (source == sinkSource &&
        cs::GetSinkKind(sink, &status) == CS_SINK_MJPEG) {
      // Add USB-only passthrough
      int port = cs::GetMjpegServerPort(sink, &status);
      values.emplace_back(MakeStreamValue("172.22.11.2", port));
      break;
    }
  }
#endif

  // Set table value
  return values;
}

void CameraServer::Impl::UpdateStreamValues() {
  std::scoped_lock lock(m_mutex);
  // Over all the sinks...
  for (const auto& i : m_sinks) {
    CS_Status status = 0;
    CS_Sink sink = i.second.GetHandle();

    // Get the source's subtable (if none exists, we're done)
    CS_Source source = m_fixedSources.lookup(sink);
    if (source == 0) {
      source = cs::GetSinkSource(sink, &status);
    }
    if (source == 0) {
      continue;
    }
    auto table = m_tables.lookup(source);
    if (table) {
      // Don't set stream values if this is a HttpCamera passthrough
      if (cs::GetSourceKind(source, &status) == CS_SOURCE_HTTP) {
        continue;
      }

      // Set table value
      auto values = GetSinkStreamValues(sink);
      if (!values.empty()) {
        table->GetEntry("streams").SetStringArray(values);
      }
    }
  }

  // Over all the sources...
  for (const auto& i : m_sources) {
    CS_Source source = i.second.GetHandle();

    // Get the source's subtable (if none exists, we're done)
    auto table = m_tables.lookup(source);
    if (table) {
      // Set table value
      auto values = GetSourceStreamValues(source);
      if (!values.empty()) {
        table->GetEntry("streams").SetStringArray(values);
      }
    }
  }
}

static std::string PixelFormatToString(int pixelFormat) {
  switch (pixelFormat) {
    case cs::VideoMode::PixelFormat::kMJPEG:
      return "MJPEG";
    case cs::VideoMode::PixelFormat::kYUYV:
      return "YUYV";
    case cs::VideoMode::PixelFormat::kRGB565:
      return "RGB565";
    case cs::VideoMode::PixelFormat::kBGR:
      return "BGR";
    case cs::VideoMode::PixelFormat::kGray:
      return "Gray";
    default:
      return "Unknown";
  }
}

static std::string VideoModeToString(const cs::VideoMode& mode) {
  std::string rv;
  wpi::raw_string_ostream oss{rv};
  oss << mode.width << "x" << mode.height;
  oss << " " << PixelFormatToString(mode.pixelFormat) << " ";
  oss << mode.fps << " fps";
  return oss.str();
}

static std::vector<std::string> GetSourceModeValues(int source) {
  std::vector<std::string> rv;
  CS_Status status = 0;
  for (const auto& mode : cs::EnumerateSourceVideoModes(source, &status)) {
    rv.emplace_back(VideoModeToString(mode));
  }
  return rv;
}

static void PutSourcePropertyValue(nt::NetworkTable* table,
                                   const cs::VideoEvent& event, bool isNew) {
  wpi::SmallString<64> name;
  wpi::SmallString<64> infoName;
  if (wpi::StringRef{event.name}.startswith("raw_")) {
    name = "RawProperty/";
    name += event.name;
    infoName = "RawPropertyInfo/";
    infoName += event.name;
  } else {
    name = "Property/";
    name += event.name;
    infoName = "PropertyInfo/";
    infoName += event.name;
  }

  wpi::SmallString<64> buf;
  CS_Status status = 0;
  nt::NetworkTableEntry entry = table->GetEntry(name);
  switch (event.propertyKind) {
    case CS_PROP_BOOLEAN:
      if (isNew) {
        entry.SetDefaultBoolean(event.value != 0);
      } else {
        entry.SetBoolean(event.value != 0);
      }
      break;
    case CS_PROP_INTEGER:
    case CS_PROP_ENUM:
      if (isNew) {
        entry.SetDefaultDouble(event.value);
        table->GetEntry(infoName + "/min")
            .SetDouble(cs::GetPropertyMin(event.propertyHandle, &status));
        table->GetEntry(infoName + "/max")
            .SetDouble(cs::GetPropertyMax(event.propertyHandle, &status));
        table->GetEntry(infoName + "/step")
            .SetDouble(cs::GetPropertyStep(event.propertyHandle, &status));
        table->GetEntry(infoName + "/default")
            .SetDouble(cs::GetPropertyDefault(event.propertyHandle, &status));
      } else {
        entry.SetDouble(event.value);
      }
      break;
    case CS_PROP_STRING:
      if (isNew) {
        entry.SetDefaultString(event.valueStr);
      } else {
        entry.SetString(event.valueStr);
      }
      break;
    default:
      break;
  }
}

CameraServer::Impl::Impl() : m_nextPort(kBasePort) {
  // We publish sources to NetworkTables using the following structure:
  // "/CameraPublisher/{Source.Name}/" - root
  // - "source" (string): Descriptive, prefixed with type (e.g. "usb:0")
  // - "streams" (string array): URLs that can be used to stream data
  // - "description" (string): Description of the source
  // - "connected" (boolean): Whether source is connected
  // - "mode" (string): Current video mode
  // - "modes" (string array): Available video modes
  // - "Property/{Property}" - Property values
  // - "PropertyInfo/{Property}" - Property supporting information

  // Listener for video events
  m_videoListener = cs::VideoListener{
      [=](const cs::VideoEvent& event) {
        CS_Status status = 0;
        switch (event.kind) {
          case cs::VideoEvent::kSourceCreated: {
            // Create subtable for the camera
            auto table = m_publishTable->GetSubTable(event.name);
            {
              std::scoped_lock lock(m_mutex);
              m_tables.insert(std::make_pair(event.sourceHandle, table));
            }
            wpi::SmallString<64> buf;
            table->GetEntry("source").SetString(
                MakeSourceValue(event.sourceHandle, buf));
            wpi::SmallString<64> descBuf;
            table->GetEntry("description")
                .SetString(cs::GetSourceDescription(event.sourceHandle, descBuf,
                                                    &status));
            table->GetEntry("connected")
                .SetBoolean(cs::IsSourceConnected(event.sourceHandle, &status));
            table->GetEntry("streams").SetStringArray(
                GetSourceStreamValues(event.sourceHandle));
            auto mode = cs::GetSourceVideoMode(event.sourceHandle, &status);
            table->GetEntry("mode").SetDefaultString(VideoModeToString(mode));
            table->GetEntry("modes").SetStringArray(
                GetSourceModeValues(event.sourceHandle));
            break;
          }
          case cs::VideoEvent::kSourceDestroyed: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              table->GetEntry("source").SetString("");
              table->GetEntry("streams").SetStringArray(
                  std::vector<std::string>{});
              table->GetEntry("modes").SetStringArray(
                  std::vector<std::string>{});
            }
            break;
          }
          case cs::VideoEvent::kSourceConnected: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              // update the description too (as it may have changed)
              wpi::SmallString<64> descBuf;
              table->GetEntry("description")
                  .SetString(cs::GetSourceDescription(event.sourceHandle,
                                                      descBuf, &status));
              table->GetEntry("connected").SetBoolean(true);
            }
            break;
          }
          case cs::VideoEvent::kSourceDisconnected: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              table->GetEntry("connected").SetBoolean(false);
            }
            break;
          }
          case cs::VideoEvent::kSourceVideoModesUpdated: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              table->GetEntry("modes").SetStringArray(
                  GetSourceModeValues(event.sourceHandle));
            }
            break;
          }
          case cs::VideoEvent::kSourceVideoModeChanged: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              table->GetEntry("mode").SetString(VideoModeToString(event.mode));
            }
            break;
          }
          case cs::VideoEvent::kSourcePropertyCreated: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              PutSourcePropertyValue(table.get(), event, true);
            }
            break;
          }
          case cs::VideoEvent::kSourcePropertyValueUpdated: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              PutSourcePropertyValue(table.get(), event, false);
            }
            break;
          }
          case cs::VideoEvent::kSourcePropertyChoicesUpdated: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              wpi::SmallString<64> name{"PropertyInfo/"};
              name += event.name;
              name += "/choices";
              auto choices =
                  cs::GetEnumPropertyChoices(event.propertyHandle, &status);
              table->GetEntry(name).SetStringArray(choices);
            }
            break;
          }
          case cs::VideoEvent::kSinkSourceChanged:
          case cs::VideoEvent::kSinkCreated:
          case cs::VideoEvent::kSinkDestroyed:
          case cs::VideoEvent::kNetworkInterfacesChanged: {
            m_addresses = cs::GetNetworkInterfaces();
            UpdateStreamValues();
            break;
          }
          default:
            break;
        }
      },
      0x4fff, true};

  // Listener for NetworkTable events
  // We don't currently support changing settings via NT due to
  // synchronization issues, so just update to current setting if someone
  // else tries to change it.
  wpi::SmallString<64> buf;
  m_tableListener = nt::NetworkTableInstance::GetDefault().AddEntryListener(
      kPublishName + wpi::Twine('/'),
      [=](const nt::EntryNotification& event) {
        wpi::StringRef relativeKey =
            event.name.substr(wpi::StringRef(kPublishName).size() + 1);

        // get source (sourceName/...)
        auto subKeyIndex = relativeKey.find('/');
        if (subKeyIndex == wpi::StringRef::npos) {
          return;
        }
        wpi::StringRef sourceName = relativeKey.slice(0, subKeyIndex);
        auto sourceIt = m_sources.find(sourceName);
        if (sourceIt == m_sources.end()) {
          return;
        }

        // get subkey
        relativeKey = relativeKey.substr(subKeyIndex + 1);

        // handle standard names
        wpi::StringRef propName;
        nt::NetworkTableEntry entry{event.entry};
        if (relativeKey == "mode") {
          // reset to current mode
          entry.SetString(VideoModeToString(sourceIt->second.GetVideoMode()));
          return;
        } else if (relativeKey.startswith("Property/")) {
          propName = relativeKey.substr(9);
        } else if (relativeKey.startswith("RawProperty/")) {
          propName = relativeKey.substr(12);
        } else {
          return;  // ignore
        }

        // everything else is a property
        auto property = sourceIt->second.GetProperty(propName);
        switch (property.GetKind()) {
          case cs::VideoProperty::kNone:
            return;
          case cs::VideoProperty::kBoolean:
            entry.SetBoolean(property.Get() != 0);
            return;
          case cs::VideoProperty::kInteger:
          case cs::VideoProperty::kEnum:
            entry.SetDouble(property.Get());
            return;
          case cs::VideoProperty::kString:
            entry.SetString(property.GetString());
            return;
          default:
            return;
        }
      },
      NT_NOTIFY_IMMEDIATE | NT_NOTIFY_UPDATE);
}

CameraServer::CameraServer() : m_impl(new Impl) {}

CameraServer::~CameraServer() = default;

cs::UsbCamera CameraServer::StartAutomaticCapture() {
  cs::UsbCamera camera = StartAutomaticCapture(m_impl->m_defaultUsbDevice++);
  auto csShared = GetCameraServerShared();
  csShared->ReportUsbCamera(camera.GetHandle());
  return camera;
}

cs::UsbCamera CameraServer::StartAutomaticCapture(int dev) {
  cs::UsbCamera camera{"USB Camera " + wpi::Twine(dev), dev};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportUsbCamera(camera.GetHandle());
  return camera;
}

cs::UsbCamera CameraServer::StartAutomaticCapture(const wpi::Twine& name,
                                                  int dev) {
  cs::UsbCamera camera{name, dev};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportUsbCamera(camera.GetHandle());
  return camera;
}

cs::UsbCamera CameraServer::StartAutomaticCapture(const wpi::Twine& name,
                                                  const wpi::Twine& path) {
  cs::UsbCamera camera{name, path};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportUsbCamera(camera.GetHandle());
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(const wpi::Twine& host) {
  return AddAxisCamera("Axis Camera", host);
}

cs::AxisCamera CameraServer::AddAxisCamera(const char* host) {
  return AddAxisCamera("Axis Camera", host);
}

cs::AxisCamera CameraServer::AddAxisCamera(const std::string& host) {
  return AddAxisCamera("Axis Camera", host);
}

cs::AxisCamera CameraServer::AddAxisCamera(wpi::ArrayRef<std::string> hosts) {
  return AddAxisCamera("Axis Camera", hosts);
}

cs::AxisCamera CameraServer::AddAxisCamera(const wpi::Twine& name,
                                           const wpi::Twine& host) {
  cs::AxisCamera camera{name, host};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportAxisCamera(camera.GetHandle());
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(const wpi::Twine& name,
                                           const char* host) {
  cs::AxisCamera camera{name, host};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportAxisCamera(camera.GetHandle());
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(const wpi::Twine& name,
                                           const std::string& host) {
  cs::AxisCamera camera{name, host};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportAxisCamera(camera.GetHandle());
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(const wpi::Twine& name,
                                           wpi::ArrayRef<std::string> hosts) {
  cs::AxisCamera camera{name, hosts};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportAxisCamera(camera.GetHandle());
  return camera;
}

cs::MjpegServer CameraServer::AddSwitchedCamera(const wpi::Twine& name) {
  // create a dummy CvSource
  cs::CvSource source{name, cs::VideoMode::PixelFormat::kMJPEG, 160, 120, 30};
  cs::MjpegServer server = StartAutomaticCapture(source);
  m_impl->m_fixedSources[server.GetHandle()] = source.GetHandle();

  return server;
}

cs::MjpegServer CameraServer::StartAutomaticCapture(
    const cs::VideoSource& camera) {
  AddCamera(camera);
  auto server = AddServer(wpi::Twine("serve_") + camera.GetName());
  server.SetSource(camera);
  return server;
}

cs::CvSink CameraServer::GetVideo() {
  cs::VideoSource source;
  {
    auto csShared = GetCameraServerShared();
    std::scoped_lock lock(m_impl->m_mutex);
    if (m_impl->m_primarySourceName.empty()) {
      csShared->SetCameraServerError("no camera available");
      return cs::CvSink{};
    }
    auto it = m_impl->m_sources.find(m_impl->m_primarySourceName);
    if (it == m_impl->m_sources.end()) {
      csShared->SetCameraServerError("no camera available");
      return cs::CvSink{};
    }
    source = it->second;
  }
  return GetVideo(std::move(source));
}

cs::CvSink CameraServer::GetVideo(const cs::VideoSource& camera) {
  wpi::SmallString<64> name{"opencv_"};
  name += camera.GetName();

  {
    std::scoped_lock lock(m_impl->m_mutex);
    auto it = m_impl->m_sinks.find(name);
    if (it != m_impl->m_sinks.end()) {
      auto kind = it->second.GetKind();
      if (kind != cs::VideoSink::kCv) {
        auto csShared = GetCameraServerShared();
        csShared->SetCameraServerError("expected OpenCV sink, but got " +
                                       wpi::Twine(kind));
        return cs::CvSink{};
      }
      return *static_cast<cs::CvSink*>(&it->second);
    }
  }

  cs::CvSink newsink{name};
  newsink.SetSource(camera);
  AddServer(newsink);
  return newsink;
}

cs::CvSink CameraServer::GetVideo(const wpi::Twine& name) {
  wpi::SmallString<64> nameBuf;
  wpi::StringRef nameStr = name.toStringRef(nameBuf);
  cs::VideoSource source;
  {
    std::scoped_lock lock(m_impl->m_mutex);
    auto it = m_impl->m_sources.find(nameStr);
    if (it == m_impl->m_sources.end()) {
      auto csShared = GetCameraServerShared();
      csShared->SetCameraServerError("could not find camera " + nameStr);
      return cs::CvSink{};
    }
    source = it->second;
  }
  return GetVideo(source);
}

cs::CvSource CameraServer::PutVideo(const wpi::Twine& name, int width,
                                    int height) {
  cs::CvSource source{name, cs::VideoMode::kMJPEG, width, height, 30};
  StartAutomaticCapture(source);
  return source;
}

cs::MjpegServer CameraServer::AddServer(const wpi::Twine& name) {
  int port;
  {
    std::scoped_lock lock(m_impl->m_mutex);
    port = m_impl->m_nextPort++;
  }
  return AddServer(name, port);
}

cs::MjpegServer CameraServer::AddServer(const wpi::Twine& name, int port) {
  cs::MjpegServer server{name, port};
  AddServer(server);
  return server;
}

void CameraServer::AddServer(const cs::VideoSink& server) {
  std::scoped_lock lock(m_impl->m_mutex);
  m_impl->m_sinks.try_emplace(server.GetName(), server);
}

void CameraServer::RemoveServer(const wpi::Twine& name) {
  std::scoped_lock lock(m_impl->m_mutex);
  wpi::SmallString<64> nameBuf;
  m_impl->m_sinks.erase(name.toStringRef(nameBuf));
}

cs::VideoSink CameraServer::GetServer() {
  wpi::SmallString<64> name;
  {
    std::scoped_lock lock(m_impl->m_mutex);
    if (m_impl->m_primarySourceName.empty()) {
      auto csShared = GetCameraServerShared();
      csShared->SetCameraServerError("no camera available");
      return cs::VideoSink{};
    }
    name = "serve_";
    name += m_impl->m_primarySourceName;
  }
  return GetServer(name);
}

cs::VideoSink CameraServer::GetServer(const wpi::Twine& name) {
  wpi::SmallString<64> nameBuf;
  wpi::StringRef nameStr = name.toStringRef(nameBuf);
  std::scoped_lock lock(m_impl->m_mutex);
  auto it = m_impl->m_sinks.find(nameStr);
  if (it == m_impl->m_sinks.end()) {
    auto csShared = GetCameraServerShared();
    csShared->SetCameraServerError("could not find server " + nameStr);
    return cs::VideoSink{};
  }
  return it->second;
}

void CameraServer::AddCamera(const cs::VideoSource& camera) {
  std::string name = camera.GetName();
  std::scoped_lock lock(m_impl->m_mutex);
  if (m_impl->m_primarySourceName.empty()) {
    m_impl->m_primarySourceName = name;
  }
  m_impl->m_sources.try_emplace(name, camera);
}

void CameraServer::RemoveCamera(const wpi::Twine& name) {
  std::scoped_lock lock(m_impl->m_mutex);
  wpi::SmallString<64> nameBuf;
  m_impl->m_sources.erase(name.toStringRef(nameBuf));
}

void CameraServer::SetSize(int size) {
  std::scoped_lock lock(m_impl->m_mutex);
  if (m_impl->m_primarySourceName.empty()) {
    return;
  }
  auto it = m_impl->m_sources.find(m_impl->m_primarySourceName);
  if (it == m_impl->m_sources.end()) {
    return;
  }
  if (size == kSize160x120) {
    it->second.SetResolution(160, 120);
  } else if (size == kSize320x240) {
    it->second.SetResolution(320, 240);
  } else if (size == kSize640x480) {
    it->second.SetResolution(640, 480);
  }
}
