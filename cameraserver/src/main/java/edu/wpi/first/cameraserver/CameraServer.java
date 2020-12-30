// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cameraserver;

import edu.wpi.cscore.AxisCamera;
import edu.wpi.cscore.CameraServerJNI;
import edu.wpi.cscore.CvSink;
import edu.wpi.cscore.CvSource;
import edu.wpi.cscore.MjpegServer;
import edu.wpi.cscore.UsbCamera;
import edu.wpi.cscore.VideoEvent;
import edu.wpi.cscore.VideoException;
import edu.wpi.cscore.VideoListener;
import edu.wpi.cscore.VideoMode;
import edu.wpi.cscore.VideoMode.PixelFormat;
import edu.wpi.cscore.VideoProperty;
import edu.wpi.cscore.VideoSink;
import edu.wpi.cscore.VideoSource;
import edu.wpi.first.networktables.EntryListenerFlags;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Singleton class for creating and keeping camera servers. Also publishes camera information to
 * NetworkTables.
 */
public final class CameraServer {
  public static final int kBasePort = 1181;

  @Deprecated public static final int kSize640x480 = 0;
  @Deprecated public static final int kSize320x240 = 1;
  @Deprecated public static final int kSize160x120 = 2;

  private static final String kPublishName = "/CameraPublisher";
  private static CameraServer server;

  /** Get the CameraServer instance. */
  public static synchronized CameraServer getInstance() {
    if (server == null) {
      server = new CameraServer();
    }
    return server;
  }

  private final AtomicInteger m_defaultUsbDevice;
  private String m_primarySourceName;
  private final Map<String, VideoSource> m_sources;
  private final Map<String, VideoSink> m_sinks;
  private final Map<Integer, NetworkTable> m_tables; // indexed by source handle
  // source handle indexed by sink handle
  private final Map<Integer, Integer> m_fixedSources;
  private final NetworkTable m_publishTable;
  private final VideoListener m_videoListener; // NOPMD
  private final int m_tableListener; // NOPMD
  private int m_nextPort;
  private String[] m_addresses;

  @SuppressWarnings("MissingJavadocMethod")
  private static String makeSourceValue(int source) {
    switch (VideoSource.getKindFromInt(CameraServerJNI.getSourceKind(source))) {
      case kUsb:
        return "usb:" + CameraServerJNI.getUsbCameraPath(source);
      case kHttp:
        {
          String[] urls = CameraServerJNI.getHttpCameraUrls(source);
          if (urls.length > 0) {
            return "ip:" + urls[0];
          } else {
            return "ip:";
          }
        }
      case kCv:
        return "cv:";
      default:
        return "unknown:";
    }
  }

  @SuppressWarnings("MissingJavadocMethod")
  private static String makeStreamValue(String address, int port) {
    return "mjpg:http://" + address + ":" + port + "/?action=stream";
  }

  @SuppressWarnings({"MissingJavadocMethod", "PMD.AvoidUsingHardCodedIP"})
  private synchronized String[] getSinkStreamValues(int sink) {
    // Ignore all but MjpegServer
    if (VideoSink.getKindFromInt(CameraServerJNI.getSinkKind(sink)) != VideoSink.Kind.kMjpeg) {
      return new String[0];
    }

    // Get port
    int port = CameraServerJNI.getMjpegServerPort(sink);

    // Generate values
    ArrayList<String> values = new ArrayList<>(m_addresses.length + 1);
    String listenAddress = CameraServerJNI.getMjpegServerListenAddress(sink);
    if (!listenAddress.isEmpty()) {
      // If a listen address is specified, only use that
      values.add(makeStreamValue(listenAddress, port));
    } else {
      // Otherwise generate for hostname and all interface addresses
      values.add(makeStreamValue(CameraServerJNI.getHostname() + ".local", port));
      for (String addr : m_addresses) {
        if ("127.0.0.1".equals(addr)) {
          continue; // ignore localhost
        }
        values.add(makeStreamValue(addr, port));
      }
    }

    return values.toArray(new String[0]);
  }

  @SuppressWarnings({"MissingJavadocMethod", "PMD.AvoidUsingHardCodedIP"})
  private synchronized String[] getSourceStreamValues(int source) {
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

    if (CameraServerSharedStore.getCameraServerShared().isRoboRIO()) {
      // Look to see if we have a passthrough server for this source
      // Only do this on the roboRIO
      for (VideoSink i : m_sinks.values()) {
        int sink = i.getHandle();
        int sinkSource = CameraServerJNI.getSinkSource(sink);
        if (source == sinkSource
            && VideoSink.getKindFromInt(CameraServerJNI.getSinkKind(sink))
                == VideoSink.Kind.kMjpeg) {
          // Add USB-only passthrough
          String[] finalValues = Arrays.copyOf(values, values.length + 1);
          int port = CameraServerJNI.getMjpegServerPort(sink);
          finalValues[values.length] = makeStreamValue("172.22.11.2", port);
          return finalValues;
        }
      }
    }

    return values;
  }

  @SuppressWarnings({
    "MissingJavadocMethod",
    "PMD.AvoidUsingHardCodedIP",
    "PMD.CyclomaticComplexity"
  })
  private synchronized void updateStreamValues() {
    // Over all the sinks...
    for (VideoSink i : m_sinks.values()) {
      int sink = i.getHandle();

      // Get the source's subtable (if none exists, we're done)
      int source =
          Objects.requireNonNullElseGet(
              m_fixedSources.get(sink), () -> CameraServerJNI.getSinkSource(sink));

      if (source == 0) {
        continue;
      }
      NetworkTable table = m_tables.get(source);
      if (table != null) {
        // Don't set stream values if this is a HttpCamera passthrough
        if (VideoSource.getKindFromInt(CameraServerJNI.getSourceKind(source))
            == VideoSource.Kind.kHttp) {
          continue;
        }

        // Set table value
        String[] values = getSinkStreamValues(sink);
        if (values.length > 0) {
          table.getEntry("streams").setStringArray(values);
        }
      }
    }

    // Over all the sources...
    for (VideoSource i : m_sources.values()) {
      int source = i.getHandle();

      // Get the source's subtable (if none exists, we're done)
      NetworkTable table = m_tables.get(source);
      if (table != null) {
        // Set table value
        String[] values = getSourceStreamValues(source);
        if (values.length > 0) {
          table.getEntry("streams").setStringArray(values);
        }
      }
    }
  }

  @SuppressWarnings("MissingJavadocMethod")
  private static String pixelFormatToString(PixelFormat pixelFormat) {
    switch (pixelFormat) {
      case kMJPEG:
        return "MJPEG";
      case kYUYV:
        return "YUYV";
      case kRGB565:
        return "RGB565";
      case kBGR:
        return "BGR";
      case kGray:
        return "Gray";
      default:
        return "Unknown";
    }
  }

  /// Provide string description of video mode.
  /// The returned string is "{width}x{height} {format} {fps} fps".
  @SuppressWarnings("MissingJavadocMethod")
  private static String videoModeToString(VideoMode mode) {
    return mode.width
        + "x"
        + mode.height
        + " "
        + pixelFormatToString(mode.pixelFormat)
        + " "
        + mode.fps
        + " fps";
  }

  @SuppressWarnings("MissingJavadocMethod")
  private static String[] getSourceModeValues(int sourceHandle) {
    VideoMode[] modes = CameraServerJNI.enumerateSourceVideoModes(sourceHandle);
    String[] modeStrings = new String[modes.length];
    for (int i = 0; i < modes.length; i++) {
      modeStrings[i] = videoModeToString(modes[i]);
    }
    return modeStrings;
  }

  @SuppressWarnings({"MissingJavadocMethod", "PMD.CyclomaticComplexity"})
  private static void putSourcePropertyValue(NetworkTable table, VideoEvent event, boolean isNew) {
    String name;
    String infoName;
    if (event.name.startsWith("raw_")) {
      name = "RawProperty/" + event.name;
      infoName = "RawPropertyInfo/" + event.name;
    } else {
      name = "Property/" + event.name;
      infoName = "PropertyInfo/" + event.name;
    }

    NetworkTableEntry entry = table.getEntry(name);
    try {
      switch (event.propertyKind) {
        case kBoolean:
          if (isNew) {
            entry.setDefaultBoolean(event.value != 0);
          } else {
            entry.setBoolean(event.value != 0);
          }
          break;
        case kInteger:
        case kEnum:
          if (isNew) {
            entry.setDefaultDouble(event.value);
            table
                .getEntry(infoName + "/min")
                .setDouble(CameraServerJNI.getPropertyMin(event.propertyHandle));
            table
                .getEntry(infoName + "/max")
                .setDouble(CameraServerJNI.getPropertyMax(event.propertyHandle));
            table
                .getEntry(infoName + "/step")
                .setDouble(CameraServerJNI.getPropertyStep(event.propertyHandle));
            table
                .getEntry(infoName + "/default")
                .setDouble(CameraServerJNI.getPropertyDefault(event.propertyHandle));
          } else {
            entry.setDouble(event.value);
          }
          break;
        case kString:
          if (isNew) {
            entry.setDefaultString(event.valueStr);
          } else {
            entry.setString(event.valueStr);
          }
          break;
        default:
          break;
      }
    } catch (VideoException ignored) {
      // ignore
    }
  }

  @SuppressWarnings({
    "MissingJavadocMethod",
    "PMD.UnusedLocalVariable",
    "PMD.ExcessiveMethodLength",
    "PMD.NPathComplexity"
  })
  private CameraServer() {
    m_defaultUsbDevice = new AtomicInteger();
    m_sources = new HashMap<>();
    m_sinks = new HashMap<>();
    m_fixedSources = new HashMap<>();
    m_tables = new HashMap<>();
    m_publishTable = NetworkTableInstance.getDefault().getTable(kPublishName);
    m_nextPort = kBasePort;
    m_addresses = new String[0];

    // We publish sources to NetworkTables using the following structure:
    // "/CameraPublisher/{Source.Name}/" - root
    // - "source" (string): Descriptive, prefixed with type (e.g. "usb:0")
    // - "streams" (string array): URLs that can be used to stream data
    // - "description" (string): Description of the source
    // - "connected" (boolean): Whether source is connected
    // - "mode" (string): Current video mode
    // - "modes" (string array): Available video modes
    // - "Property/{Property}" - Property values
    // - "PropertyInfo/{Property}" - Property supporting information

    // Listener for video events
    m_videoListener =
        new VideoListener(
            event -> {
              switch (event.kind) {
                case kSourceCreated:
                  {
                    // Create subtable for the camera
                    NetworkTable table = m_publishTable.getSubTable(event.name);
                    m_tables.put(event.sourceHandle, table);
                    table.getEntry("source").setString(makeSourceValue(event.sourceHandle));
                    table
                        .getEntry("description")
                        .setString(CameraServerJNI.getSourceDescription(event.sourceHandle));
                    table
                        .getEntry("connected")
                        .setBoolean(CameraServerJNI.isSourceConnected(event.sourceHandle));
                    table
                        .getEntry("streams")
                        .setStringArray(getSourceStreamValues(event.sourceHandle));
                    try {
                      VideoMode mode = CameraServerJNI.getSourceVideoMode(event.sourceHandle);
                      table.getEntry("mode").setDefaultString(videoModeToString(mode));
                      table
                          .getEntry("modes")
                          .setStringArray(getSourceModeValues(event.sourceHandle));
                    } catch (VideoException ignored) {
                      // Do nothing. Let the other event handlers update this if there is an error.
                    }
                    break;
                  }
                case kSourceDestroyed:
                  {
                    NetworkTable table = m_tables.get(event.sourceHandle);
                    if (table != null) {
                      table.getEntry("source").setString("");
                      table.getEntry("streams").setStringArray(new String[0]);
                      table.getEntry("modes").setStringArray(new String[0]);
                    }
                    break;
                  }
                case kSourceConnected:
                  {
                    NetworkTable table = m_tables.get(event.sourceHandle);
                    if (table != null) {
                      // update the description too (as it may have changed)
                      table
                          .getEntry("description")
                          .setString(CameraServerJNI.getSourceDescription(event.sourceHandle));
                      table.getEntry("connected").setBoolean(true);
                    }
                    break;
                  }
                case kSourceDisconnected:
                  {
                    NetworkTable table = m_tables.get(event.sourceHandle);
                    if (table != null) {
                      table.getEntry("connected").setBoolean(false);
                    }
                    break;
                  }
                case kSourceVideoModesUpdated:
                  {
                    NetworkTable table = m_tables.get(event.sourceHandle);
                    if (table != null) {
                      table
                          .getEntry("modes")
                          .setStringArray(getSourceModeValues(event.sourceHandle));
                    }
                    break;
                  }
                case kSourceVideoModeChanged:
                  {
                    NetworkTable table = m_tables.get(event.sourceHandle);
                    if (table != null) {
                      table.getEntry("mode").setString(videoModeToString(event.mode));
                    }
                    break;
                  }
                case kSourcePropertyCreated:
                  {
                    NetworkTable table = m_tables.get(event.sourceHandle);
                    if (table != null) {
                      putSourcePropertyValue(table, event, true);
                    }
                    break;
                  }
                case kSourcePropertyValueUpdated:
                  {
                    NetworkTable table = m_tables.get(event.sourceHandle);
                    if (table != null) {
                      putSourcePropertyValue(table, event, false);
                    }
                    break;
                  }
                case kSourcePropertyChoicesUpdated:
                  {
                    NetworkTable table = m_tables.get(event.sourceHandle);
                    if (table != null) {
                      try {
                        String[] choices =
                            CameraServerJNI.getEnumPropertyChoices(event.propertyHandle);
                        table
                            .getEntry("PropertyInfo/" + event.name + "/choices")
                            .setStringArray(choices);
                      } catch (VideoException ignored) {
                        // ignore
                      }
                    }
                    break;
                  }
                case kSinkSourceChanged:
                case kSinkCreated:
                case kSinkDestroyed:
                case kNetworkInterfacesChanged:
                  {
                    m_addresses = CameraServerJNI.getNetworkInterfaces();
                    updateStreamValues();
                    break;
                  }
                default:
                  break;
              }
            },
            0x4fff,
            true);

    // Listener for NetworkTable events
    // We don't currently support changing settings via NT due to
    // synchronization issues, so just update to current setting if someone
    // else tries to change it.
    m_tableListener =
        NetworkTableInstance.getDefault()
            .addEntryListener(
                kPublishName + "/",
                event -> {
                  String relativeKey = event.name.substring(kPublishName.length() + 1);

                  // get source (sourceName/...)
                  int subKeyIndex = relativeKey.indexOf('/');
                  if (subKeyIndex == -1) {
                    return;
                  }
                  String sourceName = relativeKey.substring(0, subKeyIndex);
                  VideoSource source = m_sources.get(sourceName);
                  if (source == null) {
                    return;
                  }

                  // get subkey
                  relativeKey = relativeKey.substring(subKeyIndex + 1);

                  // handle standard names
                  String propName;
                  if ("mode".equals(relativeKey)) {
                    // reset to current mode
                    event.getEntry().setString(videoModeToString(source.getVideoMode()));
                    return;
                  } else if (relativeKey.startsWith("Property/")) {
                    propName = relativeKey.substring(9);
                  } else if (relativeKey.startsWith("RawProperty/")) {
                    propName = relativeKey.substring(12);
                  } else {
                    return; // ignore
                  }

                  // everything else is a property
                  VideoProperty property = source.getProperty(propName);
                  switch (property.getKind()) {
                    case kNone:
                      return;
                    case kBoolean:
                      // reset to current setting
                      event.getEntry().setBoolean(property.get() != 0);
                      return;
                    case kInteger:
                    case kEnum:
                      // reset to current setting
                      event.getEntry().setDouble(property.get());
                      return;
                    case kString:
                      // reset to current setting
                      event.getEntry().setString(property.getString());
                      return;
                    default:
                      return;
                  }
                },
                EntryListenerFlags.kImmediate | EntryListenerFlags.kUpdate);
  }

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * <p>You should call this method to see a camera feed on the dashboard. If you also want to
   * perform vision processing on the roboRIO, use getVideo() to get access to the camera images.
   *
   * <p>The first time this overload is called, it calls {@link #startAutomaticCapture(int)} with
   * device 0, creating a camera named "USB Camera 0". Subsequent calls increment the device number
   * (e.g. 1, 2, etc).
   */
  public UsbCamera startAutomaticCapture() {
    UsbCamera camera = startAutomaticCapture(m_defaultUsbDevice.getAndIncrement());
    CameraServerSharedStore.getCameraServerShared().reportUsbCamera(camera.getHandle());
    return camera;
  }

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * <p>This overload calls {@link #startAutomaticCapture(String, int)} with a name of "USB Camera
   * {dev}".
   *
   * @param dev The device number of the camera interface
   */
  public UsbCamera startAutomaticCapture(int dev) {
    UsbCamera camera = new UsbCamera("USB Camera " + dev, dev);
    startAutomaticCapture(camera);
    CameraServerSharedStore.getCameraServerShared().reportUsbCamera(camera.getHandle());
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
    CameraServerSharedStore.getCameraServerShared().reportUsbCamera(camera.getHandle());
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
    CameraServerSharedStore.getCameraServerShared().reportUsbCamera(camera.getHandle());
    return camera;
  }

  /**
   * Start automatically capturing images to send to the dashboard from an existing camera.
   *
   * @param camera Camera
   */
  public MjpegServer startAutomaticCapture(VideoSource camera) {
    addCamera(camera);
    MjpegServer server = addServer("serve_" + camera.getName());
    server.setSource(camera);
    return server;
  }

  /**
   * Adds an Axis IP camera.
   *
   * <p>This overload calls {@link #addAxisCamera(String, String)} with name "Axis Camera".
   *
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  public AxisCamera addAxisCamera(String host) {
    return addAxisCamera("Axis Camera", host);
  }

  /**
   * Adds an Axis IP camera.
   *
   * <p>This overload calls {@link #addAxisCamera(String, String[])} with name "Axis Camera".
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
    // Create a passthrough MJPEG server for USB access
    startAutomaticCapture(camera);
    CameraServerSharedStore.getCameraServerShared().reportAxisCamera(camera.getHandle());
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
    // Create a passthrough MJPEG server for USB access
    startAutomaticCapture(camera);
    CameraServerSharedStore.getCameraServerShared().reportAxisCamera(camera.getHandle());
    return camera;
  }

  /**
   * Adds a virtual camera for switching between two streams. Unlike the other addCamera methods,
   * this returns a VideoSink rather than a VideoSource. Calling setSource() on the returned object
   * can be used to switch the actual source of the stream.
   */
  public MjpegServer addSwitchedCamera(String name) {
    // create a dummy CvSource
    CvSource source = new CvSource(name, VideoMode.PixelFormat.kMJPEG, 160, 120, 30);
    MjpegServer server = startAutomaticCapture(source);
    synchronized (this) {
      m_fixedSources.put(server.getHandle(), source.getHandle());
    }

    return server;
  }

  /**
   * Get OpenCV access to the primary camera feed. This allows you to get images from the camera for
   * image processing on the roboRIO.
   *
   * <p>This is only valid to call after a camera feed has been added with startAutomaticCapture()
   * or addServer().
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
   * Get OpenCV access to the specified camera. This allows you to get images from the camera for
   * image processing on the roboRIO.
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
   * Get OpenCV access to the specified camera. This allows you to get images from the camera for
   * image processing on the roboRIO.
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
   * Create a MJPEG stream with OpenCV input. This can be called to pass custom annotated images to
   * the dashboard.
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
   * Get server for the primary camera feed.
   *
   * <p>This is only valid to call after a camera feed has been added with startAutomaticCapture()
   * or addServer().
   */
  public VideoSink getServer() {
    synchronized (this) {
      if (m_primarySourceName == null) {
        throw new VideoException("no camera available");
      }
      return getServer("serve_" + m_primarySourceName);
    }
  }

  /**
   * Gets a server by name.
   *
   * @param name Server name
   */
  public VideoSink getServer(String name) {
    synchronized (this) {
      return m_sinks.get(name);
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
}
