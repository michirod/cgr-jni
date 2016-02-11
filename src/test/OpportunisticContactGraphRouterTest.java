package test;

import java.io.File;

import cgr_jni.Utils;
import core.DTNHost;
import core.Message;
import core.NetworkInterface;
import core.SimClock;
import core.SimScenario;
import routing.MessageRouter;
import routing.OpportunisticContactGraphRouter;

public class OpportunisticContactGraphRouterTest extends AbstractRouterTest {
	private static final int NROF_HOSTS = 6;
	private OpportunisticContactGraphRouter r1,r2,r3,r4,r5,r6;
	protected static final int TRANSMIT_SPEED = 10000;
	
	@Override
	public void setUp() throws Exception {
		ts.putSetting(SimScenario.SCENARIO_NS + "." + 
				SimScenario.NROF_GROUPS_S, "1");
		ts.putSetting(SimScenario.GROUP_NS + "." + 
				core.SimScenario.NROF_HOSTS_S, "" + NROF_HOSTS);
		ts.putSetting(Message.TTL_SECONDS_S, "true");
		ts.putSetting(MessageRouter.MSG_TTL_S, "3600");
		ts.putSetting(TestUtils.IFACE_NS + "." + 
				NetworkInterface.TRANSMIT_RANGE_S, "1");
		ts.putSetting(TestUtils.IFACE_NS + "." + 
				NetworkInterface.TRANSMIT_SPEED_S, ""+TRANSMIT_SPEED);
		OpportunisticContactGraphRouter routerProto = 
				new OpportunisticContactGraphRouter(ts);
		setRouterProto(routerProto);
		super.setUp();	
		Utils.init(utils.getAllHosts());
		for (DTNHost h : utils.getAllHosts())
		{
			disconnect(h);
		}
		
		r1 = (OpportunisticContactGraphRouter)h1.getRouter();
		r2 = (OpportunisticContactGraphRouter)h2.getRouter();
		r3 = (OpportunisticContactGraphRouter)h3.getRouter();
		r4 = (OpportunisticContactGraphRouter)h4.getRouter();
		r5 = (OpportunisticContactGraphRouter)h5.getRouter();
		r6 = (OpportunisticContactGraphRouter)h6.getRouter();
	}
	
	@Override
	public void tearDown() throws Exception 
	{
		for (DTNHost h : Utils.getAllNodes())
		{
			OpportunisticContactGraphRouter r = 
					(OpportunisticContactGraphRouter) h.getRouter();
			r.finalize();
		}
	}

	/**
	 * TEST 1
	 * Each node has a contact with his previous and next nodeNbr
	 * All contacts happen simultaneously.
	 * Each node create a message for his next nodeNbr.
	 * Messages should be delivered through a single hop route.
	 */
	public void testRouting1()
	{		
		//test visuale
		r3.processLine("l range");
		r3.processLine("l contact");
		
		Message m1 = new Message(h1,h2, msgId1, 10);
		h1.createNewMessage(m1);
		Message m2 = new Message(h2,h3, msgId2, 10);
		h2.createNewMessage(m2);
		Message m3 = new Message(h3,h4, msgId3, 10);
		h3.createNewMessage(m3);
		Message m4 = new Message(h4,h5, msgId4, 10); 
		h4.createNewMessage(m4);
		Message m5 = new Message(h5,h6, msgId5, 10);
		h5.createNewMessage(m5);
		Message m6 = new Message(h6,h1, "pippo", 10);
		h6.createNewMessage(m6);
		checkCreates(6);
		
		updateAllNodes();
		
 		assertEquals(1, r1.getLimboSize());
 		assertEquals(1, r2.getLimboSize());
 		assertEquals(1, r3.getLimboSize());
 		assertEquals(1, r4.getLimboSize());
 		assertEquals(1, r5.getLimboSize());
 		assertEquals(1, r6.getLimboSize());
		
		clock.advance(11);
		h1.forceConnection(h2, null, true);
		h2.forceConnection(h3, null, true);
		h3.forceConnection(h4, null, true);
		h4.forceConnection(h5, null, true);
		h5.forceConnection(h6, null, true);
		h6.forceConnection(h1, null, true);

		for (int i = 0; i < 2000; i++)
		{
			clock.advance(0.1);
			updateAllNodes();
		}

		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 0);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 0);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 0);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 0);	
		
		assertEquals(true, r2.isDeliveredMessage(m1));
		assertEquals(true, r3.isDeliveredMessage(m2));
		assertEquals(true, r4.isDeliveredMessage(m3));
		assertEquals(true, r5.isDeliveredMessage(m4));
		assertEquals(true, r6.isDeliveredMessage(m5));
		assertEquals(true, r1.isDeliveredMessage(m6));
	}
	
	/**
	 * TEST 2
	 * Each node has a contact with his previous and next nodeNbr
	 * All contacts happen simultaneously.
	 * Node 1 creates 20 Messages whose destination are all the simulated
	 * nodes (node 1 included)
	 */
	public void testRouting2()
	{		
		Message m;
		int i;
		disconnect(h1);
		for (i = 0; i < 20; i++)
		{
			m = new Message(h1, Utils.getHostFromNumber((i % 5) + 1), "MSG_" + i, 10);
			h1.createNewMessage(m);
		}
		updateAllNodes();
		clock.advance(11);
		h1.forceConnection(h2, null, true);
		testWait(1, 1);
		h2.forceConnection(h3, null, true);
		testWait(1, 1);
		h3.forceConnection(h4, null, true);
		testWait(1, 1);
		h4.forceConnection(h5, null, true);
		testWait(1, 1);
		h5.forceConnection(h1, null, true);

		updateAllNodes();
		
		for (int j = 0; j < 2000; j++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		
		int deliveredCount = 0;
		for (DTNHost h : Utils.getAllNodes())
		{
			OpportunisticContactGraphRouter r = (OpportunisticContactGraphRouter) h.getRouter();
			deliveredCount += r.getDeliveredCount();
		}
		
		assertEquals(20, deliveredCount);
	}
	
	public void testReq_0033_prob_cgr()
	{
		String cp_path = (new 
				File("resources/cp_req-0033-prob-CGR.txt")).getAbsolutePath();
		r1.readContactPlan(cp_path);
		r2.readContactPlan(cp_path);
		r3.readContactPlan(cp_path);
		r4.readContactPlan(cp_path);
		r5.readContactPlan(cp_path);
		r6.readContactPlan(cp_path);
		// visual test
		r3.processLine("l range");
		r3.processLine("l contact");

		
		Message m1 = new Message(h1, h6, "MSG_0", 10);
		Message m2 = new Message(h1, h4, "MSG_1", 10);
		Message m3 = new Message(h1, h5, "MSG_2", 10);
		
		updateAllNodes();
		clock.advance(5);
		h1.createNewMessage(m1);
		
		testWait(6, 0.1);
		h1.forceConnection(h2, null, true);
		testWait(3, 0.1);
		h1.createNewMessage(m2);
		testWait(6, 0.1);
		disconnect(h1);
		disconnect(h2);
		testWait(5, 0.1);
		h1.forceConnection(h3, null, true);
		testWait(3, 0.1);
		h1.createNewMessage(m3);
		testWait(6, 0.1);
		disconnect(h1);
		disconnect(h3);
		testWait(5, 0.1);
		h2.forceConnection(h4,  null,  true);
		testWait(8, 0.1);
		disconnect(h2);
		disconnect(h4);
		testWait(5, 0.1);
		h3.forceConnection(h5, null, true);
		testWait(8, 0.1);
		disconnect(h3);
		disconnect(h5);
		assertEquals(1, r1.getLimboSize());
		assertEquals(true, r4.isDeliveredMessage(m2));
		assertEquals(true, r5.isDeliveredMessage(m3));
	}
	
	public void testRoutingSimple()
	{
		testWait(30, 1);
		r1.processLine("l range");
		r1.processLine("l contact");
		r2.processLine("l range");
		r2.processLine("l contact");
		System.out.println();
		System.out.println("" + SimClock.getTime() + ": Node 1 and Node2 connected");
		h1.forceConnection(h2, null, true);
		testWait(10, 1);
		r1.processLine("l range");
		r1.processLine("l contact");
		r2.processLine("l range");
		r2.processLine("l contact");
		System.out.println();
		System.out.println("" + SimClock.getTime() + ": Node 1 and Node2 disconnected");
		disconnect(h1);
		r1.processLine("l range");
		r1.processLine("l contact");
		r2.processLine("l range");
		r2.processLine("l contact");
		testWait(30, 1);
		System.out.println();
		System.out.println("" + SimClock.getTime() + ": Node 1 and Node2 connected");
		h1.forceConnection(h2, null, true);
		testWait(10, 1);
		r1.processLine("l range");
		r1.processLine("l contact");
		r2.processLine("l range");
		r2.processLine("l contact");
		System.out.println();
		System.out.println("" + SimClock.getTime() + ": Node 1 and Node2 disconnected");
		disconnect(h1);
		r1.processLine("l range");
		r1.processLine("l contact");
		r2.processLine("l range");
		r2.processLine("l contact");
		testWait(30, 1);
		System.out.println();
		System.out.println("" + SimClock.getTime() + ": Node 1 and Node2 connected");
		h1.forceConnection(h2, null, true);
		testWait(10, 1);
		r1.processLine("l range");
		r1.processLine("l contact");
		r2.processLine("l range");
		r2.processLine("l contact");
		System.out.println();
		System.out.println("" + SimClock.getTime() + ": Node 1 and Node2 disconnected");
		disconnect(h1);
		r1.processLine("l range");
		r1.processLine("l contact");
		r2.processLine("l range");
		r2.processLine("l contact");
		testWait(30, 1);
		
		assertEquals(1, 1);
	}
	
	protected void testWait(double sec, double gran)
	{
		sec = sec / gran;
		for (int i = 0; i < sec; i++)
		{
			clock.advance(gran);
			updateAllNodes();
		}
	}
}
