package org.usfirst.frc.team192.robot;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import com.ni.vision.NIVision;
import com.ni.vision.NIVision.Image;
import com.ni.vision.NIVision.RawData;
import com.ni.vision.VisionException;

//replicates CameraServer.cpp in java lib

public class CameraServer {

	public static CameraServer getInstance() {
		if (server == null) {
			server = new CameraServer();
		}
		return server;
	}

	private static CameraServer server;
	private AtomicBoolean ready;
	private Thread serverThread;
	private int m_quality;
	private static final int kPort = 1180;
	private static final byte[] kMagicNumber = { 0x01, 0x00, 0x00, 0x00 };
	private static final int kSize640x480 = 0;
	private static final int kSize320x240 = 1;
	private static final int kSize160x120 = 2;
	private static final int kHardwareCompression = -1;
	private static final String kDefaultCameraName = "cam1";
	private List<Byte> m_imageData;
	private boolean m_autoCaptureStarted;

	private CameraServer() {
		m_imageData = new ArrayList<Byte>();
		m_quality = 50;
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
		serverThread.start();
	}

	/**
	 * Manually change the image that is served by the MJPEG stream. This can be
	 * called to pass custom annotated images to the dashboard. Note that, for
	 * 640x480 video, this method could take between 40 and 50 milliseconds to
	 * complete.
	 *
	 * This shouldn't be called if {@link #StartAutomaticCapture} is called.
	 *
	 * @param image
	 *            The IMAQ image to show on the dashboard
	 */
	public synchronized void setImage(Image image) {
		// handle multi-threadedness

		/* Flatten the IMAQ image to a JPEG */
		RawData data = NIVision.imaqFlatten(image,
				NIVision.FlattenType.FLATTEN_IMAGE,
				NIVision.CompressionType.COMPRESSION_JPEG, 10 * m_quality);
		ByteBuffer buffer = data.getBuffer();

		/* Find the start of the JPEG data */
		boolean startOfImage = false;
		for (int i = 0; i < buffer.limit() - 1; i++) {
			if (buffer.get(i) == 0xFF && buffer.get(i + 1) == 0xD8) {
				startOfImage = true;
			}
			if (startOfImage) {
				m_imageData.add(buffer.get(i));
			}
		}

		if (m_imageData.size() < 2) {
			throw new VisionException(
					"data size of flattened image is less than 2. Try another camera!");
		}

		data.free();

		ready.set(true);
	}

	/**
	 * Start automatically capturing images to send to the dashboard.
	 *
	 * You should call this method to just see a camera feed on the dashboard
	 * without doing any vision processing on the roboRIO. {@link #SetImage}
	 * shouldn't be called after this is called.
	 *
	 * @param cameraName
	 *            The name of the camera interface (e.g. "cam1")
	 */
	public void startAutomaticCapture(String cameraName) {
		synchronized (this) {
			if (m_autoCaptureStarted) {
				// print
				// "you fucked up \"Automatic capture has already been started\""
				return;
			}
			m_autoCaptureStarted = true;
		}

		CaptureRunnable runnable = new CaptureRunnable(cameraName);
		Thread captureThread = new Thread(runnable);
		captureThread.start();

	}

	private class CaptureRunnable implements Runnable {
		String name;

		public CaptureRunnable(String name) {
			this.name = name;
		}

		@Override
		public void run() {
			Image frame = NIVision.imaqCreateImage(NIVision.ImageType.IMAGE_RGB, 0);
			int id = NIVision.IMAQdxOpenCamera(name, NIVision.IMAQdxCameraControlMode.CameraControlModeController);
			NIVision.IMAQdxConfigureGrab(id);
			NIVision.IMAQdxStartAcquisition(id);
			
			while (true){
				NIVision.IMAQdxGrab(id, frame, 1);
				setImage(frame);
			}
			
//			NIVision.IMAQdxStopAcquisition(id);
//			NIVision.IMAQdxCloseCamera(id);

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
	 * Set the quality of the compressed image sent to the dashboard
	 *
	 * @param quality
	 *            The quality of the JPEG image, from 0 to 100
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
	 * @throws IOException
	 * @throws InterruptedException
	 */
	protected void serve() throws IOException, InterruptedException {

		ServerSocket socket = new ServerSocket();
		socket.setReuseAddress(true);
		InetSocketAddress address = new InetSocketAddress(kPort);
		socket.bind(address);

		while (true) {
			try {
				Socket s = socket.accept();
				InputStream is = s.getInputStream();
				OutputStream os = s.getOutputStream();

				byte[] fpsData = new byte[4];
				byte[] compressionData = new byte[4];
				byte[] sizeData = new byte[4];

				is.read(fpsData);
				is.read(compressionData);
				is.read(sizeData);

				int fps = byteArrayToInt(fpsData);
				int compression = byteArrayToInt(compressionData);
				int size = byteArrayToInt(sizeData);

				if (compression != kHardwareCompression) {
					// print to driverstation -->
					// ("Choose \"USB Camera HW\" on the dashboard");
					s.close();
					continue;
				}

				long period = (long) (1000 / (1.0 * fps));

				while (true) {

					long t0 = System.currentTimeMillis();

					while (!ready.get())
						;
					ready.set(false);

					// write numbers
					try {
						os.write(kMagicNumber);

						// write size of image
						synchronized (this) {
							os.write(intTobyteArray(m_imageData.size()));

							byte[] imageData = new byte[m_imageData.size()];
							for (int i = 0; i < m_imageData.size(); i++) {
								imageData[i] = m_imageData.get(i).byteValue();
							}

							os.write(imageData);
						}
						long dt = System.currentTimeMillis() - t0;
						Thread.sleep(period - dt);

					} catch (IOException ex) {
						break;
					}
				}

			} catch (IOException ex) {
				// print error to driverstation
				continue;
			}
		}
	}

	public byte[] intTobyteArray(int n) {
		byte[] arr = new byte[4];

		ByteOrder order = ByteOrder.nativeOrder();

		if (order == ByteOrder.LITTLE_ENDIAN) {
			for (int i = 0; i < arr.length; i++) {
				arr[i] = (byte) ((n >> i * 8) & 0xFF);
			}
		} else {
			for (int i = arr.length - 1; i >= 0; i--) {
				arr[i] = (byte) ((n >> i * 8) & 0xFF);
			}
		}

		return arr;
	}

	public int byteArrayToInt(byte[] a) {
		int value = 0;

		ByteOrder order = ByteOrder.nativeOrder();

		if (order == ByteOrder.LITTLE_ENDIAN) {
			for (int i = 0; i < a.length; i++) {
				int b = (int) a[i];
				value += b << (i * 8);
			}
		} else {
			for (int i = a.length - 1; i >= 0; i--) {
				int b = (int) a[i];
				value += b << (i * 8);
			}
		}

		return value;
	}
}

