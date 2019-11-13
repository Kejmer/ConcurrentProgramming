package Watek;

public class Pierwsze {
	private static volatile boolean czyZlozona = false;
	private static volatile int runningThreads;
	
	private static class Miner implements Runnable {
		
		private int initial;
		private final int liczba;
		
		public Miner(int init, int liczba) {
			this.initial = init;
			this.liczba = liczba;
		} 
		
		@Override
		public void run() {
			while (initial*initial <= n) {
				if (n % initial == 0) {
					czyZlozona = true;
				}
				initial += 30;
			}
			runningThreads--;
		}
	}
	
	public static boolean sprawdzCzyPierwsza(int n) {
		czyZlozona = false;
		int first_primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
		for (int i : first_primes) {
			if (n%i == 0) {
				return false;
			}
		} 
		
		int initial_values[8] = {31, 37, 41, 43, 47, 49, 53, 59};
		for (int i : initial_values) {
			Thread t = new Thread(new Miner(i, n));
			t.start();
			runningThreads++;
		}
		while(runningThreads)
		
	}
	
	public static void main(String args[]) {
		int wyn = 0;
		for (int i=2; i<=10000; i++) {
			if (sprawdzCzyPierwsza(i))
				wyn++;
		}
		System.out.println(wyn);
	}
}