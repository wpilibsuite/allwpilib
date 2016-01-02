/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation.ds;

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DragGestureEvent;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragSource;
import java.awt.dnd.DragSourceDragEvent;
import java.awt.dnd.DragSourceDropEvent;
import java.awt.dnd.DragSourceEvent;
import java.awt.dnd.DragSourceListener;
import java.util.List;

import javax.swing.DropMode;
import javax.swing.JList;
import javax.swing.TransferHandler;

import edu.wpi.first.wpilibj.simulation.ds.ISimJoystick;
import edu.wpi.first.wpilibj.simulation.ds.JoystickProvider;

@SuppressWarnings("serial")
public class JoystickList extends JList<ISimJoystick> {
	private JoystickProvider joystickProvider;
	List<ISimJoystick> joysticks;

	public JoystickList(JoystickProvider joystickProvider) {
		super();
		this.joystickProvider = joystickProvider;

		setDragEnabled(true);
		setDropMode(DropMode.INSERT);

		setTransferHandler(new DropHandler(this));
		new DragListener(this);
	}

	public void moveElement(int index, int dropTargetIndex) {
		ISimJoystick move = joysticks.get(index);
		joysticks.add(dropTargetIndex, move);
		joysticks.remove(index < dropTargetIndex ? index : index + 1);
		setListData(joysticks);
	}

	public void setListData(List<ISimJoystick> sticks) {
		joysticks = sticks;
		setListData(sticks.toArray(new ISimJoystick[0]));
		joystickProvider.setJoysticks(sticks);
	}

	class DragListener implements DragSourceListener, DragGestureListener {
		JoystickList list;

		DragSource ds = new DragSource();

		public DragListener(JoystickList list) {
			this.list = list;
			ds.createDefaultDragGestureRecognizer(
					list, DnDConstants.ACTION_MOVE, this);
		}

		public void dragGestureRecognized(DragGestureEvent dge) {
			StringSelection transferable = new StringSelection(
					Integer.toString(list.getSelectedIndex()));
			ds.startDrag(dge, DragSource.DefaultCopyDrop, transferable, this);
		}

		public void dragEnter(DragSourceDragEvent dsde) {}
		public void dragExit(DragSourceEvent dse) {}
		public void dragOver(DragSourceDragEvent dsde) {}
		public void dragDropEnd(DragSourceDropEvent dsde) {}
		public void dropActionChanged(DragSourceDragEvent dsde) {}
	}

	class DropHandler extends TransferHandler {
		JoystickList list;

		public DropHandler(JoystickList list) {
			this.list = list;
		}

		public boolean canImport(TransferHandler.TransferSupport support) {
			if (!support.isDataFlavorSupported(DataFlavor.stringFlavor)) {
				return false;
			}
			JList.DropLocation dl = (JList.DropLocation) support
					.getDropLocation();
			if (dl.getIndex() == -1) {
				return false;
			} else {
				return true;
			}
		}

		public boolean importData(TransferHandler.TransferSupport support) {
			if (!canImport(support)) {
				return false;
			}

			Transferable transferable = support.getTransferable();
			String indexString;
			try {
				indexString = (String) transferable.getTransferData(DataFlavor.stringFlavor);
			} catch (Exception e) {
				return false;
			}

			int index = Integer.parseInt(indexString);
			JList.DropLocation dl = (JList.DropLocation) support.getDropLocation();
			int dropTargetIndex = dl.getIndex();
			
			list.moveElement(index, dropTargetIndex);

			return true;
		}
	}
}
