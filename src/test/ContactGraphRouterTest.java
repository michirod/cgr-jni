package test;

import java.io.File;

import cgr.Libcgr;
import core.Message;
import core.SimClock;
import core.SimScenario;
import routing.ContactGraphRouter;
import routing.MessageRouter;

public class ContactGraphRouterTest extends AbstractRouterTest {
	
	private static final String CONTACT_PLAN_FILE = "resources/contactPlan_prova.txt";
	private static final int NROF_HOSTS = 6;
	private ContactGraphRouter r1,r2,r3,r4,r5,r6;
	
	@Override
	public void setUp() throws Exception {
		
		ts.putSetting(MessageRouter.B_SIZE_S, ""+BUFFER_SIZE);
		ts.putSetting(SimScenario.SCENARIO_NS + "." + 
				SimScenario.NROF_GROUPS_S, "1");
		ts.putSetting(SimScenario.GROUP_NS + "." + 
				core.SimScenario.NROF_HOSTS_S, "" + NROF_HOSTS);
		ts.putSetting(Message.TTL_SECONDS_S, "true");
		ts.putSetting(MessageRouter.MSG_TTL_S, "3600");
		setRouterProto(new ContactGraphRouter(ts));
		super.setUp();	
		String cp_path = (new File(CONTACT_PLAN_FILE)).getAbsolutePath();
		System.out.println(SimClock.getIntTime());
		r1 = (ContactGraphRouter)h1.getRouter();
		r2 = (ContactGraphRouter)h2.getRouter();
		r3 = (ContactGraphRouter)h3.getRouter();
		r4 = (ContactGraphRouter)h4.getRouter();
		r5 = (ContactGraphRouter)h5.getRouter();
		r6 = (ContactGraphRouter)h6.getRouter();
		r1.readContactPlan(cp_path);
		r2.readContactPlan(cp_path);
		r3.readContactPlan(cp_path);
		r4.readContactPlan(cp_path);
		r5.readContactPlan(cp_path);
		r6.readContactPlan(cp_path);
		
		//test visuale
		r3.processLine("l range");
		r3.processLine("l contact");
			
	}
	
	public void testRouting(){
			
		Message m1 = new Message(h1,h2, msgId2, 1000);
		h1.createNewMessage(m1);
		Message m2 = new Message(h2,h3, msgId3, 1000);
		h2.createNewMessage(m2);
		Message m3 = new Message(h3,h4, msgId4, 1000);
		h3.createNewMessage(m3);
		Message m4 = new Message(h4,h5, msgId5, 1000); 
		h4.createNewMessage(m4);
		Message m5 = new Message(h5,h6, msgId5, 1000);
		h5.createNewMessage(m5);
		Message m6 = new Message(h6,h1, "pippo", 1000);
		h6.createNewMessage(m6);
		
		checkCreates(6);
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 1);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 1);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 1);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 1);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 1);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 1);
		
		clock.advance(11);
		updateAllNodes();
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 0);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 0);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 0);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 0);		
		
		}
}
