package routing;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import core.Connection;
import core.Message;
import core.PriorityMessage;
import core.Settings;
import core.SimError;
import util.Tuple;

public class PriorityEpidemicRouter extends ActiveRouter {

	protected boolean removing;
	/**
	 * Constructor. Creates a new message router based on the settings in
	 * the given Settings object.
	 * @param s The settings object
	 */
	public PriorityEpidemicRouter(Settings s) {
		super(s);
		this.removing = false;
	}

	/**
	 * Copy constructor.
	 * @param r The router prototype where setting values are copied from
	 */
	protected PriorityEpidemicRouter(PriorityEpidemicRouter r){
		super(r);
		this.removing = r.removing;
	}
	
	@Override
	public void update() {
		super.update();
		if (isTransferring() || !canStartTransfer()) {
			return; // transferring, don't try other connections yet
		}

		// Try first the messages that can be delivered to final recipient
		if (exchangeDeliverableMessages() != null) {
			return; // started a transfer, don't try others (yet)
		}

		// then try any/all message to any/all connection
		this.tryAllMessagesToAllConnections();
	}
	
	@Override
	public PriorityEpidemicRouter replicate() {
		return new PriorityEpidemicRouter(this);
	}

	@Override
	protected Message getNextMessageToRemove(boolean excludeMsgBeingSent)
	{
		Collection<Message> messages = this.getMessageCollection();
		Message oldest = null;
		List<Message> lmessages = new ArrayList<Message>(messages);
		setRemoving(true);
		sortByQueueMode(lmessages);
		for(int i=0;i<lmessages.size();i++)
		{
			
			if (excludeMsgBeingSent && 
					isSending(lmessages.get(lmessages.size()-1-i).getId()))
			{
				continue; // skip the message(s) that router is sending
			}
			oldest=lmessages.get(lmessages.size()-1-i);		
			setRemoving(false);
			return oldest;
		}	
		setRemoving(false);
		return oldest;		
	}

	@SuppressWarnings("unchecked")
	@Override
	protected List sortByQueueMode(List list) {	
		Collections.sort(list, new Comparator() 
		{
			/* Compare two tuples or two messages by their priority and
			  then by Receive time */
			public int compare(Object o1,Object o2)
			{
				int pdiff;
				double diff;
				Message m1,m2;
				if (o1 instanceof Tuple)
				{
					m1 = ((Tuple<Message, Connection>)o1).getKey();
					m2 = ((Tuple<Message, Connection>)o2).getKey();
				}
				else if (o1 instanceof Message)
				{
					m1= (Message)o1;
					m2= (Message)o2;						
				}
				else 
				{
					throw new SimError("Invalid type of objects in " +
							"the list");
				}
				
				pdiff= ((PriorityMessage)m1).getPriority() - ((PriorityMessage)m2).getPriority();
				
				if (pdiff != 0)						
					return (pdiff < 0 ? 1 : -1);
				
				diff = m1.getReceiveTime() - m2.getReceiveTime();
				if (diff == 0) 
				{
					return 0;
				}
				else if (!isRemoving())
				{					
					return (diff < 0 ? -1 : 1);
				}	
				//if isRemoving i need the oldest with lower priority
				else if(isRemoving())
				{
					return (diff < 0 ? 1 : -1);
				}
				
				//not reached
				return 0;
			}			 
		});
		
		return list;
	}

	public boolean isRemoving() {
		return removing;
	}

	public void setRemoving(boolean removing) {
		this.removing = removing;
	}
	
	

}
