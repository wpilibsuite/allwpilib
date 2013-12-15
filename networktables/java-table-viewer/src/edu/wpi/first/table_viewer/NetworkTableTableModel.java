package edu.wpi.first.table_viewer;

import java.util.*;

import javax.swing.table.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.tables.*;

public class NetworkTableTableModel extends AbstractTableModel implements ITableListener {

	private final NetworkTableNode node;

	public NetworkTableTableModel(NetworkTableNode node) {
		this.node = node;
		node.addTableListener(this, true);
	}
	
	private final List<String> keys = new ArrayList<String>();

	@Override
	public void valueChanged(ITable source, String key, Object value, boolean isNew) {
		if(isNew){
			keys.add(key);
			fireTableRowsInserted(keys.size()-1, keys.size()-1);
		}
		else{
			int row = keys.indexOf(key);
			fireTableRowsUpdated(row, row);
		}
	}

	@Override
	public int getRowCount() {
		return keys.size();
	}

	@Override
	public int getColumnCount() {
		return 3;
	}
	
	@Override
	public String getColumnName(int column){
		if(column==0)
			return "Key";
		if(column==1)
			return "Value";
		return "Sequence Number";
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		if(columnIndex==0)
			return keys.get(rowIndex);
		try {
			if(columnIndex==1)
				return node.getValue(keys.get(rowIndex));
			
			return (int)node.getEntryStore().getEntry(keys.get(rowIndex)).getSequenceNumber();
		} catch (TableKeyNotDefinedException e) {
			return null;
		}
	}

}
