package cgr_jni;

import java.util.Collection;

import core.DTNHost;
import core.SimScenario;

public class Utils {
	
	protected static Utils instance = null;
	protected Collection<DTNHost> hostsReference;
	
	/**
	 * Default constructor.
	 * Get hosts from SimScenario
	 */
	protected Utils()
	{
		this(null);
	}
	protected Utils(Collection<DTNHost> hosts)
	{
		if (hosts == null) // Default value
			hostsReference = SimScenario.getInstance().getHosts();
		else
			hostsReference = hosts;
	}
	
	public static Utils init(Collection<DTNHost> hosts)
	{
		instance = new Utils(hosts);
		return instance;
	}
	
	public static Utils init()
	{
		instance = new Utils();
		return instance;
	}
	
	public static Utils getInstance()
	{
		if (instance == null)
			instance = new Utils();
		return instance;
	}
	
	protected Collection<DTNHost> getAllNodesFromEnv()
	{
		return hostsReference;
	}
	
	public static Collection<DTNHost> getAllNodes()
	{
		return getInstance().getAllNodesFromEnv();
	}

	public static DTNHost getNodeFromNumber(long nodeNbr)
	{
		for(DTNHost host : getAllNodes()){
			if(host.getAddress() == nodeNbr){
				return host;
			}
		}
		return null;
	}

}
