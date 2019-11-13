package Watek;

public class Watek {
	private static class Worker implements Runnable {
		private static final int N_WATKOW = 100;
		
		private final int zanuzenie;
		
		public Worker(int zanuzenie) {
			this.zanuzenie = zanuzenie;
		}
		
		@Override
		public void run() {
			Thread t = Thread.currentThread();
			t.setName("Podwątek " + zanuzenie);
			long start = System.currentTimeMillis();
			System.out.println(t.getName() + " rozpoczyna pracę.");
			Thread dziecko = new Thread(new Worker(zanuzenie + 1));
			if (zanuzenie < N_WATKOW)
				dziecko.start();
			
			for (int i=0; i<10; i++) {
				try {
					Thread.sleep(10 * (N_WATKOW - zanuzenie + 1)); 
				} catch (Exception e) {
					
				}
				System.out.println(t.getName() + " pracuje " + i + " raz!");
			}
			
			while (dziecko.isAlive()) {
				// pusta
			}
			
			System.out.println(t.getName() + " kończy pracę po " + (System.currentTimeMillis()-start) + "ms");
		}
	}
	
	public static void main(String args[]) {
		Thread delayedWorker = new Thread(new Worker(1));
		System.out.println("ZACZYNAMY WYŚCIG");
		delayedWorker.start();
		while (delayedWorker.isAlive()) {
			// nic
		}
		
		System.out.println("Koniec");
	}
}