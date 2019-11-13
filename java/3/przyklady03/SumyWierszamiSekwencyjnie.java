package przyklady03;

import java.util.function.IntBinaryOperator;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class SumyWierszamiSekwencyjnie {

    private static final int WIERSZE = 10;
    private static final int KOLUMNY = 100;
    
    private static final CyclicBarrier bariera;
    private static final int[][] komorki = new int[WIERSZE][KOLUMNY];
    private static int iter = 0;

    static {
        bariera = new CyclicBarrier(KOLUMNY, new Runnable() {
                
            @Override
            public void run() {
                int suma = 0;
                for (int j = 0; j < KOLUMNY; j++) {
                    suma += komorki[iter][j];
                }
                System.out.println(iter + " " + suma);
                iter++;
            }
        });
    }    
    
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
                    komorki[i][kolumna] = macierz.applyAsInt(i, kolumna);
                    bariera.await();
                }
            } catch (InterruptedException | BrokenBarrierException e) {
                System.out.println(kolumna + " przerwany.");
            }
        }
    }

    private static void piszSumyWierszy(int wiersze, int kolumny, IntBinaryOperator macierz) {
        for (int k = 0; k < kolumny; ++k) {
            new Thread(new Licznik(wiersze, k, macierz)).start();
        }
    }

    public static void main(String[] args) {
        piszSumyWierszy(WIERSZE, KOLUMNY, (wiersz, kolumna) -> {
            int a = 2 * kolumna + 1;
            return (wiersz + 1) * (a % 4 - 2) * a;
        });
    }

}
