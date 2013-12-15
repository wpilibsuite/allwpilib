package edu.wpi.first.wpilib.plugins.core.ant;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Status;

public class AntPropertiesParser {
	List<InputStream> files;

	public AntPropertiesParser(List<InputStream> files) {
		this.files = files;
	}
	
	@SuppressWarnings("serial")
	public AntPropertiesParser(final InputStream file) {
		this(new ArrayList<InputStream>() {{add(file);}});
	}
	
	public Properties getProperties() throws CoreException {
		return getProperties(null);
	}

	
	public Properties getProperties(Properties defaults) throws CoreException {
		Properties props = defaults;
		
		for (int i = files.size()-1; i >= 0; i--) {
			props = getPropertyFile(files.get(i), props);
		}
		
		return props;
	}
	
	public Properties getPropertyFile(InputStream resource, Properties defaults) throws CoreException {
		Properties props;
		if (defaults == null) props = new Properties();
		else props = new Properties(defaults);
		try {
			props.load(resource);
		} catch (IOException e1) {
			System.out.println("Issue loading file: "+resource);
			e1.printStackTrace();
			return null;
		}
		
		// TODO: Improve parsing of ant properties
		boolean overflow = false;
		for (Object key : props.keySet().toArray()) {
			int count = 0;
			while (props.getProperty((String) key).toString().contains("$") && count < 10) {
				Pattern pattern = Pattern.compile( "\\$\\{(.*?)\\}" );
		        Matcher matcher = pattern.matcher( props.getProperty((String) key).toString() );
		        StringBuffer sb = new StringBuffer();
		        while ( matcher.find() ) {
		        	String prop = matcher.group().substring(2, matcher.group().length()-1);
		        	if (props.getProperty(prop) != null) {
		        		matcher.appendReplacement(sb, Matcher.quoteReplacement(props.getProperty(prop)));
		        	} else if (prop.equals("user.home")) {
		        		matcher.appendReplacement(sb, Matcher.quoteReplacement(System.getProperty("user.home")));
		        	} else {
		        		throw new CoreException(new Status(0, "WPI", "Could not parse build.properties file, unsupported property: "+prop));
		        	}
		        }
		        matcher.appendTail( sb );
		        props.setProperty((String) key, sb.toString());	

				count++;
			}
			if (count >= 50) overflow = true;
		}
		if (overflow) throw new CoreException(new Status(0, "WPI", "Could not parse build.properties file"));
		
		return props;
	}
}
