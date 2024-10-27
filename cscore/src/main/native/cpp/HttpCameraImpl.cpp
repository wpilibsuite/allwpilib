// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HttpCameraImpl.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/MemAlloc.h>
#include <wpi/StringExtras.h>
#include <wpi/timestamp.h>
#include <wpinet/TCPConnector.h>

#include "Instance.h"
#include "JpegUtil.h"
#include "Log.h"
#include "Notifier.h"
#include "Telemetry.h"
#include "c_util.h"

using namespace cs;

HttpCameraImpl::HttpCameraImpl(std::string_view name, CS_HttpCameraKind kind,
                               wpi::Logger& logger, Notifier& notifier,
                               Telemetry& telemetry)
    : SourceImpl{name, logger, notifier, telemetry}, m_kind{kind} {}

HttpCameraImpl::~HttpCameraImpl() {
  m_active = false;

  // force wakeup of monitor thread
  m_monitorCond.notify_one();

  // join monitor thread
  if (m_monitorThread.joinable()) {
    m_monitorThread.join();
  }

  // Close file if it's open
  {
    std::scoped_lock lock(m_mutex);
    if (m_streamConn) {
      m_streamConn->stream->close();
    }
    if (m_settingsConn) {
      m_settingsConn->stream->close();
    }
  }

  // force wakeup of camera thread in case it's waiting on cv
  m_sinkEnabledCond.notify_one();

  // join camera thread
  if (m_streamThread.joinable()) {
    m_streamThread.join();
  }

  // force wakeup of settings thread
  m_settingsCond.notify_one();

  // join settings thread
  if (m_settingsThread.joinable()) {
    m_settingsThread.join();
  }
}

void HttpCameraImpl::Start() {
  // Kick off the stream and settings threads
  m_streamThread = std::thread(&HttpCameraImpl::StreamThreadMain, this);
  m_settingsThread = std::thread(&HttpCameraImpl::SettingsThreadMain, this);
  m_monitorThread = std::thread(&HttpCameraImpl::MonitorThreadMain, this);
}

void HttpCameraImpl::MonitorThreadMain() {
  while (m_active) {
    std::unique_lock lock(m_mutex);
    // sleep for 1 second between checks
    m_monitorCond.wait_for(lock, std::chrono::seconds(1),
                           [=, this] { return !m_active; });

    if (!m_active) {
      break;
    }

    // check to see if we got any frames, and close the stream if not
    // (this will result in an error at the read point, and ultimately
    // a reconnect attempt)
    if (m_streamConn && m_frameCount == 0) {
      SWARNING("Monitor detected stream hung, disconnecting");
      m_streamConn->stream->close();
    }

    // reset the frame counter
    m_frameCount = 0;
  }

  SDEBUG("Monitor Thread exiting");
}

void HttpCameraImpl::StreamThreadMain() {
  while (m_active) {
    SetConnected(false);

    // sleep between retries
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    // disconnect if not enabled
    if (!IsEnabled()) {
      std::unique_lock lock(m_mutex);
      if (m_streamConn) {
        m_streamConn->stream->close();
      }
      // Wait for enable
      m_sinkEnabledCond.wait(lock,
                             [=, this] { return !m_active || IsEnabled(); });
      if (!m_active) {
        return;
      }
    }

    // connect
    wpi::SmallString<64> boundary;
    wpi::HttpConnection* conn = DeviceStreamConnect(boundary);

    if (!m_active) {
      break;
    }

    // keep retrying
    if (!conn) {
      continue;
    }

    // update connected since we're actually connected
    SetConnected(true);

    // stream
    DeviceStream(conn->is, boundary.str());
    {
      std::unique_lock lock(m_mutex);
      m_streamConn = nullptr;
    }
  }

  SDEBUG("Camera Thread exiting");
  SetConnected(false);
}

wpi::HttpConnection* HttpCameraImpl::DeviceStreamConnect(
    wpi::SmallVectorImpl<char>& boundary) {
  // Build the request
  wpi::HttpRequest req;
  {
    std::scoped_lock lock(m_mutex);
    if (m_locations.empty()) {
      SERROR("locations array is empty!?");
      std::this_thread::sleep_for(std::chrono::seconds(1));
      return nullptr;
    }
    if (m_nextLocation >= m_locations.size()) {
      m_nextLocation = 0;
    }
    req = wpi::HttpRequest{m_locations[m_nextLocation++], m_streamSettings};
    m_streamSettingsUpdated = false;
  }

  // Try to connect
  auto stream =
      wpi::TCPConnector::connect(req.host.c_str(), req.port, m_logger, 1);

  if (!m_active || !stream) {
    return nullptr;
  }

  auto connPtr = std::make_unique<wpi::HttpConnection>(std::move(stream), 1);
  wpi::HttpConnection* conn = connPtr.get();

  // update m_streamConn
  {
    std::scoped_lock lock(m_mutex);
    m_frameCount = 1;  // avoid a race with monitor thread
    m_streamConn = std::move(connPtr);
  }

  std::string warn;
  if (!conn->Handshake(req, &warn)) {
    SWARNING("{}", warn);
    std::scoped_lock lock(m_mutex);
    m_streamConn = nullptr;
    return nullptr;
  }

  // Parse Content-Type header to get the boundary
  auto [mediaType, contentType] = wpi::split(conn->contentType.str(), ';');
  mediaType = wpi::trim(mediaType);
  if (mediaType != "multipart/x-mixed-replace") {
    SWARNING("\"{}\": unrecognized Content-Type \"{}\"", req.host.str(),
             mediaType);
    std::scoped_lock lock(m_mutex);
    m_streamConn = nullptr;
    return nullptr;
  }

  // media parameters
  boundary.clear();
  while (!contentType.empty()) {
    std::string_view keyvalue;
    std::tie(keyvalue, contentType) = wpi::split(contentType, ';');
    contentType = wpi::ltrim(contentType);
    auto [key, value] = wpi::split(keyvalue, '=');
    if (wpi::trim(key) == "boundary") {
      value = wpi::trim(wpi::trim(value), '"');  // value may be quoted
      if (wpi::starts_with(value, "--")) {
        value = wpi::substr(value, 2);
      }
      boundary.append(value.begin(), value.end());
    }
  }

  if (boundary.empty()) {
    SWARNING("\"{}\": empty multi-part boundary or no Content-Type",
             req.host.str());
    std::scoped_lock lock(m_mutex);
    m_streamConn = nullptr;
    return nullptr;
  }

  return conn;
}

void HttpCameraImpl::DeviceStream(wpi::raw_istream& is,
                                  std::string_view boundary) {
  // Stored here so we reuse it from frame to frame
  std::string imageBuf;

  // keep track of number of bad images received; if we receive 3 bad images
  // in a row, we reconnect
  int numErrors = 0;

  // streaming loop
  while (m_active && !is.has_error() && IsEnabled() && numErrors < 3 &&
         !m_streamSettingsUpdated) {
    if (!FindMultipartBoundary(is, boundary, nullptr)) {
      break;
    }

    // Read the next two characters after the boundary (normally \r\n)
    // Handle just \n for LabVIEW however
    char eol[2];
    is.read(eol, 1);
    if (!m_active || is.has_error()) {
      break;
    }
    if (eol[0] != '\n') {
      is.read(eol + 1, 1);
      if (!m_active || is.has_error()) {
        break;
      }
      // End-of-stream is indicated with trailing --
      if (eol[0] == '-' && eol[1] == '-') {
        break;
      }
    }

    if (!DeviceStreamFrame(is, imageBuf)) {
      ++numErrors;
    } else {
      numErrors = 0;
    }
  }
}

bool HttpCameraImpl::DeviceStreamFrame(wpi::raw_istream& is,
                                       std::string& imageBuf) {
  // Read the headers
  wpi::SmallString<64> contentTypeBuf;
  wpi::SmallString<64> contentLengthBuf;
  if (!ParseHttpHeaders(is, &contentTypeBuf, &contentLengthBuf)) {
    SWARNING("disconnected during headers");
    PutError("disconnected during headers", wpi::Now());
    return false;
  }

  // Check the content type (if present)
  if (!contentTypeBuf.str().empty() &&
      !wpi::starts_with(contentTypeBuf, "image/jpeg")) {
    auto errMsg = fmt::format("received unknown Content-Type \"{}\"",
                              contentTypeBuf.str());
    SWARNING("{}", errMsg);
    PutError(errMsg, wpi::Now());
    return false;
  }

  int width, height;
  if (auto v = wpi::parse_integer<unsigned int>(contentLengthBuf, 10)) {
    // We know how big it is!  Just get a frame of the right size and read
    // the data directly into it.
    unsigned int contentLength = v.value();
    auto image =
        AllocImage(VideoMode::PixelFormat::kMJPEG, 0, 0, contentLength);
    is.read(image->data(), contentLength);
    if (!m_active || is.has_error()) {
      return false;
    }
    if (!GetJpegSize(image->str(), &width, &height)) {
      SWARNING("did not receive a JPEG image");
      PutError("did not receive a JPEG image", wpi::Now());
      return false;
    }
    image->width = width;
    image->height = height;
    PutFrame(std::move(image), wpi::Now());
  } else {
    // Ugh, no Content-Length?  Read the blocks of the JPEG file.
    if (!ReadJpeg(is, imageBuf, &width, &height)) {
      SWARNING("did not receive a JPEG image");
      PutError("did not receive a JPEG image", wpi::Now());
      return false;
    }
    PutFrame(VideoMode::PixelFormat::kMJPEG, width, height, imageBuf,
             wpi::Now());
  }

  ++m_frameCount;

  // update video mode if not set
  std::scoped_lock lock(m_mutex);
  if (m_mode.pixelFormat != VideoMode::PixelFormat::kMJPEG ||
      m_mode.width == 0 || m_mode.height == 0) {
    m_mode.pixelFormat = VideoMode::PixelFormat::kMJPEG;
    m_mode.width = width;
    m_mode.height = height;
  }
  return true;
}

void HttpCameraImpl::SettingsThreadMain() {
  for (;;) {
    wpi::HttpRequest req;
    {
      std::unique_lock lock(m_mutex);
      m_settingsCond.wait(lock, [=, this] {
        return !m_active || (m_prefLocation != -1 && !m_settings.empty());
      });
      if (!m_active) {
        break;
      }

      // Build the request
      req = wpi::HttpRequest{m_locations[m_prefLocation], m_settings};
    }

    DeviceSendSettings(req);
  }

  SDEBUG("Settings Thread exiting");
}

void HttpCameraImpl::DeviceSendSettings(wpi::HttpRequest& req) {
  // Try to connect
  auto stream =
      wpi::TCPConnector::connect(req.host.c_str(), req.port, m_logger, 1);

  if (!m_active || !stream) {
    return;
  }

  auto connPtr = std::make_unique<wpi::HttpConnection>(std::move(stream), 1);
  wpi::HttpConnection* conn = connPtr.get();

  // update m_settingsConn
  {
    std::scoped_lock lock(m_mutex);
    m_settingsConn = std::move(connPtr);
  }

  // Just need a handshake as settings are sent via GET parameters
  std::string warn;
  if (!conn->Handshake(req, &warn)) {
    SWARNING("{}", warn);
  }

  conn->stream->close();
}

CS_HttpCameraKind HttpCameraImpl::GetKind() const {
  std::scoped_lock lock(m_mutex);
  return m_kind;
}

bool HttpCameraImpl::SetUrls(std::span<const std::string> urls,
                             CS_Status* status) {
  std::vector<wpi::HttpLocation> locations;
  for (const auto& url : urls) {
    bool error = false;
    std::string errorMsg;
    locations.emplace_back(url, &error, &errorMsg);
    if (error) {
      SERROR("{}", errorMsg);
      *status = CS_BAD_URL;
      return false;
    }
  }

  std::scoped_lock lock(m_mutex);
  m_locations.swap(locations);
  m_nextLocation = 0;
  m_streamSettingsUpdated = true;
  return true;
}

std::vector<std::string> HttpCameraImpl::GetUrls() const {
  std::scoped_lock lock(m_mutex);
  std::vector<std::string> urls;
  for (const auto& loc : m_locations) {
    urls.push_back(loc.url);
  }
  return urls;
}

void HttpCameraImpl::CreateProperty(std::string_view name,
                                    std::string_view httpParam,
                                    bool viaSettings, CS_PropertyKind kind,
                                    int minimum, int maximum, int step,
                                    int defaultValue, int value) const {
  std::scoped_lock lock(m_mutex);
  m_propertyData.emplace_back(std::make_unique<PropertyData>(
      name, httpParam, viaSettings, kind, minimum, maximum, step, defaultValue,
      value));

  m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CREATED, name,
                                  m_propertyData.size() + 1, kind, value, {});
}

template <typename T>
void HttpCameraImpl::CreateEnumProperty(
    std::string_view name, std::string_view httpParam, bool viaSettings,
    int defaultValue, int value, std::initializer_list<T> choices) const {
  std::scoped_lock lock(m_mutex);
  m_propertyData.emplace_back(std::make_unique<PropertyData>(
      name, httpParam, viaSettings, CS_PROP_ENUM, 0, choices.size() - 1, 1,
      defaultValue, value));

  auto& enumChoices = m_propertyData.back()->enumChoices;
  enumChoices.clear();
  for (const auto& choice : choices) {
    enumChoices.emplace_back(choice);
  }

  m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CREATED, name,
                                  m_propertyData.size() + 1, CS_PROP_ENUM,
                                  value, {});
  m_notifier.NotifySourceProperty(*this, CS_SOURCE_PROPERTY_CHOICES_UPDATED,
                                  name, m_propertyData.size() + 1, CS_PROP_ENUM,
                                  value, {});
}

std::unique_ptr<PropertyImpl> HttpCameraImpl::CreateEmptyProperty(
    std::string_view name) const {
  return std::make_unique<PropertyData>(name);
}

bool HttpCameraImpl::CacheProperties(CS_Status* status) const {
  std::scoped_lock lock(m_mutex);

  // Pretty typical set of video modes
  m_videoModes.clear();
  m_videoModes.emplace_back(VideoMode::kMJPEG, 640, 480, 30);
  m_videoModes.emplace_back(VideoMode::kMJPEG, 320, 240, 30);
  m_videoModes.emplace_back(VideoMode::kMJPEG, 160, 120, 30);

  m_properties_cached = true;
  return true;
}

void HttpCameraImpl::SetProperty(int property, int value, CS_Status* status) {
  // TODO
}

void HttpCameraImpl::SetStringProperty(int property, std::string_view value,
                                       CS_Status* status) {
  // TODO
}

void HttpCameraImpl::SetBrightness(int brightness, CS_Status* status) {
  // TODO
}

int HttpCameraImpl::GetBrightness(CS_Status* status) const {
  // TODO
  return 0;
}

void HttpCameraImpl::SetWhiteBalanceAuto(CS_Status* status) {
  // TODO
}

void HttpCameraImpl::SetWhiteBalanceHoldCurrent(CS_Status* status) {
  // TODO
}

void HttpCameraImpl::SetWhiteBalanceManual(int value, CS_Status* status) {
  // TODO
}

void HttpCameraImpl::SetExposureAuto(CS_Status* status) {
  // TODO
}

void HttpCameraImpl::SetExposureHoldCurrent(CS_Status* status) {
  // TODO
}

void HttpCameraImpl::SetExposureManual(int value, CS_Status* status) {
  // TODO
}

bool HttpCameraImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {
  if (mode.pixelFormat != VideoMode::kMJPEG) {
    return false;
  }
  std::scoped_lock lock(m_mutex);
  m_mode = mode;
  m_streamSettings.clear();
  if (mode.width != 0 && mode.height != 0) {
    m_streamSettings["resolution"] =
        fmt::format("{}x{}", mode.width, mode.height);
  }
  if (mode.fps != 0) {
    m_streamSettings["fps"] = fmt::format("{}", mode.fps);
  }
  m_streamSettingsUpdated = true;
  return true;
}

void HttpCameraImpl::NumSinksChanged() {
  // ignore
}

void HttpCameraImpl::NumSinksEnabledChanged() {
  m_sinkEnabledCond.notify_one();
}

bool AxisCameraImpl::CacheProperties(CS_Status* status) const {
  CreateProperty("brightness", "ImageSource.I0.Sensor.Brightness", true,
                 CS_PROP_INTEGER, 0, 100, 1, 50, 50);
  CreateEnumProperty("white_balance", "ImageSource.I0.Sensor.WhiteBalance",
                     true, 0, 0,
                     {"auto", "hold", "fixed_outdoor1", "fixed_outdoor2",
                      "fixed_indoor", "fixed_fluor1", "fixed_fluor2"});
  CreateProperty("color_level", "ImageSource.I0.Sensor.ColorLevel", true,
                 CS_PROP_INTEGER, 0, 100, 1, 50, 50);
  CreateEnumProperty("exposure", "ImageSource.I0.Sensor.Exposure", true, 0, 0,
                     {"auto", "hold", "flickerfree50", "flickerfree60"});
  CreateProperty("exposure_priority", "ImageSource.I0.Sensor.ExposurePriority",
                 true, CS_PROP_INTEGER, 0, 100, 1, 50, 50);

  // TODO: get video modes from device
  std::scoped_lock lock(m_mutex);
  m_videoModes.clear();
  m_videoModes.emplace_back(VideoMode::kMJPEG, 640, 480, 30);
  m_videoModes.emplace_back(VideoMode::kMJPEG, 480, 360, 30);
  m_videoModes.emplace_back(VideoMode::kMJPEG, 320, 240, 30);
  m_videoModes.emplace_back(VideoMode::kMJPEG, 240, 180, 30);
  m_videoModes.emplace_back(VideoMode::kMJPEG, 176, 144, 30);
  m_videoModes.emplace_back(VideoMode::kMJPEG, 160, 120, 30);

  m_properties_cached = true;
  return true;
}

namespace cs {

CS_Source CreateHttpCamera(std::string_view name, std::string_view url,
                           CS_HttpCameraKind kind, CS_Status* status) {
  auto& inst = Instance::GetInstance();
  std::shared_ptr<HttpCameraImpl> source;
  switch (kind) {
    case CS_HTTP_AXIS:
      source = std::make_shared<AxisCameraImpl>(name, inst.logger,
                                                inst.notifier, inst.telemetry);
      break;
    default:
      source = std::make_shared<HttpCameraImpl>(name, kind, inst.logger,
                                                inst.notifier, inst.telemetry);
      break;
  }
  std::string urlStr{url};
  if (!source->SetUrls(std::span{&urlStr, 1}, status)) {
    return 0;
  }
  return inst.CreateSource(CS_SOURCE_HTTP, source);
}

CS_Source CreateHttpCamera(std::string_view name,
                           std::span<const std::string> urls,
                           CS_HttpCameraKind kind, CS_Status* status) {
  auto& inst = Instance::GetInstance();
  if (urls.empty()) {
    *status = CS_EMPTY_VALUE;
    return 0;
  }
  auto source = std::make_shared<HttpCameraImpl>(name, kind, inst.logger,
                                                 inst.notifier, inst.telemetry);
  if (!source->SetUrls(urls, status)) {
    return 0;
  }
  return inst.CreateSource(CS_SOURCE_HTTP, source);
}

CS_HttpCameraKind GetHttpCameraKind(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_HTTP) {
    *status = CS_INVALID_HANDLE;
    return CS_HTTP_UNKNOWN;
  }
  return static_cast<HttpCameraImpl&>(*data->source).GetKind();
}

void SetHttpCameraUrls(CS_Source source, std::span<const std::string> urls,
                       CS_Status* status) {
  if (urls.empty()) {
    *status = CS_EMPTY_VALUE;
    return;
  }
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_HTTP) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  static_cast<HttpCameraImpl&>(*data->source).SetUrls(urls, status);
}

std::vector<std::string> GetHttpCameraUrls(CS_Source source,
                                           CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_HTTP) {
    *status = CS_INVALID_HANDLE;
    return std::vector<std::string>{};
  }
  return static_cast<HttpCameraImpl&>(*data->source).GetUrls();
}

}  // namespace cs

extern "C" {

CS_Source CS_CreateHttpCamera(const struct WPI_String* name,
                              const struct WPI_String* url,
                              CS_HttpCameraKind kind, CS_Status* status) {
  return cs::CreateHttpCamera(wpi::to_string_view(name),
                              wpi::to_string_view(url), kind, status);
}

CS_Source CS_CreateHttpCameraMulti(const struct WPI_String* name,
                                   const struct WPI_String* urls, int count,
                                   CS_HttpCameraKind kind, CS_Status* status) {
  wpi::SmallVector<std::string, 4> vec;
  vec.reserve(count);
  for (int i = 0; i < count; ++i) {
    vec.emplace_back(wpi::to_string_view(&urls[i]));
  }
  return cs::CreateHttpCamera(wpi::to_string_view(name), vec, kind, status);
}

CS_HttpCameraKind CS_GetHttpCameraKind(CS_Source source, CS_Status* status) {
  return cs::GetHttpCameraKind(source, status);
}

void CS_SetHttpCameraUrls(CS_Source source, const struct WPI_String* urls,
                          int count, CS_Status* status) {
  wpi::SmallVector<std::string, 4> vec;
  vec.reserve(count);
  for (int i = 0; i < count; ++i) {
    vec.emplace_back(wpi::to_string_view(&urls[i]));
  }
  cs::SetHttpCameraUrls(source, vec, status);
}

WPI_String* CS_GetHttpCameraUrls(CS_Source source, int* count,
                                 CS_Status* status) {
  auto urls = cs::GetHttpCameraUrls(source, status);
  WPI_String* out = WPI_AllocateStringArray(urls.size());
  *count = urls.size();
  for (size_t i = 0; i < urls.size(); ++i) {
    cs::ConvertToC(&out[i], urls[i]);
  }
  return out;
}

}  // extern "C"
