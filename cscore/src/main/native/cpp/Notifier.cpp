// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Notifier.h"

#include <utility>

#include "Handle.h"
#include "Instance.h"
#include "SinkImpl.h"
#include "SourceImpl.h"

using namespace cs;

Notifier::Notifier() {}

Notifier::~Notifier() {}

void Notifier::Start() {
  DoStart();
}

unsigned int Notifier::Add(std::function<void(const RawEvent& event)> callback,
                           int eventMask) {
  return DoAdd(callback, eventMask);
}

unsigned int Notifier::AddPolled(unsigned int pollerUid, int eventMask) {
  return DoAdd(pollerUid, eventMask);
}

void Notifier::NotifySource(std::string_view name, CS_Source source,
                            CS_EventKind kind) {
  Send(UINT_MAX, name, source, static_cast<RawEvent::Kind>(kind));
}

void Notifier::NotifySource(const SourceImpl& source, CS_EventKind kind) {
  auto handleData = Instance::GetInstance().FindSource(source);
  NotifySource(source.GetName(), handleData.first, kind);
}

void Notifier::NotifySourceVideoMode(const SourceImpl& source,
                                     const VideoMode& mode) {
  auto handleData = Instance::GetInstance().FindSource(source);
  Send(UINT_MAX, source.GetName(), handleData.first, mode);
}

void Notifier::NotifySourceProperty(const SourceImpl& source, CS_EventKind kind,
                                    std::string_view propertyName, int property,
                                    CS_PropertyKind propertyKind, int value,
                                    std::string_view valueStr) {
  auto handleData = Instance::GetInstance().FindSource(source);
  Send(UINT_MAX, propertyName, handleData.first,
       static_cast<RawEvent::Kind>(kind),
       Handle{handleData.first, property, Handle::kProperty}, propertyKind,
       value, valueStr);
}

void Notifier::NotifySink(std::string_view name, CS_Sink sink,
                          CS_EventKind kind) {
  Send(UINT_MAX, name, sink, static_cast<RawEvent::Kind>(kind));
}

void Notifier::NotifySink(const SinkImpl& sink, CS_EventKind kind) {
  auto handleData = Instance::GetInstance().FindSink(sink);
  NotifySink(sink.GetName(), handleData.first, kind);
}

void Notifier::NotifySinkSourceChanged(std::string_view name, CS_Sink sink,
                                       CS_Source source) {
  RawEvent event{name, sink, RawEvent::kSinkSourceChanged};
  event.sourceHandle = source;
  Send(UINT_MAX, std::move(event));
}

void Notifier::NotifySinkProperty(const SinkImpl& sink, CS_EventKind kind,
                                  std::string_view propertyName, int property,
                                  CS_PropertyKind propertyKind, int value,
                                  std::string_view valueStr) {
  auto handleData = Instance::GetInstance().FindSink(sink);
  Send(UINT_MAX, propertyName, handleData.first,
       static_cast<RawEvent::Kind>(kind),
       Handle{handleData.first, property, Handle::kSinkProperty}, propertyKind,
       value, valueStr);
}

void Notifier::NotifyNetworkInterfacesChanged() {
  Send(UINT_MAX, RawEvent::kNetworkInterfacesChanged);
}

void Notifier::NotifyTelemetryUpdated() {
  Send(UINT_MAX, RawEvent::kTelemetryUpdated);
}

void Notifier::NotifyUsbCamerasChanged() {
  Send(UINT_MAX, RawEvent::kUsbCamerasChanged);
}
