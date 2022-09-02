// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;

public class EventVector {
    private ReentrantLock lock = new ReentrantLock();
    private List<Integer> events = new ArrayList<>();

    public void add(int handle) {
        lock.lock();
        try {
            events.add(handle);
        } finally {
            lock.unlock();
        }
    }

    public void remove(int handle) {
        lock.lock();
        try {
            events.removeIf(x -> x.intValue() == handle);
        } finally {
            lock.unlock();
        }
    }

    public void wakeup() {
        lock.lock();
        try {
            for (Integer eventHandle : events) {
                WPIUtilJNI.setEvent(eventHandle);
            }
        } finally {
            lock.unlock();
        }
    }
}
