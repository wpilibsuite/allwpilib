// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_NOTIFIER_H_
#define CSCORE_NOTIFIER_H_

#include <functional>
#include <utility>

#include <wpi/CallbackManager.h>

#include "Handle.h"
#include "cscore_cpp.h"

namespace cs {

class SinkImpl;
class SourceImpl;

namespace impl {

struct ListenerData : public wpi::CallbackListenerData<
                          std::function<void(const RawEvent& event)>> {
  ListenerData() = default;
  ListenerData(std::function<void(const RawEvent& event)> callback_,
               int eventMask_)
      : CallbackListenerData(std::move(callback_)), eventMask(eventMask_) {}
  ListenerData(unsigned int pollerUid_, int eventMask_)
      : CallbackListenerData(pollerUid_), eventMask(eventMask_) {}

  int eventMask;
};

class NotifierThread
    : public wpi::CallbackThread<NotifierThread, RawEvent, ListenerData> {
 public:
  NotifierThread(std::function<void()> on_start, std::function<void()> on_exit)
      : CallbackThread(std::move(on_start), std::move(on_exit)) {}

  bool Matches(const ListenerData& listener, const RawEvent& data) {
    return (data.kind & listener.eventMask) != 0;
  }

  void SetListener(RawEvent* data, unsigned int listener_uid) {
    data->listener = Handle(listener_uid, Handle::kListener);
  }

  void DoCallback(std::function<void(const RawEvent& event)> callback,
                  const RawEvent& data) {
    callback(data);
  }
};

}  // namespace impl

class Notifier : public wpi::CallbackManager<Notifier, impl::NotifierThread> {
  friend class NotifierTest;

 public:
  Notifier();
  ~Notifier();

  void Start();

  unsigned int Add(std::function<void(const RawEvent& event)> callback,
                   int eventMask);
  unsigned int AddPolled(unsigned int pollerUid, int eventMask);

  // Notification events
  void NotifySource(std::string_view name, CS_Source source, CS_EventKind kind);
  void NotifySource(const SourceImpl& source, CS_EventKind kind);
  void NotifySourceVideoMode(const SourceImpl& source, const VideoMode& mode);
  void NotifySourceProperty(const SourceImpl& source, CS_EventKind kind,
                            std::string_view propertyName, int property,
                            CS_PropertyKind propertyKind, int value,
                            std::string_view valueStr);
  void NotifySink(std::string_view name, CS_Sink sink, CS_EventKind kind);
  void NotifySink(const SinkImpl& sink, CS_EventKind kind);
  void NotifySinkSourceChanged(std::string_view name, CS_Sink sink,
                               CS_Source source);
  void NotifySinkProperty(const SinkImpl& sink, CS_EventKind kind,
                          std::string_view propertyName, int property,
                          CS_PropertyKind propertyKind, int value,
                          std::string_view valueStr);
  void NotifyNetworkInterfacesChanged();
  void NotifyTelemetryUpdated();
  void NotifyUsbCamerasChanged();
};

}  // namespace cs

#endif  // CSCORE_NOTIFIER_H_
