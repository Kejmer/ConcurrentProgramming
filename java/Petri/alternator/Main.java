package alternator;

import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.HashMap;
import java.util.Map;
import java.util.HashSet;
import java.util.ArrayList;
import java.util.Set;

import petrinet.PetriNet;
import petrinet.Transition;

public class Main {
	
private static enum Place {
	CriticalA, CriticalB, CriticalC, MemoryA, MemoryB, MemoryC
}

  private static class Writer implements Runnable {
    private String name;
    private Collection<Transition<Place>> critical;
    private Collection<Transition<Place>> memory;
    private PetriNet net;
    private int numOfPrints;
    
    public Writer(String name, Transition<Place> critical, Transition<Place> memory, PetriNet net) {
      this.name = name;
      this.critical = Collections.singleton(critical);
      this.memory = Collections.singleton(memory);
      this.net = net;
    }
    
    @Override
    public void run() {
      try {
        while (true) {
          net.fire(critical);
          numOfPrints++;
          System.out.println(name);
          System.out.println('.');
          net.fire(memory);
        }
      } catch (InterruptedException e) {
          System.out.println(name + " wykonał się " + numOfPrints + " razy!");
      }
    }
  }
	
	public static void main(String[] args) {
  	//Building the net
    //Places
    Map<Place, Integer> begin = new HashMap<>();
    PetriNet<Place> net = new PetriNet<>(begin, false);
    
    //Transitions
    ArrayList<Transition<Place>> transitions = new ArrayList<>();
    HashMap<Place, Integer> input = new HashMap<>(), output = new HashMap<>();
    HashSet<Place> empty = new HashSet<>(), inhibitor = new HashSet<>();
    
    // Print A
    output.put(Place.CriticalA, 1);
    output.put(Place.MemoryA, 1);
    inhibitor.add(Place.MemoryA);
    inhibitor.add(Place.CriticalB);
    inhibitor.add(Place.CriticalC);
    
    Transition<Place> criticalA = new Transition<Place>(input, empty, inhibitor, output);  
    transitions.add(criticalA);
    output.clear();
    inhibitor.clear();
    
    // Print B
    output.put(Place.CriticalB, 1);
    output.put(Place.MemoryB, 1);
    inhibitor.add(Place.CriticalA);
    inhibitor.add(Place.MemoryB);
    inhibitor.add(Place.CriticalC);
    
    Transition<Place> criticalB = new Transition<Place>(input, empty, inhibitor, output);  
    transitions.add(criticalB);
    output.clear();
    inhibitor.clear();
    
    // Print C
    output.put(Place.CriticalC, 1);
    output.put(Place.MemoryC, 1);
    inhibitor.add(Place.CriticalA);
    inhibitor.add(Place.CriticalB);
    inhibitor.add(Place.MemoryC);
    
    Transition<Place> criticalC = new Transition<Place>(input, empty, inhibitor, output);  
    transitions.add(criticalC);
    output.clear();
    inhibitor.clear();
    
    // Release A
    input.put(Place.CriticalA, 1);
    empty.add(Place.MemoryB);
    empty.add(Place.MemoryC);
  
    Transition<Place> memoryA = new Transition<Place>(input, empty, inhibitor, output);  
    transitions.add(memoryA);
    input.clear();
    empty.clear();
    
    // Release B
    empty.add(Place.MemoryA);
    input.put(Place.CriticalB, 1);
    empty.add(Place.MemoryC);
  
    Transition<Place> memoryB = new Transition<Place>(input, empty, inhibitor, output);  
    transitions.add(memoryB);
    input.clear();
    empty.clear();
    
    // Release C
    empty.add(Place.MemoryA);
    empty.add(Place.MemoryB);
    input.put(Place.CriticalC, 1);
  
    Transition<Place> memoryC = new Transition<Place>(input, empty, inhibitor, output);  
    transitions.add(memoryC);
    input.clear();
    empty.clear();
    
    //Petri net is finished
    
    Set<Map<Place, Integer>> possibleTokenization = net.reachable(transitions);
    
    Thread aThread = new Thread (new Writer("A", criticalA, memoryA, net));
    Thread bThread = new Thread (new Writer("B", criticalB, memoryB, net));
    Thread cThread = new Thread (new Writer("C", criticalC, memoryC, net));
    
    ArrayList<Place> criticalSection = new ArrayList<>();
    criticalSection.add(Place.CriticalA);
    criticalSection.add(Place.CriticalB);
    criticalSection.add(Place.CriticalC);
    
    
    
    aThread.setName("Wątek A");
    bThread.setName("Wątek B");
    cThread.setName("Wątek C");
    
    try {
        aThread.start();
        bThread.start();
        cThread.start();
        
        Thread.sleep(100);

        aThread.interrupt();
        bThread.interrupt();
        cThread.interrupt();
        System.out.println("ROZMIAR TOKENIZACJI == " + possibleTokenization.size());
        
        boolean isSafe = true;
        
        for (Map<Place, Integer> tokenization : possibleTokenization) {
            int criticalSum = 0;
            for (Place p : criticalSection) {
                criticalSum += tokenization.getOrDefault(p, 0);
            } 
            if (criticalSum > 1) {
                isSafe = false;
            }
        }
        
        System.out.println("WĄTEK JEST " + (isSafe ? "BEZPIECZNY" : "NIEZABEZPIECZONY"));        
        
    } catch (InterruptedException e) {
        System.out.println("Przerwano wątek główny");
    }
    
  }
}
