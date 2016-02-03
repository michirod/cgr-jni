package cgr_jni.smlist;

import cgr_jni.list.DefaultListElt;
import cgr_jni.psm.PsmAddress;

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
