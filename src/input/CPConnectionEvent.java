package input;

import core.DTNHost;
import core.Settings;
import core.World;

@SuppressWarnings("serial")
public class CPConnectionEvent extends ConnectionEvent
{
	public Settings settings;
	
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
