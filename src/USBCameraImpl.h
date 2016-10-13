/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_USBCAMERAIMPL_H_
#define CAMERASERVER_USBCAMERAIMPL_H_

#include <atomic>
#include <thread>
#include <vector>

#ifdef __linux__
#include <linux/videodev2.h>
#endif

#include "llvm/raw_ostream.h"
#include "llvm/SmallVector.h"
#include "llvm/StringMap.h"
#include "llvm/STLExtras.h"
#include "support/raw_istream.h"

#include "SourceImpl.h"
#include "USBCameraBuffer.h"

namespace cs {

class USBCameraImpl : public SourceImpl {
 public:
  USBCameraImpl(llvm::StringRef name, llvm::StringRef path);
  ~USBCameraImpl() override;

  llvm::StringRef GetDescription(
      llvm::SmallVectorImpl<char>& buf) const override;
  bool IsConnected() const override;

  // Property functions
  int GetPropertyIndex(llvm::StringRef name) const override;
  llvm::ArrayRef<int> EnumerateProperties(llvm::SmallVectorImpl<int>& vec,
                                          CS_Status* status) const override;
  CS_PropertyType GetPropertyType(int property) const override;
  llvm::StringRef GetPropertyName(int property,
                                  llvm::SmallVectorImpl<char>& buf,
                                  CS_Status* status) const override;
  int GetProperty(int property, CS_Status* status) const override;
  void SetProperty(int property, int value, CS_Status* status) override;
  int GetPropertyMin(int property, CS_Status* status) const override;
  int GetPropertyMax(int property, CS_Status* status) const override;
  int GetPropertyStep(int property, CS_Status* status) const override;
  int GetPropertyDefault(int property, CS_Status* status) const override;
  llvm::StringRef GetStringProperty(int property,
                                    llvm::SmallVectorImpl<char>& buf,
                                    CS_Status* status) const override;
  void SetStringProperty(int property, llvm::StringRef value,
                         CS_Status* status) override;
  std::vector<std::string> GetEnumPropertyChoices(
      int property, CS_Status* status) const override;

  VideoMode GetVideoMode(CS_Status* status) const override;
  bool SetVideoMode(const VideoMode& mode, CS_Status* status) override;
  bool SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                      CS_Status* status) override;
  bool SetResolution(int width, int height, CS_Status* status) override;
  bool SetFPS(int fps, CS_Status* status) override;
  std::vector<VideoMode> EnumerateVideoModes(CS_Status* status) const override;

  void NumSinksChanged() override;
  void NumSinksEnabledChanged() override;

  // Property data
  struct PropertyData {
    PropertyData() = default;
#ifdef __linux__
#ifdef VIDIOC_QUERY_EXT_CTRL
    PropertyData(const struct v4l2_query_ext_ctrl& ctrl);
#endif
    PropertyData(const struct v4l2_queryctrl& ctrl);
#endif

    std::string name;
    unsigned id;  // implementation-level id
    int type;  // implementation type, not CS_PropertyType!
    CS_PropertyType propType{CS_PROP_NONE};
    int minimum;
    int maximum;
    int step;
    int defaultValue;
    int value{0};
    std::string valueStr;
    std::vector<std::string> enumChoices;
    bool valueSet{false};
  };

  // Messages passed to/from camera thread
  struct Message {
    enum Type {
      kNone = 0,
      kCmdSetMode,
      kCmdSetPixelFormat,
      kCmdSetResolution,
      kCmdSetFPS,
      kCmdSetProperty,
      kCmdSetPropertyStr,
      kNumSinksChanged,         // no response
      kNumSinksEnabledChanged,  // no response
      // Responses
      kOk,
      kError
    };

    Type type;
    int data[4];
    std::string dataStr;
  };

 private:
  // Message pool access
  std::unique_ptr<Message> CreateMessage(Message::Type type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_messagePool.empty()) return llvm::make_unique<Message>();
    auto rv = std::move(m_messagePool.back());
    m_messagePool.pop_back();
    rv->type = type;
    return rv;
  }
  void DestroyMessage(std::unique_ptr<Message> message) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_messagePool.emplace_back(std::move(message));
  }

  // Send a message to the camera thread and wait for a response (generic)
  std::unique_ptr<Message> SendAndWait(std::unique_ptr<Message> msg) const;
  // Send a message to the camera thread with no response
  void Send(std::unique_ptr<Message> msg) const;

  // Cache properties.  Immediately successful if properties are already cached.
  // If they are not, tries to connect to the camera to do so; returns false and
  // sets status to CS_SOURCE_IS_DISCONNECTED if that too fails.
  bool CacheProperties(CS_Status* status) const;

  // The camera processing thread
  void CameraThreadMain();

  // Functions used by CameraThreadMain()
  void DeviceDisconnect();
  void DeviceConnect();
  bool DeviceStreamOn();
  bool DeviceStreamOff();
  void DeviceProcessCommands();
  void DeviceSetFPS();
  void DeviceCacheMode();
  void DeviceCacheProperty(PropertyData&& prop);
  void DeviceCacheProperties();
  void DeviceCacheVideoModes();
  bool DeviceGetProperty(PropertyData* prop);
  bool DeviceSetProperty(std::unique_lock<std::mutex>& lock,
                         const PropertyData& prop);

  //
  // Variables only used within camera thread
  //
  bool m_streaming;
  bool m_modeSetPixelFormat{false};
  bool m_modeSetResolution{false};
  bool m_modeSetFPS{false};
#ifdef __linux__
  unsigned m_capabilities = 0;
#endif
  // Number of buffers to ask OS for
  static constexpr int kNumBuffers = 4;
  std::array<USBCameraBuffer, kNumBuffers> m_buffers;

  //
  // Path and description: These never change, so not protected by mutex.
  //
  std::string m_path;
  std::string m_description;

#ifdef __linux__
  std::atomic_int m_fd;
  std::atomic_int m_command_fd;  // for command eventfd
#endif

  std::atomic_bool m_active;  // set to false to terminate thread
  std::thread m_cameraThread;

  //
  // Variables protected by m_mutex
  //

  // Cached camera information (properties and video modes)
  mutable std::vector<PropertyData> m_propertyData;
  mutable llvm::StringMap<int> m_properties;
  std::vector<VideoMode> m_videoModes;
  std::atomic_bool m_properties_cached{false};

  // Get a property; must be called with m_mutex held.
  PropertyData* GetProperty(int property) {
    if (property <= 0 || static_cast<size_t>(property) > m_propertyData.size())
      return nullptr;
    return &m_propertyData[property - 1];
  }
  const PropertyData* GetProperty(int property) const {
    if (property <= 0 || static_cast<size_t>(property) > m_propertyData.size())
      return nullptr;
    return &m_propertyData[property - 1];
  }

  // Current video mode (updated by camera thread)
  VideoMode m_mode;

  // Message pool and queues
  mutable std::vector<std::unique_ptr<Message>> m_messagePool;
  mutable std::vector<std::unique_ptr<Message>> m_commands;
  mutable std::vector<std::unique_ptr<Message>> m_responses;
  mutable std::condition_variable m_responseCv;

  mutable std::mutex m_mutex;
};

}  // namespace cs

#endif  // CAMERASERVER_USBCAMERAIMPL_H_
