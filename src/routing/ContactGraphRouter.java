package routing;
import java.util.List;
import java.util.Collection;
import java.util.LinkedList;
import java.util.TreeMap;

import cgr.Libcgr;
import cgr.Utils;
import core.Connection;
import core.DTNHost;
import core.Message;
import core.MessageListener;
import core.Settings;

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
				}
			}
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
		
		public void insertBundleIntoOutduct( Message message){
			this.queue.add(message);
		}
		
		public int getOutductSize(){
			return this.getQueue().size();
		}
		
		@Override
		public String toString()
		{
			StringBuilder b = new StringBuilder();
			b.append(" to: ");
			b.append(host.toString());
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
	 
	public Outduct getLimbo(){
		return this.limbo; 
	}
	
	@Override
	public void update(){
		super.update();
		if (isTransferring() || !canStartTransfer()) {
			return; // transferring, don't try other connections yet
		}
		
		// Try first the messages that can be delivered to final recipient
		if (exchangeDeliverableMessages() != null) {
			return; // started a transfer, don't try others (yet)
		}		
		
		List<Connection> connections = super.getConnections();
		for(Connection c : connections){
			Outduct o = outducts.get(c.getOtherNode(getHost()));
			while(!o.getQueue().isEmpty()){
				for(Message m : o.getQueue()){
					if(super.startTransfer(m, c) == RCV_OK){
						o.removeMessageFromOutduct(m);
					}
				}
				
			}
						
		}			
	}
	
	@Override
	public int receiveMessage(Message m, DTNHost from) {
		if(m.getTo().equals(this.getHost()))
			return super.receiveMessage(m, from);
		else{
			cgrForward(m, m.getTo());
			return super.receiveMessage(m, from);
		}
	}
	

	@Override
	public MessageRouter replicate() {
		// TODO Auto-generated method stub
		return new ContactGraphRouter(this);
	}
	
	@Override 
	public boolean createNewMessage(Message m) {
		if (super.createNewMessage(m))	
		{
			cgrForward(m, m.getTo());
			return true;
		}
		return false;
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
	
	public void cgrForward(Message m, DTNHost terminusNode)
	{
		Libcgr.cgrForward(this.getHost().getAddress(), m, terminusNode.getAddress());
	}

}
