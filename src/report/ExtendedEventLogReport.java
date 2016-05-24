package report;

import java.io.File;
import java.util.List;

import core.Connection;
import core.DTNHost;
import input.CPConnectionEvent;
import input.CPEventsReader;
import input.ExternalEvent;

public class ExtendedEventLogReport extends EventLogReport
{
	private File file = new File("/home/federico/workspace/TheOneSimulator/data/contact.txt");
	private CPEventsReader reader = new CPEventsReader(file);
	private List<ExternalEvent> events = reader.readEvents(500);
	
	@Override
	public void hostsConnected(DTNHost host1, DTNHost host2) {
		int connectionSpeed = 0;
		String speed = "";
		
		for (Connection c : host1.getConnections())
		{
			for (ExternalEvent ev : events)
			{
				connectionSpeed = ((CPConnectionEvent) ev).getSpeed();
			}
			
			if (c.getOtherNode(host1).equals(host2))
			{
				speed = String.valueOf(connectionSpeed);
			}
		}
		processEvent("CONN", host1, host2, null,
				"up"+ " "+speed);
	}
}
