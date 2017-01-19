/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CameraServer.h"

#include "Utility.h"
#include "WPIErrors.h"
#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"
#include "ntcore_cpp.h"

using namespace frc;

CameraServer* CameraServer::GetInstance() {
  static CameraServer instance;
  return &instance;
}

static llvm::StringRef MakeSourceValue(CS_Source source,
                                       llvm::SmallVectorImpl<char>& buf) {
  CS_Status status = 0;
  buf.clear();
  switch (cs::GetSourceKind(source, &status)) {
    case cs::VideoSource::kUsb: {
      llvm::StringRef prefix{"usb:"};
      buf.append(prefix.begin(), prefix.end());
      auto path = cs::GetUsbCameraPath(source, &status);
      buf.append(path.begin(), path.end());
      break;
    }
    case cs::VideoSource::kHttp: {
      llvm::StringRef prefix{"ip:"};
      buf.append(prefix.begin(), prefix.end());
      auto urls = cs::GetHttpCameraUrls(source, &status);
      if (!urls.empty()) buf.append(urls[0].begin(), urls[0].end());
      break;
    }
    case cs::VideoSource::kCv:
      // FIXME: Should be "cv:", but LabVIEW dashboard requires "usb:".
      // https://github.com/wpilibsuite/allwpilib/issues/407
      return "usb:";
    default:
      return "unknown:";
  }

  return llvm::StringRef{buf.begin(), buf.size()};
}

static std::string MakeStreamValue(llvm::StringRef address, int port) {
  std::string rv;
  llvm::raw_string_ostream stream(rv);
  stream << "mjpg:http://" << address << ':' << port << "/?action=stream";
  stream.flush();
  return rv;
}

std::shared_ptr<ITable> CameraServer::GetSourceTable(CS_Source source) {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_tables.lookup(source);
}

std::vector<std::string> CameraServer::GetSinkStreamValues(CS_Sink sink) {
  CS_Status status = 0;

  // Ignore all but MjpegServer
  if (cs::GetSinkKind(sink, &status) != CS_SINK_MJPEG)
    return std::vector<std::string>{};

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
      if (addr == "127.0.0.1") continue;  // ignore localhost
      values.emplace_back(MakeStreamValue(addr, port));
    }
  }

  return values;
}

std::vector<std::string> CameraServer::GetSourceStreamValues(CS_Source source) {
  CS_Status status = 0;

  // Ignore all but HttpCamera
  if (cs::GetSourceKind(source, &status) != CS_SOURCE_HTTP)
    return std::vector<std::string>{};

  // Generate values
  auto values = cs::GetHttpCameraUrls(source, &status);
  for (auto& value : values) value = "mjpg:" + value;

  // Look to see if we have a passthrough server for this source
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

  // Set table value
  return values;
}

void CameraServer::UpdateStreamValues() {
  std::lock_guard<std::mutex> lock(m_mutex);
  // Over all the sinks...
  for (const auto& i : m_sinks) {
    CS_Status status = 0;
    CS_Sink sink = i.second.GetHandle();

    // Get the source's subtable (if none exists, we're done)
    CS_Source source = cs::GetSinkSource(sink, &status);
    if (source == 0) continue;
    auto table = m_tables.lookup(source);
    if (table) {
      // Don't set stream values if this is a HttpCamera passthrough
      if (cs::GetSourceKind(source, &status) == CS_SOURCE_HTTP) continue;

      // Set table value
      auto values = GetSinkStreamValues(sink);
      if (!values.empty()) table->PutStringArray("streams", values);
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
      if (!values.empty()) table->PutStringArray("streams", values);
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
#if 0
static cs::VideoMode::PixelFormat PixelFormatFromString(llvm::StringRef str) {
  if (str == "MJPEG" || str == "mjpeg" || str == "JPEG" || str == "jpeg")
    return cs::VideoMode::PixelFormat::kMJPEG;
  if (str == "YUYV" || str == "yuyv") return cs::VideoMode::PixelFormat::kYUYV;
  if (str == "RGB565" || str == "rgb565")
    return cs::VideoMode::PixelFormat::kRGB565;
  if (str == "BGR" || str == "bgr") return cs::VideoMode::PixelFormat::kBGR;
  if (str == "GRAY" || str == "Gray" || str == "gray")
    return cs::VideoMode::PixelFormat::kGray;
  return cs::VideoMode::PixelFormat::kUnknown;
}

static cs::VideoMode VideoModeFromString(llvm::StringRef modeStr) {
  cs::VideoMode mode;
  size_t pos;

  // width: [0-9]+
  pos = modeStr.find_first_not_of("0123456789");
  llvm::StringRef widthStr = modeStr.slice(0, pos);
  modeStr = modeStr.drop_front(pos).ltrim();  // drop whitespace too

  // 'x'
  if (modeStr.empty() || modeStr[0] != 'x') return mode;
  modeStr = modeStr.drop_front(1).ltrim();  // drop whitespace too

  // height: [0-9]+
  pos = modeStr.find_first_not_of("0123456789");
  llvm::StringRef heightStr = modeStr.slice(0, pos);
  modeStr = modeStr.drop_front(pos).ltrim();  // drop whitespace too

  // format: all characters until whitespace
  pos = modeStr.find_first_of(" \t\n\v\f\r");
  llvm::StringRef formatStr = modeStr.slice(0, pos);
  modeStr = modeStr.drop_front(pos).ltrim();  // drop whitespace too

  // fps: [0-9.]+
  pos = modeStr.find_first_not_of("0123456789.");
  llvm::StringRef fpsStr = modeStr.slice(0, pos);
  modeStr = modeStr.drop_front(pos).ltrim();  // drop whitespace too

  // "fps"
  if (!modeStr.startswith("fps")) return mode;

  // make fps an integer string by dropping after the decimal
  fpsStr = fpsStr.slice(0, fpsStr.find('.'));

  // convert width, height, and fps to integers
  if (widthStr.getAsInteger(10, mode.width)) return mode;
  if (heightStr.getAsInteger(10, mode.height)) return mode;
  if (fpsStr.getAsInteger(10, mode.fps)) return mode;

  // convert format to enum value
  mode.pixelFormat = PixelFormatFromString(formatStr);

  return mode;
}
#endif
static std::string VideoModeToString(const cs::VideoMode& mode) {
  std::string rv;
  llvm::raw_string_ostream oss{rv};
  oss << mode.width << "x" << mode.height;
  oss << " " << PixelFormatToString(mode.pixelFormat) << " ";
  oss << mode.fps << " fps";
  return oss.str();
}

static std::vector<std::string> GetSourceModeValues(int source) {
  std::vector<std::string> rv;
  CS_Status status = 0;
  for (const auto& mode : cs::EnumerateSourceVideoModes(source, &status))
    rv.emplace_back(VideoModeToString(mode));
  return rv;
}

static inline llvm::StringRef Concatenate(llvm::StringRef lhs,
                                          llvm::StringRef rhs,
                                          llvm::SmallVectorImpl<char>& buf) {
  buf.clear();
  llvm::raw_svector_ostream oss{buf};
  oss << lhs << rhs;
  return oss.str();
}

static void PutSourcePropertyValue(ITable* table, const cs::VideoEvent& event,
                                   bool isNew) {
  llvm::SmallString<64> name;
  llvm::SmallString<64> infoName;
  if (llvm::StringRef{event.name}.startswith("raw_")) {
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

  llvm::SmallString<64> buf;
  CS_Status status = 0;
  switch (event.propertyKind) {
    case cs::VideoProperty::kBoolean:
      if (isNew)
        table->SetDefaultBoolean(name, event.value != 0);
      else
        table->PutBoolean(name, event.value != 0);
      break;
    case cs::VideoProperty::kInteger:
    case cs::VideoProperty::kEnum:
      if (isNew) {
        table->SetDefaultNumber(name, event.value);
        table->PutNumber(Concatenate(infoName, "/min", buf),
                         cs::GetPropertyMin(event.propertyHandle, &status));
        table->PutNumber(Concatenate(infoName, "/max", buf),
                         cs::GetPropertyMax(event.propertyHandle, &status));
        table->PutNumber(Concatenate(infoName, "/step", buf),
                         cs::GetPropertyStep(event.propertyHandle, &status));
        table->PutNumber(Concatenate(infoName, "/default", buf),
                         cs::GetPropertyDefault(event.propertyHandle, &status));
      } else {
        table->PutNumber(name, event.value);
      }
      break;
    case cs::VideoProperty::kString:
      if (isNew)
        table->SetDefaultString(name, event.valueStr);
      else
        table->PutString(name, event.valueStr);
      break;
    default:
      break;
  }
}

CameraServer::CameraServer()
    : m_publishTable{NetworkTable::GetTable(kPublishName)},
      m_nextPort(kBasePort) {
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
              std::lock_guard<std::mutex> lock(m_mutex);
              m_tables.insert(std::make_pair(event.sourceHandle, table));
            }
            llvm::SmallString<64> buf;
            table->PutString("source",
                             MakeSourceValue(event.sourceHandle, buf));
            llvm::SmallString<64> descBuf;
            table->PutString(
                "description",
                cs::GetSourceDescription(event.sourceHandle, descBuf, &status));
            table->PutBoolean("connected", cs::IsSourceConnected(
                                               event.sourceHandle, &status));
            table->PutStringArray("streams",
                                  GetSourceStreamValues(event.sourceHandle));
            auto mode = cs::GetSourceVideoMode(event.sourceHandle, &status);
            table->SetDefaultString("mode", VideoModeToString(mode));
            table->PutStringArray("modes",
                                  GetSourceModeValues(event.sourceHandle));
            break;
          }
          case cs::VideoEvent::kSourceDestroyed: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              table->PutString("source", "");
              table->PutStringArray("streams", std::vector<std::string>{});
              table->PutStringArray("modes", std::vector<std::string>{});
            }
            break;
          }
          case cs::VideoEvent::kSourceConnected: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              // update the description too (as it may have changed)
              llvm::SmallString<64> descBuf;
              table->PutString("description",
                               cs::GetSourceDescription(event.sourceHandle,
                                                        descBuf, &status));
              table->PutBoolean("connected", true);
            }
            break;
          }
          case cs::VideoEvent::kSourceDisconnected: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) table->PutBoolean("connected", false);
            break;
          }
          case cs::VideoEvent::kSourceVideoModesUpdated: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table)
              table->PutStringArray("modes",
                                    GetSourceModeValues(event.sourceHandle));
            break;
          }
          case cs::VideoEvent::kSourceVideoModeChanged: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) table->PutString("mode", VideoModeToString(event.mode));
            break;
          }
          case cs::VideoEvent::kSourcePropertyCreated: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) PutSourcePropertyValue(table.get(), event, true);
            break;
          }
          case cs::VideoEvent::kSourcePropertyValueUpdated: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) PutSourcePropertyValue(table.get(), event, false);
            break;
          }
          case cs::VideoEvent::kSourcePropertyChoicesUpdated: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              llvm::SmallString<64> name{"PropertyInfo/"};
              name += event.name;
              name += "/choices";
              auto choices =
                  cs::GetEnumPropertyChoices(event.propertyHandle, &status);
              table->PutStringArray(name, choices);
            }
            break;
          }
          case cs::VideoEvent::kSinkSourceChanged:
          case cs::VideoEvent::kSinkCreated:
          case cs::VideoEvent::kSinkDestroyed: {
            UpdateStreamValues();
            break;
          }
          case cs::VideoEvent::kNetworkInterfacesChanged: {
            m_addresses = cs::GetNetworkInterfaces();
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
  llvm::SmallString<64> buf;
  m_tableListener = nt::AddEntryListener(
      Concatenate(kPublishName, "/", buf),
      [=](unsigned int uid, llvm::StringRef key,
          std::shared_ptr<nt::Value> value, unsigned int flags) {
        llvm::StringRef relativeKey =
            key.substr(llvm::StringRef(kPublishName).size() + 1);

        // get source (sourceName/...)
        auto subKeyIndex = relativeKey.find('/');
        if (subKeyIndex == llvm::StringRef::npos) return;
        llvm::StringRef sourceName = relativeKey.slice(0, subKeyIndex);
        auto sourceIt = m_sources.find(sourceName);
        if (sourceIt == m_sources.end()) return;

        // get subkey
        relativeKey = relativeKey.substr(subKeyIndex + 1);

        // handle standard names
        llvm::StringRef propName;
        if (relativeKey == "mode") {
          // reset to current mode
          nt::SetEntryValue(key, nt::Value::MakeString(VideoModeToString(
                                     sourceIt->second.GetVideoMode())));
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
            nt::SetEntryValue(key, nt::Value::MakeBoolean(property.Get() != 0));
            return;
          case cs::VideoProperty::kInteger:
          case cs::VideoProperty::kEnum:
            nt::SetEntryValue(key, nt::Value::MakeDouble(property.Get()));
            return;
          case cs::VideoProperty::kString:
            nt::SetEntryValue(key, nt::Value::MakeString(property.GetString()));
            return;
          default:
            return;
        }
      },
      NT_NOTIFY_IMMEDIATE | NT_NOTIFY_UPDATE);
}

cs::UsbCamera CameraServer::StartAutomaticCapture() {
  return StartAutomaticCapture(m_defaultUsbDevice++);
}

cs::UsbCamera CameraServer::StartAutomaticCapture(int dev) {
  llvm::SmallString<64> buf;
  llvm::raw_svector_ostream name{buf};
  name << "USB Camera " << dev;

  cs::UsbCamera camera{name.str(), dev};
  StartAutomaticCapture(camera);
  return camera;
}

cs::UsbCamera CameraServer::StartAutomaticCapture(llvm::StringRef name,
                                                  int dev) {
  cs::UsbCamera camera{name, dev};
  StartAutomaticCapture(camera);
  return camera;
}

cs::UsbCamera CameraServer::StartAutomaticCapture(llvm::StringRef name,
                                                  llvm::StringRef path) {
  cs::UsbCamera camera{name, path};
  StartAutomaticCapture(camera);
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(llvm::StringRef host) {
  return AddAxisCamera("Axis Camera", host);
}

cs::AxisCamera CameraServer::AddAxisCamera(const char* host) {
  return AddAxisCamera("Axis Camera", host);
}

cs::AxisCamera CameraServer::AddAxisCamera(const std::string& host) {
  return AddAxisCamera("Axis Camera", host);
}

cs::AxisCamera CameraServer::AddAxisCamera(llvm::ArrayRef<std::string> hosts) {
  return AddAxisCamera("Axis Camera", hosts);
}

cs::AxisCamera CameraServer::AddAxisCamera(llvm::StringRef name,
                                           llvm::StringRef host) {
  cs::AxisCamera camera{name, host};
  StartAutomaticCapture(camera);
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(llvm::StringRef name,
                                           const char* host) {
  cs::AxisCamera camera{name, host};
  StartAutomaticCapture(camera);
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(llvm::StringRef name,
                                           const std::string& host) {
  cs::AxisCamera camera{name, host};
  StartAutomaticCapture(camera);
  return camera;
}

cs::AxisCamera CameraServer::AddAxisCamera(llvm::StringRef name,
                                           llvm::ArrayRef<std::string> hosts) {
  cs::AxisCamera camera{name, hosts};
  StartAutomaticCapture(camera);
  return camera;
}

void CameraServer::StartAutomaticCapture(const cs::VideoSource& camera) {
  llvm::SmallString<64> name{"serve_"};
  name += camera.GetName();

  AddCamera(camera);
  auto server = AddServer(name);
  server.SetSource(camera);
}

cs::CvSink CameraServer::GetVideo() {
  cs::VideoSource source;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_primarySourceName.empty()) {
      wpi_setWPIErrorWithContext(CameraServerError, "no camera available");
      return cs::CvSink{};
    }
    auto it = m_sources.find(m_primarySourceName);
    if (it == m_sources.end()) {
      wpi_setWPIErrorWithContext(CameraServerError, "no camera available");
      return cs::CvSink{};
    }
    source = it->second;
  }
  return GetVideo(std::move(source));
}

cs::CvSink CameraServer::GetVideo(const cs::VideoSource& camera) {
  llvm::SmallString<64> name{"opencv_"};
  name += camera.GetName();

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_sinks.find(name);
    if (it != m_sinks.end()) {
      auto kind = it->second.GetKind();
      if (kind != cs::VideoSink::kCv) {
        llvm::SmallString<64> buf;
        llvm::raw_svector_ostream err{buf};
        err << "expected OpenCV sink, but got " << kind;
        wpi_setWPIErrorWithContext(CameraServerError, err.str());
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

cs::CvSink CameraServer::GetVideo(llvm::StringRef name) {
  cs::VideoSource source;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_sources.find(name);
    if (it == m_sources.end()) {
      llvm::SmallString<64> buf;
      llvm::raw_svector_ostream err{buf};
      err << "could not find camera " << name;
      wpi_setWPIErrorWithContext(CameraServerError, err.str());
      return cs::CvSink{};
    }
    source = it->second;
  }
  return GetVideo(source);
}

cs::CvSource CameraServer::PutVideo(llvm::StringRef name, int width,
                                    int height) {
  cs::CvSource source{name, cs::VideoMode::kMJPEG, width, height, 30};
  StartAutomaticCapture(source);
  return source;
}

cs::MjpegServer CameraServer::AddServer(llvm::StringRef name) {
  int port;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    port = m_nextPort++;
  }
  return AddServer(name, port);
}

cs::MjpegServer CameraServer::AddServer(llvm::StringRef name, int port) {
  cs::MjpegServer server{name, port};
  AddServer(server);
  return server;
}

void CameraServer::AddServer(const cs::VideoSink& server) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_sinks.emplace_second(server.GetName(), server);
}

void CameraServer::RemoveServer(llvm::StringRef name) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_sinks.erase(name);
}

cs::VideoSink CameraServer::GetServer() {
  llvm::SmallString<64> name;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_primarySourceName.empty()) {
      wpi_setWPIErrorWithContext(CameraServerError, "no camera available");
      return cs::VideoSink{};
    }
    name = "serve_";
    name += m_primarySourceName;
  }
  return GetServer(name);
}

cs::VideoSink CameraServer::GetServer(llvm::StringRef name) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto it = m_sinks.find(name);
  if (it == m_sinks.end()) {
    llvm::SmallString<64> buf;
    llvm::raw_svector_ostream err{buf};
    err << "could not find server " << name;
    wpi_setWPIErrorWithContext(CameraServerError, err.str());
    return cs::VideoSink{};
  }
  return it->second;
}

void CameraServer::AddCamera(const cs::VideoSource& camera) {
  std::string name = camera.GetName();
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_primarySourceName.empty()) m_primarySourceName = name;
  m_sources.emplace_second(name, camera);
}

void CameraServer::RemoveCamera(llvm::StringRef name) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_sources.erase(name);
}

void CameraServer::SetSize(int size) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_primarySourceName.empty()) return;
  auto it = m_sources.find(m_primarySourceName);
  if (it == m_sources.end()) return;
  if (size == kSize160x120)
    it->second.SetResolution(160, 120);
  else if (size == kSize320x240)
    it->second.SetResolution(320, 240);
  else if (size == kSize640x480)
    it->second.SetResolution(640, 480);
}
