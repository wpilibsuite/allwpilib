/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_ENTRYNOTIFIER_H_
#define NTCORE_ENTRYNOTIFIER_H_

#include <atomic>
#include <memory>
#include <string>

#include "CallbackManager.h"
#include "Handle.h"
#include "IEntryNotifier.h"
#include "ntcore_cpp.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt {

namespace impl {

struct EntryListenerData
    : public ListenerData<std::function<void(const EntryNotification& event)>> {
  EntryListenerData() = default;
  EntryListenerData(
      std::function<void(const EntryNotification& event)> callback_,
      StringRef prefix_, unsigned int flags_)
      : ListenerData(callback_), prefix(prefix_), flags(flags_) {}
  EntryListenerData(
      std::function<void(const EntryNotification& event)> callback_,
      NT_Entry entry_, unsigned int flags_)
      : ListenerData(callback_), entry(entry_), flags(flags_) {}
  EntryListenerData(unsigned int poller_uid_, StringRef prefix_,
                    unsigned int flags_)
      : ListenerData(poller_uid_), prefix(prefix_), flags(flags_) {}
  EntryListenerData(unsigned int poller_uid_, NT_Entry entry_,
                    unsigned int flags_)
      : ListenerData(poller_uid_), entry(entry_), flags(flags_) {}

  std::string prefix;
  NT_Entry entry = 0;
  unsigned int flags;
};

class EntryNotifierThread
    : public CallbackThread<EntryNotifierThread, EntryNotification,
                            EntryListenerData> {
 public:
  explicit EntryNotifierThread(int inst) : m_inst(inst) {}

  bool Matches(const EntryListenerData& listener,
               const EntryNotification& data);

  void SetListener(EntryNotification* data, unsigned int listener_uid) {
    data->listener =
        Handle(m_inst, listener_uid, Handle::kEntryListener).handle();
  }

  void DoCallback(std::function<void(const EntryNotification& event)> callback,
                  const EntryNotification& data) {
    callback(data);
  }

  int m_inst;
};

}  // namespace impl

class EntryNotifier
    : public IEntryNotifier,
      public CallbackManager<EntryNotifier, impl::EntryNotifierThread> {
  friend class EntryNotifierTest;
  friend class CallbackManager<EntryNotifier, impl::EntryNotifierThread>;

 public:
  explicit EntryNotifier(int inst, wpi::Logger& logger);

  void Start();

  bool local_notifiers() const override;

  unsigned int Add(std::function<void(const EntryNotification& event)> callback,
                   wpi::StringRef prefix, unsigned int flags) override;
  unsigned int Add(std::function<void(const EntryNotification& event)> callback,
                   unsigned int local_id, unsigned int flags) override;
  unsigned int AddPolled(unsigned int poller_uid, wpi::StringRef prefix,
                         unsigned int flags) override;
  unsigned int AddPolled(unsigned int poller_uid, unsigned int local_id,
                         unsigned int flags) override;

  void NotifyEntry(unsigned int local_id, StringRef name,
                   std::shared_ptr<Value> value, unsigned int flags,
                   unsigned int only_listener = UINT_MAX) override;

 private:
  int m_inst;
  wpi::Logger& m_logger;
  std::atomic_bool m_local_notifiers;
};

}  // namespace nt

#endif  // NTCORE_ENTRYNOTIFIER_H_
