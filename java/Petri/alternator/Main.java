package alternator;

import petrinet.*;

public class Main {
	
	private static enum Place {
		CriticalA, CriticalB, CriticalC, MemoryA, MemoryB, MemoryC
	}
  
  private class Writer extends Runnable {
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
        net.fire(Collections.singleton(critical));
        System.out.println(name);
        System.out.println(memory);
        net.fire(Collections.singleton(memory));
      }
    }
  }
	
	public static void main(String[] args) {
  	//Building the net
    //Places
    Map<Place, Integer> begin = new HashMap<>();
    PetriNet<Place, Integer> net = new PetriNet<>(begin, false);
    
    //Transitions
    ArrayList<Transition<Place>> transitions = new ArrayList<>();
    HashMap<Place, Integer> input = new HashMap<>(), output = new HashMap<>();
    HashSet<Place> empty = new HashSet<>(), inhibitor = new HashSet<>();
    
    // Print A
    output.put(Place.CriticalA, 1);
    output.add(Place.MemoryA, 1);
    inhibitor.add(Place.MemoryA);
    inhibitor.add(Place.CriticalB);
    inhibitor.add(Place.CriticalC);
    
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    output.clear();
    inhibitor.clear();
    
    // Print B
    output.put(Place.CriticalB, 1);
    output.add(Place.MemoryB, 1);
    inhibitor.add(Place.CriticalA);
    inhibitor.add(Place.MemoryB);
    inhibitor.add(Place.CriticalC);
    
    
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    output.clear();
    inhibitor.clear();
    
    // Print C
    output.put(Place.CriticalC, 1);
    output.add(Place.MemoryC, 1);
    inhibitor.add(Place.CriticalA);
    inhibitor.add(Place.CriticalB);
    inhibitor.add(Place.MemoryC);
    
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    output.clear();
    inhibitor.clear();
    
    // Release A
    input.put(Place.CriticalA, 1);
    reset.add(Place.CriticalB);
    reset.add(Place.CriticalC);
  
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    input.clear();
    reset.clear();
    
    // Release B
    reset.add(Place.CriticalA);
    input.put(Place.CriticalB, 1);
    reset.add(Place.CriticalC);
  
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    input.clear();
    reset.clear();
    
    // Release C
    reset.add(Place.CriticalA);
    reset.add(Place.CriticalB);
    input.put(Place.CriticalC, 1);
  
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
    input.clear();
    reset.clear();
    
    //Petri net is finished
    try {
      

    }
    
  }
}
