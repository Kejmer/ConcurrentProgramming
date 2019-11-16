package multiplicator;

import petrinet.*;
import java.util.HashMap;
import java.util.Collection;
import java.util.Collections;
import java.util.ArrayList;
import java.util.Map;
import java.util.Scanner;

public class Main {
	
	private static enum Place {
		InputA, InputB, BufferA, BufferB, Result
	}
	
	public static void main(String[] args) {
		Scanner reader = new Scanner(System.in);  
		int a = reader.nextInt();  
	 	int b = reader.nextInt();
	 	
		//Building the net
		//Places
		Map<Place, Integer> begin = new HashMap<>();
		begin.put(Place.InputA, a);
		begin.put(Place.InputB, b);
		PetriNet<Place, Integer> net = new PetriNet<>(begin, false);
		
		//Transitions
		ArrayList<Transition<Place>> transitions = new ArrayList<>();
		HashMap<Place, Integer> input = new HashMap<>(), output = new HashMap<>();
		HashSet<Place> empty = new HashSet<>(), inhibitor = new HashSet<>();
		
		//InputA -> BufferA
		input.put(Place.InputA, 1);
		output.put(Place.BufferA, 1);
		inhibitor.add(Place.BufferA);
		inhibitor.add(Place.BufferB);
		
		transitions.add(new Transition<Place>(input, empty, inhibitor, output));
		input.clear();
		output.clear();
		inhibitor.clear();
		
		//calculating result
		input.put(Place.InputB, 1);
		input.put(Place.BufferA, 1);
		output.put(Place.Result, 1);
		output.put(Place.BufferB, 1);
		output.put(Place.BufferA, 1);
		
		transitions.add(new Transition<Place>(input, empty, inhibitor, output));
		input.clear();
		output.clear();
		
		//refiling B
		input.put(Place.BufferB, 1);
		output.put(Place.InputB, 1);
		inhibitor.add(Place.BufferA);
		
		transitions.add(new Transition<Place>(input, empty, inhibitor, output));
		input.clear();
		output.clear();
		inhibitor.clear();
		
		//Substracting 1 from multiplier
		input.put(Place.BufferB, 1);
		inhibitor.add(Place.InputA, 1);
		
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
		input.clear();
		inhibitor.clear();
		
		//Final transition
		inhibitor.add(Place.InputA);
		inhibitor.add(Place.BufferA);
		
		Transition<Place> finalTransition = new Transition<Place>(input, empty, inhibitor, output);
		inhibitor.clear();
		
		//Petri net is finished
		try {
			

		}
		
	}
}
