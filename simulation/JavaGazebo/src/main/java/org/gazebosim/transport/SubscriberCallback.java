package org.gazebosim.transport;

public interface SubscriberCallback<T> {
	void callback(T msg);
}
