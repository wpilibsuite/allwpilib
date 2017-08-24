/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <set>
#include <string>

#include "ErrorBase.h"
#include "SmartDashboard/NamedSendable.h"
#include "tables/ITableListener.h"

namespace frc {

class CommandGroup;
class Subsystem;

/**
 * The Command class is at the very core of the entire command framework.
 * Every command can be started with a call to {@link Command#Start() Start()}.
 * Once a command is started it will call {@link Command#Initialize()
 * Initialize()}, and then will repeatedly call
 * {@link Command#Execute() Execute()} until the
 * {@link Command#IsFinished() IsFinished()} returns true.  Once it does,
 * {@link Command#End() End()} will be called.
 *
 * <p>However, if at any point while it is running {@link Command#Cancel()
 * Cancel()} is called, then the command will be stopped and
 * {@link Command#Interrupted() Interrupted()} will be called.</p>
 *
 * <p>If a command uses a {@link Subsystem}, then it should specify that it does
 * so by calling the {@link Command#Requires(Subsystem) Requires(...)} method
 * in its constructor. Note that a Command may have multiple requirements, and
 * {@link Command#Requires(Subsystem) Requires(...)} should be called for each
 * one.</p>
 *
 * <p>If a command is running and a new command with shared requirements is
 * started, then one of two things will happen.  If the active command is
 * interruptible, then {@link Command#Cancel() Cancel()} will be called and the
 * command will be removed to make way for the new one.  If the active command
 * is not interruptible, the other one will not even be started, and the active
 * one will continue functioning.</p>
 *
 * @see CommandGroup
 * @see Subsystem
 */
class Command : public ErrorBase, public NamedSendable, public ITableListener {
  friend class CommandGroup;
  friend class Scheduler;

 public:
  Command();
  explicit Command(const std::string& name);
  explicit Command(double timeout);
  Command(const std::string& name, double timeout);
  virtual ~Command();
  double TimeSinceInitialized() const;
  void Requires(Subsystem* s);
  bool IsCanceled() const;
  void Start();
  bool Run();
  void Cancel();
  bool IsRunning() const;
  bool IsInterruptible() const;
  void SetInterruptible(bool interruptible);
  bool DoesRequire(Subsystem* subsystem) const;
  typedef std::set<Subsystem*> SubsystemSet;
  SubsystemSet GetRequirements() const;
  CommandGroup* GetGroup() const;
  void SetRunWhenDisabled(bool run);
  bool WillRunWhenDisabled() const;
  int GetID() const;

 protected:
  void SetTimeout(double timeout);
  bool IsTimedOut() const;
  bool AssertUnlocked(const std::string& message);
  void SetParent(CommandGroup* parent);
  void ClearRequirements();

  virtual void Initialize();
  virtual void Execute();

  /**
   * Returns whether this command is finished.
   * If it is, then the command will be removed and {@link Command#end() end()}
   * will be called.
   *
   * <p>It may be useful for a team to reference the {@link Command#isTimedOut()
   * isTimedOut()} method for time-sensitive commands.</p>
   *
   * <p>Returning false will result in the command never ending automatically.
   * It may still be cancelled manually or interrupted by another command.
   * Returning true will result in the command executing once and finishing
   * immediately. We recommend using {@link InstantCommand} for this.</p>
   *
   * @return whether this command is finished.
   * @see Command#isTimedOut() isTimedOut()
   */
  virtual bool IsFinished() = 0;

  virtual void End();
  virtual void Interrupted();

  virtual void _Initialize();
  virtual void _Interrupted();
  virtual void _Execute();
  virtual void _End();
  virtual void _Cancel();

  friend class ConditionalCommand;

 private:
  void LockChanges();
  /*synchronized*/ void Removed();
  void StartRunning();
  void StartTiming();

  /** The name of this command */
  std::string m_name;

  /** The time since this command was initialized */
  double m_startTime = -1;

  /** The time (in seconds) before this command "times out" (or -1 if no
   * timeout) */
  double m_timeout;

  /** Whether or not this command has been initialized */
  bool m_initialized = false;

  /** The requirements (or null if no requirements) */
  SubsystemSet m_requirements;

  /** Whether or not it is running */
  bool m_running = false;

  /** Whether or not it is interruptible*/
  bool m_interruptible = true;

  /** Whether or not it has been canceled */
  bool m_canceled = false;

  /** Whether or not it has been locked */
  bool m_locked = false;

  /** Whether this command should run when the robot is disabled */
  bool m_runWhenDisabled = false;

  /** The {@link CommandGroup} this is in */
  CommandGroup* m_parent = nullptr;

  int m_commandID = m_commandCounter++;
  static int m_commandCounter;

 public:
  std::string GetName() const override;
  void InitTable(std::shared_ptr<ITable> subtable) override;
  std::shared_ptr<ITable> GetTable() const override;
  std::string GetSmartDashboardType() const override;
  void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew) override;

 protected:
  std::shared_ptr<ITable> m_table;
};

}  // namespace frc
