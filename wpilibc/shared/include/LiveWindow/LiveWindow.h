#ifndef _LIVE_WINDOW_H
#define _LIVE_WINDOW_H

#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITable.h"
#include "Commands/Scheduler.h"
#include <vector>
#include <map>
#include <memory>

struct LiveWindowComponent {
  std::string subsystem;
  std::string name;
  bool isSensor = false;

  LiveWindowComponent() = default;
  LiveWindowComponent(std::string subsystem, std::string name, bool isSensor) {
    this->subsystem = subsystem;
    this->name = name;
    this->isSensor = isSensor;
  }
};

/**
 * The LiveWindow class is the public interface for putting sensors and
 * actuators
 * on the LiveWindow.
 *
 * @author Brad Miller
 */
class LiveWindow {
 public:
  static LiveWindow *GetInstance();
  void Run();
  void AddSensor(const std::string &subsystem, const std::string &name,
                 LiveWindowSendable *component);
  void AddSensor(const std::string &subsystem, const std::string &name,
                 LiveWindowSendable &component);
  void AddSensor(const std::string &subsystem, const std::string &name,
                 std::shared_ptr<LiveWindowSendable> component);
  void AddActuator(const std::string &subsystem, const std::string &name,
                   LiveWindowSendable *component);
  void AddActuator(const std::string &subsystem, const std::string &name,
                   LiveWindowSendable &component);
  void AddActuator(const std::string &subsystem, const std::string &name,
                   std::shared_ptr<LiveWindowSendable> component);

  void AddSensor(std::string type, int channel, LiveWindowSendable *component);
  void AddActuator(std::string type, int channel,
                   LiveWindowSendable *component);
  void AddActuator(std::string type, int module, int channel,
                   LiveWindowSendable *component);

  bool IsEnabled() const { return m_enabled; }
  void SetEnabled(bool enabled);

 protected:
  LiveWindow();
  virtual ~LiveWindow() = default;

 private:
  void UpdateValues();
  void Initialize();
  void InitializeLiveWindowComponents();

  std::vector<std::shared_ptr<LiveWindowSendable>> m_sensors;
  std::map<std::shared_ptr<LiveWindowSendable>, LiveWindowComponent> m_components;

  std::shared_ptr<ITable> m_liveWindowTable;
  std::shared_ptr<ITable> m_statusTable;

  Scheduler *m_scheduler;

  bool m_enabled = false;
  bool m_firstTime = true;
};

#endif
