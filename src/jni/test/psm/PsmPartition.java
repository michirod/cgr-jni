package jni.test.psm;

import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.TreeSet;

public class PsmPartition {
	private int id;
	private TreeSet<PsmAddress> addresses;
	private LinkedHashMap<String, Long> catalog;
	
	public PsmPartition(int id)
	{
		this.id = id;
		this.catalog = new LinkedHashMap<>(1);
		this.addresses = new TreeSet<>();
	}
	
	public int getId()
	{
		return this.id;
	}
	/**
	 * 
	 * @param name
	 * @param objLocation
	 * @param entry
	 * @return the address of the object on success, -1 if @name is not found in catalog.
	 */
	public long psmLocate(String name)
	{
		Long add = catalog.get(name);
		if (add == null)
		{
			return -1;
		}
		else
		{
			return add.longValue();
		}
	}
	
	public int psmCatlg(String name, long objLocation)
	{
		if (catalog.containsKey(name))
		{
			return -1;
		}
		catalog.put(name, objLocation);
		return 0;
	}
	
	public int psmUncatlg(String name)
	{
		if (! catalog.containsKey(name))
		{
			return -1;
		}
		catalog.remove(name);
		return 0;
	}
	
	public PsmAddress psmAlloc(long pointer)
	{
		PsmAddress a = new PsmAddress(pointer);
		if (addresses.add(a) == false)
			return null;
		return a;
	}
	
	public void psmFree(PsmAddress address)
	{
		addresses.remove(address);
	}
	
	public PsmAddress getAddress(long pointer)
	{
		PsmAddress temp = new PsmAddress(pointer);
		for (PsmAddress a : addresses)
		{
			if (a.equals(temp))
				return a;
		}
		return null; 
	}

}
