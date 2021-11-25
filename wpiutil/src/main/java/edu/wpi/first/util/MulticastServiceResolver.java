// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

public class MulticastServiceResolver implements AutoCloseable {
    private final int handle;

    public MulticastServiceResolver(String serviceType) {
      handle = WPIUtilJNI.createMulticastServiceResolver(serviceType);
    }

    @Override
    public void close() {
      WPIUtilJNI.freeMulticastServiceResolver(handle);
    }

    public void start() {
      WPIUtilJNI.startMulticastServiceResolver(handle);
    }

    public void stop() {
      WPIUtilJNI.stopMulticastServiceResolver(handle);
    }

    public boolean hasImplementation() {
      return WPIUtilJNI.getMulticastServiceResolverHasImplementation(handle);
    }

    public int getEventHandle() {
        return WPIUtilJNI.getMulticastServiceResolverEventHandle(handle);
    }

    public ServiceData getData() {
        return WPIUtilJNI.getMulticastServiceResolverData(handle);
    }
}
