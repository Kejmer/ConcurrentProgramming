package multiplicator;

import java.util.HashMap;
import java.util.Collection;
import java.util.Collections;
import java.util.ArrayList;
import java.util.Map;
import java.util.Scanner;
import java.util.HashSet;
import java.util.Set;

import petrinet.PetriNet;
import petrinet.Transition;

public class Main {
	
	private static enum Place {
		InputA, InputB, BufferA, BufferB, Result
	}
	
	private static class Counter implements Runnable {
    private Collection<Transition<Place>> transitions;
    private PetriNet net;
    private int idx;
    private int numOfTransitions;

    public Counter(Collection<Transition<Place>> transitions, PetriNet net, int idx) {
      this.transitions = transitions;
      this.net = net;
      this.idx = idx;
      this.numOfTransitions = 0;
    }

    @Override
    public void run() {
      try {
        while (true) {
          net.fire(transitions);
          numOfTransitions++;
        }
      } catch (InterruptedException e) {
      	System.out.println("Wątek " + idx + " wykonał " + numOfTransitions + " przejść.");
      }
    }
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
		PetriNet<Place> net = new PetriNet<>(begin, false);
		
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
		input.put(Place.BufferA, 1);
		inhibitor.add(Place.InputB);
		
    transitions.add(new Transition<Place>(input, empty, inhibitor, output));
		input.clear();
		inhibitor.clear();
		
		//Final transition
		inhibitor.add(Place.InputA);
		inhibitor.add(Place.BufferA);
		inhibitor.add(Place.BufferB);
		
		Transition<Place> finalTransition = new Transition<Place>(input, empty, inhibitor, output);
		inhibitor.clear();
		
		//Petri net is finished
		try {
			ArrayList<Thread> workers = new ArrayList<>();
			for (int i = 0; i < 4; i++) {
				workers.add(new Thread(new Counter(transitions, net, i)));
			}
			for (Thread t : workers) {
				t.start();
			}
			
			net.fire(Collections.singleton(finalTransition));
			
			//Checking if every transition is no longer legal (except finalTransition)
			Set<Map<Place, Integer>> possibleTokenization = net.reachable(transitions); 
			
			if (possibleTokenization.size() > 1) {
				System.err.println("Niektóre przejścia wciąż są dozwolone");
			} else {
				for (Map<Place, Integer> tokenization : possibleTokenization) {
					int result = tokenization.get(Place.Result);
					System.out.println(result);
				}
			}
			
			for (Thread t : workers) {
				t.interrupt();
			}
			
		} catch (InterruptedException e) {
			System.out.println("Wątek main został przerwany!");
		}
		
	}
}
