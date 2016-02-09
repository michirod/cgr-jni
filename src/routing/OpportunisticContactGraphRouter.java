package routing;

import core.Connection;
import core.Settings;

public class OpportunisticContactGraphRouter extends ContactGraphRouter {

	/**
	 * Copy constructor.
	 * @param r The router prototype where setting values are copied from
	 */
	protected OpportunisticContactGraphRouter(ActiveRouter r) {
		super(r);
	}
	
	/**
	 * Constructor. Creates a new message router based on the settings in
	 * the given Settings object.
	 * @param s The settings object
	 */
	public OpportunisticContactGraphRouter(Settings s) {
		super(s);
	}
	
	protected void discoveredContactStart(Connection con)
	{
		exchangeCurrentDiscoveredContacts(con);
		excangeContactHistory(con);
		predictContacts();
		contactPlanChanged();
	}

	protected void discoveredContactEnd(Connection con)
	{
		removeContact(con);
	}
		
	private void predictContacts() {
		// TODO Auto-generated method stub
		
	}

	private void excangeContactHistory(Connection con) {
		// TODO Auto-generated method stub
		
	}

	private void exchangeCurrentDiscoveredContacts(Connection con) {
		// TODO Auto-generated method stub
		
	}
	
	private void removeContact(Connection con) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void changedConnection(Connection con) {
		super.changedConnection(con);
		if (con.isUp()) // this is a new connection
		{
			
		}
		else // this connection went down
		{
			
		}
	}
}
