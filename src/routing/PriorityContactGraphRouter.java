package routing;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import cgr_jni.Libcgr;
import core.Connection;
import core.DTNHost;
import core.Message;
import core.PriorityMessage;
import core.Settings;
import core.SimClock;
import util.Tuple;


public class PriorityContactGraphRouter extends ContactGraphRouter {

	
	public class OverbookingStructure
	{
		//class to manage overbooking 
		//every message that adds overbooking to an outduct create a new instance
		
		private DTNHost toHost;
		private double overbooked;
		private double protect;
		//whichQueue and queueIndex indicate the next bundle to handle
		private int whichQueue;
		private int queueIndex;
		
		public OverbookingStructure(DTNHost toHost, double overbooked, double protect)
		{
			this.toHost = toHost;
			this.overbooked = overbooked;
			this.protect = protect;
		}

		public DTNHost getToHost()
		{
			return toHost;
		}

		public double getOverbooked()
		{
			return overbooked;
		}

		public double getProtect() 
		{
			return protect;
		}
		
		public void subtractFromOverbooked(double sub)
		{
			overbooked -= sub;
		}
		
		public void subtractFromProtected(double sub)
		{
			protect -= sub; 
		}
		
		public boolean isOverbooked()
		{
			return (overbooked>0.0);
		}
		
		public int getWhichQueue()
		{
			return whichQueue;
		}
		
		public void setWhichQueue(int queue)
		{
			if(queue<0 || queue >2)
				throw new IllegalArgumentException("Queue must be in 0..2");
			
			whichQueue = queue;
		}
		
		public int getQueueIndex()
		{
			return queueIndex;
		}
		
		public void setQueueIndex(int index)
		{		
			queueIndex = index;
		}
	}
	public class PriorityOutduct extends ContactGraphRouter.Outduct 
	{

		private LinkedList<Message> bulkQueue;
		private LinkedList<Message> expeditedQueue;
		private long bulkBacklog;
		private long normalBacklog;
		private long expeditedBacklog; 
		
		public PriorityOutduct(DTNHost host) {
			super(host);
			bulkBacklog = 0;
			normalBacklog = 0;
			expeditedBacklog = 0;
			bulkQueue = new LinkedList<Message>();
			expeditedQueue =  new LinkedList<Message>();
			
		}

		public LinkedList<Message> getBulkQueue() {
			return bulkQueue;
		}

		public LinkedList<Message> getExpeditedQueue() {
			return expeditedQueue;
		}
		
		public LinkedList<Message> getNormalQueue(){
			return super.getQueue();
		}
		
		public long getBulkBacklog() {
			return bulkBacklog;
		}

		public long getNormalBacklog() {
			return normalBacklog;
		}

		public long getExpeditedBacklog() {
			return expeditedBacklog;
		}
		
		@Override
		public int getEnqueuedMessageNum(){
			return getQueue().size() + bulkQueue.size() + expeditedQueue.size();
		}

		@Override
		public boolean containsMessage(Message m)
		{
			switch ( ((PriorityMessage)m).getPriority() )
			{
				case 0:
					for(Message mex : this.bulkQueue)
						if(mex.getId().equals(m.getId()))
							return true;
							
				case 1: 
					for(Message mex : this.getNormalQueue())
						if(mex.getId().equals(m.getId()))
							return true;
				case 2: 
					for(Message mex : this.expeditedQueue)
						if(mex.getId().equals(m.getId()))
							return true;
			}
			
			return false;
		}

		@Override
		public void insertMessageIntoOutduct(Message message) 
		{
			
			if(((PriorityMessage)message).isReforwarded())
			{
				//if the message was from this outduct gets its place back
				if( ((PriorityMessage)message).getReforwardedFrom().equals(this.getHost()) )
				{
					int index = ((PriorityMessage)message).getReforwardIndex();
					switch ( ((PriorityMessage)message).getPriority() )
					{
						case 0: 
							this.bulkQueue.add(index,message);
							this.bulkBacklog += message.getSize();
							break;							
						case 1: 
							this.getNormalQueue().add(index,message);
							this.normalBacklog += message.getSize();
							break;
						case 2: 
							this.expeditedQueue.add(index,message);
							this.expeditedBacklog += message.getSize();
							break;
					}
					
					((PriorityMessage)message).setReforwarded(false);
					message.updateProperty(OUTDUCT_REF_PROP, getHost().getAddress());
					totalEnqueuedBytes += message.getSize();
					return;
				}
			}
			
			switch ( ((PriorityMessage)message).getPriority() )
			{
				case 0: 
					this.bulkQueue.add(message);
					this.bulkBacklog += message.getSize();
					break;							
				case 1: 
					this.getNormalQueue().add(message);
					this.normalBacklog += message.getSize();
					break;
				case 2: 
					this.expeditedQueue.add(message);
					this.expeditedBacklog += message.getSize();
					break;
			}
			boolean thisIsLimbo = (getHost() == null);
			if (thisIsLimbo)
			{
				message.updateProperty(OUTDUCT_REF_PROP, LIMBO_ID);
				return;
			}
			else if (isMessageIntoLimbo(message))
			{
				removeMessageFromLimbo(message);
			}
			message.updateProperty(OUTDUCT_REF_PROP, getHost().getAddress());
			totalEnqueuedBytes += message.getSize();
		}

		@Override
		public void removeMessageFromOutduct(Message m) 
		{
			switch ( ((PriorityMessage)m).getPriority() )
			{
				case 0:
				{
					Iterator<Message> iter = getBulkQueue().iterator();
					Message m1;
					while (iter.hasNext())
					{
						m1 = iter.next();
						if (m1.equals(m))
						{	
							iter.remove();
							m.updateProperty(OUTDUCT_REF_PROP, NONE_ID);
							bulkBacklog -= m.getSize();
							totalEnqueuedBytes -= m.getSize();
							return;
						}
					}
				}
														
				case 1: 
				{
					Iterator<Message> iter = getNormalQueue().iterator();
					Message m1;
					while (iter.hasNext())
					{
						m1 = iter.next();
						if (m1.equals(m))
						{	
							iter.remove();
							m.updateProperty(OUTDUCT_REF_PROP, NONE_ID);
							normalBacklog -= m.getSize();
							totalEnqueuedBytes -= m.getSize();
							return;
						}
					}
				}
					
				case 2: 
				{
					Iterator<Message> iter = getExpeditedQueue().iterator();
					Message m1;
					while (iter.hasNext())
					{
						m1 = iter.next();
						if (m1.equals(m))
						{	
							iter.remove();
							m.updateProperty(OUTDUCT_REF_PROP, NONE_ID);
							expeditedBacklog -= m.getSize();
							totalEnqueuedBytes -= m.getSize();
							return;
						}
					}
				}
			}
		}
		
	}
	
	protected LinkedList<OverbookingStructure> listOverbooked = new LinkedList<OverbookingStructure>();
	
	public PriorityContactGraphRouter(ActiveRouter r) {
		super(r);		
	}

	public PriorityContactGraphRouter(Settings s) {
		super(s);
		
	}

	@Override
	public void updateOutducts(Collection<DTNHost> hosts) 
	{
		if (outducts.size() != hosts.size())
		{
			for (DTNHost h : hosts)
			{
				if (! outducts.keySet().contains(h))
				{
					outducts.put(h, new PriorityOutduct(h));
				}
			}
		}		
	}

	@Override
	public MessageRouter replicate() {
		return new PriorityContactGraphRouter(this);
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
				if( ((PriorityOutduct)o).getExpeditedQueue().size() !=0 )
					forTuples.add(new Tuple<Message, Connection>( ((PriorityOutduct)o).getExpeditedQueue().getFirst(), c));
				
				else if( ((PriorityOutduct)o).getNormalQueue().size() !=0 )
					forTuples.add(new Tuple<Message, Connection>( ((PriorityOutduct)o).getNormalQueue().getFirst(), c));
				
				else if( ((PriorityOutduct)o).getBulkQueue().size() !=0 )
					forTuples.add(new Tuple<Message, Connection>( ((PriorityOutduct)o).getBulkQueue().getFirst(), c));
			}
			DTNHost next = outducts.higherKey(o.getHost());
			if (next == null)
				next = outducts.firstKey();
			o = outducts.get(next);
		}
		return forTuples;
	}
	
	//Priorities in limbo are not treated
	@Override
	protected void checkExpiredRoutes()
	{
		List<Message> expired = new ArrayList<>(getNrofMessages());
		for (Outduct o : getOutducts().values())
		{
			for (Message m : ((PriorityOutduct)o).getExpeditedQueue())
			{
				long fwdTimelimit = (long) m.getProperty(ROUTE_FORWARD_TIMELIMIT_PROP);
				if (fwdTimelimit == 0) // This Message hasn't been routed yet
					return;
				if (SimClock.getIntTime() > fwdTimelimit)
				{
					expired.add(m);
				}
			}
			for (Message m : ((PriorityOutduct)o).getNormalQueue())
			{
				long fwdTimelimit = (long) m.getProperty(ROUTE_FORWARD_TIMELIMIT_PROP);
				if (fwdTimelimit == 0) // This Message hasn't been routed yet
					return;
				if (SimClock.getIntTime() > fwdTimelimit)
				{
					expired.add(m);
				}
			}
			for (Message m : ((PriorityOutduct)o).getBulkQueue())
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

	//called after every cgrForward, this method call cgrForward too so there will be a chain of calls.
	//every cgrForward can add a new overbooking structure to the list so i have to handle the last one
	//in order to close the chain.
	public void manageOverbooking()
	{
		if(listOverbooked.isEmpty())
					return;		
		
		OverbookingStructure current = listOverbooked.getLast();
		
		//don't reforward protected bundle
		while(current.getProtect()>0.0)
		{
			if(current.getWhichQueue() == 0)
			{
				current.subtractFromProtected( ((PriorityOutduct)getOutducts().get(current.getToHost())).getBulkQueue().get(current.queueIndex).getSize() );
				current.setQueueIndex(current.getQueueIndex()-1);
				if(current.queueIndex<0)
				{
					if(((PriorityOutduct)getOutducts().get(current.getToHost())).getNormalBacklog() >0)
					{
						current.setWhichQueue(1);
						current.setQueueIndex(((PriorityOutduct)getOutducts().get(current.getToHost())).getNormalQueue().size()-1 );
					}
					else
					{
						current.setWhichQueue(2);
						current.setQueueIndex(((PriorityOutduct)getOutducts().get(current.getToHost())).getExpeditedQueue().size()-1 );
					}
				}
				
				continue;
				
			}
			else if(current.getWhichQueue() == 1)
			{
				current.subtractFromProtected( ((PriorityOutduct)getOutducts().get(current.getToHost())).getNormalQueue().get(current.queueIndex).getSize() );
				current.setQueueIndex(current.getQueueIndex()-1);
				if(current.queueIndex<0)
				{
					current.setWhichQueue(2);
					current.setQueueIndex(((PriorityOutduct)getOutducts().get(current.getToHost())).getExpeditedQueue().size()-1 );
				}
				
				continue;
			}
			else
			{
				current.subtractFromProtected( ((PriorityOutduct)getOutducts().get(current.getToHost())).getExpeditedQueue().get(current.queueIndex).getSize() );
				current.setQueueIndex(current.getQueueIndex()-1);
			}
		}
		
		Message m=null ;
		
		if(current.getWhichQueue() == 0)
		{
			m = ((PriorityOutduct)getOutducts().get(current.getToHost())).getBulkQueue().get(current.queueIndex) ;
			current.subtractFromOverbooked(m.getSize());
			current.setQueueIndex(current.getQueueIndex()-1);
			
			if(current.queueIndex<0 && current.isOverbooked())
			{
				if(((PriorityOutduct)getOutducts().get(current.getToHost())).getNormalBacklog() >0)
				{
					current.setWhichQueue(1);
					current.setQueueIndex(((PriorityOutduct)getOutducts().get(current.getToHost())).getNormalQueue().size()-1 );
				}
				else
				{
					current.setWhichQueue(2);
					current.setQueueIndex(((PriorityOutduct)getOutducts().get(current.getToHost())).getExpeditedQueue().size()-1 );
				}
			}			
		}
		else if(current.getWhichQueue() == 1)
		{
			m = ((PriorityOutduct)getOutducts().get(current.getToHost())).getNormalQueue().get(current.queueIndex) ;
			current.subtractFromOverbooked(m.getSize());
			current.setQueueIndex(current.getQueueIndex()-1);
			
			if(current.queueIndex<0 && current.isOverbooked())
			{
				current.setWhichQueue(2);
				current.setQueueIndex(((PriorityOutduct)getOutducts().get(current.getToHost())).getExpeditedQueue().size()-1 );
			}
			
			
		}
		else
		{
			m = ((PriorityOutduct)getOutducts().get(current.getToHost())).getExpeditedQueue().get(current.queueIndex) ;
			current.subtractFromOverbooked(m.getSize());
			current.setQueueIndex(current.getQueueIndex()-1);
		}
		
		((PriorityOutduct)getOutducts().get(current.getToHost())).removeMessageFromOutduct(m);
		if(!current.isOverbooked())
			listOverbooked.remove(current);
		
		//set this parameters in order to insert the message in the previous position if necessary
		//used in insertMessageintoOutduct
		((PriorityMessage)m).setReforwarded(true);
		((PriorityMessage)m).setReforwardedFrom(current.toHost);
		((PriorityMessage)m).setReforwardIndex(current.getQueueIndex()+1);
		cgrForward(m, m.getTo());	
	}
	
	//this method is called via JNI, it adds a new overbooking Structure that will be handled in manageOverbooking()
	//if the c library is compiled without the ONE_SIMULATION option this method is never called 
	public void setManageOverbooking(DTNHost to,double overbooked,double protect)
	{
		OverbookingStructure entry= new OverbookingStructure(to, overbooked, protect);
		if( ((PriorityOutduct)getOutducts().get(to)).getBulkBacklog() >0 )
		{
			entry.setQueueIndex(((PriorityOutduct)getOutducts().get(to)).getBulkQueue().size()-1 );
			entry.setWhichQueue(0);
		}
		else if( ((PriorityOutduct)getOutducts().get(to)).getNormalBacklog() >0 )
		{
			entry.setQueueIndex(((PriorityOutduct)getOutducts().get(to)).getNormalQueue().size()-1 );
			entry.setWhichQueue(1);
		}
		else if( ((PriorityOutduct)getOutducts().get(to)).getExpeditedBacklog() >0 )
		{
			entry.setQueueIndex(((PriorityOutduct)getOutducts().get(to)).getExpeditedQueue().size()-1 );
			entry.setWhichQueue(2);
		}
		listOverbooked.add(entry);
		return;
	}
	
	@Override
	public int cgrForward(Message m, DTNHost terminusNode)
	{
		Libcgr.cgrForward(this.getHost().getAddress(), m, terminusNode.getAddress());
		manageOverbooking();
		return 1;
	}

}
