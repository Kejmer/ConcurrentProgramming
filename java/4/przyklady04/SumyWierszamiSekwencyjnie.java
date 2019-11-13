package przyklady04;

import java.util.function.IntBinaryOperator;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class SumyWierszamiSekwencyjnie {

    private static final int WIERSZE = 100;
    private static final int KOLUMNY = 10;
    
    private static final ConcurrentMap<Integer, LinkedBlockingQueue<Integer>> kolejka = new ConcurrentHashMap<>();
    // private static final ArrayList<LinkedBlockingQueue<Integer>> kolejka = new ArrayList<LinkedBlockingQueue<Integer>>(); //= new LinkedBlockingQueue<>(KOLUMNY)[WIERSZE];
    
    private static class Licznik implements Runnable {
        private final int wiersze;
        private final int kolumna;
        private final IntBinaryOperator macierz;
        
        public Licznik(int wiersze, int kolumna, IntBinaryOperator macierz) {
            this.wiersze = wiersze;
            this.kolumna = kolumna;
            this.macierz = macierz;
        }
        
        @Override
        public void run() {
            try {
                for (int i = 0; i < wiersze; i++) {
                    LinkedBlockingQueue<Integer> q = kolejka.computeIfAbsent(i, (k) -> new LinkedBlockingQueue<Integer>());
                    q.put(macierz.applyAsInt(i,kolumna));
                }
            } catch (InterruptedException e) {
                System.out.println(kolumna + " przerwany.");
            }
        }
    }

    private static void piszSumyWierszy(int wiersze, int kolumny, IntBinaryOperator macierz) {
      
        for (int k = 0; k < kolumny; ++k) {
            new Thread(new Licznik(wiersze, k, macierz)).start();
        }
        try {
            for (int i = 0; i < WIERSZE; i++) {
                int sum = 0;
                LinkedBlockingQueue<Integer> q = kolejka.computeIfAbsent(i, (k) -> new LinkedBlockingQueue<Integer>());
                for (int j = 0; j < KOLUMNY; j++) 
                    sum += q.take();
                
                System.out.println(i + " " + sum);
            }
        } catch (InterruptedException e) {
            System.out.println("Wątek główny przerwany --- ");
        }
    }

    public static void main(String[] args) {
        piszSumyWierszy(WIERSZE, KOLUMNY, (wiersz, kolumna) -> {
            int a = 2 * kolumna + 1;
            return (wiersz + 1) * (a % 4 - 2) * a;
        });
    }

}
