package petrinet;

import java.util.Collection;
import java.util.Map;
import java.util.Set;
import java.util.Queue;

public class PetriNet<T> {
	
	private class ReachableHelper<T> {
		Set<Map<T, Integer>> possibleTokenization = new Set<>();
	}
	
	private Map<T, Integer> tokenization;
	private final boolean fair;
	private Queue<Thread> threadQueue = new Queue<Thread>();

	public PetriNet(Map<T, Integer> initial, boolean fair) {
		this.tokenization = initial;
		this.fair = fair;
	}

	public Set<Map<T, Integer>> reachable(Collection<Transition<T>> transitions) {
		Set<Map<T, Integer>> possibleTokenization = new Set<>();
		
	}

	public Transition<T> fire(Collection<Transition<T>> transitions) throws InterruptedException {
	
	}
	
	private IncompatibleTransition createException(T t, String type) {
		String s = t.toString();
		if (s.isEmpty()) {
			s = type;
		}
		return new IncompatibleTransition(s);
	}
	
	public boolean allowedTransition(Transition<T> trans) throws IncompatibleTransition {
		for (Map.Entry<T, Integer> entry : trans.getInput().entrySet()) {
			if (tokenization.containsKey(entry.getKey())) {
				throw createException(entry.getKey(), "input");
			}
			if (tokenization.get(entry.getKey()) < entry.getValue()) {
				return false;
			}
		}
		
		for (Map.Entry<T, Integer> entry : trans.getOutput().entrySet()) {
			if (tokenization.containsKey(entry.getKey())) {
				throw createException(entry.getKey(), "output");
			}
		}
		
		for (T t : trans.getReset()) {
			if (tokenization.containsKey(t)) {
				throw createException(t, "reset");
			}
		}
		
		for (T t : trans.getInhibitor()) {
			if (tokenization.containsKey(t)) {
				throw createException(t, "inhibitor");
			}
			if (tokenization.get(t) > 0) {
				return false;
			}
		}
		return true;
	}

}

