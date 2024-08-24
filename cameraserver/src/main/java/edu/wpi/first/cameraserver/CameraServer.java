// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cameraserver;

import edu.wpi.first.cscore.AxisCamera;
import edu.wpi.first.cscore.CameraServerJNI;
import edu.wpi.first.cscore.CvSink;
import edu.wpi.first.cscore.CvSource;
import edu.wpi.first.cscore.MjpegServer;
import edu.wpi.first.cscore.UsbCamera;
import edu.wpi.first.cscore.VideoEvent;
import edu.wpi.first.cscore.VideoException;
import edu.wpi.first.cscore.VideoListener;
import edu.wpi.first.cscore.VideoMode;
import edu.wpi.first.cscore.VideoSink;
import edu.wpi.first.cscore.VideoSource;
import edu.wpi.first.networktables.BooleanEntry;
import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.IntegerEntry;
import edu.wpi.first.networktables.IntegerPublisher;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.networktables.StringArrayPublisher;
import edu.wpi.first.networktables.StringArrayTopic;
import edu.wpi.first.networktables.StringEntry;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.util.PixelFormat;
import java.lang.ref.Reference;
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
  /** CameraServer base port. */
  public static final int kBasePort = 1181;

  private static final String kPublishName = "/CameraPublisher";

  private static final class PropertyPublisher implements AutoCloseable {
    @SuppressWarnings({"PMD.MissingBreakInSwitch", "PMD.ImplicitSwitchFallThrough", "fallthrough"})
    PropertyPublisher(NetworkTable table, VideoEvent event) {
      String name;
      String infoName;
      if (event.name.startsWith("raw_")) {
        name = "RawProperty/" + event.name;
        infoName = "RawPropertyInfo/" + event.name;
      } else {
        name = "Property/" + event.name;
        infoName = "PropertyInfo/" + event.name;
      }

      try {
        switch (event.propertyKind) {
          case kBoolean:
            m_booleanValueEntry = table.getBooleanTopic(name).getEntry(false);
            m_booleanValueEntry.setDefault(event.value != 0);
            break;
          case kEnum:
            m_choicesTopic = table.getStringArrayTopic(infoName + "/choices");
            // fall through
          case kInteger:
            m_integerValueEntry = table.getIntegerTopic(name).getEntry(0);
            m_minPublisher = table.getIntegerTopic(infoName + "/min").publish();
            m_maxPublisher = table.getIntegerTopic(infoName + "/max").publish();
            m_stepPublisher = table.getIntegerTopic(infoName + "/step").publish();
            m_defaultPublisher = table.getIntegerTopic(infoName + "/default").publish();

            m_integerValueEntry.setDefault(event.value);
            m_minPublisher.set(CameraServerJNI.getPropertyMin(event.propertyHandle));
            m_maxPublisher.set(CameraServerJNI.getPropertyMax(event.propertyHandle));
            m_stepPublisher.set(CameraServerJNI.getPropertyStep(event.propertyHandle));
            m_defaultPublisher.set(CameraServerJNI.getPropertyDefault(event.propertyHandle));
            break;
          case kString:
            m_stringValueEntry = table.getStringTopic(name).getEntry("");
            m_stringValueEntry.setDefault(event.valueStr);
            break;
          default:
            break;
        }
      } catch (VideoException ignored) {
        // ignore
      }
    }

    void update(VideoEvent event) {
      switch (event.propertyKind) {
        case kBoolean -> {
          if (m_booleanValueEntry != null) {
            m_booleanValueEntry.set(event.value != 0);
          }
        }
        case kInteger, kEnum -> {
          if (m_integerValueEntry != null) {
            m_integerValueEntry.set(event.value);
          }
        }
        case kString -> {
          if (m_stringValueEntry != null) {
            m_stringValueEntry.set(event.valueStr);
          }
        }
        default -> {
          // NOP
        }
      }
    }

    @Override
    public void close() {
      try {
        if (m_booleanValueEntry != null) {
          m_booleanValueEntry.close();
        }
        if (m_integerValueEntry != null) {
          m_integerValueEntry.close();
        }
        if (m_stringValueEntry != null) {
          m_stringValueEntry.close();
        }
        if (m_minPublisher != null) {
          m_minPublisher.close();
        }
        if (m_maxPublisher != null) {
          m_maxPublisher.close();
        }
        if (m_stepPublisher != null) {
          m_stepPublisher.close();
        }
        if (m_defaultPublisher != null) {
          m_defaultPublisher.close();
        }
        if (m_choicesPublisher != null) {
          m_choicesPublisher.close();
        }
      } finally {
        Reference.reachabilityFence(m_videoListener);
      }
    }

    BooleanEntry m_booleanValueEntry;
    IntegerEntry m_integerValueEntry;
    StringEntry m_stringValueEntry;
    IntegerPublisher m_minPublisher;
    IntegerPublisher m_maxPublisher;
    IntegerPublisher m_stepPublisher;
    IntegerPublisher m_defaultPublisher;
    StringArrayTopic m_choicesTopic;
    StringArrayPublisher m_choicesPublisher;
  }

  private static final class SourcePublisher implements AutoCloseable {
    SourcePublisher(NetworkTable table, int sourceHandle) {
      this.m_table = table;
      m_sourcePublisher = table.getStringTopic("source").publish();
      m_descriptionPublisher = table.getStringTopic("description").publish();
      m_connectedPublisher = table.getBooleanTopic("connected").publish();
      m_streamsPublisher = table.getStringArrayTopic("streams").publish();
      m_modeEntry = table.getStringTopic("mode").getEntry("");
      m_modesPublisher = table.getStringArrayTopic("modes").publish();

      m_sourcePublisher.set(makeSourceValue(sourceHandle));
      m_descriptionPublisher.set(CameraServerJNI.getSourceDescription(sourceHandle));
      m_connectedPublisher.set(CameraServerJNI.isSourceConnected(sourceHandle));
      m_streamsPublisher.set(getSourceStreamValues(sourceHandle));

      try {
        VideoMode mode = CameraServerJNI.getSourceVideoMode(sourceHandle);
        m_modeEntry.setDefault(videoModeToString(mode));
        m_modesPublisher.set(getSourceModeValues(sourceHandle));
      } catch (VideoException ignored) {
        // Do nothing. Let the other event handlers update this if there is an error.
      }
    }

    @Override
    public void close() throws Exception {
      m_sourcePublisher.close();
      m_descriptionPublisher.close();
      m_connectedPublisher.close();
      m_streamsPublisher.close();
      m_modeEntry.close();
      m_modesPublisher.close();
      for (PropertyPublisher pp : m_properties.values()) {
        pp.close();
      }
    }

    final NetworkTable m_table;
    final StringPublisher m_sourcePublisher;
    final StringPublisher m_descriptionPublisher;
    final BooleanPublisher m_connectedPublisher;
    final StringArrayPublisher m_streamsPublisher;
    final StringEntry m_modeEntry;
    final StringArrayPublisher m_modesPublisher;
    final Map<Integer, PropertyPublisher> m_properties = new HashMap<>();
  }

  private static final AtomicInteger m_defaultUsbDevice = new AtomicInteger();
  private static String m_primarySourceName;
  private static final Map<String, VideoSource> m_sources = new HashMap<>();
  private static final Map<String, VideoSink> m_sinks = new HashMap<>();
  private static final Map<Integer, SourcePublisher> m_publishers =
      new HashMap<>(); // indexed by source handle
  // source handle indexed by sink handle
  private static final Map<Integer, Integer> m_fixedSources = new HashMap<>();
  private static final NetworkTable m_publishTable =
      NetworkTableInstance.getDefault().getTable(kPublishName);

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
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  private static final VideoListener m_videoListener =
      new VideoListener(
          event -> {
            synchronized (CameraServer.class) {
              switch (event.kind) {
                case kSourceCreated -> {
                  // Create subtable for the camera
                  NetworkTable table = m_publishTable.getSubTable(event.name);
                  m_publishers.put(
                      event.sourceHandle, new SourcePublisher(table, event.sourceHandle));
                }
                case kSourceDestroyed -> {
                  SourcePublisher publisher = m_publishers.remove(event.sourceHandle);
                  if (publisher != null) {
                    try {
                      publisher.close();
                    } catch (Exception e) {
                      // ignore (nothing we can do about it)
                    }
                  }
                }
                case kSourceConnected -> {
                  SourcePublisher publisher = m_publishers.get(event.sourceHandle);
                  if (publisher != null) {
                    // update the description too (as it may have changed)
                    publisher.m_descriptionPublisher.set(
                        CameraServerJNI.getSourceDescription(event.sourceHandle));
                    publisher.m_connectedPublisher.set(true);
                  }
                }
                case kSourceDisconnected -> {
                  SourcePublisher publisher = m_publishers.get(event.sourceHandle);
                  if (publisher != null) {
                    publisher.m_connectedPublisher.set(false);
                  }
                }
                case kSourceVideoModesUpdated -> {
                  SourcePublisher publisher = m_publishers.get(event.sourceHandle);
                  if (publisher != null) {
                    publisher.m_modesPublisher.set(getSourceModeValues(event.sourceHandle));
                  }
                }
                case kSourceVideoModeChanged -> {
                  SourcePublisher publisher = m_publishers.get(event.sourceHandle);
                  if (publisher != null) {
                    publisher.m_modeEntry.set(videoModeToString(event.mode));
                  }
                }
                case kSourcePropertyCreated -> {
                  SourcePublisher publisher = m_publishers.get(event.sourceHandle);
                  if (publisher != null) {
                    publisher.m_properties.put(
                        event.propertyHandle, new PropertyPublisher(publisher.m_table, event));
                  }
                }
                case kSourcePropertyValueUpdated -> {
                  SourcePublisher publisher = m_publishers.get(event.sourceHandle);
                  if (publisher != null) {
                    PropertyPublisher pp = publisher.m_properties.get(event.propertyHandle);
                    if (pp != null) {
                      pp.update(event);
                    }
                  }
                }
                case kSourcePropertyChoicesUpdated -> {
                  SourcePublisher publisher = m_publishers.get(event.sourceHandle);
                  if (publisher != null) {
                    PropertyPublisher pp = publisher.m_properties.get(event.propertyHandle);
                    if (pp != null && pp.m_choicesTopic != null) {
                      try {
                        String[] choices =
                            CameraServerJNI.getEnumPropertyChoices(event.propertyHandle);
                        if (pp.m_choicesPublisher == null) {
                          pp.m_choicesPublisher = pp.m_choicesTopic.publish();
                        }
                        pp.m_choicesPublisher.set(choices);
                      } catch (VideoException ignored) {
                        // ignore (just don't publish choices if we can't get them)
                      }
                    }
                  }
                }
                case kSinkSourceChanged,
                    kSinkCreated,
                    kSinkDestroyed,
                    kNetworkInterfacesChanged -> {
                  m_addresses = CameraServerJNI.getNetworkInterfaces();
                  updateStreamValues();
                }
                default -> {
                  // NOP
                }
              }
            }
          },
          0x4fff,
          true);

  private static int m_nextPort = kBasePort;
  private static String[] m_addresses = new String[0];

  /**
   * Return URI of source with the given index.
   *
   * @param source Source index.
   */
  private static String makeSourceValue(int source) {
    return switch (VideoSource.getKindFromInt(CameraServerJNI.getSourceKind(source))) {
      case kUsb -> "usb:" + CameraServerJNI.getUsbCameraPath(source);
      case kHttp -> {
        String[] urls = CameraServerJNI.getHttpCameraUrls(source);
        if (urls.length > 0) {
          yield "ip:" + urls[0];
        } else {
          yield "ip:";
        }
      }
      case kCv -> "cv:";
      case kRaw -> "raw:";
      case kUnknown -> "unknown:";
    };
  }

  /**
   * Return URI of stream with the given address and port.
   *
   * @param address Stream IP address.
   * @param port Stream remote port.
   */
  private static String makeStreamValue(String address, int port) {
    return "mjpg:http://" + address + ":" + port + "/?action=stream";
  }

  /**
   * Return URI of sink stream with the given index.
   *
   * @param sink Sink index.
   */
  private static synchronized String[] getSinkStreamValues(int sink) {
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

  /**
   * Return list of stream source URIs for the given source index.
   *
   * @param source Source index.
   */
  private static synchronized String[] getSourceStreamValues(int source) {
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

  /** Update list of stream URIs. */
  private static synchronized void updateStreamValues() {
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
      SourcePublisher publisher = m_publishers.get(source);
      if (publisher != null) {
        // Don't set stream values if this is a HttpCamera passthrough
        if (VideoSource.getKindFromInt(CameraServerJNI.getSourceKind(source))
            == VideoSource.Kind.kHttp) {
          continue;
        }

        // Set table value
        String[] values = getSinkStreamValues(sink);
        if (values.length > 0) {
          publisher.m_streamsPublisher.set(values);
        }
      }
    }

    // Over all the sources...
    for (VideoSource i : m_sources.values()) {
      int source = i.getHandle();

      // Get the source's subtable (if none exists, we're done)
      SourcePublisher publisher = m_publishers.get(source);
      if (publisher != null) {
        // Set table value
        String[] values = getSourceStreamValues(source);
        if (values.length > 0) {
          publisher.m_streamsPublisher.set(values);
        }
      }
    }
  }

  /** Provide string description of pixel format. */
  private static String pixelFormatToString(PixelFormat pixelFormat) {
    return switch (pixelFormat) {
      case kMJPEG -> "MJPEG";
      case kYUYV -> "YUYV";
      case kRGB565 -> "RGB565";
      case kBGR -> "BGR";
      case kBGRA -> "BGRA";
      case kGray -> "Gray";
      case kY16 -> "Y16";
      case kUYVY -> "UYVY";
      case kUnknown -> "Unknown";
    };
  }

  /**
   * Provide string description of video mode.
   *
   * <p>The returned string is "{width}x{height} {format} {fps} fps".
   */
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

  /**
   * Get list of video modes for the given source handle.
   *
   * @param sourceHandle Source handle.
   */
  private static String[] getSourceModeValues(int sourceHandle) {
    VideoMode[] modes = CameraServerJNI.enumerateSourceVideoModes(sourceHandle);
    String[] modeStrings = new String[modes.length];
    for (int i = 0; i < modes.length; i++) {
      modeStrings[i] = videoModeToString(modes[i]);
    }
    return modeStrings;
  }

  private CameraServer() {}

  /**
   * Start automatically capturing images to send to the dashboard.
   *
   * <p>You should call this method to see a camera feed on the dashboard. If you also want to
   * perform vision processing on the roboRIO, use getVideo() to get access to the camera images.
   *
   * <p>The first time this overload is called, it calls {@link #startAutomaticCapture(int)} with
   * device 0, creating a camera named "USB Camera 0". Subsequent calls increment the device number
   * (e.g. 1, 2, etc).
   *
   * @return The USB camera capturing images.
   */
  public static UsbCamera startAutomaticCapture() {
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
   * @return The USB camera capturing images.
   */
  public static UsbCamera startAutomaticCapture(int dev) {
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
   * @return The USB camera capturing images.
   */
  public static UsbCamera startAutomaticCapture(String name, int dev) {
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
   * @return The USB camera capturing images.
   */
  public static UsbCamera startAutomaticCapture(String name, String path) {
    UsbCamera camera = new UsbCamera(name, path);
    startAutomaticCapture(camera);
    CameraServerSharedStore.getCameraServerShared().reportUsbCamera(camera.getHandle());
    return camera;
  }

  /**
   * Start automatically capturing images to send to the dashboard from an existing camera.
   *
   * @param camera Camera
   * @return The MJPEG server serving images from the given camera.
   */
  public static MjpegServer startAutomaticCapture(VideoSource camera) {
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
   * @return The Axis camera capturing images.
   * @deprecated Call startAutomaticCapture with a HttpCamera instead.
   */
  @Deprecated(forRemoval = true, since = "2025")
  @SuppressWarnings("removal")
  public static AxisCamera addAxisCamera(String host) {
    return addAxisCamera("Axis Camera", host);
  }

  /**
   * Adds an Axis IP camera.
   *
   * <p>This overload calls {@link #addAxisCamera(String, String[])} with name "Axis Camera".
   *
   * @param hosts Array of Camera host IPs/DNS names
   * @return The Axis camera capturing images.
   * @deprecated Call startAutomaticCapture with a HttpCamera instead.
   */
  @Deprecated(forRemoval = true, since = "2025")
  @SuppressWarnings("removal")
  public static AxisCamera addAxisCamera(String[] hosts) {
    return addAxisCamera("Axis Camera", hosts);
  }

  /**
   * Adds an Axis IP camera.
   *
   * @param name The name to give the camera
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   * @return The Axis camera capturing images.
   * @deprecated Call startAutomaticCapture with a HttpCamera instead.
   */
  @Deprecated(forRemoval = true, since = "2025")
  @SuppressWarnings("removal")
  public static AxisCamera addAxisCamera(String name, String host) {
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
   * @return The Axis camera capturing images.
   * @deprecated Call startAutomaticCapture with a HttpCamera instead.
   */
  @Deprecated(forRemoval = true, since = "2025")
  @SuppressWarnings("removal")
  public static AxisCamera addAxisCamera(String name, String[] hosts) {
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
   *
   * @param name The name to give the camera
   * @return The MJPEG server serving images from the given camera.
   */
  public static MjpegServer addSwitchedCamera(String name) {
    // create a dummy CvSource
    CvSource source = new CvSource(name, PixelFormat.kMJPEG, 160, 120, 30);
    MjpegServer server = startAutomaticCapture(source);
    synchronized (CameraServer.class) {
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
   *
   * @return OpenCV sink for the primary camera feed
   */
  public static CvSink getVideo() {
    VideoSource source;
    synchronized (CameraServer.class) {
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
   * @return OpenCV sink for the specified camera
   */
  public static CvSink getVideo(VideoSource camera) {
    String name = "opencv_" + camera.getName();

    synchronized (CameraServer.class) {
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
   * @param camera Camera (e.g. as returned by startAutomaticCapture).
   * @param pixelFormat Desired pixelFormat of the camera
   * @return OpenCV sink for the specified camera
   */
  public static CvSink getVideo(VideoSource camera, PixelFormat pixelFormat) {
    String name = "opencv_" + camera.getName();

    synchronized (CameraServer.class) {
      VideoSink sink = m_sinks.get(name);
      if (sink != null) {
        VideoSink.Kind kind = sink.getKind();
        if (kind != VideoSink.Kind.kCv) {
          throw new VideoException("expected OpenCV sink, but got " + kind);
        }
        return (CvSink) sink;
      }
    }

    CvSink newsink = new CvSink(name, pixelFormat);
    newsink.setSource(camera);
    addServer(newsink);
    return newsink;
  }

  /**
   * Get OpenCV access to the specified camera. This allows you to get images from the camera for
   * image processing on the roboRIO.
   *
   * @param name Camera name
   * @return OpenCV sink for the specified camera
   */
  public static CvSink getVideo(String name) {
    VideoSource source;
    synchronized (CameraServer.class) {
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
   * @return OpenCV source for the MJPEG stream
   */
  public static CvSource putVideo(String name, int width, int height) {
    CvSource source = new CvSource(name, PixelFormat.kMJPEG, width, height, 30);
    startAutomaticCapture(source);
    return source;
  }

  /**
   * Adds a MJPEG server at the next available port.
   *
   * @param name Server name
   * @return The MJPEG server
   */
  public static MjpegServer addServer(String name) {
    int port;
    synchronized (CameraServer.class) {
      port = m_nextPort;
      m_nextPort++;
    }
    return addServer(name, port);
  }

  /**
   * Adds a MJPEG server.
   *
   * @param name Server name
   * @param port Server port
   * @return The MJPEG server
   */
  public static MjpegServer addServer(String name, int port) {
    MjpegServer server = new MjpegServer(name, port);
    addServer(server);
    return server;
  }

  /**
   * Adds an already created server.
   *
   * @param server Server
   */
  public static void addServer(VideoSink server) {
    synchronized (CameraServer.class) {
      m_sinks.put(server.getName(), server);
    }
  }

  /**
   * Removes a server by name.
   *
   * @param name Server name
   */
  public static void removeServer(String name) {
    synchronized (CameraServer.class) {
      m_sinks.remove(name);
    }
  }

  /**
   * Get server for the primary camera feed.
   *
   * <p>This is only valid to call after a camera feed has been added with startAutomaticCapture()
   * or addServer().
   *
   * @return The server for the primary camera feed
   */
  public static VideoSink getServer() {
    synchronized (CameraServer.class) {
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
   * @return The server
   */
  public static VideoSink getServer(String name) {
    synchronized (CameraServer.class) {
      return m_sinks.get(name);
    }
  }

  /**
   * Adds an already created camera.
   *
   * @param camera Camera
   */
  public static void addCamera(VideoSource camera) {
    String name = camera.getName();
    synchronized (CameraServer.class) {
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
  public static void removeCamera(String name) {
    synchronized (CameraServer.class) {
      m_sources.remove(name);
    }
  }
}
