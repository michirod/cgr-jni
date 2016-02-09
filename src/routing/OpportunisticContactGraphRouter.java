package routing;

import java.util.ArrayList;

import core.Connection;
import core.DTNHost;
import core.Message;
import core.Settings;

public class OpportunisticContactGraphRouter extends ContactGraphRouter {

	public static final String XMIT_COPIES_PROP = "XmitCopies";
	public static final String XMIT_COPIES_COUNT_PROP = "XmitCopiesCount";
	public static final String DLV_CONFIDENCE_PROP = "DlvConfidence";
	
	
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
	
	@Override 
	public boolean createNewMessage(Message m) {
		m.addProperty(XMIT_COPIES_PROP, new int[0]);
		m.addProperty(XMIT_COPIES_COUNT_PROP, 0);
		m.addProperty(DLV_CONFIDENCE_PROP, 0.0);
		return super.createNewMessage(m);
	}
	
	@Override
	public Message messageTransferred(String id, DTNHost from) {
		Message transferred = super.messageTransferred(id, from);
		transferred.updateProperty(XMIT_COPIES_PROP, new int[0]);
	}
}
