/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SourceImpl.h"

#include <algorithm>
#include <cstring>

#include "Log.h"

using namespace cs;

static constexpr std::size_t kMaxFramesAvail = 32;

// DHT data for MJPEG images that don't have it.
static const unsigned char dhtData[] = {
    0xff, 0xc4, 0x01, 0xa2, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
    0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x01, 0x00, 0x03,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05,
    0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04,
    0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22,
    0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15,
    0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
    0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
    0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95,
    0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
    0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2,
    0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5,
    0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
    0xfa, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05,
    0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04,
    0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22,
    0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33,
    0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25,
    0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
    0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94,
    0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
    0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};

SourceImpl::SourceImpl(llvm::StringRef name)
    : m_name{name}, m_frame{*this, nullptr} {}

SourceImpl::~SourceImpl() {
  // Wake up anyone who is waiting.  This also clears the current frame,
  // which is good because its destructor will call back into the class.
  Wakeup();
  // Set a flag so ReleaseFrame() doesn't re-add them to m_framesAvail.
  // Put in a block so we destroy before the destructor ends.
  {
    m_destroyFrames = true;
    auto frames = std::move(m_framesAvail);
  }
  // Everything else can clean up itself.
}

void SourceImpl::SetDescription(llvm::StringRef description) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_description = description;
}

llvm::StringRef SourceImpl::GetDescription(
    llvm::SmallVectorImpl<char>& buf) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  buf.append(m_description.begin(), m_description.end());
  return llvm::StringRef{buf.data(), buf.size()};
}

uint64_t SourceImpl::GetCurFrameTime() {
  std::unique_lock<std::mutex> lock{m_frameMutex};
  return m_frame.time();
}

Frame SourceImpl::GetCurFrame() {
  std::unique_lock<std::mutex> lock{m_frameMutex};
  return m_frame;
}

Frame SourceImpl::GetNextFrame() {
  std::unique_lock<std::mutex> lock{m_frameMutex};
  auto oldTime = m_frame.time();
  m_frameCv.wait(lock, [=] { return m_frame.time() != oldTime; });
  return m_frame;
}

void SourceImpl::Wakeup() {
  {
    std::lock_guard<std::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, nullptr};
  }
  m_frameCv.notify_all();
}

int SourceImpl::GetPropertyIndex(llvm::StringRef name) const {
  // We can't fail, so instead we create a new index if caching fails.
  CS_Status status = 0;
  if (!m_properties_cached) CacheProperties(&status);
  std::lock_guard<std::mutex> lock(m_mutex);
  int& ndx = m_properties[name];
  if (ndx == 0) {
    // create a new index
    ndx = m_propertyData.size() + 1;
    m_propertyData.emplace_back();
  }
  return ndx;
}

llvm::ArrayRef<int> SourceImpl::EnumerateProperties(
    llvm::SmallVectorImpl<int>& vec, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return llvm::ArrayRef<int>{};
  std::lock_guard<std::mutex> lock(m_mutex);
  for (int i = 0; i < static_cast<int>(m_propertyData.size()); ++i)
    vec.push_back(i + 1);
  return vec;
}

CS_PropertyType SourceImpl::GetPropertyType(int property) const {
  CS_Status status = 0;
  if (!m_properties_cached && !CacheProperties(&status)) return CS_PROP_NONE;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) return CS_PROP_NONE;
  return prop->propType;
}

llvm::StringRef SourceImpl::GetPropertyName(int property,
                                            llvm::SmallVectorImpl<char>& buf,
                                            CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return llvm::StringRef{};
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return llvm::StringRef{};
  }
  // safe to not copy because we never modify it after caching
  return prop->name;
}

int SourceImpl::GetProperty(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  if ((prop->propType & (CS_PROP_BOOLEAN | CS_PROP_INTEGER | CS_PROP_ENUM)) ==
      0) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return 0;
  }
  return prop->value;
}

int SourceImpl::GetPropertyMin(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->minimum;
}

int SourceImpl::GetPropertyMax(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->maximum;
}

int SourceImpl::GetPropertyStep(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->step;
}

int SourceImpl::GetPropertyDefault(int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status)) return 0;
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return 0;
  }
  return prop->defaultValue;
}

llvm::StringRef SourceImpl::GetStringProperty(
    int property, llvm::SmallVectorImpl<char>& buf, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return llvm::StringRef{};
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return llvm::StringRef{};
  }
  if (prop->propType != CS_PROP_STRING) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return llvm::StringRef{};
  }
  buf.clear();
  buf.append(prop->valueStr.begin(), prop->valueStr.end());
  return llvm::StringRef(buf.data(), buf.size());
}

std::vector<std::string> SourceImpl::GetEnumPropertyChoices(
    int property, CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return std::vector<std::string>{};
  std::lock_guard<std::mutex> lock(m_mutex);
  auto prop = GetProperty(property);
  if (!prop) {
    *status = CS_INVALID_PROPERTY;
    return std::vector<std::string>{};
  }
  if (prop->propType != CS_PROP_ENUM) {
    *status = CS_WRONG_PROPERTY_TYPE;
    return std::vector<std::string>{};
  }
  return prop->enumChoices;
}

VideoMode SourceImpl::GetVideoMode(CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return VideoMode{};
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_mode;
}

bool SourceImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                                CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) return false;
  mode.pixelFormat = pixelFormat;
  return SetVideoMode(mode, status);
}

bool SourceImpl::SetResolution(int width, int height, CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) return false;
  mode.width = width;
  mode.height = height;
  return SetVideoMode(mode, status);
}

bool SourceImpl::SetFPS(int fps, CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) return false;
  mode.fps = fps;
  return SetVideoMode(mode, status);
}

std::vector<VideoMode> SourceImpl::EnumerateVideoModes(
    CS_Status* status) const {
  if (!m_properties_cached && !CacheProperties(status))
    return std::vector<VideoMode>{};
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_videoModes;
}

void SourceImpl::PutFrame(VideoMode::PixelFormat pixelFormat, int width,
                          int height, llvm::StringRef data, Frame::Time time) {
  std::size_t dataSize = data.size();

  // If MJPEG, determine if we need to add DHT to it, and allocate enough space
  // for adding it if required.
  bool addDHT = false;
  std::size_t locSOF = dataSize;
  if (pixelFormat == VideoMode::kMJPEG) {
    // Search first 2048 bytes (or until SOS) for DHT tag
    for (llvm::StringRef::size_type i = 0; i < 2048 && i < data.size(); ++i) {
      if (static_cast<unsigned char>(data[i]) != 0xff) continue;  // not a tag
      unsigned char tag = static_cast<unsigned char>(data[i + 1]);
      if (tag == 0xda) break;       // SOS
      if (tag == 0xc4) goto done;   // DHT
      if (tag == 0xc0) locSOF = i;  // SOF
    }
    // Only add DHT if we also found SOF (insertion point)
    if (locSOF != dataSize) {
      addDHT = true;
      dataSize += sizeof(dhtData);
    }
  }
done:

  std::unique_ptr<Frame::Data> frameData;
  {
    std::lock_guard<std::mutex> lock{m_poolMutex};
    // find the smallest existing frame that is at least big enough.
    int found = -1;
    for (std::size_t i = 0; i < m_framesAvail.size(); ++i) {
      // is it big enough?
      if (m_framesAvail[i] && m_framesAvail[i]->capacity >= dataSize) {
        // is it smaller than the last found?
        if (found < 0 ||
            m_framesAvail[i]->capacity < m_framesAvail[found]->capacity) {
          // yes, update
          found = i;
        }
      }
    }

    // if nothing found, allocate a new buffer
    if (found < 0)
      frameData.reset(new Frame::Data{dataSize});
    else
      frameData = std::move(m_framesAvail[found]);
  }

  // Initialize frame data
  frameData->refcount = 0;
  frameData->time = time;
  frameData->size = dataSize;
  frameData->pixelFormat = pixelFormat;
  frameData->width = width;
  frameData->height = height;

  // Copy in image data
  DEBUG4("Copying data to " << ((void*)frameData->data) << " from "
                            << ((void*)data.data()) << " (" << data.size()
                            << " bytes)" << (addDHT ? ", adding DHT" : ""));
  if (addDHT) {
    // Insert DHT data immediately before SOF
    std::memcpy(frameData->data, data.data(), locSOF);
    std::memcpy(frameData->data + locSOF, dhtData, sizeof(dhtData));
    std::memcpy(frameData->data + locSOF + sizeof(dhtData),
                data.data() + locSOF, data.size() - locSOF);
  } else {
    std::memcpy(frameData->data, data.data(), data.size());
  }

  // Update frame
  {
    std::lock_guard<std::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, std::move(frameData)};
  }

  // Signal listeners
  m_frameCv.notify_all();
}

void SourceImpl::ReleaseFrame(std::unique_ptr<Frame::Data> data) {
  std::lock_guard<std::mutex> lock{m_poolMutex};
  if (m_destroyFrames) return;
  // Return the frame to the pool.  First try to find an empty slot, otherwise
  // add it to the end.
  auto it = std::find(m_framesAvail.begin(), m_framesAvail.end(), nullptr);
  if (it != m_framesAvail.end())
    (*it) = std::move(data);
  else if (m_framesAvail.size() > kMaxFramesAvail) {
    // Replace smallest buffer; don't need to check for null because the above
    // find would have found it.
    auto it2 = std::min_element(m_framesAvail.begin(), m_framesAvail.end(),
                                [](const std::unique_ptr<Frame::Data>& a,
                                   const std::unique_ptr<Frame::Data>& b) {
                                  return a->capacity < b->capacity;
                                });
    if ((*it2)->capacity < data->capacity)
      *it2 = std::move(data);
  } else
    m_framesAvail.emplace_back(std::move(data));
}
