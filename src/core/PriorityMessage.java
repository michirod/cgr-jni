package core;

public class PriorityMessage extends Message {

	/** priority of the message */
	private int priority;
	
	/**
	 * Creates a new PriorityMessage.
	 * @param from Who the message is (originally) from
	 * @param to Who the message is (originally) to
	 * @param id Message identifier (must be unique for message but
	 * 	will be the same for all replicates of the message)
	 * @param size Size of the message (in bytes)
	 * @param priority Priority of the message (range 0..2, from "bulk" to "expedited")
	 */
	public PriorityMessage(DTNHost from, DTNHost to, String id, int size, int priority) {
		super(from, to, id, size);
		this.priority = priority;
	}

	/** Returns the priority of the PriorityMessage 
	 * @return the priority of the PriorityMessage
	 */
	public int getPriority() {
		return priority;
	}

	@Override
	protected void copyFrom(Message m) {
		super.copyFrom(m);
		//this.priority = ((PriorityMessage)m).getPriority();
	}

	@Override
	public Message replicate() {
		PriorityMessage m = new PriorityMessage(this.getFrom(), this.getTo(), this.getId(), this.getSize(), priority);
		m.copyFrom(this);
		return m;
	}
	
	

	
}
