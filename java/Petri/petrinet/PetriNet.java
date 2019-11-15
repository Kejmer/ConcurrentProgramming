package petrinet;

import java.util.Collection;
import java.util.Map;
import java.util.Set;
import java.util.Queue;
import java.util.HashSet;
import java.util.concurrent.Semaphore;


public class PetriNet<T> {
	
	private Semaphore mutex;
	
	private Map<T, Integer> tokenization;
	
	private Queue<Thread> threadQueue = new Queue<Thread>();

	public PetriNet(Map<T, Integer> initial, boolean fair) {
		this.tokenization = initial;
		this.mutex = new Semaphore(1, fair);
	}
	
	private void generateReachable(Collection<Transition<T>> transitions, Set<Map<T, Integer>> possibleTokenization, Map<T, Integer> currentState) {
		for (Transition<T> t : transitions) {
			if (allowedTransition(t, currentState)) {
				Map<T, Integer> tokenizationCopy = currentState.clone();
				getNextState(t, tokenizationCopy);
				if (possibleTokenization.contains(tokenizationCopy)) {
					possibleTokenization.add(tokenizationCopy);
					generateReachable(transitions, possibleTokenization, currentState);
				}
			}
		}
	}

	public Set<Map<T, Integer>> reachable(Collection<Transition<T>> transitions) {
		Set<Map<T, Integer>> possibleTokenization = new HashSet<>();
		generateReachable(transitions, possibleTokenization, tokenization);
		return possibleTokenization;
	}

	public Transition<T> fire(Collection<Transition<T>> transitions) throws InterruptedException {
		while (true) {
			try {
				mutex.acquire();
				
				for (Transition<T> t : transitions) {
					if (allowedTransition(t, tokenization)) {
						getNextState(t, tokenization);
						return t;
					}
				}
				
			} finally {
				mutex.release();
			}
		}
	}
	
	private IncompatibleTransition createException(T t, String type) {
		String s = t.toString();
		if (s.isEmpty()) {
			s = type;
		}
		return new IncompatibleTransition(s);
	}
	
	//Zakładamy że przejście jest dozwolone
	private void getNextState(Transition<T> trans, Map<T, Integer> state) {
		for (Map.Entry<T, Integer> entry : trans.getInput()) {
			state.replace(entry.getKey(), state.get(entry.getKey()) - entry.getValue());
		}
		
		for (T entry : trans.getReset()) {
			state.replace(T, 0);
		}
		
		for (Map.Entry<T, Integer> entry : trans.getOutput()) {
			state.replace(entry.getKey(), state.get(entry.getKey()) + entry.getValue());
		}
	}
	
	private boolean allowedTransition(Transition<T> trans, Map<T, Integer> currentTokenization) throws IncompatibleTransition {
		for (Map.Entry<T, Integer> entry : trans.getInput().entrySet()) {
			if (currentTokenization.containsKey(entry.getKey())) {
				throw createException(entry.getKey(), "input");
			}
			if (currentTokenization.get(entry.getKey()) < entry.getValue()) {
				return false;
			}
		}
		
		for (Map.Entry<T, Integer> entry : trans.getOutput().entrySet()) {
			if (currentTokenization.containsKey(entry.getKey())) {
				throw createException(entry.getKey(), "output");
			}
		}
		
		for (T t : trans.getReset()) {
			if (currentTokenization.containsKey(t)) {
				throw createException(t, "reset");
			}
		}
		
		for (T t : trans.getInhibitor()) {
			if (currentTokenization.containsKey(t)) {
				throw createException(t, "inhibitor");
			}
			if (currentTokenization.get(t) > 0) {
				return false;
			}
		}
		return true;
	}

}

