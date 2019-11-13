package multiplicator;

import petrinet.*;
import java.util.HashMap;
import java.util.Collection;
import java.util.Collections;
import java.util.ArrayList;
import java.util.Map;

public class Main {
	
	private static enum Place {
		InputA, InputB, BufferA, BufferB, Result
	}
	
	private static Map<Place, Integer> marking(int a, int b, int c, int d) {
        Map<Place, Integer> result = new HashMap<>();
        putPositive(result, Place.A, a);
        putPositive(result, Place.B, b);
        putPositive(result, Place.C, c);
        putPositive(result, Place.D, d);
        return result;
    }
	
	public static void main(String[] args) {
		try {
			int a, b;
			a = 3;
			b = 4;
			//Tutaj dodać skaner który to wczyta
			
			
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
			
			transitions.add(new Transition<Place>(input.clone(), empty.clone(), inhibitor.clone(), output.clone()));
			input.clear();
			output.clear();
			inhibitor.clear();
			
			//calculating result
			input.put(Place.InputB, 1);
			input.put(Place.BufferA, 1);
			output.put(Place.Result, 1);
			output.put(Place.BufferB, 1);
			output.put(Place.BufferA, 1);
			
			transitions.add(new Transition<Place>(input.clone(), empty.clone(), inhibitor.clone(), output.clone()));
			input.clear();
			output.clear();
			
			//refiling B
			input.put(Place.BufferB, 1);
			output.put(Place.InputB, 1);
			inhibitor.add(Place.BufferA);
			
			transitions.add(new Transition<Place>(input.clone(), empty.clone(), inhibitor.clone(), output.clone()));
			input.clear();
			output.clear();
			inhibitor.clear();
			
			//Substracting 1 from multiplier
			input.put(Place.BufferB, 1);
			inhibitor.add(Place.InputA, 1);
			
			transitions.add(new Transition<Place>(input.clone(), empty.clone(), inhibitor.clone(), output.clone()));
			input.clear();
			inhibitor.clear();
			
			//Final transition
			
			inhibitor.add(Place.InputA);
			inhibitor.add(Place.BufferA);
			
			Transition<Place> finalTransition = new Transition<>(input.clone(), empty.clone(), inhibitor.clone(), output.clone());
			inhibitor.clear();
			
			//Petri net is finished
			

		}
		
	}
}
