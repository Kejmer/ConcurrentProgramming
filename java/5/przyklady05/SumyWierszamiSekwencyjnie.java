package przyklady05;

import java.util.function.IntBinaryOperator;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class SumyWierszamiSekwencyjnie {

    private static final int WIERSZE = 100;
    private static final int KOLUMNY = 10;
    private static final int WATKI = 4;
    
    private static final ExecutorService pula = Executors.newFixedThreadPool(WATKI);
    private static IntBinaryOperator macierz;
    private static final ConcurrentMap<Integer, LinkedBlockingQueue<Future<Integer> > > kolejka = new ConcurrentHashMap<>();
    
    private static final int[] sumy = new int[WIERSZE];
    
    private static class Licznik implements Callable<Integer> {
        private final int wiersz;
        private final int kolumna;
        
        public Licznik(int wiersz, int kolumna) {
            this.wiersz = wiersz;
            this.kolumna = kolumna;
        }
        
        @Override
        public Integer call() {
            return macierz.applyAsInt(wiersz, kolumna);
        } 
    }
    
    private static class Menago implements Runnable {
        private final int kolumna;
        
        public Menago(int kolumna) {
            this.kolumna = kolumna;
        }
        
        @Override
        public void run() {
            try {
                for (int i = 0; i < WIERSZE; i++) {
                    LinkedBlockingQueue<Future<Integer>> q = kolejka.computeIfAbsent(i, (k) -> new LinkedBlockingQueue<Future<Integer>>());
                    Callable<Integer> praca = new Licznik(i, kolumna);
                    Future<Integer> zlecenie = pula.submit(praca);
                    q.put(zlecenie);
                }
            } catch (InterruptedException e) {
                System.out.println(kolumna + " przerwany.");
            } catch (Exception e) {
                
            }
        }
    }

    private static void piszSumyWierszy(int wiersze, int kolumny) {
        try {
            for (int k = 0; k < kolumny; ++k) {
                pula.submit(new Menago(k));
            }
            
            for (int k = 0; k < wiersze; k++) {
                LinkedBlockingQueue<Future<Integer>> q = kolejka.computeIfAbsent(k, (l) -> new LinkedBlockingQueue<Future<Integer>>());
                int sum = 0;
                
                try {
                    for (int i = 0; i < KOLUMNY; i++) {
                        sum += q.take().get();
                    }
                    System.out.println(k + " " + sum);
                } catch (InterruptedException e) {
                    System.out.println("Sumator " + k + " przerwany.");  
                } catch (ExecutionException e) {
                    System.out.println("Licznik " + k + " przerwany.");  
                }  
            }
            
        } finally {
            pula.shutdown();
        }
    }

    public static void main(String[] args) {
        macierz = (wiersz, kolumna) -> {
            int a = 2 * kolumna + 1;
            return (wiersz + 1) * (a % 4 - 2) * a;
        };
        piszSumyWierszy(WIERSZE, KOLUMNY);
    }

}
