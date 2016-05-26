package input;

import core.DTNHost;
import core.Settings;
import core.World;

/** Class used to define a specific ConnectionEvent, made by the ContactPlan reading class.
 *  It extends ConnectionEvent adding the transmit datarate between hosts (param: speed)
 *  
 *   The constructor calls the superclass' one and extends it by adding the speed parameter */

public class CPConnectionEvent extends ConnectionEvent
{
	public Settings settings;
	
	/** Speed of the connection between two nodes. It's simply the datarate used by the network interface */
	private int speed;

	public CPConnectionEvent(int from, int to, String interf, boolean up, double time, int speed) 
	{
		super(from, to, interf, up, time);
		this.speed = speed;
	}
	
	@Override
	public void processEvent(World world) {
		DTNHost from = world.getNodeByAddress(this.fromAddr);
		DTNHost to = world.getNodeByAddress(this.toAddr);

		from.forceConnection(to, interfaceId, this.isUp);
	}
	
	public int getSpeed()
	{
		return this.speed;
	}

	@Override
	public String toString() {
		return "CONN " + (isUp ? "up" : "down") + " @" + this.time + " " +
				this.fromAddr+"<->"+this.toAddr+ " . c" +this.speed;
	}
}
