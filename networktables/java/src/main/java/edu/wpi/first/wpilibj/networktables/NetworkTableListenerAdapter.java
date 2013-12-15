package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.tables.*;

/**
 * An adapter that is used to filter value change notifications and make the path relative to the NetworkTable
 * 
 * @author Mitchell
 *
 */
public class NetworkTableListenerAdapter implements ITableListener {
	
	private final ITableListener targetListener;
	private final ITable targetSource;
	private final String prefix;

	/**
	 * Create a new adapter
	 * @param prefix the prefix that will be filtered/removed from the beginning of the key
	 * @param targetSource the source that events passed to the target listener will appear to come from
	 * @param targetListener the listener where events are forwarded to
	 */
	public NetworkTableListenerAdapter(String prefix, ITable targetSource, ITableListener targetListener){
		this.prefix = prefix;
		this.targetSource = targetSource;
		this.targetListener = targetListener;
	}

	public void valueChanged(ITable source, String key, Object value, boolean isNew) {//TODO use string cache
		if(key.startsWith(prefix)){
			String relativeKey = key.substring(prefix.length());
			if(contains(relativeKey, NetworkTable.PATH_SEPARATOR))
				return;
			targetListener.valueChanged(targetSource, relativeKey, value, isNew);
		}
	}
	private static boolean contains(String source, char target){
		for(int i = 0; i<source.length(); ++i)
			if(source.charAt(i)==target)
				return true;
		return false;
	}

}
