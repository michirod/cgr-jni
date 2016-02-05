package cgr_jni.lyst;

/**
 * Classe che implementa un elemento di LinkedList
 * @author michele
 *
 */
public class LystElt {
	
	private Lyst list;
	/**
	 * Queste sono inutili
	 */
	private LystElt prev, next;
	/**
	 * data e' un long perche' prende dal C direttamente il puntatore all'oggetto da mettere nella lista
	 * In pratica un elemento della lista e' un puntatore a un'area di memoria gestita dal C.
	 * Il C lo salva nella lista e lo tira fuori quando gli serve, usando funzioni di java.
	 */
	private long data;
	
	public LystElt(long data)
	{
		this.data = data;
		this.list = null;
		this.prev = null;
		this.next = null;
	}

	public Lyst getList() {
		return list;
	}

	public void setList(Lyst list) {
		this.list = list;
	}

	public LystElt getPrev() {
		return prev;
	}

	public void setPrev(LystElt prev) {
		this.prev = prev;
	}

	public LystElt getNext() {
		return next;
	}

	public void setNext(LystElt next) {
		this.next = next;
	}

	public long getData() {
		return data;
	}

	public void setData(long data) {
		this.data = data;
	}
	
	

}
