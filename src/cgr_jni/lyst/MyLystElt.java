package cgr_jni.lyst;

/**
 * Classe che implementa un elemento di LinkedList
 * @author michele
 *
 */
public class MyLystElt {
	
	private MyLyst list;
	/**
	 * Queste sono inutili
	 */
	private MyLystElt prev, next;
	/**
	 * data e' un long perche' prende dal C direttamente il puntatore all'oggetto da mettere nella lista
	 * In pratica un elemento della lista e' un puntatore a un'area di memoria gestita dal C.
	 * Il C lo salva nella lista e lo tira fuori quando gli serve, usando funzioni di java.
	 */
	private long data;
	
	public MyLystElt(long data)
	{
		this.data = data;
		this.list = null;
		this.prev = null;
		this.next = null;
	}

	public MyLyst getList() {
		return list;
	}

	public void setList(MyLyst list) {
		this.list = list;
	}

	public MyLystElt getPrev() {
		return prev;
	}

	public void setPrev(MyLystElt prev) {
		this.prev = prev;
	}

	public MyLystElt getNext() {
		return next;
	}

	public void setNext(MyLystElt next) {
		this.next = next;
	}

	public long getData() {
		return data;
	}

	public void setData(long data) {
		this.data = data;
	}
	
	

}
