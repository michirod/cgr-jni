package cgr_jni.lyst;

import java.util.LinkedList;
/**
 * Classe che implementa un gestore di LinkedList
 * 
 * @author michele
 *
 */

public class MyLyst {
	
	private LinkedList<MyLystElt> list;
	private long deleteCallbackFunction = 0;
	private long deleteFunctionUserdata = 0;
		
	public MyLyst()
	{
		this.list = new LinkedList<>();
	}
	
	public void clear()
	{
		list.clear();
	}
	
	public MyLystElt insertFirst(long data)
	{
		MyLystElt el = new MyLystElt(data);
		el.setList(this);
		list.addFirst(el);
		return el;
	}
	
	public MyLystElt insertLast(long data)
	{
		MyLystElt el = new MyLystElt(data);
		el.setList(this);
		list.addLast(el);
		return el;
	}
		
	public static MyLyst lyst_create_using(int idx)
	{
		return lyst_create();
	}

	public static MyLyst lyst_create() {
		return new MyLyst();
	}
	
	public static void lyst_clear(MyLyst list)
	{
		list.clear();
	}
	
	public static void lyst_destroy(MyLyst list)
	{
		list.clear();
		list = null;
	}
	
	public static MyLystElt lyst_insert_last(MyLyst list, long data)
	{
		return list.insertLast(data);
	}
	
	public static void lyst_delete(MyLystElt el)
	{
		el.getList().list.remove(el);
		el = null;
	}
	
	public static MyLystElt lyst_first(MyLyst list)
	{
		if (list.list.isEmpty())
			return null;
		return list.list.getFirst();
	}
	
	public static MyLystElt lyst_last(MyLyst list)
	{
		if (list.list.isEmpty())
			return null;
		return list.list.getLast();
	}
	
	public static MyLystElt lyst_next(MyLystElt el)
	{
		LinkedList<MyLystElt> list = el.getList().list;
		MyLystElt result = null;
		int i = list.indexOf(el);
		try {
			result = list.get(i + 1);
		} catch (IndexOutOfBoundsException e)
		{
			return null;
		}
		return result;
	}
	
	public static MyLystElt lyst_prev(MyLystElt el)
	{
		LinkedList<MyLystElt> list = el.getList().list;
		int i = list.indexOf(el);
		if (i == 0)
			return null; // el is the first element of the list
		else
			return list.get(i - 1);
	}
	
	public static long lyst_data(MyLystElt el)
	{
		return el.getData();
	}

	public static long lyst_data_set(MyLystElt el, long data)
	{
		long prevData = el.getData();
		el.setData(data);
		return prevData;
	}

	public static void lyst_delete_set(MyLyst list, long callbackFunction, long userdata)
	{
		list.deleteCallbackFunction = callbackFunction;
		list.deleteFunctionUserdata = userdata;
	}
	
	public static long getDeleteFunction(MyLyst list)
	{
		return list.deleteCallbackFunction;
	}
	public static long getDeleteUserdata(MyLyst list)
	{
		return list.deleteFunctionUserdata;
	}
	
	
	public static MyLyst getLyst(MyLystElt elt)
	{
		return elt.getList();
	}
	
}
