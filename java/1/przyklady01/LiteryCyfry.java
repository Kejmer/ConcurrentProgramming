package przyklady01;

public class LiteryCyfry {

    private static class Piszący implements Runnable {

        private static final int N_ZNAKOW = 5000;

        private final char pierwszy;
        private final char ostatni;

        public Piszący(char pierwszy, char ostatni) {
            this.pierwszy = pierwszy;
            this.ostatni = ostatni;
        }

        @Override
        public void run() {
            char c = pierwszy;
            for (int i = 0; i < N_ZNAKOW; ++i) {
                System.out.print(c);
                ++c;
                if (c > ostatni) {
                    c = pierwszy;
                }
            }
        }

    }

    public static void main(String args[]) {
        Thread litery = new Thread(new Piszący('a', 'z'));
        Thread cyfry = new Thread(new Piszący('0', '9'));
        System.out.println("Początek");
        litery.start();
        cyfry.start();
        while (litery.isAlive()) {
            // pusta
        }
        while (cyfry.isAlive()) {
            // pusta
        }
        System.out.println();
        System.out.println("Koniec");
    }

}
