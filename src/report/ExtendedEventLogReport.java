package report;

import java.io.File;
import java.util.List;

import core.Connection;
import core.DTNHost;
import input.CPConnectionEvent;
import input.CPEventsReader;
import input.ExternalEvent;

/** Extends EventLogReport in order to use the transmitSpeed defined in the external Contact Plan 
 * 
 * No constructor needed */

public class ExtendedEventLogReport extends EventLogReport
{
	/** External ContactPlan parameters: 
	 * 
	 * -File: it specifies the absolute path of the input ".txt" containing the user-defined ContactPlan;
	 * 		 if the path changes on local disk it must be changed in this class as well. 
	 * -Reader: it associates the CPEventsReader defined in the input package and the file before created.
	 * -Events: list of external events read from the external file. IMPORTANT: must be used a cast*/
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
				/** Cast needed in order to use the event speed, as defined in the Contact Plan */
				connectionSpeed = ((CPConnectionEvent) ev).getSpeed();
			}
			
			if (c.getOtherNode(host1).equals(host2))
			{
				speed = String.valueOf(connectionSpeed);
			}
		}
		/** "CONN" and "up" used because not specified by the reader 
		 * Remember: Contact Plan only specifies open/close of connections between nodes */
		processEvent("CONN", host1, host2, null,
				"up"+ " "+speed);
	}
}
