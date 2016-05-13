package core;

import input.ExtendedEventQueueHandler;

public class ExtendedSimScenario extends SimScenario
{
	protected ExtendedSimScenario()
	{
		super.eqHandler = new ExtendedEventQueueHandler();
	}
}
