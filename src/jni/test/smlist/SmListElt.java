package jni.test.smlist;

import jni.test.list.DefaultListElt;
import jni.test.psm.PsmAddress;

public class SmListElt  extends DefaultListElt{
	
	public SmListElt(SmList list, SmListElt prev, SmListElt next, PsmAddress data)
	{
		super(list, prev, next, data);
	}

	public PsmAddress getData() {
		return (PsmAddress) data;
	}

	public void setData(PsmAddress data) {
		this.data = data;
	}	

}
