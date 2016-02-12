package routing;

import cgr_jni.Libocgr;
import core.Connection;
import core.DTNHost;
import core.Message;
import core.Settings;

public class OpportunisticContactGraphRouter extends ContactGraphRouter {

	public static final String OCGR_NS = "OpportunisticContactGraphRouter";
	
	
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
		contactPlanChanged();
	}
		
	/**
	 * This function invoke the predictContacts algorithm on the local node
	 */
	private void predictContacts() {
		System.out.println("NODE " + getHost().getAddress() 
				+ " CONTACT PREDICTION INITIATED");
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
			System.out.println("EXCHANGING CONTACT HISTORY between "
					+ getHost().getAddress() + " and " + con.getOtherNode(
							getHost()).getAddress());
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
			System.out.println("EXCHANGING CURRENT DISCOVERED CONTACTS between "
					+ getHost().getAddress() + " and " + con.getOtherNode(
							getHost()).getAddress());
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
		System.out.println("NODE " + getHost().getAddress() 
				+ " REMOVE DISCOVERED CONTACT");
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
		return super.createNewMessage(m);
	}
	
	@Override
	public Message messageTransferred(String id, DTNHost from) {
		Message transferred = super.messageTransferred(id, from);
		return transferred;
	}
	
	@Override
	protected String getRouterName() {
		return OCGR_NS;
	}
}
