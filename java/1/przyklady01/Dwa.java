package przyklady01;

public class Dwa {

    private static void pisz() {
        System.out.println(Thread.currentThread());
    }

    private static class Pomocniczy implements Runnable {

        @Override
        public void run() {
            pisz();
        }

    }

    public static void main(String[] args) {
        Thread.currentThread().setName("Główny");
        Runnable r = new Pomocniczy();
        Thread t = new Thread(r, "Pomocniczy");
        t.start();
        pisz();
    }

}
