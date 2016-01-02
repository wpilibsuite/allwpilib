/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.concurrent.atomic.AtomicBoolean;

import com.ni.vision.NIVision;
import com.ni.vision.NIVision.Image;
import com.ni.vision.NIVision.RawData;
import com.ni.vision.VisionException;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.vision.USBCamera;

// replicates CameraServer.cpp in java lib

public class CameraServer {

  private static final int kPort = 1180;
  private static final byte[] kMagicNumber = {0x01, 0x00, 0x00, 0x00};
  private static final int kSize640x480 = 0;
  private static final int kSize320x240 = 1;
  private static final int kSize160x120 = 2;
  private static final int kHardwareCompression = -1;
  private static final String kDefaultCameraName = "cam1";
  private static final int kMaxImageSize = 200000;
  private static CameraServer server;

  public static CameraServer getInstance() {
    if (server == null) {
      server = new CameraServer();
    }
    return server;
  }

  private Thread serverThread;
  private int m_quality;
  private boolean m_autoCaptureStarted;
  private boolean m_hwClient = true;
  private USBCamera m_camera;
  private CameraData m_imageData;
  private Deque<ByteBuffer> m_imageDataPool;

  private class CameraData {
    RawData data;
    int start;

    public CameraData(RawData d, int s) {
      data = d;
      start = s;
    }
  }

  private CameraServer() {
    m_quality = 50;
    m_camera = null;
    m_imageData = null;
    m_imageDataPool = new ArrayDeque<>(3);
    for (int i = 0; i < 3; i++) {
      m_imageDataPool.addLast(ByteBuffer.allocateDirect(kMaxImageSize));
    }
    serverThread = new Thread(new Runnable() {
      public void run() {
        try {
          serve();
        } catch (IOException e) {
          // do stuff here
        } catch (InterruptedException e) {
          // do stuff here
        }
      }
    });
    serverThread.setName("CameraServer Send Thread");
    serverThread.start();
  }

  private synchronized void setImageData(RawData data, int start) {
    if (m_imageData != null && m_imageData.data != null) {
      m_imageData.data.free();
      if (m_imageData.data.getBuffer() != null)
        m_imageDataPool.addLast(m_imageData.data.getBuffer());
      m_imageData = null;
    }
    m_imageData = new CameraData(data, start);
    notifyAll();
  }

  /**
   * Manually change the image that is served by the MJPEG stream. This can be
   * called to pass custom annotated images to the dashboard. Note that, for
   * 640x480 video, this method could take between 40 and 50 milliseconds to
   * complete.
   *
   * This shouldn't be called if {@link #startAutomaticCapture} is called.
   *
   * @param image The IMAQ image to show on the dashboard
   */
  public void setImage(Image image) {
    // handle multi-threadedness

    /* Flatten the IMAQ image to a JPEG */

    RawData data =
        NIVision.imaqFlatten(image, NIVision.FlattenType.FLATTEN_IMAGE,
            NIVision.CompressionType.COMPRESSION_JPEG, 10 * m_quality);
    ByteBuffer buffer = data.getBuffer();
    boolean hwClient;

    synchronized (this) {
      hwClient = m_hwClient;
    }

    /* Find the start of the JPEG data */
    int index = 0;
    if (hwClient) {
      while (index < buffer.limit() - 1) {
        if ((buffer.get(index) & 0xff) == 0xFF && (buffer.get(index + 1) & 0xff) == 0xD8)
          break;
        index++;
      }
    }

    if (buffer.limit() - index - 1 <= 2) {
      throw new VisionException("data size of flattened image is less than 2. Try another camera! ");
    }

    setImageData(data, index);
  }

  /**
   * Start automatically capturing images to send to the dashboard. You should
   * call this method to just see a camera feed on the dashboard without doing
   * any vision processing on the roboRIO. {@link #setImage} shouldn't be called
   * after this is called. This overload calles
   * {@link #startAutomaticCapture(String)} with the default camera name
   */
  public void startAutomaticCapture() {
    startAutomaticCapture(USBCamera.kDefaultCameraName);
  }

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * You should call this method to just see a camera feed on the dashboard
   * without doing any vision processing on the roboRIO. {@link #setImage}
   * shouldn't be called after this is called.
   *
   * @param cameraName The name of the camera interface (e.g. "cam1")
   */
  public void startAutomaticCapture(String cameraName) {
    try {
      USBCamera camera = new USBCamera(cameraName);
      camera.openCamera();
      startAutomaticCapture(camera);
    } catch (VisionException ex) {
      DriverStation.reportError(
          "Error when starting the camera: " + cameraName + " " + ex.getMessage(), true);
    }
  }

  public synchronized void startAutomaticCapture(USBCamera camera) {
    if (m_autoCaptureStarted)
      return;
    m_autoCaptureStarted = true;
    m_camera = camera;

    m_camera.startCapture();

    Thread captureThread = new Thread(new Runnable() {
      @Override
      public void run() {
        capture();
      }
    });
    captureThread.setName("Camera Capture Thread");
    captureThread.start();
  }

  protected void capture() {
    Image frame = NIVision.imaqCreateImage(NIVision.ImageType.IMAGE_RGB, 0);
    while (true) {
      boolean hwClient;
      ByteBuffer dataBuffer = null;
      synchronized (this) {
        hwClient = m_hwClient;
        if (hwClient) {
          dataBuffer = m_imageDataPool.removeLast();
        }
      }

      try {
        if (hwClient && dataBuffer != null) {
          // Reset the image buffer limit
          dataBuffer.limit(dataBuffer.capacity() - 1);
          m_camera.getImageData(dataBuffer);
          setImageData(new RawData(dataBuffer), 0);
        } else {
          m_camera.getImage(frame);
          setImage(frame);
        }
      } catch (VisionException ex) {
        DriverStation.reportError("Error when getting image from the camera: " + ex.getMessage(),
            true);
        if (dataBuffer != null) {
          synchronized (this) {
            m_imageDataPool.addLast(dataBuffer);
            Timer.delay(.1);
          }
        }
      }
    }
  }



  /**
   * check if auto capture is started
   *
   */
  public synchronized boolean isAutoCaptureStarted() {
    return m_autoCaptureStarted;
  }

  /**
   * Sets the size of the image to use. Use the public kSize constants to set
   * the correct mode, or set it directory on a camera and call the appropriate
   * autoCapture method
   *$
   * @param size The size to use
   */
  public synchronized void setSize(int size) {
    if (m_camera == null)
      return;
    switch (size) {
      case kSize640x480:
        m_camera.setSize(640, 480);
        break;
      case kSize320x240:
        m_camera.setSize(320, 240);
        break;
      case kSize160x120:
        m_camera.setSize(160, 120);
        break;
    }
  }

  /**
   * Set the quality of the compressed image sent to the dashboard
   *
   * @param quality The quality of the JPEG image, from 0 to 100
   */
  public synchronized void setQuality(int quality) {
    m_quality = quality > 100 ? 100 : quality < 0 ? 0 : quality;
  }

  /**
   * Get the quality of the compressed image sent to the dashboard
   *
   * @return The quality, from 0 to 100
   */
  public synchronized int getQuality() {
    return m_quality;
  }

  /**
   * Run the M-JPEG server.
   *
   * This function listens for a connection from the dashboard in a background
   * thread, then sends back the M-JPEG stream.
   *
   * @throws IOException if the Socket connection fails
   * @throws InterruptedException if the sleep is interrupted
   */
  protected void serve() throws IOException, InterruptedException {

    ServerSocket socket = new ServerSocket();
    socket.setReuseAddress(true);
    InetSocketAddress address = new InetSocketAddress(kPort);
    socket.bind(address);

    while (true) {
      try {
        Socket s = socket.accept();

        DataInputStream is = new DataInputStream(s.getInputStream());
        DataOutputStream os = new DataOutputStream(s.getOutputStream());

        int fps = is.readInt();
        int compression = is.readInt();
        int size = is.readInt();

        if (compression != kHardwareCompression) {
          DriverStation.reportError("Choose \"USB Camera HW\" on the dashboard", false);
          s.close();
          continue;
        }

        // Wait for the camera
        synchronized (this) {
          System.out.println("Camera not yet ready, awaiting image");
          if (m_camera == null)
            wait();
          m_hwClient = compression == kHardwareCompression;
          if (!m_hwClient)
            setQuality(100 - compression);
          else if (m_camera != null)
            m_camera.setFPS(fps);
          setSize(size);
        }

        long period = (long) (1000 / (1.0 * fps));
        while (true) {
          long t0 = System.currentTimeMillis();
          CameraData imageData = null;
          synchronized (this) {
            wait();
            imageData = m_imageData;
            m_imageData = null;
          }

          if (imageData == null)
            continue;
          // Set the buffer position to the start of the data,
          // and then create a new wrapper for the data at
          // exactly that position
          imageData.data.getBuffer().position(imageData.start);
          byte[] imageArray = new byte[imageData.data.getBuffer().remaining()];
          imageData.data.getBuffer().get(imageArray, 0, imageData.data.getBuffer().remaining());

          // write numbers
          try {
            os.write(kMagicNumber);
            os.writeInt(imageArray.length);
            os.write(imageArray);
            os.flush();
            long dt = System.currentTimeMillis() - t0;

            if (dt < period) {
              Thread.sleep(period - dt);
            }
          } catch (IOException | UnsupportedOperationException ex) {
            DriverStation.reportError(ex.getMessage(), true);
            break;
          } finally {
            imageData.data.free();
            if (imageData.data.getBuffer() != null) {
              synchronized (this) {
                m_imageDataPool.addLast(imageData.data.getBuffer());
              }
            }
          }
        }
      } catch (IOException ex) {
        DriverStation.reportError(ex.getMessage(), true);
        continue;
      }
    }
  }
}
