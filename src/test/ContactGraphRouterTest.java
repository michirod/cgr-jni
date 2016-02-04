package test;

import java.io.File;
import java.util.Collection;

import cgr_jni.Utils;
import core.DTNHost;
import core.Message;
import core.SimScenario;
import routing.ContactGraphRouter;
import routing.MessageRouter;

public class ContactGraphRouterTest extends AbstractRouterTest {
	
	private static final String CONTACT_PLAN_FILE = "resources/contactPlan_prova.txt";
	private static final String CONTACT_PLAN_FILE2 = "resources/cp_prova2.txt";
	private static final String CONTACT_PLAN_TEST4 = "resources/cp_testRouting4.txt";
	
	private static final int NROF_HOSTS = 6;
	private ContactGraphRouter r1,r2,r3,r4,r5,r6;
	private static ContactGraphRouterTest instance = null;
	
	@Override
	public void setUp() throws Exception {
		instance = this;
		ts.putSetting(SimScenario.SCENARIO_NS + "." + 
				SimScenario.NROF_GROUPS_S, "1");
		ts.putSetting(SimScenario.GROUP_NS + "." + 
				core.SimScenario.NROF_HOSTS_S, "" + NROF_HOSTS);
		ts.putSetting(Message.TTL_SECONDS_S, "true");
		ts.putSetting(MessageRouter.MSG_TTL_S, "3600");
		ContactGraphRouter routerProto = new ContactGraphRouter(ts);
		setRouterProto(routerProto);
		super.setUp();	
		Utils.init(utils.getAllHosts());
		
		r1 = (ContactGraphRouter)h1.getRouter();
		r2 = (ContactGraphRouter)h2.getRouter();
		r3 = (ContactGraphRouter)h3.getRouter();
		r4 = (ContactGraphRouter)h4.getRouter();
		r5 = (ContactGraphRouter)h5.getRouter();
		r6 = (ContactGraphRouter)h6.getRouter();
	}
	
	@Override
	public void tearDown() throws Exception 
	{
		for (DTNHost h : Utils.getAllNodes())
		{
			ContactGraphRouter r = (ContactGraphRouter) h.getRouter();
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
	public void testRouting1(){
		
		String cp_path = (new File(CONTACT_PLAN_FILE)).getAbsolutePath();
		r1.readContactPlan(cp_path);
		r2.readContactPlan(cp_path);
		r3.readContactPlan(cp_path);
		r4.readContactPlan(cp_path);
		r5.readContactPlan(cp_path);
		r6.readContactPlan(cp_path);
		
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
		
 		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 1);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 1);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 1);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 1);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 1);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 1);
		
		clock.advance(11);
		h1.forceConnection(h2, null, true);
		h2.forceConnection(h3, null, true);
		h3.forceConnection(h4, null, true);
		h4.forceConnection(h5, null, true);
		h5.forceConnection(h6, null, true);
		h6.forceConnection(h1, null, true);

		for (int i = 0; i < 2000; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}

		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 0);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 0);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 0);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 0);	
		
		assertEquals(true, r2.isDeliveredMessage(m1));
		
		}

	/**
	 * TEST 2
	 * Each node has a contact with his previous and next nodeNbr
	 * All contacts happen simultaneously.
	 * Node 1 creates 20 Messages whose destination are all the simulated
	 * nodes (node 1 included)
	 * All messages should be delivered within the end of the contacts.
	 */
	public void testRouting2()
	{
		String cp_path = (new File(CONTACT_PLAN_FILE)).getAbsolutePath();
		r1.readContactPlan(cp_path);
		r2.readContactPlan(cp_path);
		r3.readContactPlan(cp_path);
		r4.readContactPlan(cp_path);
		r5.readContactPlan(cp_path);
		r6.readContactPlan(cp_path);
		
		Message m;
		int i;
		disconnect(h1);
		for (i = 0; i < 20; i++)
		{
			m = new Message(h1, getNodeFromNbr((i % 5) + 1), "MSG_" + i, 10);
			h1.createNewMessage(m);
		}
		updateAllNodes();
		clock.advance(11);
		h1.forceConnection(h2, null, true);
		h2.forceConnection(h3, null, true);
		h3.forceConnection(h4, null, true);
		h4.forceConnection(h5, null, true);
		h5.forceConnection(h6, null, true);

		for (int j = 0; j < 2000; j++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		
		int deliveredCount = 0;
		for (DTNHost h : Utils.getAllNodes())
		{
			ContactGraphRouter r = (ContactGraphRouter) h.getRouter();
			deliveredCount += r.getDeliveredCount();
		}
		
		assertEquals(20, deliveredCount);
	}
	
	/**
	 * TEST 3
	 * Each node has a contact with his previous and next nodeNbr
	 * Contacts happen sequentially with disconnected intervals.
	 */
	public void testRouting3(){
	 
	    String cp_path = (new File(CONTACT_PLAN_FILE)).getAbsolutePath();
		r1.readContactPlan(cp_path);
		r2.readContactPlan(cp_path);
		r3.readContactPlan(cp_path);
		r4.readContactPlan(cp_path);
		r5.readContactPlan(cp_path);
		r6.readContactPlan(cp_path);
		
		Message m1 = new Message(h1,h3, msgId1, 10);
		h1.createNewMessage(m1);
		Message m2 = new Message(h2,h4, msgId2, 10);
		h2.createNewMessage(m2);
		Message m3 = new Message(h3,h5, msgId3, 10);
		h3.createNewMessage(m3);
		Message m4 = new Message(h4,h6, msgId4, 10); 
		h4.createNewMessage(m4);
		Message m5 = new Message(h5,h6, msgId5, 10);
		h5.createNewMessage(m5);
		Message m6 = new Message(h6,h1, "pippo", 10);
		h6.createNewMessage(m6);
		checkCreates(6);
		
		updateAllNodes();
		
 		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 1);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 1);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 1);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 1);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 1);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 1);
		//1st round, contact 10-30
		clock.advance(10);
		h1.forceConnection(h2, null, true);
		

		for (int i = 0; i < 20; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		//inserisco la disconnect 
		
		disconnect(h1);
		disconnect(h2);		
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 2);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 1);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 1);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 1);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 1);	
		
		//no message delivered, 1st deliver h3 no contact available
		
		//2nd round contact 40-80
		for (int i = 0; i < 10; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}	
		
		h2.forceConnection(h3, null, true);		

		for (int i = 0; i < 40; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}	
		
		disconnect(h2);
		disconnect(h3);		
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 2);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 1);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 1);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 1);
		
		//m1 delivered  
		assertEquals(true, r3.isDeliveredMessage(m1));
		
		//3rd round contact 100-150
		
		for (int i = 0; i < 20; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}		
		
		h3.forceConnection(h4, null, true);		

		for (int i = 0; i < 50; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}		
		
		disconnect(h3);
		disconnect(h4);		
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 0);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 2);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 1);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 1);
		
		//m2 delivered
		
		assertEquals(true, r4.isDeliveredMessage(m2));
		//4th round contact 170-200
		
		for (int i = 0; i < 20; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		
		
		h4.forceConnection(h5, null, true);
		
		for (int i = 0; i < 30; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}		
		
		disconnect(h4);
		disconnect(h5);		
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 0);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 0);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 2);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 1);
		
		//m3 delivered
		assertEquals(true, r5.isDeliveredMessage(m3));
		//5th round contact 250-300
		
		for (int i = 0; i < 50; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}		
		
		h5.forceConnection(h6, null, true);		

		for (int i = 0; i < 50; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		
		
		disconnect(h5);
		disconnect(h6);
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 0);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 0);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 0);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 1);
		
		//m4 delivered
		assertEquals(true, r6.isDeliveredMessage(m4));
		//m5 delivered
		assertEquals(true, r6.isDeliveredMessage(m5));
		//6th round
		
		for (int i = 0; i < 20; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		
		
		h6.forceConnection(h1, null, true);

		for (int i = 0; i < 60; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		
		disconnect(h1);
		disconnect(h6);
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h4).getQueue().size(), 0);
		assertEquals(r4.getOutducts().get(h5).getQueue().size(), 0);
		assertEquals(r5.getOutducts().get(h6).getQueue().size(), 0);
		assertEquals(r6.getOutducts().get(h1).getQueue().size(), 0);
		
		//m6 delivered
		assertEquals(true, r1.isDeliveredMessage(m6));
		

	}

	public void testRouting4(){
		String cp_path = (new File(CONTACT_PLAN_TEST4)).getAbsolutePath();
		r1.readContactPlan(cp_path);
		r2.readContactPlan(cp_path);
		r3.readContactPlan(cp_path);
		r4.readContactPlan(cp_path);
		r5.readContactPlan(cp_path);
		r6.readContactPlan(cp_path);
		
		Message m1 = new Message(h1,h4, msgId1, 10);
		h1.createNewMessage(m1);
		Message m2 = new Message(h1,h5, msgId2, 10);
		h1.createNewMessage(m2);
		Message m3 = new Message(h1,h6, msgId3, 10);
		h1.createNewMessage(m3);
		Message m4 = new Message(h2,h4, msgId4, 10); 
		h2.createNewMessage(m4);
		Message m5 = new Message(h5,h6, msgId5, 10);
		h5.createNewMessage(m5);
		Message m6 = new Message(h4,h5, "pippo", 10);
		h4.createNewMessage(m6);
		checkCreates(6);
		
		updateAllNodes();
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 2);
		assertEquals(r1.getOutducts().get(h3).getQueue().size(), 1);
		assertEquals(r2.getOutducts().get(h4).getQueue().size(), 1);
		assertTrue(r4.isMessageIntoLimbo(m6));
		System.out.println("\nMessage "+m6+"in r4 limbo: no routes available for its destination\n");
		assertTrue(r5.isMessageIntoLimbo(m5));
		System.out.println("\nMessage "+m5+"in r5 limbo: no routes available for its destination\n");
		
		
		clock.advance(10);
		//1st round contacts 10-30
		
		h1.forceConnection(h2, null, true);
		h1.forceConnection(h3, null, true);
		

		for (int i = 0; i < 20; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		//inserisco la disconnect 
		
		disconnect(h1);
		disconnect(h2);
		disconnect(h3);
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r1.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h4).getQueue().size(), 3);
		assertEquals(r3.getOutducts().get(h5).getQueue().size(), 1);
		assertTrue(r4.isMessageIntoLimbo(m6));
		System.out.println("\nMessage "+m6+" in r4 limbo: no routes available for its destination\n");
		assertTrue(r5.isMessageIntoLimbo(m5));
		System.out.println("\nMessage "+m5+" in r5 limbo: no routes available for its destination\n");
		
		//no messages delivered
		//2nd round contacts 70-80 and 60-90
		
		for (int i = 0; i < 30; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}		
		
		h2.forceConnection(h4, null, true);
		
		for (int i = 0; i < 10; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}		
		
		h3.forceConnection(h5, null, true);
		
		for (int i = 0; i < 10; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}		
		
		disconnect(h3);
		disconnect(h5);
		
		for (int i = 0; i < 10; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}		
		
		disconnect(h2);
		disconnect(h4);
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r1.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h4).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h5).getQueue().size(), 0);
		assertEquals(r4.getOutducts().get(h6).getQueue().size(), 1);
		assertTrue(r4.isMessageIntoLimbo(m6));
		System.out.println("\nMessage "+m6+" in r4 limbo: no routes available for its destination\n");
		assertTrue(r5.isMessageIntoLimbo(m5));
		System.out.println("\nMessage "+m5+" in r5 limbo: no routes available for its destination\n");
		
		assertEquals(true, r4.isDeliveredMessage(m1));
		assertEquals(true, r4.isDeliveredMessage(m4));
		assertEquals(true, r5.isDeliveredMessage(m2));
		
		//3rd round contact 100-130
		
		for (int i = 0; i < 10; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		
		h4.forceConnection(h6, null, true);
		
		for (int i = 0; i < 30; i++)
		{
			clock.advance(1);
			updateAllNodes();
		}
		
		
		disconnect(h4);
		disconnect(h6);
		
		
		assertEquals(r1.getOutducts().get(h2).getQueue().size(), 0);
		assertEquals(r1.getOutducts().get(h3).getQueue().size(), 0);
		assertEquals(r2.getOutducts().get(h4).getQueue().size(), 0);
		assertEquals(r3.getOutducts().get(h5).getQueue().size(), 0);
		assertEquals(r4.getOutducts().get(h6).getQueue().size(), 0);
		assertTrue(r4.isMessageIntoLimbo(m6));
		System.out.println("\nMessage "+m6+" in r4 limbo: no routes available for its destination\n");
		assertTrue(r5.isMessageIntoLimbo(m5));
		System.out.println("\nMessage "+m5+" in r5 limbo: no routes available for its destination\n");
		
		assertEquals(true, r6.isDeliveredMessage(m3));
		
	}
	
	/**
	 * TEST 4
	 * Each node has a contact with his previous and next nodeNbr
	 * Contacts happen sequentially with disconnected intervals.
	 * NO CONTACT PLAN PROVIDED TO THE CGR lib, therefore no messages
	 * should be forwarded. All messages should remain into limbo.
	 * 
	 */
	public void testRouting5()
	{
		 /*
		  * NO CONTACT PLAN PROVIDED
		  * Same connections as testRouting3
		  * Messages should remain into limbo
		  */
			
			Message m1 = new Message(h1,h3, msgId1, 10);
			h1.createNewMessage(m1);
			Message m2 = new Message(h2,h4, msgId2, 10);
			h2.createNewMessage(m2);
			Message m3 = new Message(h3,h5, msgId3, 10);
			h3.createNewMessage(m3);
			Message m4 = new Message(h4,h6, msgId4, 10); 
			h4.createNewMessage(m4);
			Message m5 = new Message(h5,h6, msgId5, 10);
			h5.createNewMessage(m5);
			Message m6 = new Message(h6,h1, "pippo", 10);
			h6.createNewMessage(m6);
			checkCreates(6);
			
			updateAllNodes();
			
			//check all messages are into limbo
	 		assertEquals(1, r1.getLimboSize());
	 		assertEquals(1, r2.getLimboSize());
	 		assertEquals(1, r3.getLimboSize());
	 		assertEquals(1, r4.getLimboSize());
	 		assertEquals(1, r5.getLimboSize());
	 		assertEquals(1, r6.getLimboSize());
	 		
			//1st round, contact 10-30
			clock.advance(10);
			h1.forceConnection(h2, null, true);
			

			for (int i = 0; i < 20; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}
			//inserisco la disconnect 
			
			disconnect(h1);
			disconnect(h2);		
			
			//check all message are into limbo
	 		assertEquals(1, r1.getLimboSize());
	 		assertEquals(1, r2.getLimboSize());
	 		assertEquals(1, r3.getLimboSize());
	 		assertEquals(1, r4.getLimboSize());
	 		assertEquals(1, r5.getLimboSize());
	 		assertEquals(1, r6.getLimboSize());
			
			//no message delivered, 1st deliver h3 no contact available
			
			//2nd round contact 40-80
			for (int i = 0; i < 10; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}	
			
			h2.forceConnection(h3, null, true);		

			for (int i = 0; i < 40; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}	
			
			disconnect(h2);
			disconnect(h3);		

			//check all message are into limbo
	 		assertEquals(1, r1.getLimboSize());
	 		assertEquals(1, r2.getLimboSize());
	 		assertEquals(1, r3.getLimboSize());
	 		assertEquals(1, r4.getLimboSize());
	 		assertEquals(1, r5.getLimboSize());
	 		assertEquals(1, r6.getLimboSize());
			
			//3rd round contact 100-150
			
			for (int i = 0; i < 20; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}		
			
			h3.forceConnection(h4, null, true);		

			for (int i = 0; i < 50; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}		
			
			disconnect(h3);
			disconnect(h4);		

			//check all message are into limbo
	 		assertEquals(1, r1.getLimboSize());
	 		assertEquals(1, r2.getLimboSize());
	 		assertEquals(1, r3.getLimboSize());
	 		assertEquals(1, r4.getLimboSize());
	 		assertEquals(1, r5.getLimboSize());
	 		assertEquals(1, r6.getLimboSize());
			
			//4th round contact 170-200
			
			for (int i = 0; i < 20; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}
			
			
			h4.forceConnection(h5, null, true);
			
			for (int i = 0; i < 30; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}		
			
			disconnect(h4);
			disconnect(h5);		

			//check all message are into limbo
	 		assertEquals(1, r1.getLimboSize());
	 		assertEquals(1, r2.getLimboSize());
	 		assertEquals(1, r3.getLimboSize());
	 		assertEquals(1, r4.getLimboSize());
	 		assertEquals(1, r5.getLimboSize());
	 		assertEquals(1, r6.getLimboSize());
	 		
			//5th round contact 250-300
			
			for (int i = 0; i < 50; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}		
			
			h5.forceConnection(h6, null, true);		

			for (int i = 0; i < 50; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}
			
			
			disconnect(h5);
			disconnect(h6);
			

			//check all message are into limbo
	 		assertEquals(1, r1.getLimboSize());
	 		assertEquals(1, r2.getLimboSize());
	 		assertEquals(1, r3.getLimboSize());
	 		assertEquals(1, r4.getLimboSize());
	 		assertEquals(1, r5.getLimboSize());
	 		assertEquals(1, r6.getLimboSize());
			
			//6th round
			
			for (int i = 0; i < 20; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}
			
			
			h6.forceConnection(h1, null, true);

			for (int i = 0; i < 60; i++)
			{
				clock.advance(1);
				updateAllNodes();
			}
			
			disconnect(h1);
			disconnect(h6);

			//check all message are into limbo
	 		assertEquals(1, r1.getLimboSize());
	 		assertEquals(1, r2.getLimboSize());
	 		assertEquals(1, r3.getLimboSize());
	 		assertEquals(1, r4.getLimboSize());
	 		assertEquals(1, r5.getLimboSize());
	 		assertEquals(1, r6.getLimboSize());
			
	}
	
	public static ContactGraphRouterTest getInstance()
	{
		return instance;
	}

	public Collection<DTNHost> getHosts() {
		return utils.getAllHosts();
	}

	public DTNHost getNodeFromNbr(long nodeNbr)
	{
		return Utils.getNodeFromNumber(nodeNbr);
	}

}