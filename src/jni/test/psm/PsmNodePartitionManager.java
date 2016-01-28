package jni.test.psm;

import java.util.HashMap;

public class PsmNodePartitionManager {
	private long nodeNum;
	private HashMap<Integer, PsmPartition> partitions = new HashMap<>();
	
	public PsmNodePartitionManager(long nodeNum)
	{
		this.nodeNum = nodeNum;
	}
	
	public PsmPartition newPartition(int partNum)
	{
		PsmPartition partition = new PsmPartition(partNum);
		addPartition(partition);
		return partition;
	}

	public PsmPartition getPartition(int partNum)
	{
		return partitions.get(partNum);
	}
	
	public void addPartition(PsmPartition partition)
	{
		partitions.put(partition.getId(), partition);
	}
	
	public void removePartition(int partNum)
	{
		partitions.remove(partNum);
	}
}
