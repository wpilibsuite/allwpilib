/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.cscore.AxisCamera;
import edu.wpi.cscore.CameraServerJNI;
import edu.wpi.cscore.CvSink;
import edu.wpi.cscore.CvSource;
import edu.wpi.cscore.MjpegServer;
import edu.wpi.cscore.UsbCamera;
import edu.wpi.cscore.VideoException;
import edu.wpi.cscore.VideoListener;
import edu.wpi.cscore.VideoMode;
import edu.wpi.cscore.VideoSink;
import edu.wpi.cscore.VideoSource;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.networktables.NetworkTablesJNI;
import edu.wpi.first.wpilibj.tables.ITable;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Singleton class for creating and keeping camera servers.
 * Also publishes camera information to NetworkTables.
 */
public class CameraServer {
  public static final int kBasePort = 1181;

  @Deprecated
  public static final int kSize640x480 = 0;
  @Deprecated
  public static final int kSize320x240 = 1;
  @Deprecated
  public static final int kSize160x120 = 2;

  private static final String kPublishName = "/CameraPublisher";
  private static CameraServer server;

  /**
   * Get the CameraServer instance.
   */
  public static CameraServer getInstance() {
    if (server == null) {
      server = new CameraServer();
    }
    return server;
  }

  private String m_primarySourceName;
  private HashMap<String, VideoSource> m_sources;
  private HashMap<String, VideoSink> m_sinks;
  private HashMap<Integer, ITable> m_tables;  // indexed by source handle
  private ITable m_publishTable;
  private VideoListener m_videoListener; //NOPMD
  private int m_tableListener; //NOPMD
  private int m_nextPort;
  private String[] m_addresses;

  @SuppressWarnings("JavadocMethod")
  private static String makeSourceValue(int source) {
    switch (VideoSource.getKindFromInt(CameraServerJNI.getSourceKind(source))) {
      case kUsb:
        return "usb:" + CameraServerJNI.getUsbCameraPath(source);
      case kHttp: {
        String[] urls = CameraServerJNI.getHttpCameraUrls(source);
        if (urls.length > 0) {
          return "ip:" + urls[0];
        } else {
          return "ip:";
        }
      }
      case kCv:
        // FIXME: Should be "cv:", but LabVIEW dashboard requires "usb:".
        // https://github.com/wpilibsuite/allwpilib/issues/407
        return "usb:";
      default:
        return "unknown:";
    }
  }

  @SuppressWarnings("JavadocMethod")
  private static String makeStreamValue(String address, int port) {
    return "mjpg:http://" + address + ":" + port + "/?action=stream";
  }

  @SuppressWarnings("JavadocMethod")
  private synchronized ITable getSourceTable(int source) {
    return m_tables.get(source);
  }

  @SuppressWarnings({"JavadocMethod", "PMD.AvoidUsingHardCodedIP"})
  private synchronized String[] getSinkStreamValues(int sink) {
    // Ignore all but MjpegServer
    if (VideoSink.getKindFromInt(CameraServerJNI.getSinkKind(sink)) != VideoSink.Kind.kMjpeg) {
      return new String[0];
    }

    // Get port
    int port = CameraServerJNI.getMjpegServerPort(sink);

    // Generate values
    ArrayList<String> values = new ArrayList<String>(m_addresses.length + 1);
    String listenAddress = CameraServerJNI.getMjpegServerListenAddress(sink);
    if (!listenAddress.isEmpty()) {
      // If a listen address is specified, only use that
      values.add(makeStreamValue(listenAddress, port));
    } else {
      // Otherwise generate for hostname and all interface addresses
      values.add(makeStreamValue(CameraServerJNI.getHostname() + ".local", port));
      for (String addr : m_addresses) {
        if (addr.equals("127.0.0.1")) {
          continue;  // ignore localhost
        }
        values.add(makeStreamValue(addr, port));
      }
    }

    return values.toArray(new String[0]);
  }

  @SuppressWarnings("JavadocMethod")
  private static String[] getSourceStreamValues(int source) {
    // Ignore all but HttpCamera
    if (VideoSource.getKindFromInt(CameraServerJNI.getSourceKind(source))
            != VideoSource.Kind.kHttp) {
      return new String[0];
    }

    // Generate values
    String[] values = CameraServerJNI.getHttpCameraUrls(source);
    for (int j = 0; j < values.length; j++) {
      values[j] = "mjpg:" + values[j];
    }

    return values;
  }

  @SuppressWarnings({"JavadocMethod", "PMD.AvoidUsingHardCodedIP"})
  private synchronized void updateStreamValues() {
    // Over all the sinks...
    for (VideoSink i : m_sinks.values()) {
      int sink = i.getHandle();

      // Get the source's subtable (if none exists, we're done)
      int source = CameraServerJNI.getSinkSource(sink);
      ITable table = m_tables.get(source);
      if (table != null) {
        // Set table value
        String[] values = getSinkStreamValues(sink);
        if (values.length > 0) {
          table.putStringArray("streams", values);
        }
      }
    }

    // Over all the sources...
    for (VideoSource i : m_sources.values()) {
      int source = i.getHandle();

      // Get the source's subtable (if none exists, we're done)
      ITable table = m_tables.get(source);
      if (table != null) {
        // Set table value
        String[] values = getSourceStreamValues(source);
        if (values.length > 0) {
          table.putStringArray("streams", values);
        }
      }
    }
  }

  @SuppressWarnings({"JavadocMethod", "PMD.UnusedLocalVariable"})
  private CameraServer() {
    m_sources = new HashMap<String, VideoSource>();
    m_sinks = new HashMap<String, VideoSink>();
    m_tables = new HashMap<Integer, ITable>();
    m_publishTable = NetworkTable.getTable(kPublishName);
    m_nextPort = kBasePort;
    m_addresses = new String[0];

    // We publish sources to NetworkTables using the following structure:
    // "/CameraPublisher/{Source.Name}/" - root
    // - "source" (string): Descriptive, prefixed with type (e.g. "usb:0")
    // - "streams" (string array): URLs that can be used to stream data
    // - properties (scaled units)

    // Listener for video events
    m_videoListener = new VideoListener(event -> {
      switch (event.kind) {
        case kSourceCreated: {
          // Create subtable for the camera
          ITable table = m_publishTable.getSubTable(event.name);
          synchronized (this) {
            m_tables.put(event.sourceHandle, table);
          }
          table.putString("source", makeSourceValue(event.sourceHandle));
          table.putString("description",
              CameraServerJNI.getSourceDescription(event.sourceHandle));
          table.putBoolean("connected", CameraServerJNI.isSourceConnected(event.sourceHandle));
          table.putStringArray("streams", getSourceStreamValues(event.sourceHandle));
          break;
        }
        case kSourceDestroyed: {
          ITable table = getSourceTable(event.sourceHandle);
          if (table != null) {
            table.putString("source", "");
            table.putStringArray("streams", new String[0]);
          }
          break;
        }
        case kSourceConnected: {
          ITable table = getSourceTable(event.sourceHandle);
          if (table != null) {
            // update the description too (as it may have changed)
            table.putString("description",
                CameraServerJNI.getSourceDescription(event.sourceHandle));
            table.putBoolean("connected", true);
          }
          break;
        }
        case kSourceDisconnected: {
          ITable table = getSourceTable(event.sourceHandle);
          if (table != null) {
            table.putBoolean("connected", false);
          }
          break;
        }
        case kSourceVideoModesUpdated: {
          break;
        }
        case kSourceVideoModeChanged: {
          break;
        }
        case kSourcePropertyCreated: {
          break;
        }
        case kSourcePropertyValueUpdated: {
          break;
        }
        case kSourcePropertyChoicesUpdated: {
          break;
        }
        case kSinkSourceChanged: {
          updateStreamValues();
          break;
        }
        case kSinkCreated: {
          break;
        }
        case kSinkDestroyed: {
          break;
        }
        case kSinkEnabled: {
          break;
        }
        case kSinkDisabled: {
          break;
        }
        case kNetworkInterfacesChanged: {
          m_addresses = CameraServerJNI.getNetworkInterfaces();
          break;
        }
        default:
          break;
      }
    }, 0x7fff, true);

    // Listener for NetworkTable events
    m_tableListener = NetworkTablesJNI.addEntryListener(kPublishName, (uid, key, value, flags) -> {
      if (!key.startsWith(kPublishName + "/")) {
        return;
      }
      String relativeKey = key.substring(kPublishName.length());

    }, ITable.NOTIFY_IMMEDIATE | ITable.NOTIFY_UPDATE);

  }

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * <p>You should call this method to see a camera feed on the dashboard.
   * If you also want to perform vision processing on the roboRIO, use
   * getVideo() to get access to the camera images.
   *
   * <p>This overload calls {@link #startAutomaticCapture(int)} with device 0,
   * creating a camera named "USB Camera 0".
   */
  public UsbCamera startAutomaticCapture() {
    return startAutomaticCapture(0);
  }

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * <p>This overload calls {@link #startAutomaticCapture(String, int)} with
   * a name of "USB Camera {dev}".
   *
   * @param dev The device number of the camera interface
   */
  public UsbCamera startAutomaticCapture(int dev) {
    UsbCamera camera = new UsbCamera("USB Camera " + dev, dev);
    startAutomaticCapture(camera);
    return camera;
  }

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * @param name The name to give the camera
   * @param dev The device number of the camera interface
   */
  public UsbCamera startAutomaticCapture(String name, int dev) {
    UsbCamera camera = new UsbCamera(name, dev);
    startAutomaticCapture(camera);
    return camera;
  }

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * @param name The name to give the camera
   * @param path The device path (e.g. "/dev/video0") of the camera
   */
  public UsbCamera startAutomaticCapture(String name, String path) {
    UsbCamera camera = new UsbCamera(name, path);
    startAutomaticCapture(camera);
    return camera;
  }

  /**
   * Start automatically capturing images to send to the dashboard from
   * an existing camera.
   *
   * @param camera Camera
   */
  public void startAutomaticCapture(VideoSource camera) {
    addCamera(camera);
    VideoSink server = addServer("serve_" + camera.getName());
    server.setSource(camera);
  }

  /**
   * Adds an Axis IP camera.
   *
   * <p>This overload calls {@link #addAxisCamera(String, String)} with
   * name "Axis Camera".
   *
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  public AxisCamera addAxisCamera(String host) {
    return addAxisCamera("Axis Camera", host);
  }

  /**
   * Adds an Axis IP camera.
   *
   * <p>This overload calls {@link #addAxisCamera(String, String[])} with
   * name "Axis Camera".
   *
   * @param hosts Array of Camera host IPs/DNS names
   */
  public AxisCamera addAxisCamera(String[] hosts) {
    return addAxisCamera("Axis Camera", hosts);
  }

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  public AxisCamera addAxisCamera(String name, String host) {
    AxisCamera camera = new AxisCamera(name, host);
    addCamera(camera);
    return camera;
  }

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param hosts Array of Camera host IPs/DNS names
   */
  public AxisCamera addAxisCamera(String name, String[] hosts) {
    AxisCamera camera = new AxisCamera(name, hosts);
    addCamera(camera);
    return camera;
  }

  /**
   * Get OpenCV access to the primary camera feed.  This allows you to
   * get images from the camera for image processing on the roboRIO.
   *
   * <p>This is only valid to call after a camera feed has been added
   * with startAutomaticCapture() or addServer().
   */
  public CvSink getVideo() {
    VideoSource source;
    synchronized (this) {
      if (m_primarySourceName == null) {
        throw new VideoException("no camera available");
      }
      source = m_sources.get(m_primarySourceName);
    }
    if (source == null) {
      throw new VideoException("no camera available");
    }
    return getVideo(source);
  }

  /**
   * Get OpenCV access to the specified camera.  This allows you to get
   * images from the camera for image processing on the roboRIO.
   *
   * @param camera Camera (e.g. as returned by startAutomaticCapture).
   */
  public CvSink getVideo(VideoSource camera) {
    String name = "opencv_" + camera.getName();

    synchronized (this) {
      VideoSink sink = m_sinks.get(name);
      if (sink != null) {
        VideoSink.Kind kind = sink.getKind();
        if (kind != VideoSink.Kind.kCv) {
          throw new VideoException("expected OpenCV sink, but got " + kind);
        }
        return (CvSink) sink;
      }
    }

    CvSink newsink = new CvSink(name);
    newsink.setSource(camera);
    addServer(newsink);
    return newsink;
  }

  /**
   * Get OpenCV access to the specified camera.  This allows you to get
   * images from the camera for image processing on the roboRIO.
   *
   * @param name Camera name
   */
  public CvSink getVideo(String name) {
    VideoSource source;
    synchronized (this) {
      source = m_sources.get(name);
      if (source == null) {
        throw new VideoException("could not find camera " + name);
      }
    }
    return getVideo(source);
  }

  /**
   * Create a MJPEG stream with OpenCV input. This can be called to pass custom
   * annotated images to the dashboard.
   *
   * @param name Name to give the stream
   * @param width Width of the image being sent
   * @param height Height of the image being sent
   */
  public CvSource putVideo(String name, int width, int height) {
    CvSource source = new CvSource(name, VideoMode.PixelFormat.kMJPEG, width, height, 30);
    startAutomaticCapture(source);
    return source;
  }

  /**
   * Adds a MJPEG server at the next available port.
   *
   * @param name Server name
   */
  public MjpegServer addServer(String name) {
    int port;
    synchronized (this) {
      port = m_nextPort;
      m_nextPort++;
    }
    return addServer(name, port);
  }

  /**
   * Adds a MJPEG server.
   *
   * @param name Server name
   */
  public MjpegServer addServer(String name, int port) {
    MjpegServer server = new MjpegServer(name, port);
    addServer(server);
    return server;
  }

  /**
   * Adds an already created server.
   *
   * @param server Server
   */
  public void addServer(VideoSink server) {
    synchronized (this) {
      m_sinks.put(server.getName(), server);
    }
  }

  /**
   * Removes a server by name.
   *
   * @param name Server name
   */
  public void removeServer(String name) {
    synchronized (this) {
      m_sinks.remove(name);
    }
  }

  /**
   * Adds an already created camera.
   *
   * @param camera Camera
   */
  public void addCamera(VideoSource camera) {
    String name = camera.getName();
    synchronized (this) {
      if (m_primarySourceName == null) {
        m_primarySourceName = name;
      }
      m_sources.put(name, camera);
    }
  }

  /**
   * Removes a camera by name.
   *
   * @param name Camera name
   */
  public void removeCamera(String name) {
    synchronized (this) {
      m_sources.remove(name);
    }
  }

  /**
   * Sets the size of the image to use. Use the public kSize constants to set the correct mode, or
   * set it directly on a camera and call the appropriate startAutomaticCapture method.
   *
   * @deprecated Use setResolution on the UsbCamera returned by startAutomaticCapture() instead.
   * @param size The size to use
   */
  @Deprecated
  public void setSize(int size) {
    VideoSource source = null;
    synchronized (this) {
      if (m_primarySourceName == null) {
        return;
      }
      source = m_sources.get(m_primarySourceName);
      if (source == null) {
        return;
      }
    }
    switch (size) {
      case kSize640x480:
        source.setResolution(640, 480);
        break;
      case kSize320x240:
        source.setResolution(320, 240);
        break;
      case kSize160x120:
        source.setResolution(160, 120);
        break;
      default:
        throw new IllegalArgumentException("Unsupported size: " + size);
    }
  }
}
