package input;

import java.util.ArrayList;
import java.util.List;

import core.Settings;

public class ExtendedEventQueueHandler extends EventQueueHandler
{
	/** Event queue settings main namespace ({@value})*/
	public static final String SETTINGS_NAMESPACE = "Events";
	/** number of event queues -setting id ({@value})*/
	public static final String NROF_SETTING = "nrof";

	/** name of the events class (for class based events) -setting id
	 * ({@value}) */
	public static final String CLASS_SETTING = "class";
	/** name of the package where event generator classes are looked from */
	public static final String CLASS_PACKAGE = "input";

	/** number of events to preload from file -setting id ({@value})*/
	public static final String PRELOAD_SETTING = "nrofPreload";
	/** path of external events file -setting id ({@value})*/
	public static final String PATH_SETTING = "filePath";

	private List<EventQueue> queues;
	
	public ExtendedEventQueueHandler()
	{
		super();
		Settings settings = new Settings(SETTINGS_NAMESPACE);
		int nrof = settings.getInt(NROF_SETTING);
	
		queues = new ArrayList<EventQueue>();
		
		for (int i=1; i <= nrof; i++)
		{
			Settings s = new Settings(SETTINGS_NAMESPACE + i);

			if (s.contains(PATH_SETTING))	// external events file 
			{ 
				int preload = 0;
				String path = "";
				if (s.contains(PRELOAD_SETTING))
				{
					preload = s.getInt(PRELOAD_SETTING);
				}
				
				path = s.getSetting(PATH_SETTING);
				queues.add(new ExtendedExternalEventsQueue(path, preload));
			}
			else if (s.contains(CLASS_SETTING))		// event generator class
			{ 
				String className = CLASS_PACKAGE + "." +s.getSetting(CLASS_SETTING);
				EventQueue eq = (EventQueue)s.createIntializedObject(className);

				queues.add(eq);
			}
		}
	}
}
