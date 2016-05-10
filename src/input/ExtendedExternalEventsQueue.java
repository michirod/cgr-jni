package input;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import core.Settings;

public class ExtendedExternalEventsQueue extends ExternalEventsQueue 
{
	
	public ExtendedExternalEventsQueue(Settings s) 
	{
		super(s);
	}
	
	public ExtendedExternalEventsQueue(String filePath, int nrofPreload)
	{
		super(filePath, nrofPreload);
		init(filePath);
	}
	
	private void init(String eeFilePath)
	{
		this.eventsFile = new File(eeFilePath);

		if (BinaryEventsReader.isBinaryEeFile(eventsFile))
		{
			this.reader = new BinaryEventsReader(eventsFile);
		}
		else 
		{
			this.reader = new CPEventsReader(eventsFile);
		}
		
//		this.reader = new CPEventsReader(eventsFile);

		this.queue = readEvents(nrofPreload);
		this.nextEventIndex = 0;
	}
	
	private List<ExternalEvent> readEvents(int nrof) 
	{
		if (allEventsRead) 
		{
			return new ArrayList<ExternalEvent>(0);
		}

		List<ExternalEvent> events = reader.readEvents(nrof);
		
		if (nrof > 0 && events.size() == 0)
		{
			reader.close();
			allEventsRead = true;
		}

		return events;
	}
	
}