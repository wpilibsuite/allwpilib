/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "EntryNotifier.h"

#include "Log.h"

using namespace nt;

EntryNotifier::EntryNotifier(int inst, wpi::Logger& logger)
    : m_inst(inst), m_logger(logger) {
  m_local_notifiers = false;
}

void EntryNotifier::Start() { DoStart(m_inst); }

bool EntryNotifier::local_notifiers() const { return m_local_notifiers; }

bool impl::EntryNotifierThread::Matches(const EntryListenerData& listener,
                                        const EntryNotification& data) {
  if (!data.value) return false;

  // Flags must be within requested flag set for this listener.
  // Because assign messages can result in both a value and flags update,
  // we handle that case specially.
  unsigned int listen_flags =
      listener.flags & ~(NT_NOTIFY_IMMEDIATE | NT_NOTIFY_LOCAL);
  unsigned int flags = data.flags & ~(NT_NOTIFY_IMMEDIATE | NT_NOTIFY_LOCAL);
  unsigned int assign_both = NT_NOTIFY_UPDATE | NT_NOTIFY_FLAGS;
  if ((flags & assign_both) == assign_both) {
    if ((listen_flags & assign_both) == 0) return false;
    listen_flags &= ~assign_both;
    flags &= ~assign_both;
  }
  if ((flags & ~listen_flags) != 0) return false;

  // must match local id or prefix
  if (listener.entry != 0 && data.entry != listener.entry) return false;
  if (listener.entry == 0 &&
      !wpi::StringRef(data.name).startswith(listener.prefix))
    return false;

  return true;
}

unsigned int EntryNotifier::Add(
    std::function<void(const EntryNotification& event)> callback,
    StringRef prefix, unsigned int flags) {
  if ((flags & NT_NOTIFY_LOCAL) != 0) m_local_notifiers = true;
  return DoAdd(callback, prefix, flags);
}

unsigned int EntryNotifier::Add(
    std::function<void(const EntryNotification& event)> callback,
    unsigned int local_id, unsigned int flags) {
  if ((flags & NT_NOTIFY_LOCAL) != 0) m_local_notifiers = true;
  return DoAdd(callback, Handle(m_inst, local_id, Handle::kEntry), flags);
}

unsigned int EntryNotifier::AddPolled(unsigned int poller_uid,
                                      wpi::StringRef prefix,
                                      unsigned int flags) {
  if ((flags & NT_NOTIFY_LOCAL) != 0) m_local_notifiers = true;
  return DoAdd(poller_uid, prefix, flags);
}

unsigned int EntryNotifier::AddPolled(unsigned int poller_uid,
                                      unsigned int local_id,
                                      unsigned int flags) {
  if ((flags & NT_NOTIFY_LOCAL) != 0) m_local_notifiers = true;
  return DoAdd(poller_uid, Handle(m_inst, local_id, Handle::kEntry), flags);
}

void EntryNotifier::NotifyEntry(unsigned int local_id, StringRef name,
                                std::shared_ptr<Value> value,
                                unsigned int flags,
                                unsigned int only_listener) {
  // optimization: don't generate needless local queue entries if we have
  // no local listeners (as this is a common case on the server side)
  if ((flags & NT_NOTIFY_LOCAL) != 0 && !m_local_notifiers) return;
  DEBUG("notifying '" << name << "' (local=" << local_id
                      << "), flags=" << flags);
  Send(only_listener, 0, Handle(m_inst, local_id, Handle::kEntry).handle(),
       name, value, flags);
}
