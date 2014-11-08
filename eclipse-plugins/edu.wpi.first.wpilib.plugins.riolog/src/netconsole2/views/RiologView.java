package netconsole2.views;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;


public class RiologView extends ViewPart {
	public static Action confAction(String name, String tooltip, String img,
			Action e) {
		e.setText(name);
		e.setToolTipText(tooltip);
		e.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages()
				.getImageDescriptor(img));
		return e;
	}

	public static byte[] getPacket(DatagramSocket socket, DatagramPacket buf) {
		try {
			socket.receive(buf);
		} catch (IOException e) {
			return null;
		}
		byte[] ret = new byte[buf.getLength()];
		System.arraycopy(buf.getData(), 0, ret, 0, ret.length);
		return ret;
	}

	public static DatagramSocket makeRecvSocket() {
		DatagramSocket socket = null;
		try {
			socket = new DatagramSocket(null);
			socket.setReuseAddress(true);
			socket.bind(new InetSocketAddress(6666));
		} catch (SocketException e) {
			e.printStackTrace();
			socket.close();
			return null;
		}
		return socket;
	}

	public static Thread startDaemonThread(Runnable r, String name) {
		Thread t = new Thread(r, name);
		t.setDaemon(true);
		t.start();
		return t;
	}

	/**
	 * The ID of the view as specified by the extension.
	 */
	public static final String ID = "netconsole2.views.RiologView";

	Text text;
	Thread listener;
	Thread transferer;
	
	volatile DatagramSocket socket_hook = null;
	volatile boolean discard = false;
	volatile boolean paused = false;
	volatile boolean cleanup = false;
	
	private Action clearAction;
	private Action pauseAction;
	private Action discardAction;
	private Action unpauseAction;
	private Action undiscardAction;
	private Button discardButton;
	private Button pauseButton;

	/**
	 * The constructor.
	 */
	public RiologView() {
	}

	private void contributeToActionBars() {
		IActionBars bars = getViewSite().getActionBars();
		fillLocalPullDown(bars.getMenuManager());
		fillLocalToolBar(bars.getToolBarManager());
	}

	/**
	 * This is a callback that will allow us to create the viewer and initialize
	 * it.
	 */
	public void createPartControl(Composite parent) {
		GridLayout glayout = new GridLayout();
		glayout.numColumns = 1;
		parent.setLayout(glayout);

		text = new Text(parent, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL
				| SWT.READ_ONLY);
		{
			GridData gdata = new GridData();
			gdata.grabExcessVerticalSpace = true;
			gdata.grabExcessHorizontalSpace = true;
			gdata.horizontalAlignment = SWT.FILL;
			gdata.verticalAlignment = SWT.FILL;
			text.setLayoutData(gdata);
		}

		Composite row = new Composite(parent, SWT.NONE);

		row.setLayout(new FillLayout(SWT.HORIZONTAL));

		{
			GridData gdata = new GridData();
			gdata.grabExcessVerticalSpace = false;
			gdata.grabExcessHorizontalSpace = true;
			gdata.horizontalAlignment = SWT.FILL;
			gdata.verticalAlignment = SWT.CENTER;
			row.setLayoutData(gdata);
		}



		// Create the help context id for the viewer's control
		PlatformUI.getWorkbench().getHelpSystem()
				.setHelp(parent, "netconsole2.text");
		makeActions();
		makeButtons(row);
		hookContextMenu();
		contributeToActionBars();
		startListening();
	}

	public void makeButtons(Composite parent) {
		pauseButton = new Button(parent, SWT.TOGGLE);
		pauseButton.setText(pauseAction.getText());
		pauseButton.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				if (pauseButton.getSelection()) {
					pauseAction.run();
				} else {
					unpauseAction.run();
				}
			}
		});
		discardButton = new Button(parent, SWT.TOGGLE);
		discardButton.setText(discardAction.getText());
		discardButton.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				if (discardButton.getSelection()) {
					discardAction.run();
				} else {
					undiscardAction.run();
				}
			}
		});
		Button clearButton = new Button(parent, SWT.PUSH);
		clearButton.setText(clearAction.getText());
		clearButton.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				clearAction.run();
			}
		});
	}
	
	@Override
	public void dispose() {
		stopListening();
		super.dispose();
	}

	private void fillContextMenu(IMenuManager manager) {
		manager.add(pauseAction);
		manager.add(unpauseAction);
		manager.add(new Separator());
		manager.add(clearAction);
		manager.add(new Separator());
		manager.add(discardAction);
		manager.add(undiscardAction);
		manager.add(new Separator());
		manager.add(confAction("Copy", "Copy selected text",
				ISharedImages.IMG_TOOL_COPY, new Action() {
					@Override
					public void run() {
						text.copy();
					}
				}));
		manager.add(confAction("Select All", "Select all text", "",
				new Action() {
					@Override
					public void run() {
						text.selectAll();
					}
				}));
	}

	private void fillLocalPullDown(IMenuManager manager) {
		manager.add(pauseAction);
		manager.add(unpauseAction);
		manager.add(new Separator());
		manager.add(clearAction);
		manager.add(new Separator());
		manager.add(discardAction);
		manager.add(undiscardAction);
	}

	private void fillLocalToolBar(IToolBarManager manager) {
		manager.add(pauseAction);
		manager.add(unpauseAction);
		manager.add(new Separator());
		manager.add(clearAction);
		manager.add(new Separator());
		manager.add(discardAction);
		manager.add(undiscardAction);
	}

	private void hookContextMenu() {
		MenuManager menuMgr = new MenuManager("#PopupMenu");
		menuMgr.setRemoveAllWhenShown(true);
		menuMgr.addMenuListener(new IMenuListener() {
			public void menuAboutToShow(IMenuManager manager) {
				RiologView.this.fillContextMenu(manager);
			}
		});
		Menu menu = menuMgr.createContextMenu(text);
		text.setMenu(menu);
	}

	private void makeActions() {
		clearAction = confAction("Clear Log", "Empty the textbox",
				ISharedImages.IMG_ETOOL_CLEAR, new Action() {
					public void run() {
						text.setText("");
					}
				});

		pauseAction = confAction("Pause Display",
				"Stop adding packets to the textbox",
				ISharedImages.IMG_ELCL_STOP, new Action() {
					public void run() {
						pauseAction.setEnabled(false);
						unpauseAction.setEnabled(true);
						pauseButton.setSelection(true);
						pauseButton.setText("Show 0 Packets");
						paused = true;
					}
				});
		pauseAction.setEnabled(true);
		unpauseAction = confAction("Continue Display",
				"Continue adding packets to the textbox",
				ISharedImages.IMG_TOOL_FORWARD, new Action() {
					public void run() {
						paused = false;
						transferer.interrupt();
						pauseAction.setEnabled(true);
						unpauseAction.setEnabled(false);
						pauseButton.setSelection(false);
						pauseButton.setText(pauseAction.getText());
					}
				});
		unpauseAction.setEnabled(false);

		discardAction = confAction("Discard Incoming",
				"Drop all incoming packets", ISharedImages.IMG_ETOOL_DELETE,
				new Action() {
					public void run() {
						discard = true;
						discardAction.setEnabled(false);
						undiscardAction.setEnabled(true);
						discardButton.setSelection(true);
						discardButton.setText(undiscardAction.getText());
					}
				});
		discardAction.setEnabled(true);
		undiscardAction = confAction("Accept Incoming",
				"Accept all incoming packets", ISharedImages.IMG_OBJ_ADD,
				new Action() {
					public void run() {
						discard = false;
						discardAction.setEnabled(true);
						undiscardAction.setEnabled(false);
						discardButton.setSelection(false);
						discardButton.setText(discardAction.getText());
					}
				});
		undiscardAction.setEnabled(false);
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		text.setFocus();
	}
	
	public static String drainToString(ArrayList<byte[]> arr) {
		int netlength = 0;
		for (byte[] b : arr) {
			netlength += b.length;
		}

		byte[] sum = new byte[netlength];
		int mark = 0;
		for (int i=0;i<arr.size();i++) {
			byte[] b = arr.get(i);
			System.arraycopy(b, 0, sum, mark, b.length);
			arr.set(i, null);
			mark += b.length;
		}
		arr.clear();
		return new String(sum);
		
	}

	void startListening() {
		final BlockingQueue<byte[]> queue = new LinkedBlockingQueue<>();
		listener = startDaemonThread(new Runnable() {
			@Override
			public void run() {
				DatagramSocket socket = makeRecvSocket();
				if (socket == null)
					return;
				socket_hook = socket;
				byte[] buf = new byte[4096];
				DatagramPacket datagram = new DatagramPacket(buf, buf.length);
				while (!Thread.interrupted()) {
					byte[] s = getPacket(socket, datagram);
					if (s != null && !discard) {
						try {
							queue.put(s);
						} catch (InterruptedException e) {
							socket.close();
							return;
						}
					}
				}
				socket.close();
			}
		}, "Riolog-Listener");
		transferer = startDaemonThread(new Runnable() {
			@Override
			public void run() {
				final ArrayList<byte[]> temp = new ArrayList<>();
				while (!cleanup) {
					try {
						temp.add(queue.take());
					} catch (InterruptedException e) {
						if (cleanup) {
							return;
						}
					}
					queue.drainTo(temp);
					if (!paused) {
						Display.getDefault().syncExec(new Runnable() {
							@Override
							public void run() {
								if (text.isDisposed())
									return;
								text.append(drainToString(temp));
							}
						});
					} else {
						Display.getDefault().syncExec(new Runnable() {
							@Override
							public void run() {
								if (paused) {
									if (temp.size() == 1) {
										pauseButton.setText("Show 1 Packet\u2002");
									} else {
										pauseButton.setText("Show " + String.valueOf(temp.size()) + " Packets");
									}
								}
							}
						});
					}
				}
			}
		}, "Riolog-Transfer");
	}

	void stopListening() {
		cleanup = true;
		if (socket_hook != null) {
			socket_hook.close();
		}
		listener.interrupt();
		transferer.interrupt();
	}
}