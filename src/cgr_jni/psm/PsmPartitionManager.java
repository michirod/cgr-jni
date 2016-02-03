package cgr_jni.psm;

import java.util.HashMap;

public class PsmPartitionManager {
	
	private static HashMap<Long, PsmNodePartitionManager> nodes = new HashMap<>();
	
	public static PsmPartition newPartition(long nodeNum, int partNum)
	{
		PsmNodePartitionManager nodeMan;
		if (! nodes.keySet().contains(nodeNum))
		{
			nodeMan = new PsmNodePartitionManager(nodeNum);
			nodes.put(nodeNum, nodeMan);
		}
		else
		{
			nodeMan = nodes.get(nodeNum);
		}
		return nodeMan.newPartition(partNum);
	}
	
	public static PsmPartition getPartition(long nodeNum, int partNum)
	{
		PsmNodePartitionManager nodeMan = nodes.get(nodeNum);
		if (nodeMan != null)
			return nodeMan.getPartition(partNum);
		else
			return null;
	}
	
	public static void erasePartition(long nodeNum, int partNum)
	{
		PsmNodePartitionManager nodeMan = nodes.get(nodeNum);
		if (nodeMan != null)
		{
			nodeMan.removePartition(partNum);
		}
	}

}
