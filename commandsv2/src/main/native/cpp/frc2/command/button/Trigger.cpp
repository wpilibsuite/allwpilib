// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/Trigger.hpp"

#include <utility>

#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/math/filter/Debouncer.hpp"

using namespace frc;
using namespace frc2;

Trigger::Trigger(const Trigger& other) = default;

void Trigger::AddBinding(wpi::unique_function<void(bool, bool)>&& body) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                body = std::move(body)]() mutable {
    bool current = condition();

    body(previous, current);

    previous = current;
  });
}

Trigger Trigger::OnChange(Command* command) {
  AddBinding([command](bool previous, bool current) {
    if (previous != current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    }
  });
  return *this;
}

Trigger Trigger::OnChange(CommandPtr&& command) {
  AddBinding([command = std::move(command)](bool previous, bool current) {
    if (previous != current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    }
  });
  return *this;
}

Trigger Trigger::OnTrue(Command* command) {
  AddBinding([command](bool previous, bool current) {
    if (!previous && current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    }
  });
  return *this;
}

Trigger Trigger::OnTrue(CommandPtr&& command) {
  AddBinding([command = std::move(command)](bool previous, bool current) {
    if (!previous && current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    }
  });
  return *this;
}

Trigger Trigger::OnFalse(Command* command) {
  AddBinding([command](bool previous, bool current) {
    if (previous && !current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    }
  });
  return *this;
}

Trigger Trigger::OnFalse(CommandPtr&& command) {
  AddBinding([command = std::move(command)](bool previous, bool current) {
    if (previous && !current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    }
  });
  return *this;
}

Trigger Trigger::WhileTrue(Command* command) {
  AddBinding([command](bool previous, bool current) {
    if (!previous && current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    } else if (previous && !current) {
      command->Cancel();
    }
  });
  return *this;
}

Trigger Trigger::WhileTrue(CommandPtr&& command) {
  AddBinding([command = std::move(command)](bool previous, bool current) {
    if (!previous && current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    } else if (previous && !current) {
      command.Cancel();
    }
  });
  return *this;
}

Trigger Trigger::WhileFalse(Command* command) {
  AddBinding([command](bool previous, bool current) {
    if (previous && !current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    } else if (!previous && current) {
      command->Cancel();
    }
  });
  return *this;
}

Trigger Trigger::WhileFalse(CommandPtr&& command) {
  AddBinding([command = std::move(command)](bool previous, bool current) {
    if (!previous && current) {
      frc2::CommandScheduler::GetInstance().Schedule(command);
    } else if (previous && !current) {
      command.Cancel();
    }
  });
  return *this;
}

Trigger Trigger::ToggleOnTrue(Command* command) {
  AddBinding([command](bool previous, bool current) {
    if (!previous && current) {
      if (command->IsScheduled()) {
        command->Cancel();
      } else {
        frc2::CommandScheduler::GetInstance().Schedule(command);
      }
    }
  });
  return *this;
}

Trigger Trigger::ToggleOnTrue(CommandPtr&& command) {
  AddBinding([command = std::move(command)](bool previous, bool current) {
    if (!previous && current) {
      if (command.IsScheduled()) {
        command.Cancel();
      } else {
        frc2::CommandScheduler::GetInstance().Schedule(command);
      }
    }
  });
  return *this;
}

Trigger Trigger::ToggleOnFalse(Command* command) {
  AddBinding([command](bool previous, bool current) {
    if (previous && !current) {
      if (command->IsScheduled()) {
        command->Cancel();
      } else {
        frc2::CommandScheduler::GetInstance().Schedule(command);
      }
    }
  });
  return *this;
}

Trigger Trigger::ToggleOnFalse(CommandPtr&& command) {
  AddBinding([command = std::move(command)](bool previous, bool current) {
    if (previous && !current) {
      if (command.IsScheduled()) {
        command.Cancel();
      } else {
        frc2::CommandScheduler::GetInstance().Schedule(command);
      }
    }
  });
  return *this;
}

Trigger Trigger::Debounce(units::second_t debounceTime,
                          frc::Debouncer::DebounceType type) {
  return Trigger(m_loop, [debouncer = frc::Debouncer(debounceTime, type),
                          condition = m_condition]() mutable {
    return debouncer.Calculate(condition());
  });
}

bool Trigger::Get() const {
  return m_condition();
}
