package cgr_jni.smlist;

import cgr_jni.list.DefaultList;
import cgr_jni.psm.PsmAddress;

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
