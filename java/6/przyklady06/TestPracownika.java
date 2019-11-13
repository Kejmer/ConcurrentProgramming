package przyklady06;

public class TestPracownika {
	private final static int GRUPY = 3;
	
	public static void main(String[] args) {
		Serwer serwer = new Serwer(GRUPY);
		
		for (int i=0; i<GRUPY*2; i++) {
			System.out.println("Start " + i );
			Thread worker = new Thread(new Pracownik(i/2, serwer));
			worker.start();
		}
	}
}