package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.tables.*;

/**
 * An adapter that is used to filter value change notifications for a specific key
 * 
 * @author Mitchell
 *
 */
public class NetworkTableKeyListenerAdapter implements ITableListener {
	
	private final ITableListener targetListener;
	private final NetworkTable targetSource;
	private final String relativeKey;
	private final String fullKey;

	/**
	 * Create a new adapter
	 * @param relativeKey the name of the key relative to the table (this is what the listener will receiver as the key)
	 * @param fullKey the full name of the key in the {@link NetworkTableNode}
	 * @param targetSource the source that events passed to the target listener will appear to come from
	 * @param targetListener the listener where events are forwarded to
	 */
	public NetworkTableKeyListenerAdapter(String relativeKey, String fullKey, NetworkTable targetSource, ITableListener targetListener){
		this.relativeKey = relativeKey;
		this.fullKey = fullKey;
		this.targetSource = targetSource;
		this.targetListener = targetListener;
	}

	public void valueChanged(ITable source, String key, Object value, boolean isNew) {
		if(key.equals(fullKey)){
			targetListener.valueChanged(targetSource, relativeKey, value, isNew);
		}
	}

}
