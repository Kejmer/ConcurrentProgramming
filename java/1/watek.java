class Loader extends Thread {
  public void run() {
    for (int i=0; i<100; i++)
      System.out.println("BOO " + i);
  }
}

class Hugo extends Thread {
  public void run() {
    for (int i=0; i<100; i++)
      System.out.println("foo " + i);
  }
}

class watek {
  public static void main(String[] args) {
    Loader obj = new Loader();
    Hugo dutka = new Hugo();
    obj.start();
    dutka.start();
    for (int i=0; i<100; i++)
      System.out.println("Hi " + i);
  }
}
