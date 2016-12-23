/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_USBCAMERAIMPL_H_
#define CS_USBCAMERAIMPL_H_

#include <atomic>
#include <thread>
#include <vector>

#ifdef __linux__
#include <linux/videodev2.h>
#endif

#include "llvm/raw_ostream.h"
#include "llvm/SmallVector.h"
#include "llvm/STLExtras.h"
#include "support/raw_istream.h"

#include "SourceImpl.h"
#include "UsbCameraBuffer.h"

namespace cs {

class UsbCameraImpl : public SourceImpl {
 public:
  UsbCameraImpl(llvm::StringRef name, llvm::StringRef path);
  ~UsbCameraImpl() override;

  void Start();

  // Property functions
  void SetProperty(int property, int value, CS_Status* status) override;
  void SetStringProperty(int property, llvm::StringRef value,
                         CS_Status* status) override;

  bool SetVideoMode(const VideoMode& mode, CS_Status* status) override;
  bool SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                      CS_Status* status) override;
  bool SetResolution(int width, int height, CS_Status* status) override;
  bool SetFPS(int fps, CS_Status* status) override;

  void NumSinksChanged() override;
  void NumSinksEnabledChanged() override;

  std::string GetPath() { return m_path; }

  // Property data
  class PropertyData : public PropertyImpl {
   public:
    PropertyData() = default;
    PropertyData(llvm::StringRef name_) : PropertyImpl{name_} {}

    // Normalized property constructor
    PropertyData(llvm::StringRef name_, int rawIndex_,
                 const PropertyData& rawProp, int defaultValue_, int value_)
        : PropertyImpl(name_, rawProp.propKind, 1, defaultValue_, value_),
          percentage{true},
          propPair{rawIndex_},
          id{rawProp.id},
          type{rawProp.type} {
      hasMinimum = true;
      minimum = 0;
      hasMaximum = true;
      maximum = 100;
    }

#ifdef __linux__
#ifdef VIDIOC_QUERY_EXT_CTRL
    PropertyData(const struct v4l2_query_ext_ctrl& ctrl);
#endif
    PropertyData(const struct v4l2_queryctrl& ctrl);
#endif

    // If this is a percentage (rather than raw) property
    bool percentage{false};

    // If not 0, index of corresponding raw/percentage property
    int propPair{0};

    unsigned id{0};  // implementation-level id
    int type{0};  // implementation type, not CS_PropertyKind!
  };

  // Messages passed to/from camera thread
  struct Message {
    enum Kind {
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

    Message(Kind kind_) : kind(kind_) {}

    Kind kind;
    int data[4];
    std::string dataStr;
  };

 protected:
  std::unique_ptr<PropertyImpl> CreateEmptyProperty(
      llvm::StringRef name) const override;

  // Cache properties.  Immediately successful if properties are already cached.
  // If they are not, tries to connect to the camera to do so; returns false and
  // sets status to CS_SOURCE_IS_DISCONNECTED if that too fails.
  bool CacheProperties(CS_Status* status) const override;

 private:
  // Message pool access
  std::unique_ptr<Message> CreateMessage(Message::Kind kind) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_messagePool.empty()) return llvm::make_unique<Message>(kind);
    auto rv = std::move(m_messagePool.back());
    m_messagePool.pop_back();
    rv->kind = kind;
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

  // The camera processing thread
  void CameraThreadMain();

  // Functions used by CameraThreadMain()
  void DeviceDisconnect();
  void DeviceConnect();
  bool DeviceStreamOn();
  bool DeviceStreamOff();
  void DeviceProcessCommands();
  void DeviceSetMode();
  void DeviceSetFPS();
  void DeviceCacheMode();
  void DeviceCacheProperty(std::unique_ptr<PropertyData> rawProp);
  void DeviceCacheProperties();
  void DeviceCacheVideoModes();
  bool DeviceGetProperty(PropertyData* prop);
  bool DeviceSetProperty(std::unique_lock<std::mutex>& lock,
                         const PropertyData& prop);
  bool DeviceSetProperty(std::unique_lock<std::mutex>& lock,
                         const PropertyData& prop, int value,
                         llvm::StringRef valueStr);

  // Command helper functions
  CS_StatusValue DeviceCmdSetProperty(std::unique_lock<std::mutex>& lock,
                                      int property, bool setString, int value,
                                      llvm::StringRef valueStr);

  // Property helper functions
  int RawToPercentage(const PropertyData& rawProp, int rawValue);
  int PercentageToRaw(const PropertyData& rawProp, int percentValue);

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
  std::array<UsbCameraBuffer, kNumBuffers> m_buffers;

  //
  // Path never changes, so not protected by mutex.
  //
  std::string m_path;

#ifdef __linux__
  std::atomic_int m_fd;
  std::atomic_int m_command_fd;  // for command eventfd
#endif

  std::atomic_bool m_active;  // set to false to terminate thread
  std::thread m_cameraThread;

  //
  // Variables protected by m_mutex
  //

  // Message pool and queues
  mutable std::vector<std::unique_ptr<Message>> m_messagePool;
  mutable std::vector<std::unique_ptr<Message>> m_commands;
  mutable std::vector<std::unique_ptr<Message>> m_responses;
  mutable std::condition_variable m_responseCv;
};

}  // namespace cs

#endif  // CS_USBCAMERAIMPL_H_
