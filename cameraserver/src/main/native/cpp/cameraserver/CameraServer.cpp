// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cameraserver/CameraServer.h"

#include <atomic>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <networktables/BooleanTopic.h>
#include <networktables/IntegerTopic.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringArrayTopic.h>
#include <networktables/StringTopic.h>
#include <wpi/DenseMap.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>
#include <wpi/mutex.h>

#include "cameraserver/CameraServerShared.h"
#include "ntcore_cpp.h"

using namespace frc;

static constexpr char const* kPublishName = "/CameraPublisher";

namespace {

struct Instance;

struct PropertyPublisher {
  PropertyPublisher(nt::NetworkTable& table, const cs::VideoEvent& event);

  void Update(const cs::VideoEvent& event);

  nt::BooleanEntry booleanValueEntry;
  nt::IntegerEntry integerValueEntry;
  nt::StringEntry stringValueEntry;
  nt::IntegerPublisher minPublisher;
  nt::IntegerPublisher maxPublisher;
  nt::IntegerPublisher stepPublisher;
  nt::IntegerPublisher defaultPublisher;
  nt::StringArrayTopic choicesTopic;
  nt::StringArrayPublisher choicesPublisher;
};

struct SourcePublisher {
  SourcePublisher(Instance& inst, std::shared_ptr<nt::NetworkTable> table,
                  CS_Source source);

  std::shared_ptr<nt::NetworkTable> table;
  nt::StringPublisher sourcePublisher;
  nt::StringPublisher descriptionPublisher;
  nt::BooleanPublisher connectedPublisher;
  nt::StringArrayPublisher streamsPublisher;
  nt::StringEntry modeEntry;
  nt::StringArrayPublisher modesPublisher;
  wpi::DenseMap<CS_Property, PropertyPublisher> properties;
};

struct Instance {
  Instance();
  SourcePublisher* GetPublisher(CS_Source source);
  std::vector<std::string> GetSinkStreamValues(CS_Sink sink);
  std::vector<std::string> GetSourceStreamValues(CS_Source source);
  void UpdateStreamValues();

  wpi::mutex m_mutex;
  std::atomic<int> m_defaultUsbDevice{0};
  std::string m_primarySourceName;
  wpi::StringMap<cs::VideoSource> m_sources;
  wpi::StringMap<cs::VideoSink> m_sinks;
  wpi::DenseMap<CS_Sink, CS_Source> m_fixedSources;
  wpi::DenseMap<CS_Source, SourcePublisher> m_publishers;
  std::shared_ptr<nt::NetworkTable> m_publishTable{
      nt::NetworkTableInstance::GetDefault().GetTable(kPublishName)};
  cs::VideoListener m_videoListener;
  int m_tableListener;
  int m_nextPort{CameraServer::kBasePort};
  std::vector<std::string> m_addresses;
};

}  // namespace

static Instance& GetInstance() {
  static Instance instance;
  return instance;
}

static std::string_view MakeSourceValue(CS_Source source,
                                        wpi::SmallVectorImpl<char>& buf) {
  CS_Status status = 0;
  buf.clear();
  switch (cs::GetSourceKind(source, &status)) {
    case CS_SOURCE_USB: {
      std::string_view prefix{"usb:"};
      buf.append(prefix.begin(), prefix.end());
      auto path = cs::GetUsbCameraPath(source, &status);
      buf.append(path.begin(), path.end());
      break;
    }
    case CS_SOURCE_HTTP: {
      std::string_view prefix{"ip:"};
      buf.append(prefix.begin(), prefix.end());
      auto urls = cs::GetHttpCameraUrls(source, &status);
      if (!urls.empty()) {
        buf.append(urls[0].begin(), urls[0].end());
      }
      break;
    }
    case CS_SOURCE_CV:
      return "cv:";
    case CS_SOURCE_RAW:
      return "raw:";
    default:
      return "unknown:";
  }

  return {buf.begin(), buf.size()};
}

static std::string MakeStreamValue(std::string_view address, int port) {
  return fmt::format("mjpg:http://{}:{}/?action=stream", address, port);
}

SourcePublisher* Instance::GetPublisher(CS_Source source) {
  auto it = m_publishers.find(source);
  if (it != m_publishers.end()) {
    return &it->second;
  } else {
    return nullptr;
  }
}

std::vector<std::string> Instance::GetSinkStreamValues(CS_Sink sink) {
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

std::vector<std::string> Instance::GetSourceStreamValues(CS_Source source) {
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

void Instance::UpdateStreamValues() {
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
    if (auto publisher = GetPublisher(source)) {
      // Don't set stream values if this is a HttpCamera passthrough
      if (cs::GetSourceKind(source, &status) == CS_SOURCE_HTTP) {
        continue;
      }

      // Set table value
      auto values = GetSinkStreamValues(sink);
      if (!values.empty()) {
        publisher->streamsPublisher.Set(values);
      }
    }
  }

  // Over all the sources...
  for (const auto& i : m_sources) {
    CS_Source source = i.second.GetHandle();

    // Get the source's subtable (if none exists, we're done)
    if (auto publisher = GetPublisher(source)) {
      // Set table value
      auto values = GetSourceStreamValues(source);
      if (!values.empty()) {
        publisher->streamsPublisher.Set(values);
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
  return fmt::format("{}x{} {} {} fps", mode.width, mode.height,
                     PixelFormatToString(mode.pixelFormat), mode.fps);
}

static std::vector<std::string> GetSourceModeValues(int source) {
  std::vector<std::string> rv;
  CS_Status status = 0;
  for (const auto& mode : cs::EnumerateSourceVideoModes(source, &status)) {
    rv.emplace_back(VideoModeToString(mode));
  }
  return rv;
}

PropertyPublisher::PropertyPublisher(nt::NetworkTable& table,
                                     const cs::VideoEvent& event) {
  std::string name;
  std::string infoName;
  if (wpi::starts_with(event.name, "raw_")) {
    name = fmt::format("RawProperty/{}", event.name);
    infoName = fmt::format("RawPropertyInfo/{}", event.name);
  } else {
    name = fmt::format("Property/{}", event.name);
    infoName = fmt::format("PropertyInfo/{}", event.name);
  }

  CS_Status status = 0;
  switch (event.propertyKind) {
    case CS_PROP_BOOLEAN:
      booleanValueEntry = table.GetBooleanTopic(name).GetEntry(false);
      booleanValueEntry.SetDefault(event.value != 0);
      break;
    case CS_PROP_ENUM:
      choicesTopic =
          table.GetStringArrayTopic(fmt::format("{}/choices", infoName));
      [[fallthrough]];
    case CS_PROP_INTEGER:
      integerValueEntry = table.GetIntegerTopic(name).GetEntry(0);
      minPublisher =
          table.GetIntegerTopic(fmt::format("{}/min", infoName)).Publish();
      maxPublisher =
          table.GetIntegerTopic(fmt::format("{}/max", infoName)).Publish();
      stepPublisher =
          table.GetIntegerTopic(fmt::format("{}/step", infoName)).Publish();
      defaultPublisher =
          table.GetIntegerTopic(fmt::format("{}/default", infoName)).Publish();

      integerValueEntry.SetDefault(event.value);
      minPublisher.Set(cs::GetPropertyMin(event.propertyHandle, &status));
      maxPublisher.Set(cs::GetPropertyMax(event.propertyHandle, &status));
      stepPublisher.Set(cs::GetPropertyStep(event.propertyHandle, &status));
      defaultPublisher.Set(
          cs::GetPropertyDefault(event.propertyHandle, &status));
      break;
    case CS_PROP_STRING:
      stringValueEntry = table.GetStringTopic(name).GetEntry("");
      stringValueEntry.SetDefault(event.valueStr);
      break;
    default:
      break;
  }
}

void PropertyPublisher::Update(const cs::VideoEvent& event) {
  switch (event.propertyKind) {
    case CS_PROP_BOOLEAN:
      if (booleanValueEntry) {
        booleanValueEntry.Set(event.value != 0);
      }
      break;
    case CS_PROP_INTEGER:
    case CS_PROP_ENUM:
      if (integerValueEntry) {
        integerValueEntry.Set(event.value);
      }
      break;
    case CS_PROP_STRING:
      if (stringValueEntry) {
        stringValueEntry.Set(event.valueStr);
      }
      break;
    default:
      break;
  }
}

SourcePublisher::SourcePublisher(Instance& inst,
                                 std::shared_ptr<nt::NetworkTable> table,
                                 CS_Source source)
    : table{table},
      sourcePublisher{table->GetStringTopic("source").Publish()},
      descriptionPublisher{table->GetStringTopic("description").Publish()},
      connectedPublisher{table->GetBooleanTopic("connected").Publish()},
      streamsPublisher{table->GetStringArrayTopic("streams").Publish()},
      modeEntry{table->GetStringTopic("mode").GetEntry("")},
      modesPublisher{table->GetStringArrayTopic("modes").Publish()} {
  CS_Status status = 0;
  wpi::SmallString<64> buf;
  sourcePublisher.Set(MakeSourceValue(source, buf));
  wpi::SmallString<64> descBuf;
  descriptionPublisher.Set(cs::GetSourceDescription(source, descBuf, &status));
  connectedPublisher.Set(cs::IsSourceConnected(source, &status));
  streamsPublisher.Set(inst.GetSourceStreamValues(source));
  auto mode = cs::GetSourceVideoMode(source, &status);
  modeEntry.SetDefault(VideoModeToString(mode));
  modesPublisher.Set(GetSourceModeValues(source));
}

Instance::Instance() {
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
      [=, this](const cs::VideoEvent& event) {
        std::scoped_lock lock(m_mutex);
        CS_Status status = 0;
        switch (event.kind) {
          case cs::VideoEvent::kSourceCreated: {
            // Create subtable for the camera
            auto table = m_publishTable->GetSubTable(event.name);
            m_publishers.insert(
                {event.sourceHandle,
                 SourcePublisher{*this, table, event.sourceHandle}});
            break;
          }
          case cs::VideoEvent::kSourceDestroyed:
            m_publishers.erase(event.sourceHandle);
            break;
          case cs::VideoEvent::kSourceConnected:
            if (auto publisher = GetPublisher(event.sourceHandle)) {
              // update the description too (as it may have changed)
              wpi::SmallString<64> descBuf;
              publisher->descriptionPublisher.Set(cs::GetSourceDescription(
                  event.sourceHandle, descBuf, &status));
              publisher->connectedPublisher.Set(true);
            }
            break;
          case cs::VideoEvent::kSourceDisconnected:
            if (auto publisher = GetPublisher(event.sourceHandle)) {
              publisher->connectedPublisher.Set(false);
            }
            break;
          case cs::VideoEvent::kSourceVideoModesUpdated:
            if (auto publisher = GetPublisher(event.sourceHandle)) {
              publisher->modesPublisher.Set(
                  GetSourceModeValues(event.sourceHandle));
            }
            break;
          case cs::VideoEvent::kSourceVideoModeChanged:
            if (auto publisher = GetPublisher(event.sourceHandle)) {
              publisher->modeEntry.Set(VideoModeToString(event.mode));
            }
            break;
          case cs::VideoEvent::kSourcePropertyCreated:
            if (auto publisher = GetPublisher(event.sourceHandle)) {
              publisher->properties.insert(
                  {event.propertyHandle,
                   PropertyPublisher{*publisher->table, event}});
            }
            break;
          case cs::VideoEvent::kSourcePropertyValueUpdated:
            if (auto publisher = GetPublisher(event.sourceHandle)) {
              auto ppIt = publisher->properties.find(event.propertyHandle);
              if (ppIt != publisher->properties.end()) {
                ppIt->second.Update(event);
              }
            }
            break;
          case cs::VideoEvent::kSourcePropertyChoicesUpdated:
            if (auto publisher = GetPublisher(event.sourceHandle)) {
              auto ppIt = publisher->properties.find(event.propertyHandle);
              if (ppIt != publisher->properties.end() &&
                  ppIt->second.choicesTopic) {
                auto choices =
                    cs::GetEnumPropertyChoices(event.propertyHandle, &status);
                if (!ppIt->second.choicesPublisher) {
                  ppIt->second.choicesPublisher =
                      ppIt->second.choicesTopic.Publish();
                }
                ppIt->second.choicesPublisher.Set(choices);
              }
            }
            break;
          case cs::VideoEvent::kSinkSourceChanged:
          case cs::VideoEvent::kSinkCreated:
          case cs::VideoEvent::kSinkDestroyed:
          case cs::VideoEvent::kNetworkInterfacesChanged:
            m_addresses = cs::GetNetworkInterfaces();
            UpdateStreamValues();
            break;
          default:
            break;
        }
      },
      0x4fff, true};
}

cs::UsbCamera CameraServer::StartAutomaticCapture() {
  cs::UsbCamera camera =
      StartAutomaticCapture(::GetInstance().m_defaultUsbDevice++);
  auto csShared = GetCameraServerShared();
  csShared->ReportUsbCamera(camera.GetHandle());
  return camera;
}

cs::UsbCamera CameraServer::StartAutomaticCapture(int dev) {
  ::GetInstance();
  cs::UsbCamera camera{fmt::format("USB Camera {}", dev), dev};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportUsbCamera(camera.GetHandle());
  return camera;
}

cs::UsbCamera CameraServer::StartAutomaticCapture(std::string_view name,
                                                  int dev) {
  ::GetInstance();
  cs::UsbCamera camera{name, dev};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportUsbCamera(camera.GetHandle());
  return camera;
}

cs::UsbCamera CameraServer::StartAutomaticCapture(std::string_view name,
                                                  std::string_view path) {
  ::GetInstance();
  cs::UsbCamera camera{name, path};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportUsbCamera(camera.GetHandle());
  return camera;
}

WPI_IGNORE_DEPRECATED
cs::AxisCamera CameraServer::AddAxisCamera(std::string_view host) {
  return AddAxisCamera("Axis Camera", host);
}

cs::AxisCamera CameraServer::AddAxisCamera(const char* host) {
  return AddAxisCamera("Axis Camera", host);
}

cs::AxisCamera CameraServer::AddAxisCamera(const std::string& host) {
  return AddAxisCamera("Axis Camera", host);
}

cs::AxisCamera CameraServer::AddAxisCamera(std::span<const std::string> hosts) {
  return AddAxisCamera("Axis Camera", hosts);
}

cs::AxisCamera CameraServer::AddAxisCamera(std::string_view name,
                                           std::string_view host) {
  ::GetInstance();
  cs::AxisCamera camera{name, host};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportAxisCamera(camera.GetHandle());
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(std::string_view name,
                                           const char* host) {
  ::GetInstance();
  cs::AxisCamera camera{name, host};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportAxisCamera(camera.GetHandle());
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(std::string_view name,
                                           const std::string& host) {
  ::GetInstance();
  cs::AxisCamera camera{name, host};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportAxisCamera(camera.GetHandle());
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(std::string_view name,
                                           std::span<const std::string> hosts) {
  ::GetInstance();
  cs::AxisCamera camera{name, hosts};
  StartAutomaticCapture(camera);
  auto csShared = GetCameraServerShared();
  csShared->ReportAxisCamera(camera.GetHandle());
  return camera;
}
WPI_UNIGNORE_DEPRECATED
cs::MjpegServer CameraServer::AddSwitchedCamera(std::string_view name) {
  auto& inst = ::GetInstance();
  // create a dummy CvSource
  cs::CvSource source{name, cs::VideoMode::PixelFormat::kMJPEG, 160, 120, 30};
  cs::MjpegServer server = StartAutomaticCapture(source);
  inst.m_fixedSources[server.GetHandle()] = source.GetHandle();

  return server;
}

cs::MjpegServer CameraServer::StartAutomaticCapture(
    const cs::VideoSource& camera) {
  AddCamera(camera);
  auto server = AddServer(fmt::format("serve_{}", camera.GetName()));
  server.SetSource(camera);
  return server;
}

cs::CvSink CameraServer::GetVideo() {
  auto& inst = ::GetInstance();
  cs::VideoSource source;
  {
    auto csShared = GetCameraServerShared();
    std::scoped_lock lock(inst.m_mutex);
    if (inst.m_primarySourceName.empty()) {
      csShared->SetCameraServerError("no camera available");
      return cs::CvSink{};
    }
    auto it = inst.m_sources.find(inst.m_primarySourceName);
    if (it == inst.m_sources.end()) {
      csShared->SetCameraServerError("no camera available");
      return cs::CvSink{};
    }
    source = it->second;
  }
  return GetVideo(std::move(source));
}

cs::CvSink CameraServer::GetVideo(const cs::VideoSource& camera) {
  auto& inst = ::GetInstance();
  wpi::SmallString<64> name{"opencv_"};
  name += camera.GetName();

  {
    std::scoped_lock lock(inst.m_mutex);
    auto it = inst.m_sinks.find(name);
    if (it != inst.m_sinks.end()) {
      auto kind = it->second.GetKind();
      if (kind != cs::VideoSink::kCv) {
        auto csShared = GetCameraServerShared();
        csShared->SetCameraServerError("expected OpenCV sink, but got {}",
                                       static_cast<int>(kind));
        return cs::CvSink{};
      }
      return *static_cast<cs::CvSink*>(&it->second);
    }
  }

  cs::CvSink newsink{name.str()};
  newsink.SetSource(camera);
  AddServer(newsink);
  return newsink;
}

cs::CvSink CameraServer::GetVideo(const cs::VideoSource& camera,
                                  cs::VideoMode::PixelFormat pixelFormat) {
  auto& inst = ::GetInstance();
  wpi::SmallString<64> name{"opencv_"};
  name += camera.GetName();

  {
    std::scoped_lock lock(inst.m_mutex);
    auto it = inst.m_sinks.find(name);
    if (it != inst.m_sinks.end()) {
      auto kind = it->second.GetKind();
      if (kind != cs::VideoSink::kCv) {
        auto csShared = GetCameraServerShared();
        csShared->SetCameraServerError("expected OpenCV sink, but got {}",
                                       static_cast<int>(kind));
        return cs::CvSink{};
      }
      return *static_cast<cs::CvSink*>(&it->second);
    }
  }

  cs::CvSink newsink{name.str(), pixelFormat};
  newsink.SetSource(camera);
  AddServer(newsink);
  return newsink;
}

cs::CvSink CameraServer::GetVideo(std::string_view name) {
  auto& inst = ::GetInstance();
  cs::VideoSource source;
  {
    std::scoped_lock lock(inst.m_mutex);
    auto it = inst.m_sources.find(name);
    if (it == inst.m_sources.end()) {
      auto csShared = GetCameraServerShared();
      csShared->SetCameraServerError("could not find camera {}", name);
      return cs::CvSink{};
    }
    source = it->second;
  }
  return GetVideo(source);
}

cs::CvSink CameraServer::GetVideo(std::string_view name,
                                  cs::VideoMode::PixelFormat pixelFormat) {
  auto& inst = ::GetInstance();
  cs::VideoSource source;
  {
    std::scoped_lock lock(inst.m_mutex);
    auto it = inst.m_sources.find(name);
    if (it == inst.m_sources.end()) {
      auto csShared = GetCameraServerShared();
      csShared->SetCameraServerError("could not find camera {}", name);
      return cs::CvSink{};
    }
    source = it->second;
  }
  return GetVideo(source, pixelFormat);
}

cs::CvSource CameraServer::PutVideo(std::string_view name, int width,
                                    int height) {
  ::GetInstance();
  cs::CvSource source{name, cs::VideoMode::kMJPEG, width, height, 30};
  StartAutomaticCapture(source);
  return source;
}

cs::MjpegServer CameraServer::AddServer(std::string_view name) {
  auto& inst = ::GetInstance();
  int port;
  {
    std::scoped_lock lock(inst.m_mutex);
    port = inst.m_nextPort++;
  }
  return AddServer(name, port);
}

cs::MjpegServer CameraServer::AddServer(std::string_view name, int port) {
  ::GetInstance();
  cs::MjpegServer server{name, port};
  AddServer(server);
  return server;
}

void CameraServer::AddServer(const cs::VideoSink& server) {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.m_mutex);
  inst.m_sinks.try_emplace(server.GetName(), server);
}

void CameraServer::RemoveServer(std::string_view name) {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.m_mutex);
  inst.m_sinks.erase(name);
}

cs::VideoSink CameraServer::GetServer() {
  auto& inst = ::GetInstance();
  std::string name;
  {
    std::scoped_lock lock(inst.m_mutex);
    if (inst.m_primarySourceName.empty()) {
      auto csShared = GetCameraServerShared();
      csShared->SetCameraServerError("no camera available");
      return cs::VideoSink{};
    }
    name = fmt::format("serve_{}", inst.m_primarySourceName);
  }
  return GetServer(name);
}

cs::VideoSink CameraServer::GetServer(std::string_view name) {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.m_mutex);
  auto it = inst.m_sinks.find(name);
  if (it == inst.m_sinks.end()) {
    auto csShared = GetCameraServerShared();
    csShared->SetCameraServerError("could not find server {}", name);
    return cs::VideoSink{};
  }
  return it->second;
}

void CameraServer::AddCamera(const cs::VideoSource& camera) {
  auto& inst = ::GetInstance();
  std::string name = camera.GetName();
  std::scoped_lock lock(inst.m_mutex);
  if (inst.m_primarySourceName.empty()) {
    inst.m_primarySourceName = name;
  }
  inst.m_sources.try_emplace(name, camera);
}

void CameraServer::RemoveCamera(std::string_view name) {
  auto& inst = ::GetInstance();
  std::scoped_lock lock(inst.m_mutex);
  inst.m_sources.erase(name);
}
