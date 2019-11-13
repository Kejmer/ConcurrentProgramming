package przyklady06;

import java.util.Random;

public class Pracownik implements Runnable {

    private final int grupa;
    private final Serwer serwer;
    private final Random RANDOM = new Random();

    private void własneSprawy() throws InterruptedException{
        // Thread.sleep(RANDOM.nextInt(2));
        Thread.sleep(500*grupa);
    }

    private void korzystam(int zasób) throws InterruptedException{
        Thread.sleep(2000);
    }

    public Pracownik(int grupa, Serwer serwer) {
        this.grupa = grupa;
        this.serwer = serwer;
    }

    @Override
    public void run() {
        try {
            System.out.println("Grupa " + grupa + " zaczyna");
            while (true) {
                własneSprawy();
                int zasób = serwer.chcęKorzystać(grupa);
                korzystam(zasób);
                serwer.skończyłem(grupa, zasób);
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.err.println("Pracownik grupy " + grupa + " przerwany");
        }
    }

}