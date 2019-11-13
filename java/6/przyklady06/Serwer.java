package przyklady06;

import java.util.ArrayList;
import java.util.List;

public class Serwer {
	private final int ZASOBY = 5;
	private final int grupy;
	private final List<Integer> wolneZasoby = new ArrayList<>();
	
	private final int[] zużycie;
	private final int[] coMaGrupa;
	
	public Serwer(int grupy) {
		for (int i=0; i<ZASOBY; i++) {
			wolneZasoby.add(i);
		}
		this.grupy = grupy;
		zużycie = new int[grupy];
		coMaGrupa = new int[grupy];
		
		for (int i=0; i<grupy; i++) {
			zużycie[i] = 0;
			coMaGrupa[i] = -1;
		}
	}
	
	public synchronized void skończyłem(int grupa, int zasób) {
		zużycie[zasób]--;
		if (zużycie[zasób] == 0) {
			wolneZasoby.add(zasób);
			coMaGrupa[grupa] = 0;
			notifyAll();
		}
	}
	
	public synchronized int chcęKorzystać(int grupa) throws InterruptedException {
		while (wolneZasoby.isEmpty() && coMaGrupa[grupa] == -1) {
			wait();
		}
		
		if (coMaGrupa[grupa] != -1) {
			zużycie[coMaGrupa[grupa]]++;
			return coMaGrupa[grupa];
		}
		
		int ind = 0;
		int zasob = wolneZasoby.get(ind);
		coMaGrupa[grupa] = zasob;
		zużycie[zasob]++;
		wolneZasoby.remove(ind);
		System.out.println("Pracownik z grupy " + grupa + " kradnie zasób " + zasob); 
		return zasob;
	}
}