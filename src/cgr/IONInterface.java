package cgr;

import routing.ContactGraphRouter;
import routing.ContactGraphRouter.Outduct;
import core.DTNHost;
import core.Message;
import core.SimScenario;

public class IONInterface {	
	
	static void cgrForward (int dtnNode, Message m){
		
	}
	
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
	
	static boolean isOutductBlocked(Outduct jOutduct)
	{
		return false;
	}
	
	static int getMaxPayloadLen(Outduct jOutduct)
	{
		return 0;
	}
	
	static String getOutductName(Outduct jOutduct)
	{
		return "" + jOutduct.getHost().getAddress();
	}
	
	static Outduct getONEOutductToNode(long localNodeNbr, long toNodeNbr){
		DTNHost local = getNodeFromNbr(localNodeNbr);
		DTNHost to= getNodeFromNbr(toNodeNbr);
		
		ContactGraphRouter localRouter = (ContactGraphRouter) local.getRouter();
		return(localRouter.getOutducts().get(to));
		
	}
		
	static DTNHost getNodeFromNbr(long nodeNbr){
		for(DTNHost host : SimScenario.getInstance().getHosts()){
			if(host.getAddress() == nodeNbr){
				return host;
			}
		}
		return null;
		
	}
	
	static int insertBundleIntoOutduct(long localNodeNbr, Message message, long toNodeNbr)
	{
		DTNHost local = getNodeFromNbr(localNodeNbr);
		DTNHost to = getNodeFromNbr(toNodeNbr);
		ContactGraphRouter localRouter = (ContactGraphRouter) local.getRouter();
		if(localRouter.getOutducts().containsKey(to)){
			localRouter.getOutducts().get(to).insertBundleIntoOutduct(message);
			return 0;
		}
		return -1;
	}

	static int insertBundleIntoLimbo(long localNodeNbr, Message message)
	{
		DTNHost local = getNodeFromNbr(localNodeNbr);
		ContactGraphRouter localRouter = (ContactGraphRouter) local.getRouter();
		localRouter.getLimbo().insertBundleIntoOutduct(message);
		return 0;	
	}
}
