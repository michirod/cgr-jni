package cgr_jni;

public class Libocgr extends Libcgr {

	public static native int predictContacts(int nodeNum);
	
	public static native int exchangeCurrentDiscoveredContatcs
	(int nodeNum1, int nodeNum2);
	
	public static native int exchangeContactHistory
	(int nodeNum1, int nodeNum2);
	
	public static native int contactDiscoveryAquired
	(int localNodeNbr, int neighborNodeNbr, int xmitRate);
	
	public static native int contactDiscoveryLost
	(int localNodeNbr, int neighborNodeNbr);

	public static native int applyDiscoveryInfos
	(int localNodeNbr, long fromNode, long toNode, 
			long fromTime, long toTime, int xmitSpeed);
}
