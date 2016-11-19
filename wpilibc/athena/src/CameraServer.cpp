/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CameraServer.h"

#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"
#include "Utility.h"
#include "WPIErrors.h"

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
    case cs::VideoSource::kUSB: {
      llvm::StringRef prefix{"usb:"};
      buf.append(prefix.begin(), prefix.end());
      auto path = cs::GetUSBCameraPath(source, &status);
      buf.append(path.begin(), path.end());
      break;
    }
    case cs::VideoSource::kHTTP: {
      llvm::StringRef prefix{"ip:"};
      buf.append(prefix.begin(), prefix.end());
      // TODO
      break;
    }
    case cs::VideoSource::kCv:
      return "cv:";
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

void CameraServer::UpdateStreamValues() {
  std::lock_guard<std::mutex> lock(m_mutex);
  // Over all the sinks...
  for (const auto& i : m_sinks) {
    CS_Status status = 0;
    // Ignore all but MJPEGServer
    if (i.second.GetKind() != cs::VideoSink::kMJPEG) continue;
    CS_Sink sink = i.second.GetHandle();

    // Get the source's subtable (if none exists, we're done)
    CS_Source source = cs::GetSinkSource(sink, &status);
    auto table = m_tables.lookup(source);
    if (!table) continue;

    // Get port
    int port = cs::GetMJPEGServerPort(sink, &status);

    // Generate values
    std::vector<std::string> values;
    auto listenAddress = cs::GetMJPEGServerListenAddress(sink, &status);
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

    // Set table value
    table->PutStringArray("streams", values);
  }
}

CameraServer::CameraServer()
    : m_publishTable{NetworkTable::GetTable(kPublishName)},
      m_nextPort(kBasePort) {
  // We publish sources to NetworkTables using the following structure:
  // "/CameraPublisher/{Source.Name}/" - root
  // - "source" (string): Descriptive, prefixed with type (e.g. "usb:0")
  // - "streams" (string array): URLs that can be used to stream data
  // - properties (scaled units)

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
            table->PutStringArray("streams", std::vector<std::string>{});
            break;
          }
          case cs::VideoEvent::kSourceDestroyed: {
            auto table = GetSourceTable(event.sourceHandle);
            if (table) {
              table->PutString("source", "");
              table->PutStringArray("streams", std::vector<std::string>{});
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
            break;
          }
          case cs::VideoEvent::kSourceVideoModeChanged: {
            break;
          }
          case cs::VideoEvent::kSourcePropertyCreated: {
            break;
          }
          case cs::VideoEvent::kSourcePropertyValueUpdated: {
            break;
          }
          case cs::VideoEvent::kSourcePropertyChoicesUpdated: {
            break;
          }
          case cs::VideoEvent::kSinkSourceChanged: {
            UpdateStreamValues();
            break;
          }
          case cs::VideoEvent::kSinkCreated: {
            break;
          }
          case cs::VideoEvent::kSinkDestroyed: {
            break;
          }
          case cs::VideoEvent::kSinkEnabled: {
            break;
          }
          case cs::VideoEvent::kSinkDisabled: {
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
      0x7fff, true};
}

cs::USBCamera CameraServer::StartAutomaticCapture() {
  return StartAutomaticCapture(0);
}

cs::USBCamera CameraServer::StartAutomaticCapture(int dev) {
  llvm::SmallString<64> buf;
  llvm::raw_svector_ostream name{buf};
  name << "USB Camera " << dev;

  cs::USBCamera camera{name.str(), dev};
  StartAutomaticCapture(camera);
  return camera;
}

cs::USBCamera CameraServer::StartAutomaticCapture(llvm::StringRef name,
                                                  int dev) {
  cs::USBCamera camera{name, dev};
  StartAutomaticCapture(camera);
  return camera;
}

cs::USBCamera CameraServer::StartAutomaticCapture(llvm::StringRef name,
                                                  llvm::StringRef path) {
  cs::USBCamera camera{name, path};
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

cs::CvSource CameraServer::PutVideo(llvm::StringRef name, int width,
                                    int height) {
  cs::CvSource source{name, cs::VideoMode::kMJPEG, width, height, 30};
  StartAutomaticCapture(source);
  return source;
}

cs::MJPEGServer CameraServer::AddServer(llvm::StringRef name) {
  int port;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    port = m_nextPort++;
  }
  return AddServer(name, port);
}

cs::MJPEGServer CameraServer::AddServer(llvm::StringRef name, int port) {
  cs::MJPEGServer server{name, port};
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
