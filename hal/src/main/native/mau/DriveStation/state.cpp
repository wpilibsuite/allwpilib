#include "state.hpp"
#include "util.hpp"
#include "include/BagelDriveData.h"

// #include "memory.hpp"

using namespace Toast;
using namespace std;

static vector<void (*)(State, State)> __trackers;
static vector<void (*)(State)> __tickers;
static vector<IterativeBase*> __iterative;

static State _DISABLED(RobotState::DISABLED);
static State _AUTO(RobotState::AUTO);
static State _TELEOP(RobotState::TELEOP);
static State _TEST(RobotState::TEST);

static RobotState _last_internal_state;

void States::Internal::set_state(RobotState rs) {
	Bagel_updateRobotState(rs);
}

void States::Internal::set_tick_timing(int ms) {
	//Memory::shared()->set_tick_timing(ms);
}

int States::Internal::get_tick_timing() {
	// return Memory::shared()->get_tick_timing();
	return 0;
}

State States::DISABLED() {
    return _DISABLED;
}

State States::AUTO() {
    return _AUTO;
}

State States::TELEOP() {
    return _TELEOP;
}

State States::TEST() {
    return _TEST;
}

State States::from_robotstate(RobotState rs) {
    if (rs == RobotState::DISABLED)   return _DISABLED;
    if (rs == RobotState::AUTO)       return _AUTO;
    if (rs == RobotState::TELEOP)     return _TELEOP;
    if (rs == RobotState::TEST)       return _TEST;
    
    return _DISABLED;
}

RobotState States::current_robotstate() {
	return Bagel_readRobotState(); //Memory::shared()->get_state_current();
}

RobotState States::last_robotstate() {
	// no-op
	return RobotState::DISABLED;
}

State States::current_state() {
    return States::from_robotstate(States::current_robotstate());
}

State States::last_state() {
    return States::from_robotstate(States::last_robotstate());
}

void States::start_tracker() {
    //while (is_process_alive(Memory::shared()->get_bootstrap_pid())) {
	//		States::_manual_trigger();
    //}
}

void States::_manual_trigger() {
	RobotState current_state = States::current_robotstate();
	State _cur = States::from_robotstate(current_state);

	if (current_state != _last_internal_state) {
		// Transition Occured
		State _old = States::from_robotstate(_last_internal_state);
		for (auto func : __trackers) {
			func(_old, _cur);
		}
		for (auto it : __iterative) {
			if (current_state == RobotState::DISABLED)
				it->disabledInit();
			else if (current_state == RobotState::AUTO)
				it->autonomousInit();
			else if (current_state == RobotState::TELEOP)
				it->teleopInit();
			else if (current_state == RobotState::TEST)
				it->testInit();
		}
	}

	// Periodic
	for (auto func : __tickers) {
		func(_cur);
	}
	for (auto it : __iterative) {
		if (current_state == RobotState::DISABLED)
			it->disabledPeriodic();
		else if (current_state == RobotState::AUTO)
			it->autonomousPeriodic();
		else if (current_state == RobotState::TELEOP)
			it->teleopPeriodic();
		else if (current_state == RobotState::TEST)
			it->testPeriodic();
	}

//	sleep_ms(States::Internal::get_tick_timing());
	_last_internal_state = current_state;
}

void States::register_tracker(void (*func)(State, State)) {
    __trackers.push_back(func);
}

void States::register_ticker(void (*func)(State)) {
    __tickers.push_back(func);
}

void States::register_iterative(IterativeBase *iterative) {
    __iterative.push_back(iterative);
}