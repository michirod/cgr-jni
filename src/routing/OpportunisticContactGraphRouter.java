package routing;

import cgr_jni.Libocgr;
import core.Connection;
import core.DTNHost;
import core.Message;
import core.Settings;

public class OpportunisticContactGraphRouter extends ContactGraphRouter {

	public static final String OCGR_NS = "OpportunisticContactGraphRouter";
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
	
	@Override
	public MessageRouter replicate() {
		return new OpportunisticContactGraphRouter(this);
	}
	
	protected void discoveredContactStart(Connection con)
	{
		exchangeCurrentDiscoveredContacts(con);
		excangeContactHistory(con);
		predictContacts();
		contactAquired(con);
		contactPlanChanged();
	}

	protected void discoveredContactEnd(Connection con)
	{
		contactLost(con);
	}
		
	/**
	 * This function invoke the predictContacts algorithm on the local node
	 */
	private void predictContacts() {
		Libocgr.predictContacts(getHost().getAddress());
	}

	/**
	 * This function copies the contact history from node con.fromNode to
	 * node con.toNode AND VICE VERSA. The operation is made only at the 
	 * sender end of the connection.
	 * @param con new discovered Connection
	 */
	private void excangeContactHistory(Connection con) {
		if (con.isInitiator(getHost()))
		{
			Libocgr.exchangeContactHistory(getHost().getAddress(), 
					con.getOtherNode(getHost()).getAddress());
		}
	}

	/**
	 * This function copies all current discovered contacts from the contact
	 * plan of node con.fromNode to the contact plan of node con.toNode
	 * AND VICE VERSA. The operation is made only at the sender end of the
	 * connection.
	 * @param con new discovered Connection
	 */
	private void exchangeCurrentDiscoveredContacts(Connection con) {
		if (con.isInitiator(getHost()))
		{
			Libocgr.exchangeCurrentDiscoveredContatcs(getHost().getAddress(), 
					con.getOtherNode(getHost()).getAddress());
		}
	}
	
	/**
	 * This function informs the OCGR of a new discovered contact
	 * @param con the new discovered Connection
	 */
	private void contactAquired(Connection con)
	{
		Libocgr.contactDiscoveryAquired(getHost().getAddress(), 
				con.getOtherNode(getHost()).getAddress(), (int)con.getSpeed());
	}

	/**
	 * This function informs the OCGR that a discovered connection went down
	 * @param con the Connection lost
	 */
	private void contactLost(Connection con) {
		Libocgr.contactDiscoveryLost(getHost().getAddress(), 
				con.getOtherNode(getHost()).getAddress());
	}

	@Override
	public void changedConnection(Connection con) {
		super.changedConnection(con);
		if (con.isUp()) // this is a new connection
		{
			discoveredContactStart(con);
		}
		else // this connection went down
		{
			discoveredContactEnd(con);
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
		transferred.updateProperty(XMIT_COPIES_COUNT_PROP, 0);
		transferred.updateProperty(DLV_CONFIDENCE_PROP, 0.0);
		return transferred;
	}
	
	@Override
	protected String getRouterName() {
		return OCGR_NS;
	}
}
