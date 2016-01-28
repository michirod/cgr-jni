package jni.test.smlist;

import jni.test.list.DefaultList;
import jni.test.psm.PsmAddress;

public class SmList extends DefaultList{
	
	public PsmAddress getUserData()
	{
		return (PsmAddress) userData;
	}
	
	public void setUserData(PsmAddress userData)
	{
		super.setUserData(userData);
	}
	
	
	public SmListElt insertFirst(PsmAddress data)
	{
		return (SmListElt) super.insertFirst(data);
	}
	
	public SmListElt insertLast(PsmAddress data)
	{
		return (SmListElt) super.insertLast(data);
	}
	
	public SmListElt insertBefore(SmListElt ref, PsmAddress data)
	{
		return (SmListElt) super.insertBefore(ref, data);
	}
	
	public SmListElt insertAfter(SmListElt ref, PsmAddress data)
	{
		return (SmListElt) super.insertAfter(ref, data);
	}
}
