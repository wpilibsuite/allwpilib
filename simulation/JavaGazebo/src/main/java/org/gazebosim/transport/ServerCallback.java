package org.gazebosim.transport;

import java.io.IOException;

public interface ServerCallback {
	void handle(Connection conn) throws IOException;
}
