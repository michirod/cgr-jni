package routing;

import java.util.LinkedList;
import java.util.Queue;

import cgr_jni.Libocgr;
import core.Connection;
import core.DTNHost;
import core.Message;
import core.Settings;

public class OpportunisticContactGraphRouter extends ContactGraphRouter {

	public static final String OCGR_NS = "OpportunisticContactGraphRouter";
	public static final String EPIDEMIC_DROPBACK_S = "epidemicDropBack";
	public static final String PREVENT_CGRFORWARD_S = "preventCGRForward";
	public static final String EPIDEMIC_FLAG_PROP = "epidemicFlag";
	protected Queue<DiscoveryInfo> pendingDiscoveryInfos = new LinkedList<>();
	protected boolean epidemicDropBack = true;
	protected boolean preventCGRForward = false;
	
	/**
	 * Copy constructor.
	 * @param r The router prototype where setting values are copied from
	 */
	protected OpportunisticContactGraphRouter(ActiveRouter r) {
		super(r);
		epidemicDropBack = ((OpportunisticContactGraphRouter)r)
				.epidemicDropBack;
		preventCGRForward = ((OpportunisticContactGraphRouter)r)
				.preventCGRForward;
	}
	
	/**
	 * Constructor. Creates a new message router based on the settings in
	 * the given Settings object.
	 * @param s The settings object
	 */
	public OpportunisticContactGraphRouter(Settings s) {
		super(s);
		Settings ocgrSettings = new Settings(OCGR_NS);
		if (ocgrSettings.contains(EPIDEMIC_DROPBACK_S))
		{
			epidemicDropBack = ocgrSettings.getBoolean(EPIDEMIC_DROPBACK_S);
		}
		if (ocgrSettings.contains(PREVENT_CGRFORWARD_S))
		{
			preventCGRForward = ocgrSettings.getBoolean(PREVENT_CGRFORWARD_S);
		}
		if (ocgrSettings.contains(DEBUG_S))
		{
			debug = ocgrSettings.getBoolean(DEBUG_S);
		}
	}
	
	@Override
	public MessageRouter replicate() {
		return new OpportunisticContactGraphRouter(this);
	}
	
	public boolean isEpidemicDropBack() {
		return epidemicDropBack;
	}

	public void setEpidemicDropBack(boolean epidemicDropBack) {
		this.epidemicDropBack = epidemicDropBack;
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
		if (debug)
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
			if (debug)
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
			if (debug)
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
		if (debug)
			System.out.println("NODE " + getHost().getAddress() 
					+ " REMOVE DISCOVERED CONTACT");
		Libocgr.contactDiscoveryLost(getHost().getAddress(), 
				con.getOtherNode(getHost()).getAddress());
	}

	@Override
	protected int startTransfer(Message m, Connection con) {
		int result = super.startTransfer(m, con);
		if (result == DENIED_OLD)
		{
			removeMessageFromOutduct(con.getOtherNode(getHost()), m);
		}
		return result;
	}
	@Override
	public void update() {
		int result;
		Message m;
		applyDiscoveryInfos();
		super.update();
		if (!isTransferring() && epidemicDropBack)
		{
			m = getFirstEpidemicMessage();
			if (m == null)
				return;
			for (Connection c : getConnections()){
				result = startTransfer(m, c);
				if (result == RCV_OK)
					m.updateProperty(EPIDEMIC_FLAG_PROP, false);
			}
		}
	}
	private Message getFirstEpidemicMessage() {
		for (Message m : limbo.getQueue())
		{
			if ((boolean) m.getProperty(EPIDEMIC_FLAG_PROP))
				return m;
		}
		return null;
	}

	@Override
	public void changedConnection(Connection con) {
		super.changedConnection(con);
		if (con.isUp()) // this is a new connection
		{
			discoveredContactStart(con);
			applyDiscoveryInfos();
		}
		else // this connection went down
		{
			applyDiscoveryInfos();
			discoveredContactEnd(con);
		}
	}
	
	@Override 
	public boolean createNewMessage(Message m) {
		m.addProperty(EPIDEMIC_FLAG_PROP, false);
		return super.createNewMessage(m);
	}
	
	@Override
	public Message messageTransferred(String id, DTNHost from) {
		Message transferred = super.messageTransferred(id, from);
		transferred.updateProperty(EPIDEMIC_FLAG_PROP, false);
		return transferred;
	}
	
	@Override
	protected String getRouterName() {
		return OCGR_NS;
	}
	
	public void addDiscoveryInfo(long fromNode, long toNode, long fromTime,
			long toTime, int xmitSpeed)
	{
		DiscoveryInfo info = new DiscoveryInfo(fromNode, toNode, fromTime,
				toTime, xmitSpeed);
		pendingDiscoveryInfos.add(info);
	}
	
	protected void applyDiscoveryInfos()
	{
		DiscoveryInfo info;
		while ((info = pendingDiscoveryInfos.poll()) != null)
		{
			Libocgr.applyDiscoveryInfos(getHost().getAddress(),
					info.fromNode, info.toNode,
					info.fromTime, info.toTime, info.xmitSpeed);
		}
	}
	
	protected class DiscoveryInfo{
		private long fromNode;
		private long toNode;
		private long fromTime;
		private long toTime;
		private int xmitSpeed;
		
		public DiscoveryInfo(long fromNode, long toNode, 
				long fromTime, long toTime, int xmitSpeed) {
			super();
			this.fromNode = fromNode;
			this.toNode = toNode;
			this.fromTime = fromTime;
			this.toTime = toTime;
			this.xmitSpeed = xmitSpeed;
		}
		
	}
	
	@Override
	public int cgrForward(Message m, DTNHost terminusNode) {
		int result;
		if (!preventCGRForward)
		{
			result = super.cgrForward(m, terminusNode);
		}
		else result = 0;
		if (result == 0 && epidemicDropBack)
		{
			/* EPIDEMIC DROP BACK */
			/*
			for (Connection c : getConnections())
			{
				Message cloned = m.replicate();
				DTNHost peer = c.getOtherNode(getHost());
				getOutducts().get(peer).insertMessageIntoOutduct(cloned, false);
			}*/
			m.updateProperty(EPIDEMIC_FLAG_PROP, true);
		}
		return result;
	}
}
