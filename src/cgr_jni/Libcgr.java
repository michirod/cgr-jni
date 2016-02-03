package cgr_jni;

import core.Message;

public class Libcgr {

	static {
		 System.loadLibrary("cgr_jni");
	}
	
	public static native int initializeNode(int nodeNum);
	public static native int finalizeNode(int nodeNum);
	public static native int readContactPlan(int nodeNum, String fileName);
	public static native int processLine(int nodeNum, String contactLine);
	public static native int cgrForward(int nodeNum, Message bundle, long terminusNodeNbr);
	public static native int genericTest(int nodeNum, Message message);

}
