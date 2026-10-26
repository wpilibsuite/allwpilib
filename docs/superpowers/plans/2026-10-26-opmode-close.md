# OpMode Close Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Expose deterministic Python OpMode cleanup with minimal Java lifecycle parity.

**Architecture:** Add default no-op virtual `wpi::OpMode::Close()`, invoke it from native lifecycle teardown after callback removal and before releasing the instance, and expose it as Python `close()` through semiwrap. Preserve destructor-based C++ cleanup.

**Tech Stack:** C++, semiwrap/pybind11, Python pytest, Catch2, Bazel.

**Spec:** User-approved chat proposal for mostrobotpy issue #341, restricted to minimal Java parity.

## Global Constraints

- Work only in `.worktrees/opmode-close`, branch `opmode-close`.
- No shutdown behavior or exception-safety changes; no Java changes.
- Close selected instances even if they never started; End remains conditional on starting.
- Do not invoke Close from a destructor.
- Redirect Bazel output to log files.

## Review Focus

- Never-started instances: close without end.
- Started instances: end before close, exactly once per instance.
- Python retained references and cycles: lifecycle close must not depend on destruction.
- Extra periodic callbacks: must stop before close and remain stopped afterward.
- Existing subclasses without Close overrides: remain valid through the default no-op.

### Task 1: Native lifecycle hook and Python binding

**Files:**
- Modify `wpilibc/src/main/native/include/wpi/opmode/OpMode.hpp`: declare and document lifecycle cleanup.
- Modify `wpilibc/src/main/native/cpp/framework/OpModeRobot.cpp`: invoke cleanup in EndCurrentOpMode.
- Modify `wpilibc/src/main/python/semiwrap/OpMode.yml`: expose Close.
- Modify `wpilibc/src/test/native/cpp/OpModeLifecycleTest.cpp`: test native lifecycle order and counts.
- Modify `wpilibc/src/test/python/framework/test_opmode_robot.py`: test Python override dispatch and retained objects.

**Interfaces:** Produces `virtual void wpi::OpMode::Close() {}` and Python `OpMode.close(self) -> None`.

- [ ] Finish baseline verification: `bazelisk test //wpilibc/src/test/native --test_filter='OpModeLifecycleTest*'` and `bazelisk test //wpilibc/src/test/python --test_arg=-k --test_arg=opmode` (using the checkout's absolute bazelisk path).
- [ ] Add native close counters and ordered events to lifecycle tests; assert never-started modes close without End, started modes End before Close, and framework Close occurs once. Add Python lifecycle tests retaining references (including a self-cycle), with close recording an event. Assert no periodic callbacks occur after close and subclasses without overrides remain usable.
- [ ] Run those targets to establish failure from the missing Close API/dispatch.
- [ ] Add the no-op virtual Close and documentation describing release-time lifecycle semantics, not guaranteed destruction. Call Close immediately before `m_currentOpMode.reset()` after callbacks are removed. Add `Close:` to the semiwrap method map.
- [ ] Run both test targets again; require PASS. Inspect generated binding behavior through the Python override tests.
- [ ] Review the diff for scope, formatting, and unintended generated changes; run `git diff --check`.
- [ ] Obtain an independent code review, fix any findings, and rerun affected tests before reporting completion.
