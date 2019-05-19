/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore.raw;

import edu.wpi.cscore.CameraServerJNI;
import edu.wpi.cscore.ImageSink;

public class RawSink extends ImageSink {
  public RawSink(String name) {
    super(CameraServerJNI.createRawSink(name));
  }

  protected long grabFrame(RawFrame frame) {
    return grabFrame(frame, 0.225);
  }

  protected long grabFrame(RawFrame frame, double timeout) {
    return CameraServerJNI.grabSinkFrameTimeout(m_handle, frame, timeout);
  }

  protected long grabFrameNoTimeout(RawFrame frame) {
    return CameraServerJNI.grabSinkFrame(m_handle, frame);
  }
}
