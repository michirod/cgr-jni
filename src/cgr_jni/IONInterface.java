package cgr_jni;

import routing.ContactGraphRouter;
import routing.OpportunisticContactGraphRouter;
import routing.ContactGraphRouter.MessageStatus;
import routing.ContactGraphRouter.Outduct;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;

import core.DTNHost;
import core.Message;

public class IONInterface {	
	
	private static DTNHost getNodeFromNbr(long nodeNbr){
		return Utils.getHostFromNumber(nodeNbr);
	}
	
	//// STATIC METHODS ACCESSED FROM JNI /////
	
	static long getMessageSenderNbr(Message message){
		return message.getFrom().getAddress();
	}
	
	static long getMessageDestinationNbr(Message message)
	{
		return message.getTo().getAddress();
	}
	
	static long getMessageCreationTime(Message message){
		return (int) message.getCreationTime();
	}
	
	static long getMessageTTL(Message message){
		long result = (long) message.getTtl();
		return result;
	}
	static long getMessageSize(Message message){
		return message.getSize();		
	}
	static void updateMessageForfeitTime(Message message, long forfeitTime)
	{
		//message.updateProperty(ContactGraphRouter.ROUTE_FORWARD_TIMELIMIT_PROP, forfeitTime);
		MessageStatus status = (MessageStatus) message.getProperty(ContactGraphRouter.MESSAGE_STATUS_PROP);
		status.updateRouteTimelimit(forfeitTime);
	}
	
	static boolean isOutductBlocked(Outduct jOutduct)
	{
		return false;
	}
	
	static int getMaxPayloadLen(Outduct jOutduct)
	{
		return 1024*1024*1024;
	}
	
	static String getOutductName(Outduct jOutduct)
	{
		return "" + jOutduct.getHost().getAddress();
	}
	
	static long getOutductTotalEnququedBytes(Outduct jOutduct)
	{
		return jOutduct.getTotalEnqueuedBytes();
	}
	
	static Outduct getONEOutductToNode(long localNodeNbr, long toNodeNbr){
		DTNHost local = getNodeFromNbr(localNodeNbr);
		DTNHost to= getNodeFromNbr(toNodeNbr);
		
		ContactGraphRouter localRouter = (ContactGraphRouter) local.getRouter();
		//Outduct result = localRouter.getOutducts().get(to);
		Outduct result = localRouter.getOutducts()[to.getAddress()];
		return result;
		
	}
	static int insertBundleIntoOutduct(long localNodeNbr, Message message, long toNodeNbr)
	{
		DTNHost local = getNodeFromNbr(localNodeNbr);
		DTNHost to = getNodeFromNbr(toNodeNbr);
		ContactGraphRouter localRouter = (ContactGraphRouter) local.getRouter();
	/*	if(localRouter.getOutducts().containsKey(to)){
			localRouter.getOutducts().get(to).insertMessageIntoOutduct(message, true);
			return 0;
		}
	*/
		
		return localRouter.putMessageIntoOutduct(to, message, true);
	}

	static int insertBundleIntoLimbo(long localNodeNbr, Message message)
	{
		DTNHost local = getNodeFromNbr(localNodeNbr);
		ContactGraphRouter localRouter = (ContactGraphRouter) local.getRouter();
		localRouter.putMessageIntoLimbo(message);
		return 0;	
	}
	
	static void cloneMessage(long localNodeNbr, Message message)
	{
		DTNHost local = getNodeFromNbr(localNodeNbr);
		ContactGraphRouter localRouter = (ContactGraphRouter) local.getRouter();
		//Message newMessage = message.replicate();
		/* xmitCopies array must be deep copied */
		/*
		int[] xmitCopies = (int[]) message.getProperty(
				ContactGraphRouter.XMIT_COPIES_PROP);
		newMessage.updateProperty(ContactGraphRouter.XMIT_COPIES_PROP,
				xmitCopies.clone());
				*/
		localRouter.putMessageIntoLimbo(message);
	}
	
	/*
	 * METHODS USED BY OPPORTUNISTIC CGR
	 */
	
	static MessageStatus getMessageStatus(Message message)
	{
		return ContactGraphRouter.getMessageStatus(message);
	}
	
	static int getMessageXmitCopiesCount(Message message)
	{
		HashSet<Integer> result;
		MessageStatus status = getMessageStatus(message);
		result = status.getXmitCopies();
		if (result != null)
			return result.size();
		return -1;
	}
	
	static int[] getMessageXmitCopies(Message message)
	{
		HashSet<Integer> result;
		MessageStatus status = getMessageStatus(message);
		result = status.getXmitCopies();
		if (result != null)
		{
			if (result.size() > 0)
			{
				return result.stream().mapToInt(i->i).toArray();
			}
			else return new int[0];
		}
		return null;
	}

	static double getMessageDlvConfidence(Message message)
	{
		double result;
		MessageStatus status = getMessageStatus(message);
		result = status.getDlvConfidence();
		return result;
	}
	
	static void setMessageXmitCopies(Message message, int[] copies)
	{
		MessageStatus status = getMessageStatus(message);
		int copiesCount = getMessageXmitCopiesCount(message);
		if (copies.length == copiesCount)
		{
			// did not change
			return;
		}
		HashSet<Integer> javaCopies = status.getXmitCopies();
		for (int c : copies)
		{
			javaCopies.add(c);
		}
	}
	
	static void setMessageDlvConfidence(Message message, double conf)
	{
		MessageStatus status = getMessageStatus(message);
		status.setDlvConfidence(conf);
	}
	
	static void sendDiscoveryInfo(long destinationNode, long fromNode,
			long toNode, long fromTime, long toTime, int xmitSpeed)
	{
		DTNHost local = getNodeFromNbr(destinationNode);
		OpportunisticContactGraphRouter localRouter = 
				(OpportunisticContactGraphRouter) local.getRouter();
		localRouter.addDiscoveryInfo(fromNode, toNode, fromTime, 
				toTime, xmitSpeed);
	}
}
