package alternator;

import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import java.util.HashMap;
import java.util.Map;
import java.util.HashSet;
import java.util.ArrayList;

import petrinet.PetriNet;
import petrinet.Transition;

public class Main {
	
private static enum Place {
	CriticalA, CriticalB, CriticalC, MemoryA, MemoryB, MemoryC
}

  private class Writer implements Runnable {
    private String name;
    private Transition<Place> critical;
    private Transition<Place> memory;
    private PetriNet net;
    
    public Writer(String name, Transition<Place> critical, Transition<Place> memory, PetriNet net) {
      this.name = name;
      this.critical = critical;
      this.memory = memory;
      this.net = net;
    }
    
    @Override
    public void run() {
      while (true) {
        try {
            net.fire(Collections.singleton(critical));
            System.out.println(name);
            System.out.println(memory);
            net.fire(Collections.singleton(memory));
        } catch (InterruptedException e) {
            System.out.println("Przerwano wątek dla " + name);
        }
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
    
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    output.clear();
    inhibitor.clear();
    
    // Print B
    output.put(Place.CriticalB, 1);
    output.put(Place.MemoryB, 1);
    inhibitor.add(Place.CriticalA);
    inhibitor.add(Place.MemoryB);
    inhibitor.add(Place.CriticalC);
    
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    output.clear();
    inhibitor.clear();
    
    // Print C
    output.put(Place.CriticalC, 1);
    output.put(Place.MemoryC, 1);
    inhibitor.add(Place.CriticalA);
    inhibitor.add(Place.CriticalB);
    inhibitor.add(Place.MemoryC);
    
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    output.clear();
    inhibitor.clear();
    
    // Release A
    input.put(Place.CriticalA, 1);
    empty.add(Place.CriticalB);
    empty.add(Place.CriticalC);
  
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    input.clear();
    empty.clear();
    
    // Release B
    empty.add(Place.CriticalA);
    input.put(Place.CriticalB, 1);
    empty.add(Place.CriticalC);
  
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    input.clear();
    empty.clear();
    
    // Release C
    empty.add(Place.CriticalA);
    empty.add(Place.CriticalB);
    input.put(Place.CriticalC, 1);
  
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    input.clear();
    empty.clear();
    
    //Petri net is finished
    
    
  }
}
