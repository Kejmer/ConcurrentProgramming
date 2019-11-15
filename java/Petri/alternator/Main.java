package alternator;

import petrinet.*;

public class Main {
	
	private static enum Place {
		CriticalA, CriticalB, CriticalC, MemoryA, MemoryB, MemoryC
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
    inhibitor.add(Place.CriticalB);
    inhibitor.add(Place.CriticalC);
    
    transitions.add(new Transition<Place>(input.clone(), empty.clone(), inhibitor.clone(), output.clone()));
    output.clear();
    inhibitor.clear();
    
    // Print B
    output.put(Place.CriticalB, 1);
    output.add(Place.MemoryB, 1);
    inhibitor.add(Place.CriticalA);
    inhibitor.add(Place.CriticalC);
    
    transitions.add(new Transition<Place>(input.clone(), empty.clone(), inhibitor.clone(), output.clone()));
    output.clear();
    inhibitor.clear();
    
    // Print C
    output.put(Place.CriticalC, 1);
    output.add(Place.MemoryC, 1);
    inhibitor.add(Place.CriticalB);
    inhibitor.add(Place.CriticalA);
    
    transitions.add(new Transition<Place>(input.clone(), empty.clone(), inhibitor.clone(), output.clone()));
    output.clear();
    inhibitor.clear();
   
    
    //Petri net is finished
    try {
      

    }
    
  }
}
