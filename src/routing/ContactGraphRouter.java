package routing;
import java.util.List;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.TreeMap;

import cgr_jni.Libcgr;
import cgr_jni.Utils;
import core.Connection;
import core.DTNHost;
import core.Message;
import core.MessageListener;
import core.Settings;
import core.SimClock;
import util.Tuple;

public class ContactGraphRouter extends ActiveRouter {
	
	public class Outduct implements Comparable<Outduct> {
		public static final int LIMBO_ID = -1;
		public static final int NONE_ID = -2;
		private DTNHost host;
		private LinkedList<Message> queue;
		protected long totalEnqueuedBytes; //modificato
		
		public Outduct(DTNHost host) {
			this.host = host;
			this.queue = new LinkedList<Message>();
			this.totalEnqueuedBytes = 0;
		}		
		
		public DTNHost getHost() {
			return host;
		}
		public void setHost(DTNHost host) {
			this.host = host;
		}
		
		public LinkedList<Message> getQueue() {
			return queue;
		}
		
		public long getTotalEnqueuedBytes() {
			return totalEnqueuedBytes;
		}
		
		public boolean containsMessage(Message m)
		{
			for(Message m1 : queue){
				if( m.getId().equals(m1.getId())){
					return true;
				}
			}
			return false;
		}
		
		public void insertMessageIntoOutduct(Message message){
			this.queue.add(message);
			boolean thisIsLimbo = host == null;
			if (thisIsLimbo)
			{
				message.updateProperty(OUTDUCT_REF_PROP, LIMBO_ID);
				return;
			}
			else if (isMessageIntoLimbo(message))
			{
				removeMessageFromLimbo(message);
			}
			message.updateProperty(OUTDUCT_REF_PROP, host.getAddress());
			totalEnqueuedBytes += message.getSize();
		}

		public void removeMessageFromOutduct(Message m){
			Iterator<Message> iter = queue.iterator();
			Message m1;
			while (iter.hasNext())
			{
				m1 = iter.next();
				if (m1.equals(m))
				{	
					iter.remove();
					m.updateProperty(OUTDUCT_REF_PROP, NONE_ID);
					totalEnqueuedBytes -= m.getSize();
					return;
				}
			}
		}
		
		public int getEnqueuedMessageNum(){
			return queue.size();
		}
		
		@Override
		public int compareTo(Outduct o) {
			return host.compareTo(o.host);
		}
		
		@Override
		public String toString()
		{
			StringBuilder b = new StringBuilder();
			b.append(" to: ");
			if (host != null)
				b.append(host.toString());
			else 
				b.append("Limbo");
			b.append(", size: ");
			b.append(queue.size());
			b.append(", msgs: [ ");
			for (Message m : queue)
			{
				b.append(m.toString() + ",");
			}
			b.insert(b.length() - 1, "]\n");
			return b.toString();
		}
	}
	
	public static final String CGR_NS = "ContactGraphRouter";
	public static final String CONTACT_PLAN_PATH_S = "ContactPlanPath";
	public static final String ROUTE_FORWARD_TIMELIMIT_PROP = "ForwardTimelimit";
	public static final String OUTDUCT_REF_PROP = "OutducReference";
	
	/** counter incremented every time a message is delivered to the local node,
	 *  i.e. the message has reached its final destination. */
	protected int deliveredCount = 0;
	private boolean contactPlanChanged = false;
	/** Used as reference for round-robin outducts sorting */
	protected DTNHost firstOutductIndex; //modificato
	protected String contactPlanPath;
	
	protected TreeMap<DTNHost, Outduct> outducts = new TreeMap<DTNHost, Outduct>(); //modificati
	protected Outduct limbo = new Outduct(null);

	/**
	 * Copy constructor.
	 * @param r The router prototype where setting values are copied from
	 */
	protected ContactGraphRouter(ActiveRouter r) {
		super(r);
		contactPlanPath = ((ContactGraphRouter) r).contactPlanPath;
	}
	
	/**
	 * Constructor. Creates a new message router based on the settings in
	 * the given Settings object.
	 * @param s The settings object
	 */
	public ContactGraphRouter(Settings s) {
		super(s);
		Settings cgrSettings = new Settings(CGR_NS);
		contactPlanPath = cgrSettings.getSetting(CONTACT_PLAN_PATH_S, "");
		firstOutductIndex = null;
	}
	
	/**
	 * Initalizes the router.
	 * it also initializes the CGR library and read the contact plan 
	 * from the file that has been provided with the 
	 * {@link ContactGraphRouter#CONTACT_PLAN_PATH_S} property.
	 * {@inheritDoc}
	 */
	@Override
	public void init(DTNHost host, List<MessageListener> mListeners) {
		super.init(host, mListeners);
		initCGR();
		if(contactPlanPath.equals(""))
			return;
		else
			readContactPlan(contactPlanPath);		
	}
	
	@Override
	public void finalize()
	{
		finalizeCGR();
	}
	
	/**
	 * Gets all the outducts currently used by this node
	 * @return the outducts
	 */
	public TreeMap<DTNHost, Outduct> getOutducts() {
		updateOutducts(Utils.getAllNodes());
		return this.outducts;
	}	
	
	/**
	 * Gets the number of Messages enqueued into a specific outduct
	 * @param h the host that the outduct is directed to
	 * @return the total number of message enqueued into the outduct
	 */
	public int getOutductSize(DTNHost h)
	{
		Outduct o = outducts.get(h);
		if (o == null)
			return -1;
		return o.getEnqueuedMessageNum();
	}
	
	/**
	 * Gets the number of Messages currently into the limbo.
	 * If a message is into the limbo it means that the CGR hasn't found a 
	 * feasible route for it.
	 * @return the total number of message into the limbo
	 */
	public int getLimboSize()
	{
		return limbo.getEnqueuedMessageNum();
	}

	/**
	 * Gets the total number of Messages that has been delivered to this node.
	 * A message is delivered when it reaches his final destination
	 * @return the number of delivered messages
	 */
	public int getDeliveredCount() {
		return deliveredCount;
	}
	
	/**
	 * When status is changed the router needs to recalculate routes for messages into 
	 * limbo. Status changes when 
	 */
	protected void contactPlanChanged()
	{
		contactPlanChanged = true;
	}
	protected boolean isContactPlanChanged()
	{
		return contactPlanChanged;
	}
	
	/**
	 * Puts a message into the limbo. 
	 * If the message is currently into an outduct, the message will be removed
	 * from it. The outduct reference property {@link ContactGraphRouter#OUTDUCT_REF_PROP}
	 * is updated. 
	 * @param message to put into the limbo
	 */
	public void putMessageIntoLimbo(Message message)
	{
		int outductNum = (int) message.getProperty(OUTDUCT_REF_PROP);
		if (outductNum >= 0)
			getOutducts().get(Utils.getHostFromNumber(outductNum)).removeMessageFromOutduct(message);
		limbo.insertMessageIntoOutduct(message);
		message.updateProperty(OUTDUCT_REF_PROP, Outduct.LIMBO_ID);
	}
	/**
	 * Remove a message from the limbo. The message won't be in any outduct and the
	 * outduct reference property {@link ContactGraphRouter#OUTDUCT_REF_PROP}
	 * is set to {@link Outduct#NONE_ID}.
	 * @param message the message to remove from limbo.
	 */
	public void removeMessageFromLimbo(Message message)
	{
		limbo.removeMessageFromOutduct(message);
		message.updateProperty(OUTDUCT_REF_PROP, Outduct.NONE_ID);
	}
	/**
	 * Checks if a message is into the limbo.
	 * @param message the message to check.
	 * @return true if the message is currently into the limbo. False otherwise.
	 */
	public boolean isMessageIntoLimbo(Message message)
	{
		return limbo.containsMessage(message);
	}
	
	/**
	 * Tries to find a feasible route for every message currently into the limbo using 
	 * {@link ContactGraphRouter#cgrForward(Message, DTNHost)}. If a route is found for
	 * a message, it is automatically removed from the limbo and moved to the selected 
	 * outduct.
	 */
	protected void tryRouteForMessageIntoLimbo()
	{
		/* I can't operate directly on the queue itself or a 
		 * ConcurrentModificationException would be thrown.
		 * insertBundleIntoOutduct() will remove the Message from the limbo.
		 */
		Object[] temp = (Object[]) limbo.getQueue().toArray();
		for (int i = 0; i < temp.length; i++)
		{
			Message m = (Message) temp[i];
			cgrForward(m, m.getTo());
		}
	}

	/**
	 * Looks through all the outducts for messages whose route is expired 
	 * and needs to be recalculated. These messages are moved into the limbo.
	 */
	protected void checkExpiredRoutes()
	{
		List<Message> expired = new ArrayList<>(getNrofMessages());
		for (Outduct o : getOutducts().values())
		{
			for (Message m : o.getQueue())
			{
				long fwdTimelimit = (long) m.getProperty(ROUTE_FORWARD_TIMELIMIT_PROP);
				if (fwdTimelimit == 0) // This Message hasn't been routed yet
					return;
				if (SimClock.getIntTime() > fwdTimelimit)
				{
					expired.add(m);
				}
			}
			for (Message m : expired)
			{
				/*
				 * If a route has expired for a message, I put it into the limbo and 
				 * invoke CGR, which possibly remove the message from limbo and
				 * enqueue it into an outduct if a route has been found.
				 */
				o.removeMessageFromOutduct(m);
				putMessageIntoLimbo(m);
				cgrForward(m, m.getTo());
			}
			expired.clear();
		}
	}

	@Override
	public void update(){
		checkExpiredRoutes();
		if (isContactPlanChanged())
			tryRouteForMessageIntoLimbo();
		super.update();
		if (!canStartTransfer()) {
			return; // allows concurrent transmission
		}
		List<Tuple<Message,Connection>> outboundMessages = getMessagesForConnected();
		Tuple<Message, Connection> sent = tryMessagesForConnected(outboundMessages);
		if (sent != null)
		{
			// transmission started
			System.out.println("" + SimClock.getTime() + 
					": Begin transmission " 
					+ sent.getKey() + " " + sent.getValue());
			// I look for next messafe starting form next outduct
			firstOutductIndex = outducts.higherKey(sent.getValue().getOtherNode(getHost()));
			if (firstOutductIndex == null)
				firstOutductIndex = outducts.firstKey();
		}
	}
	
	/**
	 * Checks if router "wants" to start receiving message 
	 * (i.e. router doesn't have the message and has room for it).
	 * Allows concurrent transmission.
	 */
	@Override
	protected int checkReceiving(Message m, DTNHost from) {
		int result = super.checkReceiving(m, from);
		if (result == TRY_LATER_BUSY)
			result = RCV_OK;
		return result;
	}

	@Override
	protected void addToMessages(Message m, boolean newMessage) 
	{
		if (newMessage)
		{
			try {
				if (m.getTo().compareTo(getHost()) == 0)
				{
					//message should not be forwarded
					if (!isDeliveredMessage(m))
						deliveredCount++;
					return;
				}
			} catch (NullPointerException e)
			{
				e.printStackTrace();
				return;
			}
		}
		putMessageIntoLimbo(m);
		super.addToMessages(m, newMessage);
		cgrForward(m, m.getTo());
	}

	@Override 
	public boolean createNewMessage(Message m) {
		m.addProperty(ROUTE_FORWARD_TIMELIMIT_PROP, (long)0);
		m.addProperty(OUTDUCT_REF_PROP, Outduct.NONE_ID);
		return super.createNewMessage(m);
	}
	
	protected Message removeFromOutducts(String id)
	{
		Message removed;
		int outductNum;
		Outduct o;
		removed = getMessage(id);
		if (removed != null)
		{
			outductNum = (int) removed.getProperty(OUTDUCT_REF_PROP);
			if (outductNum == Outduct.LIMBO_ID) // this message is into limbo
				o = limbo;
			else if (outductNum == Outduct.NONE_ID) // this message isn't in any outduct
				o = null;
			else
				o = getOutducts().get(Utils.getHostFromNumber(outductNum));
			if (o != null)
				o.removeMessageFromOutduct(removed);
			else return null;
		}
		return removed;
	}
	
	@Override
	protected Message removeFromMessages(String id) 
	{
		removeFromOutducts(id);
		return super.removeFromMessages(id);
	}

	@Override 
	protected void transferDone(Connection con)
	{
		super.transferDone(con);
		Message transferred = con.getMessage();
		removeFromOutducts(transferred.getId());
		System.out.println("" + SimClock.getTime() + 
				": End transmission " + transferred + " " + con);
	}
	
	@Override
	public Message messageTransferred(String id, DTNHost from)
	{
		Message transferred = super.messageTransferred(id, from);
		if (transferred.getTo().equals(getHost()))
		{
			deliveredCount++;
		}
		return transferred;
	}
	
	@Override
	public MessageRouter replicate() {
		return new ContactGraphRouter(this);
	}

	public void updateOutducts(Collection<DTNHost> hosts)
	{
		if (outducts.size() != hosts.size())
		{
			for (DTNHost h : hosts)
			{
				if (! outducts.keySet().contains(h))
				outducts.put(h, new Outduct(h));				
			}
		}
	}
	
	protected Connection getConnectionTo(DTNHost h)
	{
		for (Connection c : getConnections())
		{
			if (h == c.getOtherNode(this.getHost()))
			{
				return c;
			}
		}
		return null;
	}
	
	@Override 
	protected List<Tuple<Message,Connection>> getMessagesForConnected() 
	{
		if (getNrofMessages() == 0 || getConnections().size() == 0) {
			/* no messages -> empty list */
			return new ArrayList<Tuple<Message, Connection>>(0); 
		}

		List<Tuple<Message, Connection>> forTuples = 
				new ArrayList<Tuple<Message, Connection>>();
		if (firstOutductIndex == null)
			firstOutductIndex = outducts.firstKey();
		Outduct o = outducts.get(firstOutductIndex);
		Connection c;
		for (int j = 0; j < outducts.size(); j++)
		{
			if ((c = getConnectionTo(o.getHost())) != null 
					&& o.getEnqueuedMessageNum() > 0)
			{
				forTuples.add(new Tuple<Message, Connection>(o.getQueue().getFirst(), c));
			}
			DTNHost next = outducts.higherKey(o.getHost());
			if (next == null)
				next = outducts.firstKey();
			o = outducts.get(next);
		}
		return forTuples;
	}
	
	protected Tuple<Message, Connection> tryAllMessages()
	{
		List<Connection> connections = super.getConnections();
		for(Connection c : connections){
			Outduct o = getOutducts().get(c.getOtherNode(getHost()));
			for(Message m : o.getQueue()){
				if(super.startTransfer(m, c) == RCV_OK)
				{
					System.out.println("Begin transmission " + m + " " + c);
					return new Tuple<Message, Connection>(m, c);
				}
				else 
					break;
			}
		}			
		return null;
	}
	
	@Override
	public boolean isDeliveredMessage(Message m)
	{
		return super.isDeliveredMessage(m);
	}

	@Override
	public String toString() {
		StringBuilder b = new StringBuilder();
		b.append(CGR_NS);
		b.append(" node " + getHost().getAddress());
		b.append('\n');
		b.append(limbo.toString());
		b.append(outducts.toString());
		return b.toString();
	}
	
	protected void initCGR()
	{
		if (getHost().getAddress() == 0)
		{
			System.out.println("ERROR: ContactGraphRouter cannot be inizialized if "
					+ "local node number is 0");
			System.exit(1);
		}
		Libcgr.initializeNode(getHost().getAddress());
	}

	/**
	 * Finalizes the router.
	 * User needs to invoke this method at the end of the simulation to 
	 * deallocate memory used by CGR lib
	 */
	public void finalizeCGR()
	{
		Libcgr.finalizeNode(getHost().getAddress());
	}
	
	public void readContactPlan(String filePath)
	{
		Libcgr.readContactPlan(this.getHost().getAddress(), filePath);
		contactPlanChanged();
	}
	
	public void processLine(String line)
	{
		Libcgr.processLine(this.getHost().getAddress(), line);
		contactPlanChanged();
	}
	
	public int cgrForward(Message m, DTNHost terminusNode)
	{
		//return -1;
		return Libcgr.cgrForward(this.getHost().getAddress(), m, terminusNode.getAddress());
	}

}
