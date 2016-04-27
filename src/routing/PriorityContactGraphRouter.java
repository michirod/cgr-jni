package routing;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import core.Connection;
import core.DTNHost;
import core.Message;
import core.PriorityMessage;
import core.Settings;
import core.SimClock;
import routing.ContactGraphRouter.Outduct;
import util.Tuple;


public class PriorityContactGraphRouter extends ContactGraphRouter {

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

	
	

}
