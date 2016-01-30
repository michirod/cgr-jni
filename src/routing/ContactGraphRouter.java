package routing;
import java.util.List;
import java.util.Map;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.TreeMap;

import cgr.Libcgr;
import cgr.Utils;
import core.Connection;
import core.DTNHost;
import core.Message;
import core.MessageListener;
import core.Settings;
import core.SimClock;
import util.Tuple;

public class ContactGraphRouter extends ActiveRouter {
	
	public class Outduct {
		private DTNHost host;
		private LinkedList<Message> queue;
		
		public Outduct(DTNHost host) {
			this.host = host;
			this.queue = new LinkedList<Message>();
		}		
		
		public LinkedList<Message> getQueue() {
			return queue;
		}
		
		public void setQueue(LinkedList<Message> queue) {
			this.queue = queue;
		}
		
		public DTNHost getHost() {
			return host;
		}
		
		public void setHost(DTNHost host) {
			this.host = host;
		}
		public void removeMessageFromOutduct(Message m){
			for(Message m1 : this.getQueue()){
				if( m.equals(m1)){
					this.getQueue().remove(m1);
					m.updateProperty(OUTDUCT_REF, -2);
				}
			}
		}
		
		public boolean containsMessage(Message message)
		{
			return queue.contains(message);
		}
		
		public Outduct containsMessagesForNodeNbr(int toNodeNbr){
			Outduct result;
			LinkedList<Message> returnOutductMessageList = new LinkedList<Message>();
			for(Message m : this.getQueue()){
				if(m.getTo().getAddress() == toNodeNbr){
					returnOutductMessageList.add(m);
				}
			}
			result = new Outduct(this.getHost());
			result.setQueue(returnOutductMessageList);
			return result;
		}
		
		public	Outduct getONEOutductToNode(int localNodeNbr, int toNodeNbr){
			if(localNodeNbr == this.getHost().getAddress())
				return containsMessagesForNodeNbr(toNodeNbr);
			else
				return null;
		}
		
		public void insertBundleIntoOutduct(Message message){
			this.queue.add(message);
			boolean thisIsLimbo = host == null;
			if (thisIsLimbo)
			{
				message.updateProperty(OUTDUCT_REF, -1);
				return;
			}
			else if (isMessageIntoLimbo(message))
			{
				removeMessageFromLimbo(message);
			}
			message.updateProperty(OUTDUCT_REF, host.getAddress());
		}

		public int getOutductSize(){
			return this.getQueue().size();
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
	public static final String ROUTE_FORWARD_TIMELIMIT = "ForwardTimelimit";
	public static final String OUTDUCT_REF = "OutducReference";
	private int nodeNum;
	private Outduct limbo = new Outduct(null);
	
	//la chiave è il toNode
	private TreeMap<DTNHost, Outduct> outducts = new TreeMap<DTNHost, Outduct>();

	protected ContactGraphRouter(ActiveRouter r) {
		super(r);
	}
	
	public ContactGraphRouter(Settings s) {
		super(s);
		Settings cgrSettings = new Settings(CGR_NS);	
	}
	
	@Override
	public void init(DTNHost host, List<MessageListener> mListeners) {
		super.init(host, mListeners);
		initCGR();
	}

	public TreeMap<DTNHost, Outduct> getOutducts() {
		updateOutducts(Utils.getAllNodes());
		return this.outducts;
	}	

	public void putMessageIntoLimbo(Message message)
	{
		int outductNum = (int) message.getProperty(OUTDUCT_REF);
		if (outductNum >= 0)
			getOutducts().get(Utils.getNodeFromNumber(outductNum)).removeMessageFromOutduct(message);
		limbo.insertBundleIntoOutduct(message);
		message.updateProperty(OUTDUCT_REF, -1);
	}
	public void removeMessageFromLimbo(Message message)
	{
		limbo.removeMessageFromOutduct(message);
		message.updateProperty(OUTDUCT_REF, -2);
	}
	public boolean isMessageIntoLimbo(Message message)
	{
		return limbo.containsMessage(message);
	}
	
	protected void tryRouteForMessageIntoLimbo()
	{
		for (Message m : limbo.getQueue())
		{
			cgrForward(m, m.getTo());
		}
	}

	protected void checkExpiredRoutes()
	{
		for (Outduct o : getOutducts().values())
		{
			for (Message m : o.getQueue())
			{
				long fwdTimelimit = (long) m.getProperty(ROUTE_FORWARD_TIMELIMIT);
				if (fwdTimelimit == 0) // This Message hasn't been routed yet
					return;
				if (SimClock.getIntTime() > fwdTimelimit)
				{
					o.removeMessageFromOutduct(m);
					putMessageIntoLimbo(m);
				}
			}
		}
	}

	@Override
	public void update(){
		checkExpiredRoutes();
		tryRouteForMessageIntoLimbo();
		super.update();
		if (isTransferring() || !canStartTransfer()) {
			return; // transferring, don't try other connections yet
		}

		List<Connection> connections = super.getConnections();
		for(Connection c : connections){
			Outduct o = getOutducts().get(c.getOtherNode(getHost()));
			for(Message m : o.getQueue()){
				if(super.startTransfer(m, c) == RCV_OK)
				{
					System.out.println("Inizio trasmissione " + m + " " + c);
				}
				else 
					break;
			}

		}			
	}

	@Override
	protected void addToMessages(Message m, boolean newMessage) 
	{
		putMessageIntoLimbo(m);
		super.addToMessages(m, newMessage);
	}

	@Override 
	public boolean createNewMessage(Message m) {
		m.addProperty(ROUTE_FORWARD_TIMELIMIT, (long)0);
		m.addProperty(OUTDUCT_REF, 0);
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
			outductNum = (int) removed.getProperty(OUTDUCT_REF);
			if (outductNum == -1) // this message is into limbo
				o = limbo;
			else if (outductNum == -2) // this message isn't in any outduct
				o = null;
			else
				o = getOutducts().get(Utils.getNodeFromNumber(outductNum));
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
	}
	
	@Override
	public Message messageTransferred(String id, DTNHost from)
	{
		Message transferred = super.messageTransferred(id, from);
		return transferred;
	}
	
	@Override
	public MessageRouter replicate() {
		// TODO Auto-generated method stub
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
	
	@Override 
	protected List<Tuple<Message,Connection>> getMessagesForConnected() 
	{
		if (getNrofMessages() == 0 || getConnections().size() == 0) {
			/* no messages -> empty list */
			return new ArrayList<Tuple<Message, Connection>>(0); 
		}

		List<Tuple<Message, Connection>> forTuples = new ArrayList<Tuple<Message, Connection>>();
		for (Connection con : getConnections())
		{
			DTNHost to = con.getOtherNode(getHost());
			Outduct outduct = getOutducts().get(to);
			if (outduct == null)
				continue;
			for (Message m : outduct.getQueue())
			{
				forTuples.add(new Tuple<Message, Connection>(m, con));
			}
		}
		
		return forTuples;
	}
	
	@SuppressWarnings("unchecked")
	@Override 
	protected List<Tuple<Message, Connection>> sortByQueueMode(List list) 
	{
		List<Tuple<Message, Connection>> result = new ArrayList<Tuple<Message, Connection>>();
		Map<Connection, Integer> connections = new HashMap<>();
		int connectionsNum;
		for (Tuple<Message, Connection> t : (List<Tuple<Message, Connection>>) list)
		{
			if (connections.containsKey(t.getValue()))
				connections.put(t.getValue(), connections.get(t.getValue()) + 1);
			else
				connections.put(t.getValue(), 1);
		}
		connectionsNum = connections.size();
		return list;
	}
	
	@Override
	protected Connection exchangeDeliverableMessages()
	{
		List<Connection> connections = getConnections();

		if (connections.size() == 0) {
			return null;
		}
		
		@SuppressWarnings(value = "unchecked")
		Tuple<Message, Connection> t =
			tryMessagesForConnected(sortByQueueMode(getMessagesForConnected()));

		if (t != null) {
			return t.getValue(); // started transfer
		}
		return null;
	}

	@Override
	public String toString() {
		return CGR_NS;
	}
	
	public void initCGR()
	{
		Libcgr.initializeNode(getHost().getAddress());
	}
	
	public void readContactPlan(String filePath)
	{
		Libcgr.readContactPlan(this.getHost().getAddress(), filePath);
	}
	
	public void processLine(String line)
	{
		Libcgr.processLine(this.getHost().getAddress(), line);
	}
	
	public int cgrForward(Message m, DTNHost terminusNode)
	{
		return Libcgr.cgrForward(this.getHost().getAddress(), m, terminusNode.getAddress());
	}

}
