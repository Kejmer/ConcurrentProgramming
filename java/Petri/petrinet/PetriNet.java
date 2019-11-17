package petrinet;

import java.util.Collection;
import java.util.Map;
import java.util.HashMap;
import java.util.Set;
import java.util.Queue;
import java.util.HashSet;
import java.util.concurrent.Semaphore;
import java.util.LinkedList;


public class PetriNet<T> {
	
	private Semaphore mutex;
	
	private Map<T, Integer> tokenization;
	private Collection<Semaphore> threadQueue; 

	public PetriNet(Map<T, Integer> initial, boolean fair) {
		
		LinkedList<T> toErase = new LinkedList<>();
		for (T entry : initial.keySet()) {
			if (initial.get(entry) == 0) {
				toErase.add(entry);
			}
		}
		for (T entry : toErase) {
			initial.remove(entry);
		}
		
		this.tokenization = initial;
		if (fair) {
			threadQueue = new LinkedList<Semaphore>();
		} else {
			threadQueue = new HashSet<Semaphore>(); //Non deterministic iteration
		}
		this.mutex = new Semaphore(1, fair);
	}
	
	private void generateReachable(Collection<Transition<T>> transitions, Set<Map<T, Integer>> possibleTokenization, Map<T, Integer> currentState) {
		for (Transition<T> t : transitions) {
			if (allowedTransition(t, currentState)) {
				Map<T, Integer> tokenizationCopy = new HashMap<T, Integer>(currentState);
				getNextState(t, tokenizationCopy);
				
				if (!possibleTokenization.contains(tokenizationCopy)) {
					possibleTokenization.add(tokenizationCopy);
					generateReachable(transitions, possibleTokenization, tokenizationCopy);
				}
			}
		}
	}

	public Set<Map<T, Integer>> reachable(Collection<Transition<T>> transitions) {
		Set<Map<T, Integer>> possibleTokenization = new HashSet<>();
		Map<T, Integer> tokenizationCopy;
		try {
			mutex.acquire();
			tokenizationCopy = new HashMap<T, Integer>(tokenization);
			mutex.release();
		} catch (InterruptedException e) {
			mutex.release();
			return possibleTokenization;
		}
				
		possibleTokenization.add(tokenizationCopy);
		generateReachable(transitions, possibleTokenization, tokenizationCopy);
		return possibleTokenization;
	}
	
	private void wakeUpThreads() {
		for (Semaphore s : threadQueue) {
			s.release();
		}
		threadQueue.clear();
	}

	public Transition<T> fire(Collection<Transition<T>> transitions) throws InterruptedException {
		Semaphore localSemaphore = new Semaphore(0);
		while (true) {
			mutex.acquire();	
			for (Transition<T> t : transitions) {
				if (allowedTransition(t, tokenization)) {
					getNextState(t, tokenization);
					wakeUpThreads();					
					mutex.release();
					return t;
				}
			}
			threadQueue.add(localSemaphore);
			mutex.release();
			localSemaphore.acquire();
		}
	}
	
	//Zakładamy że przejście jest dozwolone
	private void getNextState(Transition<T> trans, Map<T, Integer> state) {
		for (Map.Entry<T, Integer> entry : trans.getInput().entrySet()) {
			state.replace(entry.getKey(), state.get(entry.getKey()) - entry.getValue());
			state.remove(entry.getKey(), 0);
		}
		
		for (T entry : trans.getReset()) {
			if (state.containsKey(entry)) {
				state.remove(entry);
			} 
		}
		
		for (Map.Entry<T, Integer> entry : trans.getOutput().entrySet()) {
			if (state.containsKey(entry.getKey())) {
				state.replace(entry.getKey(), state.get(entry.getKey()) + entry.getValue());
			} else {
				if (entry.getValue() > 0) {
					state.put(entry.getKey(), entry.getValue());
				}
			}
		}
	}
	
	private boolean allowedTransition(Transition<T> trans, Map<T, Integer> currentTokenization) {//} throws IncompatibleTransition {
		for (Map.Entry<T, Integer> entry : trans.getInput().entrySet()) {
			if (currentTokenization.getOrDefault(entry.getKey(), 0) < entry.getValue()) {
				return false;
			}
		}
		
		
		for (T t : trans.getInhibitor()) {
			if (currentTokenization.getOrDefault(t, 0) > 0) {
				return false;
			}
		}
		return true;
	}

}

