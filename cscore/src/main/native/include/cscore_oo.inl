/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_OO_INL_
#define CSCORE_OO_INL_

namespace cs {

inline std::string VideoProperty::GetName() const {
  m_status = 0;
  return GetPropertyName(m_handle, &m_status);
}

inline int VideoProperty::Get() const {
  m_status = 0;
  return GetProperty(m_handle, &m_status);
}

inline void VideoProperty::Set(int value) {
  m_status = 0;
  SetProperty(m_handle, value, &m_status);
}

inline int VideoProperty::GetMin() const {
  m_status = 0;
  return GetPropertyMin(m_handle, &m_status);
}

inline int VideoProperty::GetMax() const {
  m_status = 0;
  return GetPropertyMax(m_handle, &m_status);
}

inline int VideoProperty::GetStep() const {
  m_status = 0;
  return GetPropertyStep(m_handle, &m_status);
}

inline int VideoProperty::GetDefault() const {
  m_status = 0;
  return GetPropertyDefault(m_handle, &m_status);
}

inline std::string VideoProperty::GetString() const {
  m_status = 0;
  return GetStringProperty(m_handle, &m_status);
}

inline wpi::StringRef VideoProperty::GetString(
    wpi::SmallVectorImpl<char>& buf) const {
  m_status = 0;
  return GetStringProperty(m_handle, buf, &m_status);
}

inline void VideoProperty::SetString(const wpi::Twine& value) {
  m_status = 0;
  SetStringProperty(m_handle, value, &m_status);
}

inline std::vector<std::string> VideoProperty::GetChoices() const {
  m_status = 0;
  return GetEnumPropertyChoices(m_handle, &m_status);
}

inline VideoProperty::VideoProperty(CS_Property handle) : m_handle(handle) {
  m_status = 0;
  if (handle == 0)
    m_kind = kNone;
  else
    m_kind =
        static_cast<Kind>(static_cast<int>(GetPropertyKind(handle, &m_status)));
}

inline VideoProperty::VideoProperty(CS_Property handle, Kind kind)
  : m_handle(handle), m_kind(kind) {}

inline VideoSource::VideoSource(const VideoSource& source)
    : m_handle(source.m_handle == 0 ? 0
                                    : CopySource(source.m_handle, &m_status)) {}

inline VideoSource::VideoSource(VideoSource&& other) noexcept : VideoSource() {
  swap(*this, other);
}

inline VideoSource& VideoSource::operator=(VideoSource other) noexcept {
  swap(*this, other);
  return *this;
}

inline VideoSource::~VideoSource() {
  m_status = 0;
  if (m_handle != 0) ReleaseSource(m_handle, &m_status);
}

inline VideoSource::Kind VideoSource::GetKind() const {
  m_status = 0;
  return static_cast<VideoSource::Kind>(GetSourceKind(m_handle, &m_status));
}

inline std::string VideoSource::GetName() const {
  m_status = 0;
  return GetSourceName(m_handle, &m_status);
}

inline std::string VideoSource::GetDescription() const {
  m_status = 0;
  return GetSourceDescription(m_handle, &m_status);
}

inline uint64_t VideoSource::GetLastFrameTime() const {
  m_status = 0;
  return GetSourceLastFrameTime(m_handle, &m_status);
}

inline void VideoSource::SetConnectionStrategy(ConnectionStrategy strategy) {
  m_status = 0;
  SetSourceConnectionStrategy(
      m_handle, static_cast<CS_ConnectionStrategy>(static_cast<int>(strategy)),
      &m_status);
}

inline bool VideoSource::IsConnected() const {
  m_status = 0;
  return IsSourceConnected(m_handle, &m_status);
}

inline bool VideoSource::IsEnabled() const {
  m_status = 0;
  return IsSourceEnabled(m_handle, &m_status);
}

inline VideoProperty VideoSource::GetProperty(const wpi::Twine& name) {
  m_status = 0;
  return VideoProperty{GetSourceProperty(m_handle, name, &m_status)};
}

inline VideoMode VideoSource::GetVideoMode() const {
  m_status = 0;
  return GetSourceVideoMode(m_handle, &m_status);
}

inline bool VideoSource::SetVideoMode(const VideoMode& mode) {
  m_status = 0;
  return SetSourceVideoMode(m_handle, mode, &m_status);
}

inline bool VideoSource::SetVideoMode(VideoMode::PixelFormat pixelFormat,
                                      int width, int height, int fps) {
  m_status = 0;
  return SetSourceVideoMode(
      m_handle, VideoMode{pixelFormat, width, height, fps}, &m_status);
}

inline bool VideoSource::SetPixelFormat(VideoMode::PixelFormat pixelFormat) {
  m_status = 0;
  return SetSourcePixelFormat(m_handle, pixelFormat, &m_status);
}

inline bool VideoSource::SetResolution(int width, int height) {
  m_status = 0;
  return SetSourceResolution(m_handle, width, height, &m_status);
}

inline bool VideoSource::SetFPS(int fps) {
  m_status = 0;
  return SetSourceFPS(m_handle, fps, &m_status);
}

inline bool VideoSource::SetConfigJson(wpi::StringRef config) {
  m_status = 0;
  return SetSourceConfigJson(m_handle, config, &m_status);
}

inline bool VideoSource::SetConfigJson(const wpi::json& config) {
  m_status = 0;
  return SetSourceConfigJson(m_handle, config, &m_status);
}

inline std::string VideoSource::GetConfigJson() const {
  m_status = 0;
  return GetSourceConfigJson(m_handle, &m_status);
}

inline double VideoSource::GetActualFPS() const {
  m_status = 0;
  return cs::GetTelemetryAverageValue(m_handle, CS_SOURCE_FRAMES_RECEIVED,
                                      &m_status);
}

inline double VideoSource::GetActualDataRate() const {
  m_status = 0;
  return cs::GetTelemetryAverageValue(m_handle, CS_SOURCE_BYTES_RECEIVED,
                                      &m_status);
}

inline std::vector<VideoMode> VideoSource::EnumerateVideoModes() const {
  CS_Status status = 0;
  return EnumerateSourceVideoModes(m_handle, &status);
}

inline void VideoCamera::SetBrightness(int brightness) {
  m_status = 0;
  SetCameraBrightness(m_handle, brightness, &m_status);
}

inline int VideoCamera::GetBrightness() {
  m_status = 0;
  return GetCameraBrightness(m_handle, &m_status);
}

inline void VideoCamera::SetWhiteBalanceAuto() {
  m_status = 0;
  SetCameraWhiteBalanceAuto(m_handle, &m_status);
}

inline void VideoCamera::SetWhiteBalanceHoldCurrent() {
  m_status = 0;
  SetCameraWhiteBalanceHoldCurrent(m_handle, &m_status);
}

inline void VideoCamera::SetWhiteBalanceManual(int value) {
  m_status = 0;
  SetCameraWhiteBalanceManual(m_handle, value, &m_status);
}

inline void VideoCamera::SetExposureAuto() {
  m_status = 0;
  SetCameraExposureAuto(m_handle, &m_status);
}

inline void VideoCamera::SetExposureHoldCurrent() {
  m_status = 0;
  SetCameraExposureHoldCurrent(m_handle, &m_status);
}

inline void VideoCamera::SetExposureManual(int value) {
  m_status = 0;
  SetCameraExposureManual(m_handle, value, &m_status);
}

inline UsbCamera::UsbCamera(const wpi::Twine& name, int dev) {
  m_handle = CreateUsbCameraDev(name, dev, &m_status);
}

inline UsbCamera::UsbCamera(const wpi::Twine& name, const wpi::Twine& path) {
  m_handle = CreateUsbCameraPath(name, path, &m_status);
}

inline std::vector<UsbCameraInfo> UsbCamera::EnumerateUsbCameras() {
  CS_Status status = 0;
  return ::cs::EnumerateUsbCameras(&status);
}

inline std::string UsbCamera::GetPath() const {
  m_status = 0;
  return ::cs::GetUsbCameraPath(m_handle, &m_status);
}

inline UsbCameraInfo UsbCamera::GetInfo() const {
  m_status = 0;
  return ::cs::GetUsbCameraInfo(m_handle, &m_status);
}

inline void UsbCamera::SetConnectVerbose(int level) {
  m_status = 0;
  SetProperty(GetSourceProperty(m_handle, "connect_verbose", &m_status), level,
              &m_status);
}

inline HttpCamera::HttpCamera(const wpi::Twine& name, const wpi::Twine& url,
                              HttpCameraKind kind) {
  m_handle = CreateHttpCamera(
      name, url, static_cast<CS_HttpCameraKind>(static_cast<int>(kind)),
      &m_status);
}

inline HttpCamera::HttpCamera(const wpi::Twine& name, const char* url,
                              HttpCameraKind kind) {
  m_handle = CreateHttpCamera(
      name, url, static_cast<CS_HttpCameraKind>(static_cast<int>(kind)),
      &m_status);
}

inline HttpCamera::HttpCamera(const wpi::Twine& name, const std::string& url,
                              HttpCameraKind kind)
    : HttpCamera(name, wpi::Twine{url}, kind) {}

inline HttpCamera::HttpCamera(const wpi::Twine& name,
                              wpi::ArrayRef<std::string> urls,
                              HttpCameraKind kind) {
  m_handle = CreateHttpCamera(
      name, urls, static_cast<CS_HttpCameraKind>(static_cast<int>(kind)),
      &m_status);
}

template <typename T>
inline HttpCamera::HttpCamera(const wpi::Twine& name,
                              std::initializer_list<T> urls,
                              HttpCameraKind kind) {
  std::vector<std::string> vec;
  vec.reserve(urls.size());
  for (const auto& url : urls) vec.emplace_back(url);
  m_handle = CreateHttpCamera(
      name, vec, static_cast<CS_HttpCameraKind>(static_cast<int>(kind)),
      &m_status);
}

inline HttpCamera::HttpCameraKind HttpCamera::GetHttpCameraKind() const {
  m_status = 0;
  return static_cast<HttpCameraKind>(
      static_cast<int>(::cs::GetHttpCameraKind(m_handle, &m_status)));
}

inline void HttpCamera::SetUrls(wpi::ArrayRef<std::string> urls) {
  m_status = 0;
  ::cs::SetHttpCameraUrls(m_handle, urls, &m_status);
}

template <typename T>
inline void HttpCamera::SetUrls(std::initializer_list<T> urls) {
  std::vector<std::string> vec;
  vec.reserve(urls.size());
  for (const auto& url : urls) vec.emplace_back(url);
  m_status = 0;
  ::cs::SetHttpCameraUrls(m_handle, vec, &m_status);
}

inline std::vector<std::string> HttpCamera::GetUrls() const {
  m_status = 0;
  return ::cs::GetHttpCameraUrls(m_handle, &m_status);
}

inline std::string AxisCamera::HostToUrl(const wpi::Twine& host) {
  return ("http://" + host + "/mjpg/video.mjpg").str();
}

inline std::vector<std::string> AxisCamera::HostToUrl(
    wpi::ArrayRef<std::string> hosts) {
  std::vector<std::string> rv;
  rv.reserve(hosts.size());
  for (const auto& host : hosts)
    rv.emplace_back(HostToUrl(wpi::StringRef{host}));
  return rv;
}

template <typename T>
inline std::vector<std::string> AxisCamera::HostToUrl(
    std::initializer_list<T> hosts) {
  std::vector<std::string> rv;
  rv.reserve(hosts.size());
  for (const auto& host : hosts)
    rv.emplace_back(HostToUrl(wpi::StringRef{host}));
  return rv;
}

inline AxisCamera::AxisCamera(const wpi::Twine& name, const wpi::Twine& host)
    : HttpCamera(name, HostToUrl(host), kAxis) {}

inline AxisCamera::AxisCamera(const wpi::Twine& name, const char* host)
    : HttpCamera(name, HostToUrl(host), kAxis) {}

inline AxisCamera::AxisCamera(const wpi::Twine& name, const std::string& host)
    : HttpCamera(name, HostToUrl(wpi::Twine{host}), kAxis) {}

inline AxisCamera::AxisCamera(const wpi::Twine& name, wpi::StringRef host)
    : HttpCamera(name, HostToUrl(host), kAxis) {}

inline AxisCamera::AxisCamera(const wpi::Twine& name,
                              wpi::ArrayRef<std::string> hosts)
    : HttpCamera(name, HostToUrl(hosts), kAxis) {}

template <typename T>
inline AxisCamera::AxisCamera(const wpi::Twine& name,
                              std::initializer_list<T> hosts)
    : HttpCamera(name, HostToUrl(hosts), kAxis) {}

inline CvSource::CvSource(const wpi::Twine& name, const VideoMode& mode) {
  m_handle = CreateCvSource(name, mode, &m_status);
}

inline CvSource::CvSource(const wpi::Twine& name, VideoMode::PixelFormat format,
                          int width, int height, int fps) {
  m_handle =
      CreateCvSource(name, VideoMode{format, width, height, fps}, &m_status);
}

inline void CvSource::PutFrame(cv::Mat& image) {
  m_status = 0;
  PutSourceFrame(m_handle, image, &m_status);
}

inline void CvSource::NotifyError(const wpi::Twine& msg) {
  m_status = 0;
  NotifySourceError(m_handle, msg, &m_status);
}

inline void CvSource::SetConnected(bool connected) {
  m_status = 0;
  SetSourceConnected(m_handle, connected, &m_status);
}

inline void CvSource::SetDescription(const wpi::Twine& description) {
  m_status = 0;
  SetSourceDescription(m_handle, description, &m_status);
}

inline VideoProperty CvSource::CreateProperty(const wpi::Twine& name,
                                              VideoProperty::Kind kind,
                                              int minimum, int maximum,
                                              int step, int defaultValue,
                                              int value) {
  m_status = 0;
  return VideoProperty{CreateSourceProperty(
      m_handle, name, static_cast<CS_PropertyKind>(static_cast<int>(kind)),
      minimum, maximum, step, defaultValue, value, &m_status)};
}

inline VideoProperty CvSource::CreateIntegerProperty(const wpi::Twine& name,
                                                    int minimum, int maximum,
                                                    int step, int defaultValue,
                                                    int value) {
  m_status = 0;
  return VideoProperty{CreateSourceProperty(
      m_handle, name, static_cast<CS_PropertyKind>(static_cast<int>(VideoProperty::Kind::kInteger)),
      minimum, maximum, step, defaultValue, value, &m_status)};
}

inline VideoProperty CvSource::CreateBooleanProperty(const wpi::Twine& name,
                                                     bool defaultValue,
                                                     bool value) {
  m_status = 0;
  return VideoProperty{CreateSourceProperty(
      m_handle, name, static_cast<CS_PropertyKind>(static_cast<int>(VideoProperty::Kind::kBoolean)),
      0, 1, 1, defaultValue ? 1 : 0, value ? 1 : 0, &m_status)};
}

inline VideoProperty CvSource::CreateStringProperty(const wpi::Twine& name,
                                                    const wpi::Twine& value) {
  m_status = 0;
  auto prop = VideoProperty{CreateSourceProperty(
      m_handle, name, static_cast<CS_PropertyKind>(static_cast<int>(VideoProperty::Kind::kString)),
      0, 0, 0, 0, 0, &m_status)};
  prop.SetString(value);
  return prop;
}


inline void CvSource::SetEnumPropertyChoices(
    const VideoProperty& property, wpi::ArrayRef<std::string> choices) {
  m_status = 0;
  SetSourceEnumPropertyChoices(m_handle, property.m_handle, choices, &m_status);
}

template <typename T>
inline void CvSource::SetEnumPropertyChoices(const VideoProperty& property,
                                             std::initializer_list<T> choices) {
  std::vector<std::string> vec;
  vec.reserve(choices.size());
  for (const auto& choice : choices) vec.emplace_back(choice);
  m_status = 0;
  SetSourceEnumPropertyChoices(m_handle, property.m_handle, vec, &m_status);
}

inline VideoSink::VideoSink(const VideoSink& sink)
    : m_handle(sink.m_handle == 0 ? 0 : CopySink(sink.m_handle, &m_status)) {}

inline VideoSink::VideoSink(VideoSink&& other) noexcept : VideoSink() {
  swap(*this, other);
}

inline VideoSink& VideoSink::operator=(VideoSink other) noexcept {
  swap(*this, other);
  return *this;
}

inline VideoSink::~VideoSink() {
  m_status = 0;
  if (m_handle != 0) ReleaseSink(m_handle, &m_status);
}

inline VideoSink::Kind VideoSink::GetKind() const {
  m_status = 0;
  return static_cast<VideoSink::Kind>(GetSinkKind(m_handle, &m_status));
}

inline std::string VideoSink::GetName() const {
  m_status = 0;
  return GetSinkName(m_handle, &m_status);
}

inline std::string VideoSink::GetDescription() const {
  m_status = 0;
  return GetSinkDescription(m_handle, &m_status);
}

inline VideoProperty VideoSink::GetProperty(const wpi::Twine& name) {
  m_status = 0;
  return VideoProperty{GetSinkProperty(m_handle, name, &m_status)};
}

inline void VideoSink::SetSource(VideoSource source) {
  m_status = 0;
  if (!source)
    SetSinkSource(m_handle, 0, &m_status);
  else
    SetSinkSource(m_handle, source.m_handle, &m_status);
}

inline VideoSource VideoSink::GetSource() const {
  m_status = 0;
  auto handle = GetSinkSource(m_handle, &m_status);
  return VideoSource{handle == 0 ? 0 : CopySource(handle, &m_status)};
}

inline VideoProperty VideoSink::GetSourceProperty(const wpi::Twine& name) {
  m_status = 0;
  return VideoProperty{GetSinkSourceProperty(m_handle, name, &m_status)};
}

inline bool VideoSink::SetConfigJson(wpi::StringRef config) {
  m_status = 0;
  return SetSinkConfigJson(m_handle, config, &m_status);
}

inline bool VideoSink::SetConfigJson(const wpi::json& config) {
  m_status = 0;
  return SetSinkConfigJson(m_handle, config, &m_status);
}

inline std::string VideoSink::GetConfigJson() const {
  m_status = 0;
  return GetSinkConfigJson(m_handle, &m_status);
}

inline MjpegServer::MjpegServer(const wpi::Twine& name,
                                const wpi::Twine& listenAddress, int port) {
  m_handle = CreateMjpegServer(name, listenAddress, port, &m_status);
}

inline std::string MjpegServer::GetListenAddress() const {
  m_status = 0;
  return cs::GetMjpegServerListenAddress(m_handle, &m_status);
}

inline int MjpegServer::GetPort() const {
  m_status = 0;
  return cs::GetMjpegServerPort(m_handle, &m_status);
}

inline void MjpegServer::SetResolution(int width, int height) {
  m_status = 0;
  SetProperty(GetSinkProperty(m_handle, "width", &m_status), width, &m_status);
  SetProperty(GetSinkProperty(m_handle, "height", &m_status), height,
              &m_status);
}

inline void MjpegServer::SetFPS(int fps) {
  m_status = 0;
  SetProperty(GetSinkProperty(m_handle, "fps", &m_status), fps, &m_status);
}

inline void MjpegServer::SetCompression(int quality) {
  m_status = 0;
  SetProperty(GetSinkProperty(m_handle, "compression", &m_status), quality,
              &m_status);
}

inline void MjpegServer::SetDefaultCompression(int quality) {
  m_status = 0;
  SetProperty(GetSinkProperty(m_handle, "default_compression", &m_status),
              quality, &m_status);
}

inline CvSink::CvSink(const wpi::Twine& name) {
  m_handle = CreateCvSink(name, &m_status);
}

inline CvSink::CvSink(const wpi::Twine& name,
                      std::function<void(uint64_t time)> processFrame) {
  m_handle = CreateCvSinkCallback(name, processFrame, &m_status);
}

inline void CvSink::SetDescription(const wpi::Twine& description) {
  m_status = 0;
  SetSinkDescription(m_handle, description, &m_status);
}

inline uint64_t CvSink::GrabFrame(cv::Mat& image, double timeout) const {
  m_status = 0;
  return GrabSinkFrameTimeout(m_handle, image, timeout, &m_status);
}

inline uint64_t CvSink::GrabFrameNoTimeout(cv::Mat& image) const {
  m_status = 0;
  return GrabSinkFrame(m_handle, image, &m_status);
}

inline std::string CvSink::GetError() const {
  m_status = 0;
  return GetSinkError(m_handle, &m_status);
}

inline void CvSink::SetEnabled(bool enabled) {
  m_status = 0;
  SetSinkEnabled(m_handle, enabled, &m_status);
}

inline VideoSource VideoEvent::GetSource() const {
  CS_Status status = 0;
  return VideoSource{sourceHandle == 0 ? 0 : CopySource(sourceHandle, &status)};
}

inline VideoSink VideoEvent::GetSink() const {
  CS_Status status = 0;
  return VideoSink{sinkHandle == 0 ? 0 : CopySink(sinkHandle, &status)};
}

inline VideoProperty VideoEvent::GetProperty() const {
  return VideoProperty{propertyHandle,
                       static_cast<VideoProperty::Kind>(propertyKind)};
}

inline VideoListener::VideoListener(
    std::function<void(const VideoEvent& event)> callback, int eventMask,
    bool immediateNotify) {
  CS_Status status = 0;
  m_handle = AddListener(
      [=](const RawEvent& event) {
        callback(static_cast<const VideoEvent&>(event));
      },
      eventMask, immediateNotify, &status);
}

inline VideoListener::VideoListener(VideoListener&& other) noexcept
    : VideoListener() {
  swap(*this, other);
}

inline VideoListener& VideoListener::operator=(VideoListener&& other) noexcept {
  swap(*this, other);
  return *this;
}

inline VideoListener::~VideoListener() {
  CS_Status status = 0;
  if (m_handle != 0) RemoveListener(m_handle, &status);
}

}  // namespace cs

#endif  /* CSCORE_OO_INL_ */
